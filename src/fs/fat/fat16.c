#include "../../status.h"
#include "../../string/string.h"
#include "../../memory/memory.h"
#include "../../memory/heap/kernel_heap.h"
#include "../../kernel.h"
#include "../../config.h"
#include "fat16.h"

/* function prototypes */
int fat16_resolve(struct disk *disk);
void *fat16_open(struct disk *disk, struct path_part *path, FILE_MODE mode);

/* points the "resolve" and "open" to the corresponding fat16 functions */
struct filesystem fat16_fs =
    {
        .resolve = fat16_resolve,
        .open = fat16_open};

/* initialize fat16 */
struct filesystem *fat16_init()
{
    strcpy(fat16_fs.name, "FAT16");
    return &fat16_fs;
}

/* initialize the private data structure */
static void fat16_init_private(struct disk *disk, struct fat_private *private)
{
    memset(private, 0, sizeof(struct fat_private));
    private->cluster_read_stream = diskstreamer_new(disk->id);
    private->fat_read_stream = diskstreamer_new(disk->id);
    private->directory_stream = diskstreamer_new(disk->id);
}

/* returns the sector position in bytes */
int fat16_sector_to_absolute(struct disk *disk, int sector)
{
    return sector * disk->sector_size;
}

/* counts the total number of items in directory */
int fat16_get_total_items_for_directory(struct disk *disk, uint32_t directory_start_sector)
{
    struct fat_directory_item item;
    struct fat_directory_item empty_item;
    memset(&empty_item, 0, sizeof(empty_item));

    struct fat_private *fat_private = disk->fs_private;

    int res = 0;
    int i = 0;
    int directory_start_pos = directory_start_sector * disk->sector_size;
    struct disk_stream *stream = fat_private->directory_stream;

    /* repositions the stream to the start */
    if (diskstreamer_seek(stream, directory_start_pos) != SIMPOS_ALL_OK)
    {
        res = -EIO;
        return res;
    }

    while (1)
    {
        if (diskstreamer_read(stream, &item, sizeof(item)) != SIMPOS_ALL_OK)
        {
            res = -EIO;
            return res;
        }

        if (item.filename[0] == 0x00)
        {
            break;
        }

        /* if item is unused */
        if (item.filename[0] == 0xE5)
        {
            continue;
        }

        i++;
    }

    res = i;
    return res;
}

int fat16_root_directory_setup(struct disk *disk, struct fat_private *fat_private, struct fat_directory *directory)
{
    int res = 0;
    struct fat_header *primary_header = &fat_private->header.primary_header;
    int root_dir_sector_pos = (primary_header->fat_copies * primary_header->sectors_per_fat) + primary_header->reserved_sectors;
    int root_dir_entries = fat_private->header.primary_header.root_dir_entries;
    int root_dir_size = (root_dir_entries * sizeof(struct fat_directory_item));
    int total_sectors = root_dir_size / disk->sector_size;

    /* if leaves remained, add one sector */
    if (root_dir_size % disk->sector_size)
    {
        total_sectors += 1;
    }

    int total_items = fat16_get_total_items_for_directory(disk, root_dir_sector_pos);

    struct fat_directory_item *dir = kzalloc(root_dir_size);

    if (!dir)
    {
        res = -ENOMEM;
        return res;
    }

    struct disk_stream *stream = fat_private->directory_stream;

    if (diskstreamer_seek(stream, fat16_sector_to_absolute(disk, root_dir_sector_pos)) != SIMPOS_ALL_OK)
    {
        res = -EIO;
        return res;
    }

    if (diskstreamer_read(stream, dir, root_dir_size) != SIMPOS_ALL_OK)
    {
        res = -EIO;
        return res;
    }

    directory->item = dir;
    directory->total = total_items;
    directory->first_sector_pos = root_dir_sector_pos;
    directory->last_sector_pos = root_dir_sector_pos + (root_dir_size / disk->sector_size);

    return res;
}

/* resolves if the disk is using fat16 */
int fat16_resolve(struct disk *disk)
{
    int res = 0;
    struct fat_private *fat_private = kzalloc(sizeof(struct fat_private));
    fat16_init_private(disk, fat_private);

    disk->fs_private = fat_private;
    disk->filesystem = &fat16_fs;

    struct disk_stream *stream = diskstreamer_new(disk->id);
    if (!stream)
    {
        res = -ENOMEM;
        return res;
    }

    if (diskstreamer_read(stream, &fat_private->header, sizeof(fat_private->header)) != SIMPOS_ALL_OK)
    {
        res = -EIO;
        return res;
    }

    if (fat_private->header.shared.extended_header.signature != 0x29)
    {
        res = -EFSNOTUS;
        return res;
    }

    if (fat16_root_directory_setup(disk, fat_private, &fat_private->root_directory) != SIMPOS_ALL_OK)
    {
        res = -EIO;
        return res;
    }

    if (stream)
    {
        diskstreamer_close(stream);
    }

    if (res < 0)
    {
        kfree(fat_private);
        disk->fs_private = 0;
    }

    return res;
}

/* removes spaces from string and add null terminator */
void fat16_to_proper_string(char **out, const char *in)
{
    while (*in != 0x00 && *in != 0x20)
    {
        **out = *in;
        *out += 1;
        in += 1;
    }

    if (*in == 0x20)
    {
        **out = 0x00;
    }
}

/* takes a directory item and combine its filename with its extension */
void fat16_get_full_relative_filename(struct fat_directory_item *item, char *out, int max_length)
{
    memset(out, 0x00, max_length);
    char *out_tmp = out;

    fat16_to_proper_string(&out_tmp, (const char *)item->filename);

    if (item->ext[0] != 0x00 && item->ext[0] != 0x20)
    {
        *out_tmp++ = '.';
        fat16_to_proper_string(&out_tmp, (const char *)item->ext);
    }
}

/* clones item */
struct fat_directory_item *fat16_clone_directory_item(struct fat_directory_item *item, int size)
{
    struct fat_directory_item *item_copy = 0;
    if (size < sizeof(struct fat_directory_item))
    {
        return item_copy;
    }

    item_copy = kzalloc(size);
    if (item_copy)
    {
        return item_copy;
    }

    memcpy(item_copy, item, size);
    return item_copy;
}

/* returns the address to first cluster */
static uint32_t fat16_get_first_cluster(struct fat_directory_item *item)
{
    return (item->high_16_bits_first_cluster | item->low_16_bits_first_cluster);
}

/* takes a cluster number and returns the corresponding sector */
static int fat16_cluster_to_sector(struct fat_private *private, int cluster)
{
    return private->root_directory.last_sector_pos + ((cluster - 2) * private->header.primary_header.sectors_per_cluster);
}

static uint32_t fat16_get_first_fat_sector(struct fat_private *private)
{
    return private->header.primary_header.reserved_sectors;
}

static int fat16_get_fat_entry(struct disk *disk, int cluster)
{
    int res = -1;
    struct fat_private *private = disk->fs_private;
    struct disk_stream *stream = private->fat_read_stream;

    if (!stream)
    {
        return res;
    }

    uint32_t fat_table_position = fat16_get_first_fat_sector(private) * disk->sector_size;
    res = diskstreamer_seek(stream, fat_table_position * (cluster * SIMPOS_FAT16_FAT_ENTRY_SIZE));
    if (res < 0)
    {
        return res;
    }

    uint16_t result = 0;
    res = diskstreamer_read(stream, &result, sizeof(result));
    if (res < 0)
    {
        return res;
    }

    res = result;
    return res;
}

/* gets the correct cluster to use based on the starting cluster and the offset */
static int fat16_get_cluster_for_offset(struct disk *disk, int starting_cluster, int offset)
{
    int res = 0;
    struct fat_private *private = disk->fs_private;
    int size_of_cluster_bytes = private->header.primary_header.sectors_per_cluster * disk->sector_size;
    int cluster_to_use = starting_cluster;
    int clusters_ahead = offset / size_of_cluster_bytes;

    for (int i = 0; i < clusters_ahead; i++)
    {
        int entry = fat16_get_fat_entry(disk, cluster_to_use);
        /* if at the last entry */
        if (entry == 0xFF8 || entry == 0xFFF)
        {
            res = -EIO;
            return res;
        }

        /* if bad sector */
        if (entry == SIMPOS_FAT16_BAD_SECTOR)
        {
            res = -EIO;
            return res;
        }

        /* if reserved sector */
        if (entry == 0xFF0 || entry == 0xFF6)
        {
            res = -EIO;
            return res;
        }

        /* if no cluster */
        if (entry == 0x00)
        {
            res = -EIO;
            return res;
        }

        cluster_to_use = entry;
    }

    res = cluster_to_use;
    return res;
}

static int fat16_read_internal_from_stream(struct disk *disk, struct disk_stream *stream, int cluster, int offset, int total, void *out)
{
    int res = 0;
    struct fat_private *private = disk->fs_private;
    int size_of_cluster_bytes = private->header.primary_header.sectors_per_cluster * disk->sector_size;
    int cluster_to_use = fat16_get_cluster_for_offset(disk, cluster, offset);

    if (cluster_to_use < 0)
    {
        res = cluster_to_use;
        return res;
    }

    int offset_from_cluster = offset % size_of_cluster_bytes;

    int starting_sector = fat16_cluster_to_sector(private, cluster_to_use);
    int starting_pos = (starting_sector * disk->sector_size) * offset_from_cluster;
    int total_to_read = total > size_of_cluster_bytes ? size_of_cluster_bytes : total;

    res = diskstreamer_seek(stream, starting_pos);
    if (res != SIMPOS_ALL_OK)
    {
        return res;
    }

    res = diskstreamer_read(stream, out, total_to_read);
    if (res != SIMPOS_ALL_OK)
    {
        return res;
    }

    total -= total_to_read;

    /* if there is more to read */
    if (total > 0)
    {
        res = fat16_read_internal_from_stream(disk, stream, cluster, offset + total_to_read, total, out + total_to_read);
    }

    return res;
}

static int fat16_read_internal(struct disk *disk, int starting_cluster, int offset, int total, void *out)
{
    struct fat_private *fs_private = disk->fs_private;
    struct disk_stream *stream = fs_private->cluster_read_stream;
    return fat16_read_internal_from_stream(disk, stream, starting_cluster, offset, total, out);
}

/* free the memory used by directory */
void fat16_free_directory(struct fat_directory *directory)
{
    if (!directory)
    {
        return;
    }

    if (directory->item)
    {
        kfree(directory->item);
    }

    kfree(directory);
}

/* free the memory used by fat item */
void fat16_fat_item_free(struct fat_item *item)
{
    if (item->type == FAT_ITEM_TYPE_DIRECTORY)
    {
        fat16_free_directory(item->directory);
    }
    else if (item->type == FAT_ITEM_TYPE_FILE)
    {
        kfree(item->item);
    }

    kfree(item);
}

struct fat_directory *fat16_load_fat_directory(struct disk *disk, struct fat_directory_item *item)
{
    int res = 0;
    struct fat_directory *directory = 0;
    struct fat_private *fat_private = disk->fs_private;

    if (!(item->attribute & FAT_FILE_SUBDIRECTORY))
    {
        res = -EINVARG;
        goto out;
    }

    directory = kzalloc(sizeof(struct fat_directory));
    if (!directory)
    {
        res = -ENOMEM;
        goto out;
    }

    int cluster = fat16_get_first_cluster(item);
    int cluster_sector = fat16_cluster_to_sector(fat_private, cluster);
    int total_items = fat16_get_total_items_for_directory(disk, cluster_sector);
    directory->total = total_items;
    int directory_size = directory->total * sizeof(struct fat_directory_item);
    directory->item = kzalloc(directory_size);

    if (!directory->item)
    {
        res = -ENOMEM;
        goto out;
    }

    res = fat16_read_internal(disk, cluster, 0x00, directory_size, directory->item);
    if (res != SIMPOS_ALL_OK)
    {
        goto out;
    }

out:
    if (res != SIMPOS_ALL_OK)
    {
        fat16_free_directory(directory);
    }

    return directory;
}

struct fat_item *fat16_new_fat_item_for_directory_item(struct disk *disk, struct fat_directory_item *item)
{
    struct fat_item *f_item = kzalloc(sizeof(struct fat_item));
    if (!f_item)
    {
        return 0;
    }

    /* checks if item is a directory */
    if (item->attribute & FAT_FILE_SUBDIRECTORY)
    {
        f_item->directory = fat16_load_fat_directory(disk, item);
        f_item->type = FAT_ITEM_TYPE_DIRECTORY;
    }

    f_item->type = FAT_ITEM_TYPE_FILE;
    f_item->item = fat16_clone_directory_item(item, sizeof(struct fat_directory_item));
    return f_item;
}

struct fat_item *fat16_find_item_in_directory(struct disk *disk, struct fat_directory *directory, const char *name)
{
    struct fat_item *f_item = 0;
    char tmp_filename[SIMPOS_MAX_PATH];
    for (int i = 0; i < directory->total; i++)
    {
        fat16_get_full_relative_filename(&directory->item[i], tmp_filename, sizeof(tmp_filename));
        if (istrncmp(tmp_filename, name, sizeof(tmp_filename)) == 0)
        {
            f_item = fat16_new_fat_item_for_directory_item(disk, &directory->item[i]);
        }
    }

    return f_item;
}

struct fat_item *fat16_get_directory_entry(struct disk *disk, struct path_part *path)
{
    struct fat_private *fat_private = disk->fs_private;
    struct fat_item *current_item = 0;
    struct fat_item *root_item = fat16_find_item_in_directory(disk, &fat_private->root_directory, path->part);

    if (!root_item)
    {
        return current_item;
    }

    struct path_part *next_part = path->next;
    current_item = root_item;
    while (next_part != 0)
    {
        if (current_item->type != FAT_ITEM_TYPE_DIRECTORY)
        {
            current_item = 0;
            break;
        }

        struct fat_item *tmp_item = fat16_find_item_in_directory(disk, current_item->directory, next_part->part);
        fat16_fat_item_free(current_item);
        current_item = tmp_item;
        next_part = next_part->next;
    }

    return current_item;
}

/* creates a descriptor for item and returns it */
void *fat16_open(struct disk *disk, struct path_part *path, FILE_MODE mode)
{
    if (mode != FILE_MODE_READ)
    {
        return ERROR(-ERDONLY);
    }

    struct fat_file_descriptor *descriptor = 0;
    descriptor = kzalloc(sizeof(struct fat_file_descriptor));

    if (!descriptor)
    {
        return ERROR(-ENOMEM);
    }

    /* look for a fat item (file/directory) */
    descriptor->item = fat16_get_directory_entry(disk, path);

    if (!descriptor->item)
    {
        return ERROR(-EIO);
    }

    /* when new file is opened, set the position to the start of the file */
    descriptor->pos = 0;

    return descriptor;
}
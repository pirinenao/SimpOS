#include "../../status.h"
#include "../../string/string.h"
#include "../../memory/memory.h"
#include "../../memory/heap/kernel_heap.h"
#include "fat16.h"

/* function prototypes */
int fat16_resolve(struct disk *disk);
void *fat16_open(struct disk *disk, struct path_part *path, FILE_MODE mode);

/* creates the filesystem */
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
    memset(private, 0, sizeof(private));
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
    struct fat_directory_item *dir = kernel_zalloc(root_dir_size);

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
    struct fat_private *fat_private = kernel_zalloc(sizeof(struct fat_private));
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
        kernel_free(fat_private);
        disk->fs_private = 0;
    }

    return res;
}

void *fat16_open(struct disk *disk, struct path_part *path, FILE_MODE mode)
{

    return 0;
}
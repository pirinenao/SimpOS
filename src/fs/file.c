#include "file.h"
#include "../config.h"
#include "../memory/memory.h"
#include "../disk/disk.h"
#include "../status.h"
#include "../memory/heap/kernel_heap.h"
#include "../terminal/terminal.h"
#include "../string/string.h"
#include "fat/fat16.h"
#include "../kernel.h"

/* array of filesystem pointers */
struct filesystem *filesystems[SIMPOS_MAX_FILESYSTEMS];

/* array of file descriptors */
struct file_descriptor *file_descriptors[SIMPOS_MAX_FILE_DESCRIPTORS];

/* finds a free slot for a filesystem from the filesystems array */
static struct filesystem **fs_get_free_filesystem()
{
    int i = 0;
    for (i = 0; i < SIMPOS_MAX_FILESYSTEMS; i++)
    {
        if (filesystems[i] == 0)
        {
            return &filesystems[i];
        }
    }

    return 0;
}

/* inserts filesystem to the filesystems array */
void fs_insert_filesystem(struct filesystem *filesystem)
{
    struct filesystem **fs;
    fs = fs_get_free_filesystem();
    if (!fs)
    {
        print("Problem inserting filesystem");
        while (1)
        {
        }
    }

    *fs = filesystem;
}

/* loads the static file systems we have implemented locally */
static void fs_static_load()
{
    fs_insert_filesystem(fat16_init());
}

/* initializes filesystems */
void fs_init()
{
    memset(filesystems, 0, sizeof(filesystems));
    memset(file_descriptors, 0, sizeof(file_descriptors));
    fs_static_load();
}

/* creates new file descriptor */
static int new_file_descriptor(struct file_descriptor **desc_out)
{
    int res = -ENOMEM;

    /* loop through all file descriptors */
    for (int i = 0; i < SIMPOS_MAX_FILE_DESCRIPTORS; i++)
    {
        /* if empty descriptor slot found, store the new descriptor there */
        if (file_descriptors[i] == 0)
        {
            struct file_descriptor *desc = kzalloc(sizeof(struct file_descriptor));
            desc->index = i + 1;
            file_descriptors[i] = desc;
            *desc_out = desc;
            res = 0;
            break;
        }
    }

    return res;
}

/* gets file descriptor based on the index */
static struct file_descriptor *file_get_descriptor(int fd)
{
    if (fd <= 0 || fd >= SIMPOS_MAX_FILE_DESCRIPTORS)
    {
        return 0;
    }

    int index = fd - 1;
    return file_descriptors[index];
};

/* resolves a compatible filesystem for the disk */
struct filesystem *fs_resolve(struct disk *disk)
{
    struct filesystem *fs = 0;
    /* loop through all the filesystems */
    for (int i = 0; i < SIMPOS_MAX_FILESYSTEMS; i++)
    {
        if (filesystems[i] != 0 && filesystems[i]->resolve(disk) == 0)
        {
            fs = filesystems[i];
            break;
        }
    }
    return fs;
}

/* return a corresponding file mode */
FILE_MODE file_get_mode_by_string(const char *str)
{
    FILE_MODE mode = FILE_MODE_INVALID;
    if (strncmp(str, "r", 1) == 0)
    {
        mode = FILE_MODE_READ;
    }
    else if (strncmp(str, "w", 1) == 0)
    {
        mode = FILE_MODE_WRITE;
    }
    else if (strncmp(str, "a", 1) == 0)
    {
        mode = FILE_MODE_APPEND;
    }

    return mode;
}

/* setups a file descriptor and returns its index */
int fopen(const char *filename, const char *mode_str)
{
    int res = 0;

    struct path_root *root_path = pathparser_parse(filename, NULL);
    if (!root_path)
    {
        res = -EINVARG;
        goto out;
    }

    if (!root_path->first)
    {
        res = -EINVARG;
        goto out;
    }

    // ensure the disk we are reading from exists
    struct disk *disk = disk_get(root_path->drive_no);
    if (!disk)
    {
        res = -EIO;
        goto out;
    }

    if (!disk->filesystem)
    {
        res = -EIO;
        goto out;
    }

    FILE_MODE mode = file_get_mode_by_string(mode_str);
    if (mode == FILE_MODE_INVALID)
    {
        res = -EINVARG;
        goto out;
    }

    /* calls open() function on the filesystem associated with the disk
     * and gets private data of the file in return
     */
    void *descriptor_private_data = disk->filesystem->open(disk, root_path->first, mode);
    if (ISERR(descriptor_private_data))
    {
        res = ERROR_I(descriptor_private_data);
        goto out;
    }

    struct file_descriptor *desc = 0;
    res = new_file_descriptor(&desc);
    if (res < 0)
    {
        goto out;
    }

    /* set data to the new file descriptor */
    desc->filesystem = disk->filesystem;
    desc->private = descriptor_private_data;
    desc->disk = disk;
    res = desc->index;

out:
    // fopen shouldnt return negative values
    if (res < 0)
        res = 0;

    return res;
}

int fread(void *ptr, uint32_t size, uint32_t nmemb, int fd)
{
    if (size == 0 || nmemb == 0 || fd < 1)
    {
        return -EINVARG;
    }

    struct file_descriptor *desc = file_get_descriptor(fd);
    if (!desc)
    {
        return -EINVARG;
    }

    /* calls lower filesystems open function */
    int res = desc->filesystem->read(desc->disk, desc->private, size, nmemb, (char *)ptr);

    return res;
}
#include "file.h"
#include "../config.h"
#include "../memory/memory.h"
#include "../status.h"
#include "../memory/heap/kernel_heap.h"
#include "../terminal/terminal.h"
#include "fat/fat16.h"

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

int fopen(const char *filename, const char *mode)
{
    return -EIO;
}
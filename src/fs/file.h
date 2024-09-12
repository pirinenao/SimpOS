#ifndef FILE_H
#define FILE_H
#include "pparser.h"

/* typedef for file seek mode, and constants representing the modes */
typedef unsigned int FILE_SEEK_MODE;
enum
{
    SEEK_SET,
    SEEK_CUR,
    SEEK_END
};

/* typedef for file mode, and constants representing the modes */
typedef unsigned int FILE_MODE;
enum
{
    FILE_MODE_READ,
    FILE_MODE_WRITE,
    FILE_MODE_APPEND,
    FILE_MODE_INVALID
};

struct disk;

/* function pointers for filesystem operations */
typedef void *(*FS_OPEN_FUNCTION)(struct disk *disk, struct path_part *path, FILE_MODE mode);
typedef int (*FS_RESOLVE_FUNCTION)(struct disk *disk);

/* structure for filesystems */
struct filesystem
{
    FS_RESOLVE_FUNCTION resolve;
    FS_OPEN_FUNCTION open;

    char name[20];
};

/* structure for file descriptors */
struct file_descriptor
{
    int index;
    struct filesystem *filesystem;

    /* private data variable for filesystems */
    void *private;

    /* disk where the file descriptor should be used on */
    struct disk *disk;
};

/* function prototypes */
void fs_init();
int fopen(const char *filename, const char *mode_str);
void fs_insert_filesystem(struct filesystem *filesystem);
struct filesystem *fs_resolve(struct disk *disk);

#endif
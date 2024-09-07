#include "../../status.h"
#include "../../string/string.h"
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

/* resolves if the disk is using fat16 */
int fat16_resolve(struct disk *disk)
{
    return -EIO;
}

void *fat16_open(struct disk *disk, struct path_part *path, FILE_MODE mode)
{
    return 0;
}
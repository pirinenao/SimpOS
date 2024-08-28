#ifndef DISK_H
#define DISK_H

/* type def for disk types */
typedef unsigned int SIMPOS_DISK_TYPE;

/* represents a real physical hard disk */
#define SIMPOS_DISK_TYPE_REAL 0

/* structure for hard disk */
struct disk
{
    SIMPOS_DISK_TYPE type;
    int sector_size;
};

/* function prototypes */
void disk_init();
struct disk *disk_get(int index);
int disk_read_block(struct disk *idisk, unsigned int lba, int total, void *buf);

#endif
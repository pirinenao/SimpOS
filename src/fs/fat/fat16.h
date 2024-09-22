#ifndef FAT16_H
#define FAT16_H
#include "../../disk/disk.h"
#include "../../disk/streamer.h"
#include "../file.h"
#include <stdint.h>

/* definitions for systems own internal structures */
typedef unsigned int FAT_ITEM_TYPE;
#define FAT_ITEM_TYPE_DIRECTORY 0
#define FAT_ITEM_TYPE_FILE 1

/* FAT16 constants */
#define SIMPOS_FAT16_SIGNATURE 0x29
#define SIMPOS_FAT16_FAT_ENTRY_SIZE 0x02
#define SIMPOS_FAT16_BAD_SECTOR 0xFF7
#define SIMPOS_FAT16_UNUSED 0x00

/* FAT16 directory item attribute bitmasks*/
#define FAT_FILE_READ_ONLY 0x01
#define FAT_FILE_HIDDEN 0x02
#define FAT_FILE_SYSTEM 0x04
#define FAT_FILE_VOLUME_LABEL 0x08
#define FAT_FILE_SUBDIRECTORY 0x10
#define FAT_FILE_ARCHIVED 0x20
#define FAT_FILE_DEVICE 0x40
#define FAT_FILE_RESERVED 0x80

/* structure for the FAT16 header */
struct fat_header
{
    uint8_t short_jmp_ins[3];
    uint8_t oem_identifier[8];
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t fat_copies;
    uint16_t root_dir_entries;
    uint16_t number_of_sectors;
    uint8_t media_type;
    uint16_t sectors_per_fat;
    uint16_t sectors_per_track;
    uint16_t number_of_heads;
    uint32_t hidden_sectors;
    uint32_t sectors_big;

} __attribute__((packed));

/* structure for the FAT16 extended header */
struct fat_header_extended
{
    uint8_t drive_number;
    uint8_t win_nt_bit;
    uint8_t signature;
    uint32_t volume_id;
    uint8_t volume_id_string[11];
    uint8_t system_id_string[8];
} __attribute__((packed));

/* combines the header with the extended header */
struct fat_h
{
    struct fat_header primary_header;
    union fat_h_e
    {
        struct fat_header_extended extended_header;
    } shared;
};

/* structure for a FAT16 file/directory  */
struct fat_directory_item
{
    uint8_t filename[8];
    uint8_t ext[3];
    uint8_t attribute; // use the bitmasks defined below
    uint8_t reserved;
    uint8_t creation_time_tenths_of_a_sec;
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t last_access;
    uint16_t high_16_bits_first_cluster;
    uint16_t last_mod_time;
    uint16_t last_mod_date;
    uint16_t low_16_bits_first_cluster;
    uint32_t file_size;
} __attribute__((packed));

/*
 * systems own internal structures for
 * directory and item handling
 */

struct fat_directory
{
    struct fat_directory_item *item;
    int total;
    int first_sector_pos;
    int last_sector_pos;
};

struct fat_item
{
    /* item is either a file or a directory.
     * union allows us to use single memory address for that item. */

    union
    {
        struct fat_directory_item *item;
        struct fat_directory *directory;
    };

    FAT_ITEM_TYPE type;
};

struct fat_file_descriptor
{
    struct fat_item *item;
    uint32_t pos;
};

/* for storing private data of the filesystem */
struct fat_private
{
    struct fat_h header;
    struct fat_directory root_directory;
    /* streams data clusters */
    struct disk_stream *cluster_read_stream;
    /* streams the file allocation table */
    struct disk_stream *fat_read_stream;
    /* streams the directory */
    struct disk_stream *directory_stream;
};

/* function prototypes */
struct filesystem *
fat16_init();

#endif
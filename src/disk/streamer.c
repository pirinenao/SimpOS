#include "memory/heap/kernel_heap.h"
#include "streamer.h"
#include "config.h"
#include <stdbool.h>

/* creates a new disk streamer */
struct disk_stream *diskstreamer_new(int disk_id)
{
    struct disk *disk = disk_get(disk_id);

    if (!disk)
    {
        return 0;
    }

    struct disk_stream *streamer = kzalloc(sizeof(struct disk_stream));
    streamer->pos = 0;
    streamer->disk = disk;
    return streamer;
}

/* repositions the position in the disk stream */
int diskstreamer_seek(struct disk_stream *stream, int pos)
{
    stream->pos = pos;
    return 0;
}

/* reads x amount of bytes from the disk stream to the out pointer */
int diskstreamer_read(struct disk_stream *stream, void *out, int total)
{
    /* calculates the sector and offset */
    int sector = stream->pos / SIMPOS_SECTOR_SIZE;
    int offset = stream->pos % SIMPOS_SECTOR_SIZE;
    int total_to_read = total;
    bool overflow = (offset + total_to_read) >= SIMPOS_SECTOR_SIZE;
    char buf[SIMPOS_SECTOR_SIZE];

    if (overflow)
    {
        total_to_read -= (offset + total_to_read) - SIMPOS_SECTOR_SIZE;
    }

    /* reads one sector to the buffer */
    int res = disk_read_block(stream->disk, sector, 1, buf);

    if (res < 0)
    {
        return res;
    }

    /* loops through the buffer, and reads to the out */
    for (int i = 0; i < total_to_read; i++)
    {
        *(char *)out++ = buf[offset + i];
    }

    /* adjusts the position after reading */
    stream->pos += total_to_read;

    /* checks if more than a sector to read */
    if (overflow)
    {
        /* read another sector recursively */
        res = diskstreamer_read(stream, out, total - SIMPOS_SECTOR_SIZE);
    }

    return res;
}

/* free the memory allocated for the streamer */
void diskstreamer_close(struct disk_stream *stream)
{
    kfree(stream);
}
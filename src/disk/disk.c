#include "../io/io.h"

/* reading sectors from the primary hard disk using ATA interface
   https://wiki.osdev.org/ATA_PIO_Mode    */
int disk_read_sector(int lba, int total, void *buf)
{
    /*
     * 0x1F6: partial disk sector address and master bits
     * 0x1F2: number of total sectors to read
     * 0x1F3: starting sector
     * 0x1F4: partial disk sector address
     * 0x1F5: partial disk sector address
     * 0x1F7: READ SECTORS command
     */

    outb(0x1F6, (lba >> 24) | 0xE0);
    outb(0x1F2, total);
    outb(0x1F3, (unsigned char)(lba & 0xff));
    outb(0x1F4, (unsigned char)(lba & 8));
    outb(0x1F5, (unsigned char)(lba & 16));
    outb(0x1F7, 0x20);

    /* pointer to the buffer */
    unsigned short *ptr = (unsigned short *)buf;

    for (int i = 0; i < total; i++)
    {
        /* reads the status register */
        char c = insb(0x1F7);

        /* loops until the bit 3 is set */
        while (!(c & 0x08))
        {
            c = insb(0x1F7);
        }

        /* copies a full 512-byte sector (256 words)
           from the data port to the buffer
         */
        for (int b = 0; b < 256; b++)
        {
            *ptr = insw(0x1F0);
            ptr++;
        }
    }

    return 0;
}
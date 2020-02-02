#include <stdio.h>

#include "Utils.h"

// This was copied from https://stackoverflow.com/a/7776146, and modified to output the real address, not just the offset.
void HexDump(uint8_t *mem, uint32_t start, uint32_t len)
{
    uint8_t buff[17];
    uint8_t *pc = mem + start;
    uint32_t i;

    for (i = 0; i < len; i++)
    {
        // Multiple of 16 means new line (with line offset).
        if ((i % 16) == 0)
        {
            // Just don't print ASCII for the zeroth line.
            if (i != 0)
                printf (" %s\n", buff);

            // Output the offset.
            printf ("%04X:", start + i);
        }

        // Now the hex code for the specific character.
        printf (" %02X", pc[i]);

        // And store a printable ASCII character for later.
        if ((pc[i] < 0x20) || (pc[i] > 0x7e))
            buff[i % 16] = '.';
        else
            buff[i % 16] = pc[i];
        buff[(i % 16) + 1] = '\0';
    }

    // Pad out last line if not exactly 16 characters.
    while ((i % 16) != 0) {
        printf ("   ");
        i++;
    }

    // And print the final ASCII bit.
    printf (" %s\n", buff);
}
/*
 * Conway's game of life.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 *
 *
 * tio.c:
 *      Contains functions for setup and management of terminal device streams.
 *
 */

#include <errno.h>
#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "defs.h"
#include "utf8.h"

/* OSX's termios.h doesn't have the following macros */
#if defined(MACOSX)
#define XCASE 0000004
#define OLCUC 0000002
#endif

/* Terminal characteristics */
static struct termios cur_termios;
static struct termios new_termios;


/* Terminal output buffer for type-ahead detection */
#define TOUTBUFSIZE 128
static char toutbuf[TOUTBUFSIZE];

/*
 * tioinit: initalise/setup terminal device streams
 */
void tioinit(void)
{
        /* Save current terminal settings */
        tcgetattr(0, &cur_termios);

        /* The new terminal settings are based off the current ones */
        new_termios = cur_termios;

        /* Input mode flags */
        new_termios.c_iflag &= ~(IGNBRK | BRKINT | IGNPAR| PARMRK |
                                 INPCK | INLCR | IGNCR | ICRNL);

        /* Output mode flags */
        new_termios.c_oflag &= ~(OPOST | ONLCR | OLCUC | OCRNL | ONOCR | ONLRET);

        /* Local mode flags */
        new_termios.c_lflag &= ~(ISIG | ICANON | XCASE | ECHO | ECHOE | ECHOK
                                 | ECHONL | NOFLSH | TOSTOP | ECHOCTL | ECHOPRT |
                                 ECHOKE | FLUSHO | PENDIN | IEXTEN);

        /* Control character settings */
        new_termios.c_cc[VMIN] = 1;
        new_termios.c_cc[VTIME] = 0;

        /* Activate new terminal settings */
        tcsetattr(0, TCSADRAIN, &new_termios);

        /* terminal output buffer for type ahead detection */
        setbuffer(stdout, &toutbuf[0], TOUTBUFSIZE);

        ttrow = 999;
        ttcol = 999;
}

/*
 * tiorestore: restore previous terminal settings
 */
void tiorestore(void)
{
        tcsetattr(0, TCSADRAIN, &cur_termios);
}

/*
 * tiogetc:
 */
int tiogetc(void)
{
        static char buffer[32];
        static int pending;
        unicode_t c;
        int count, bytes = 1, expected;

        count = pending;

        if (!count) {
                count = read(0, buffer, sizeof(buffer));
                if (count <= 0)
                        return 0;
                pending = count;
        }


        c = (unsigned char )buffer[0];
        if (c >= 32 && c < 238)
                goto done;


        /* We need UTF8 sequences that start with 0xeo */
        expected = 2;
        if ((c & 0xe0) == 0xe0)
                expected = 6;

        if (count < expected) {
                int n;
                /* set timeout */
                new_termios.c_cc[VMIN] = 0;
                new_termios.c_cc[VTIME] = 1;
                tcsetattr(0, TCSANOW, &new_termios);

                n = read(0, buffer + count, sizeof(buffer) - count);

                /* unset timeout */
                new_termios.c_cc[VMIN] = 1;
                new_termios.c_cc[VTIME] = 0;
                tcsetattr(0, TCSANOW, &new_termios);

                if (n > 0)
                        pending += n;
        }

        if (pending > 1) {
                unsigned char second = buffer[1];

                /* XXX;  */
        }

        bytes = utf8_to_unicode(buffer, 0, pending, &c);

done:
        pending -= bytes;
        memmove(buffer, buffer+bytes, pending);
        return c;
}

/*
 * tioputc:
 */
int tioputc(int c)
{
        char utf8[6];
        int bytes;

        bytes = unicode_to_utf8(c, utf8);
        fwrite(utf8, 1, bytes, stdout);
        return 0;
}

/*
 * tioflush:
 */
void tioflush(void)
{
        int status;

        status = fflush(stdout);
        while (status < 0 && errno == EAGAIN) {
                sleep(1);
                status = fflush(stdout);
        }

        if (status < 0)
                exit(EXIT_FAILURE); /* XXX: Need a different exit code. */
}

/*
 * tiotypeahead:
 */
int tiotypeahead(void)
{
        int x = 0;

        return x;
}

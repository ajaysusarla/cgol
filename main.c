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
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cgol.h"
#include "func_defs.h"

static void usage(int status)
{
        printf("Usage: %s [options]\n\n", PROG_NAME);
        printf("       ---help     display this help and exit\n");

        exit(status);
}

static void cleanup_and_exit(int signum)
{
        int ret;

        if ((ret = slyesno("Are you sure you want to quit?")) == TRUE) {
                display_tidy();
                exit(EXIT_SUCCESS);
        }

        slwrite("");
}

int main(int argc, char **argv)
{
        if (argc == 2) {
                if (strncmp(argv[1], "--help", 6) == 0) {
                        usage(EXIT_SUCCESS);
                }
        }

        display_init();         /* Initialise display */
        window_init();          /* Initialise window */

        display_clear_screen();

        /* signal handlers */
        signal(SIGHUP, cleanup_and_exit);
        signal(SIGINT, cleanup_and_exit);
        signal(SIGTERM, cleanup_and_exit);
        signal(SIGKILL, cleanup_and_exit);

        for(;;) {
                getcmd();
        }

        exit(EXIT_SUCCESS);

}

void window_init(void)
{
}

void show_hide_grid(void)
{
}

int getcmd(void)
{
        int c;
        c = tiogetc();

        switch(c) {
        case 'q':
        case 'Q':
                cleanup_and_exit(0);
                break;
        case 'g':
        case 'G':
                show_hide_grid();
                break;
        default:
                break;
        }

        return c;
}

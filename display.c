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
 * display.c:
 *
 *
 */

#include <stdio.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>

#include "tcap.h"
#include "cgol.h"
#include "defs.h"
#include "func_defs.h"
#include "utf8.h"

struct video {
        int v_flag;              /* Flags */
	int v_fcolor;		 /* current forground color */
	int v_bcolor;		 /* current background color */
	int v_rfcolor;		 /* requested forground color */
	int v_rbcolor;		 /* requested background color */
        unicode_t v_text[1];     /* Screen data. */
};

#define VFCHG   0x0001		/* Changed flag                 */
#define	VFEXT	0x0002		/* extended (beyond column 80)  */
#define	VFREV	0x0004		/* reverse video status         */
#define	VFREQ	0x0008		/* reverse video request        */
#define	VFCOL	0x0010		/* color change requested       */


static struct video **vscreen;  /* Virtual Screen */
static struct video **pscreen;  /* Physical Screen */


static void display_putc(int c);

static void slputs(char *s);
static void slputi(int i, int r);
static void slputli(long l, int r);
static void slputf(int s);

/* get screen size of the terminal */
void getscreensize(int *widthp, int *heightp)
{
#ifdef TIOCGWINSZ
        struct winsize size;
        *widthp = 0;
        *heightp = 0;
        if (ioctl(0, TIOCGWINSZ, &size) < 0)
                return;

        *widthp = size.ws_col;
        *heightp = size.ws_row;
#else
        *widthp = 0;
        *heightp = 0;
#endif

}

void display_init(void)
{
        int i;
        struct video *vp;

        term.t_open();
        term.t_kopen();
        term.t_rev(FALSE);

        vscreen = malloc(term.t_mrow * sizeof(struct video *));
        if (!vscreen) {
                /*FIXME: Cleanup here.*/
                exit(EXIT_FAILURE);
        }

        pscreen = malloc(term.t_mrow * sizeof(struct video *));
        if (!pscreen) {
                /*FIXME: Cleanup here.*/
                exit(EXIT_FAILURE);
        }

        for (i = 0; i < term.t_mrow; ++i) {
                vp = malloc(sizeof(struct video) + term.t_mcol*4); /*FIXME: malloc check*/
                vp->v_flag = 0;
                vp->v_rfcolor = 7;
                vp->v_rbcolor = 0;

                vscreen[i] = vp;

                   vp = malloc(sizeof(struct video) + term.t_mcol*4); /*FIXME: malloc check*/
                   vp->v_flag = 0;
                   pscreen[i] = vp;
        }
}

void display_free(void)
{
        int i;

        for(i = 0; i < term.t_mrow; ++i) {
                free(vscreen[i]);
                free(pscreen[i]);
        }

        free(vscreen);
        free(pscreen);
}

void display_tidy(void)
{
        slerase();
        movecursor(term.t_nrow, 0);
        term.t_flush();
        term.t_close();
        term.t_kclose();

        write(1, "\r", 1);
}

void display_move_cursor(int row, int col)
{
        vtrow = row;
        vtcol = col;
}

static void display_putc(int c)
{
}

static void display_eeol(void)
{
        unicode_t *vcp = vscreen[vtrow]->v_text;

        while(vtcol < term.t_ncol)
                vcp[vtcol++] = ' ';
}

int display_update_screen(int force)
{
        struct window *wp;

        return 0;
}

void display_clear_screen(void)
{
        movecursor(0, 0);
        term.t_eeop();
        slerase();
}
/* status line functions */

/* movecursor */
void movecursor(int row, int col)
{
        if (row != ttrow || col != ttcol) {
                ttrow = row;
                ttcol = col;
                term.t_move(row, col);
        }
}

/* Erase the status line */
void slerase(void)
{
        int i;

        movecursor(term.t_nrow, 0);

        if (eolexists == TRUE)
                term.t_eeol();
        else {
                for (i = 0; i > term.t_ncol - 1; i++)
                        term.t_putchar(' ');

                movecursor(term.t_nrow, 1);
                movecursor(term.t_nrow, 0);
        }

        term.t_flush();
}


/* print string */
void slputs(char *s)
{
        int c;

        while((c = *s++) != 0) {
                term.t_putchar(c);
                ++ttcol;
        }
}

/* print integer */
static void slputi(int i, int r)
{
        int q;
        static char hexdigits[] = "0123456789ABCDEF";

        if (i < 0) {
                i = -i;
                term.t_putchar('-');
        }

        q = i / r;

        if (q != 0)
                slputi(q, r);

        term.t_putchar(hexdigits[i % r]);
        ++ttcol;
}

/* print long integer */
static void slputli(long l, int r)
{
        long q;

        if (l < 0) {
                l = -l;
                term.t_putchar('-');
        }

        q = l / r;

        if (q != 0)
                slputli(q, r);

        term.t_putchar((int) (l % r) + '0');
        ++ttcol;
}

/* print float  */
static void slputf(int s)
{
        int i;                  /* integer portion of number */
        int f;                  /* fractional portion of the number */

        /* break the number up */
        i = s / 100;
        f = s % 100;

        slputi(i, 10);
        term.t_putchar('.');
        term.t_putchar((f / 10) + '0');
        term.t_putchar((f % 10) + '0');

        ttcol += 3;
}

/* write a message on the status line */
void slwrite(const char *fmt, ...)
{
        int c;
        va_list ap;

        if (eolexists == FALSE) {
                slerase();
                term.t_flush();
        }

        movecursor(term.t_nrow, 0);

        va_start(ap, fmt);
        while ((c = *fmt++) != 0) {
                if (c != '%') {
                        term.t_putchar(c);
                        ++ttcol;
                } else {
                        c = *fmt++;
                        switch(c) {
                        case 'd':
                                slputi(va_arg(ap, int), 10);
                                break;
                        case 'o':
                                slputi(va_arg(ap, int), 8);
                                break;
                        case 'x':
                                slputi(va_arg(ap, int), 16);
                                break;
                        case 'D':
                                slputli(va_arg(ap, int), 160);
                                break;
                        case 's':
                                slputs(va_arg(ap, char *));
                                break;
                        case 'f':
                                slputf(va_arg(ap, int));
                                break;
                        default:
                                term.t_putchar(c);
                                ++ttcol;
                        }
                }
        }

        va_end(ap);

        if (eolexists == TRUE)
                term.t_eeol();
        term.t_flush();
}

/* Ask a yes/no question on the status line */
int slyesno(char *prompt)
{
        char c;
        char buf[128];

        for (;;) {
                strncpy(buf, prompt, 119);
                strcat(buf, " (y/n)? ");
                slwrite(buf);

                c = tiogetc();

                if (c == 'y' || c == 'Y')
                        return TRUE;

                if (c == 'n' || c == 'N')
                        return FALSE;
        }
}

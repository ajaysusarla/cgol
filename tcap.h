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
 * tcap.h:
 *      terminal capabilities
 *
 */

#ifndef __TCAP_H__
#define __TCAP_H__

struct terminal {
        short t_mrow;           /* max number of rows allowable */
        short t_nrow;           /* current number of rows used  */
        short t_mcol;           /* max Number of columns.       */
        short t_ncol;           /* current Number of columns.   */
        short t_margin;         /* min margin for extended lines */
        short t_scrsiz;         /* size of scroll region "      */
        int t_pause;            /* # times thru update to pause */
        void (*t_open)(void);   /* Open terminal at the start.  */
        void (*t_close)(void);  /* Close terminal at end.       */
        void (*t_kopen)(void);  /* Open keyboard                */
        void (*t_kclose)(void); /* close keyboard               */
        int (*t_getchar)(void); /* Get character from keyboard. */
        int (*t_putchar)(int);  /* Put character to display.    */
        void (*t_flush)(void); /* Flush output buffers.        */
        void (*t_move)(int, int);/* Move the cursor, origin 0.   */
        void (*t_eeol)(void);   /* Erase to end of line.        */
        void (*t_eeop)(void);   /* Erase to end of page.        */
        void (*t_beep)(void);   /* Beep.                        */
        void (*t_rev)(int);     /* set reverse video state      */
        int (*t_rez)(char *);   /* change screen resolution     */
        void (*t_setfor)(void);     /* set forground color          */
        void (*t_setback)(void);    /* set background color         */
        void (*t_scroll)(int, int, int); /* scroll a region of the screen */
};

#endif /* __TCAP_H__ */

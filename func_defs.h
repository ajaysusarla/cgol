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

#ifndef __FUNC_DEFS_H__
#define __FUNC_DEFS_H__

/* tio.c */
void tioinit(void);
void tiorestore(void);
int tiogetc(void);
int tioputc(int c);
void tioflush(void);
int tiotypeahead(void);

/* display.c */
void getscreensize(int *widthp, int *heightp);
void display_init(void);
void display_free(void);
void display_tidy(void);
int display_update_screen(int force);
void display_clear_screen(void);
void movecursor(int row, int col);
void slerase(void);
void slwrite(const char *fmt, ...);
int slyesno(char *prompt);

/* main.c */
void window_init(void);
void show_hide_grid(void);
int getcmd(void);
#endif  /* ___FUNC_DEFS_H___ */

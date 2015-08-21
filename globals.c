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
 * globals.c:
 *
 */

#include "cgol.h"

int ttrow = 1000;
int ttcol = 1000;
int vtrow = 0;
int vtcol = 0;
int revexists = FALSE; /* Reverse video */
int eolexists = FALSE; /* End of line */
int sgarbf = TRUE;		/* TRUE if screen is garbage    */

char sres[NBUFN];		/* current screen resolution    */

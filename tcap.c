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
 * tcap.c:
 *      terminal capabilities
 *
 */

#include <curses.h>
#include <stdio.h>
#include <term.h>
#include <stdlib.h>

#include "tcap.h"
#include "cgol.h"
#include "defs.h"
#include "func_defs.h"

#define	MARGIN	8
#define	SCRSIZ	64
#define	NPAUSE	10    /* # times thru update to pause. */
#define BEL     0x07
#define ESC     0x1B

static void tcapkopen(void);
static void tcapkclose(void);
static void tcapmove(int, int);
static void tcapeeol(void);
static void tcapeeop(void);
static void tcapbeep(void);
static void tcaprev(int);
static int tcapcres(char *);
static void tcapscrollregion(int top, int bot);
static void putpad(char *str);
static void tcapopen(void);
static void tcapclose(void);
static void tcapfcol(void);
static void tcapbcol(void);
static void tcapscroll_reg(int from, int to, int linestoscroll);
static void tcapscroll_delins(int from, int to, int linestoscroll);


#define TCAPSLEN 315
static char tcapbuf[TCAPSLEN];
static char *UP, PC, *CM, *CE, *CL, *SO, *SE;

static char *TI, *TE;

static char *CS, *DL, *AL, *SF, *SR;

struct terminal term = {
        0,
        0,
        0,
        0,
        MARGIN,
        SCRSIZ,
        NPAUSE,
        tcapopen,
	tcapclose,
	tcapkopen,
	tcapkclose,
	tiogetc,
	tioputc,
	tioflush,
	tcapmove,
	tcapeeol,
	tcapeeop,
	tcapbeep,
	tcaprev,
	tcapcres,
        tcapfcol,
        tcapbcol,
        NULL
};


static void tcapopen(void)
{
        char *t, *p;
        char tcbuf[1024];
        char *tv_stype;
        char err_str[72];
        int int_col, int_row;


        if ((tv_stype = getenv("TERM")) == NULL) {
                puts("Environment variable TERM not defined!");
                exit(EXIT_FAILURE);
        }

        if ((tgetent(tcbuf, tv_stype)) !=1) {
                sprintf(err_str, "Unknown terminal type %s!", tv_stype);
                puts(err_str);
                exit(EXIT_FAILURE);
        }

        /* Get screen size */
        getscreensize(&int_col, &int_row);
        term.t_nrow = int_row -1;
        term.t_ncol = int_col;

        if ((term.t_nrow <= 0)
            && (term.t_nrow = (short) tgetnum("li") == -1)) {
                puts("termcap entry incomplete (lines)");
                exit(EXIT_FAILURE);
        }

        if ((term.t_ncol <= 0)
            && (term.t_ncol = (short) tgetnum("co") == -1)) {
                puts("termcap entry incomplete (columns)");
                exit(EXIT_FAILURE);
        }

        term.t_mrow = term.t_nrow > MAXROW ? MAXROW : term.t_nrow;
        term.t_mcol = term.t_ncol > MAXCOL ? MAXCOL : term.t_ncol;

        p = tcapbuf;
        t = tgetstr("pc", &p);
        if (t)
                PC = *t;
        else
                PC = 0;

        CL = tgetstr("cl", &p);
        CM = tgetstr("cm", &p);
        CE = tgetstr("ce", &p);
        if (CE != NULL)
                eolexists = TRUE;
        UP = tgetstr("up", &p);
        SE = tgetstr("se", &p);
        SO = tgetstr("so", &p);
        if (SO != NULL)
                revexists = TRUE;


        tioinit();
}


static void tcapclose(void)
{
	putpad(tgoto(CM, 0, term.t_nrow));
	putpad(TE);
	tioflush();
	tiorestore();
}


static void tcapkopen(void)
{
	putpad(TI);
	tioflush();
	ttrow = 999;
	ttcol = 999;
	sgarbf = TRUE;
}

static void tcapkclose(void)
{
	putpad(TE);
	tioflush();
}

static void tcapmove(int row, int col)
{
	putpad(tgoto(CM, col, row));
}

static void tcapeeol(void)
{
	putpad(CE);
}

static void tcapeeop(void)
{
	putpad(CL);
}

static void tcaprev(int state)
{
	if (state) {
		if (SO != NULL)
			putpad(SO);
	} else if (SE != NULL)
		putpad(SE);
}

static int tcapcres(char *res)
{
	return TRUE;
}

static void tcapscroll_reg(int from, int to, int numlines)
{
	int i;
	if (to == from)
		return;
	if (to < from) {
		tcapscrollregion(to, from + numlines - 1);
		tcapmove(from + numlines - 1, 0);
		for (i = from - to; i > 0; i--)
			putpad(SF);
	} else {		/* from < to */
		tcapscrollregion(from, to + numlines - 1);
		tcapmove(from, 0);
		for (i = to - from; i > 0; i--)
			putpad(SR);
	}
	tcapscrollregion(0, term.t_nrow);
}

static void tcapscroll_delins(int from, int to, int numlines)
{
	int i;
	if (to == from)
		return;
	if (to < from) {
		tcapmove(to, 0);
		for (i = from - to; i > 0; i--)
			putpad(DL);
		tcapmove(to + numlines, 0);
		for (i = from - to; i > 0; i--)
			putpad(AL);
	} else {
		tcapmove(from + numlines, 0);
		for (i = to - from; i > 0; i--)
			putpad(DL);
		tcapmove(from, 0);
		for (i = to - from; i > 0; i--)
			putpad(AL);
	}
}

static void tcapscrollregion(int top, int bot)
{
	tioputc(PC);
	putpad(tgoto(CS, bot, top));
}


static void tcapfcol(void)
{
}

static void tcapbcol(void)
{
}

static void tcapbeep(void)
{
	tioputc(BEL);
}

static void putpad(char *str)
{
	tputs(str, 1, tioputc);
}

/****************************************************************************
 *                                                                          *
 *                         GNAT COMPILER COMPONENTS                         *
 *                                                                          *
 *                                  C I O                                   *
 *                                                                          *
 *                          C Implementation File                           *
 *                                                                          *
 *          Copyright (C) 1992-2018, Free Software Foundation, Inc.         *
 *                                                                          *
 * GNAT is free software;  you can  redistribute it  and/or modify it under *
 * terms of the  GNU General Public License as published  by the Free Soft- *
 * ware  Foundation;  either version 3,  or (at your option) any later ver- *
 * sion.  GNAT is distributed in the hope that it will be useful, but WITH- *
 * OUT ANY WARRANTY;  without even the  implied warranty of MERCHANTABILITY *
 * or FITNESS FOR A PARTICULAR PURPOSE.                                     *
 *                                                                          *
 * As a special exception under Section 7 of GPL version 3, you are granted *
 * additional permissions described in the GCC Runtime Library Exception,   *
 * version 3.1, as published by the Free Software Foundation.               *
 *                                                                          *
 * You should have received a copy of the GNU General Public License and    *
 * a copy of the GCC Runtime Library Exception along with this program;     *
 * see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see    *
 * <http://www.gnu.org/licenses/>.                                          *
 *                                                                          *
 * GNAT was originally developed  by the GNAT team at  New York University. *
 * Extensive contributions were provided by Ada Core Technologies Inc.      *
 *                                                                          *
 ****************************************************************************/

#include <sys/stat.h>
#include <stdio.h>
#include "adaint.h"
#include "Winrt.h"

#ifdef __cplusplus
extern "C" {
#endif
    
int get_char (void)
{
  return fgetc (stdin);
}

void put_char(int c)
{
    fputc(c, stdout);
}

void put_char_stderr(int c)
{
    fputc(c, stderr);
}

int get_int (void)
{
  int x;
  WINRT_fscanf_s (stdin, " %d", &x);
  return x;
}

void put_int (int x)
{
    WINRT_fprintf_s(stdout, "%d", x);
}

void put_int_stderr (int x)
{
    WINRT_fprintf_s(stderr, "%d", x);
}

#ifdef __cplusplus
}
#endif

/****************************************************************************
 *                                                                          *
 *                          GNAT RUN-TIME COMPONENTS                        *
 *                                                                          *
 *                              C S T R E A M S                             *
 *                                                                          *
 *              Auxiliary C functions for Interfaces.C.Streams              *
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

/* Routines required for implementing routines in Interfaces.C.Streams.  */

#define _LARGEFILE_SOURCE
#define _FILE_OFFSET_BITS 64

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/stat.h>

#include "adaint.h"

#ifdef __cplusplus
extern "C" {
#endif

int
__gnat_feof (FILE *stream)
{
  return (feof (stream));
}

int
__gnat_ferror (FILE *stream)
{
   return (ferror (stream));
}

int
__gnat_fileno (FILE *stream)
{
   return (_fileno (stream));
}

/* on some systems, the constants for seek are not defined, if so, then
   provide the conventional definitions */

#ifndef SEEK_SET
#define SEEK_SET 0  /* Set file pointer to offset                           */
#define SEEK_CUR 1  /* Set file pointer to its current value plus offset    */
#define SEEK_END 2  /* Set file pointer to the size of the file plus offset */
#endif

/* if L_tmpnam is not set, use a large number that should be safe */
#ifndef L_tmpnam
#define L_tmpnam 256
#endif

int    __gnat_constant_eof      = EOF;
int    __gnat_constant_iofbf    = _IOFBF;
int    __gnat_constant_iolbf    = _IOLBF;
int    __gnat_constant_ionbf    = _IONBF;
int    __gnat_constant_l_tmpnam = L_tmpnam;
int    __gnat_constant_seek_cur = SEEK_CUR;
int    __gnat_constant_seek_end = SEEK_END;
int    __gnat_constant_seek_set = SEEK_SET;

FILE *
__gnat_constant_stderr (void)
{
  return stderr;
}

FILE *
__gnat_constant_stdin (void)
{
  return stdin;
}

FILE *
__gnat_constant_stdout (void)
{
  return stdout;
}

char *
__gnat_full_name (char *nam, char *buffer)
{
  if (nam[strlen (nam) - 1] == ':')
    strcpy (buffer, nam);
  else
    {
      char *p;

      _fullpath (buffer, nam, __gnat_max_path_len);

      for (p = buffer; *p; p++)
	if (*p == '/')
	  *p = '\\';
    }
  return buffer;
}

__int64
__gnat_ftell64 (FILE *stream)
{
  return _ftelli64 (stream);
}

int
__gnat_fseek64 (FILE *stream, __int64 offset, int origin)
{
  return _fseeki64 (stream, offset, origin);
}

int
__gnat_is_fifo (const char* path)
{
  struct stat buf;
  const int status = stat(path, &buf);
  if (status == 0)
    return S_ISFIFO(buf.st_mode);
}

#ifdef __cplusplus
}
#endif

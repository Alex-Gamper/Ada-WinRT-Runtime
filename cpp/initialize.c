/****************************************************************************
 *                                                                          *
 *                         GNAT COMPILER COMPONENTS                         *
 *                                                                          *
 *                           I N I T I A L I Z E                            *
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

/*  This unit provides default implementation for __gnat_initialize ()
    which is called before the elaboration of the partition. It is provided
    in a separate file/object so that users can replace it easily.
    The default implementation should be null on most targets.  */

#define xmalloc(S) malloc (S)
#define xrealloc(V,S) realloc (V,S)

#include "raise.h"
#include <fcntl.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************/
/* __gnat_initialize (NT-mingw32 Version) */
/******************************************/

extern void __gnat_install_SEH_handler (void *);

void
__gnat_initialize (void *eh)
{
   /* Note that we do not activate this for the compiler itself to avoid a
      bootstrap path problem.  Older version of gnatbind will generate a call
      to __gnat_initialize() without argument. Therefore we cannot use eh in
      this case.  It will be possible to remove the following #ifdef at some
      point.  */
   if (eh)
     __gnat_install_SEH_handler (eh);
}

/******************************************/
/* __gnat_initialize (init_float version) */
/******************************************/

#ifdef __cplusplus
}
#endif

/****************************************************************************
 *                                                                          *
 *                         GNAT COMPILER COMPONENTS                         *
 *                                                                          *
 *                              R T F I N A L                               *
 *                                                                          *
 *                          C Implementation File                           *
 *                                                                          *
 *            Copyright (C) 2014-2018, Free Software Foundation, Inc.       *
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

#ifdef __cplusplus
extern "C" {
#endif

extern void __gnat_runtime_finalize (void);

/* This routine is called at the extreme end of execution of an Ada program
   (the call is generated by the binder). The standard routine does nothing
   at all, the intention is that this be replaced by system specific code
   where finalization is required.

   Note that __gnat_runtime_finalize() is called in adafinal()   */

extern int __gnat_rt_init_count;
/*  see initialize.c  */

#include "mingw32.h"
#include <windows.h>

extern CRITICAL_SECTION ProcListCS;
extern HANDLE ProcListEvt;

void
__gnat_runtime_finalize (void)
{
  /*  decrement the reference counter */

  __gnat_rt_init_count--;

  /*  if still some referenced return now */
  if (__gnat_rt_init_count > 0)
    return;

  /* delete critical section and event handle used for the
     processes chain list */
  DeleteCriticalSection(&ProcListCS);
  CloseHandle (ProcListEvt);
}

#ifdef __cplusplus
}
#endif

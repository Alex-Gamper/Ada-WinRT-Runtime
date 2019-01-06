/****************************************************************************
 *                                                                          *
 *                         GNAT RUN-TIME COMPONENTS                         *
 *                                                                          *
 *                               E X P E C T                                *
 *                                                                          *
 *                          C Implementation File                           *
 *                                                                          *
 *                     Copyright (C) 2001-2018, AdaCore                     *
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

#define POSIX

#include <sys/types.h>

/* This file provides the low level functionalities needed to implement Expect
   capabilities in GNAT.Expect.
   Implementations for unix and windows systems is provided.
   Dummy stubs are also provided for other systems. */

#include <windows.h>
#include <process.h>
#include <signal.h>
#include <io.h>
#include "mingw32.h"

extern int	__gnat_win32_remove_handle(HANDLE h, int pid);
extern int	__gnat_portable_no_block_spawn(char *[]);

int
__gnat_waitpid (int pid)
{
  HANDLE h = OpenProcess (PROCESS_ALL_ACCESS, FALSE, pid);
  DWORD exitcode = 1;
  DWORD res;

  if (h != NULL)
    {
      res = WaitForSingleObject (h, INFINITE);
      GetExitCodeProcess (h, &exitcode);
      CloseHandle (h);
    }

  __gnat_win32_remove_handle (NULL, pid);
  return (int) exitcode;
}

int
__gnat_expect_fork (void)
{
  return 0;
}

void
__gnat_expect_portable_execvp (int *pid, char *cmd, char *argv[])
{
  *pid = __gnat_portable_no_block_spawn (argv);
}

int
__gnat_pipe (int *fd)
{
  HANDLE read, write;

  CreatePipe (&read, &write, NULL, 0);
  fd[0]=_open_osfhandle ((intptr_t)read, 0);
  fd[1]=_open_osfhandle ((intptr_t)write, 0);
  return 0;  /* always success */
}

int
__gnat_expect_poll (int *fd,
                    int num_fd,
                    int timeout,
                    int *dead_process,
                    int *is_set)
{
#define MAX_DELAY 100

  int i, delay, infinite = 0;
  DWORD avail;
  HANDLE handles[num_fd];

  *dead_process = 0;

  for (i = 0; i < num_fd; i++)
    is_set[i] = 0;

  for (i = 0; i < num_fd; i++)
    handles[i] = (HANDLE) _get_osfhandle (fd [i]);

  /* Start with small delays, and then increase them, to avoid polling too
     much when waiting a long time */
  delay = 5;

  if (timeout < 0)
    infinite = 1;

  while (1)
    {
      for (i = 0; i < num_fd; i++)
        {
          if (!PeekNamedPipe (handles [i], NULL, 0, NULL, &avail, NULL))
            {
              *dead_process = i + 1;
              return -1;
            }
          if (avail > 0)
            {
              is_set[i] = 1;
              return 1;
            }
        }

      if (!infinite && timeout <= 0)
        return 0;

      Sleep (delay);
      timeout -= delay;

      if (delay < MAX_DELAY)
        delay += 10;
    }
}

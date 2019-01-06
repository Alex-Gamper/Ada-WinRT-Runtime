/****************************************************************************
 *                                                                          *
 *                         GNAT COMPILER COMPONENTS                         *
 *                                                                          *
 *                                  E N V                                   *
 *                                                                          *
 *                          C Implementation File                           *
 *                                                                          *
 *            Copyright (C) 2005-2018, Free Software Foundation, Inc.       *
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

# include <sys/stat.h>
# include <fcntl.h>
# include <time.h>
# define xmalloc(S) malloc (S)

#include <stdlib.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "env.h"

void
__gnat_getenv (char *name, int *len, char **value)
{
  *value = getenv (name);
  if (!*value)
    *len = 0;
  else
    *len = strlen (*value);

  return;
}

#include <Windows.h>

void
__gnat_setenv (char *name, char *value)
{
  size_t size = strlen (name) + strlen (value) + 2;
  char *expression;

  expression = (char *) xmalloc (size * sizeof (char));

  //sprintf (expression, "%s=%s", name, value);
  {
	  LPCSTR	Source = "%1=%2";;
	  DWORD_PTR	Arguments[] = { (DWORD_PTR) name , (DWORD_PTR) value };
	  DWORD Ok = FormatMessageA(FORMAT_MESSAGE_ARGUMENT_ARRAY | FORMAT_MESSAGE_FROM_STRING, (LPCVOID)Source, 0, 0, (LPSTR) expression, size, (va_list* )Arguments);
  }
  _putenv (expression);
  free (expression);
}

char*** __p__environ();

char **
__gnat_environ (void)
{
	return *__p__environ();
}

void __gnat_unsetenv (char *name)
{
  size_t size = strlen (name) + 2;
  char *expression;
  expression = (char *) xmalloc (size * sizeof (char));

  //sprintf (expression, "%s=", name);
  {
	  LPCSTR	Source = "%1=";;
	  DWORD_PTR	Arguments[] = { (DWORD_PTR) name };
	  DWORD Ok = FormatMessageA(FORMAT_MESSAGE_ARGUMENT_ARRAY | FORMAT_MESSAGE_FROM_STRING, (LPCVOID)Source, 0, 0, (LPSTR)expression, size, (va_list*)Arguments);
  }
  _putenv (expression);
  free (expression);
}

void __gnat_clearenv (void)
{
  /* On Windows, FreeBSD and MacOS there is no function to clean all the
     environment but there is a "clean" way to unset a variable. So go
     through the environ table and call __gnat_unsetenv on all entries */
  char **env = __gnat_environ ();
  size_t size;

  while (env[0] != NULL) {
    size = 0;
    while (env[0][size] != '=')
      size++;
    /* create a string that contains "name" */
    size++;
    {
      char *expression;
      expression = (char *) xmalloc (size * sizeof (char));
      strncpy (expression, env[0], size);
      expression[size - 1] = 0;
      __gnat_unsetenv (expression);
      free (expression);
    }
  }
}

#ifdef __cplusplus
}
#endif

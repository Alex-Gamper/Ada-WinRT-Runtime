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

#define MINGW_HAS_SECURE_API 1

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
#include <Windows.h>

void
__gnat_getenv (char *name, int *len, char **value)
{
    DWORD MaxSize = 2048;
    *value = xmalloc(2048);
    DWORD ActualSize = GetEnvironmentVariableA(name, *value, MaxSize);
    *len = ActualSize;
    return;
}

void
__gnat_setenv (char *name, char *value)
{
    SetEnvironmentVariableA(name, value);
}

char **
__gnat_environ (void)
{
    char** p_Environ = NULL;
    char*  p_Env = GetEnvironmentStrings();

    p_Environ = (char**)calloc(1, sizeof(void*));

    int i = 0;
    int j = 0;
    int k = 1;

    while (!((p_Env[i] == '\0') & (p_Env[i + 1] == '\0')))
    {
        if (p_Env[i] == '\0')
        {
            p_Environ = (char**)realloc(p_Environ, sizeof(void*) * (k + 1));
            p_Environ[k - 1] = (char*)calloc(1, strlen(&p_Env[j]) + 1);
            strncpy_s(p_Environ[k - 1], strlen(&p_Env[j]) + 1, &p_Env[j], strlen(&p_Env[j]));
            j = i + 1;
            ++k;
        }
        ++i;
    }
    p_Environ[k - 1] = NULL;

    BOOL Ok = FreeEnvironmentStringsA(p_Env);
    return p_Environ;
}

void __gnat_unsetenv (char *name)
{
    SetEnvironmentVariableA(name, NULL);
}

void __gnat_clearenv (void)
{
    char **env = __gnat_environ();

    if (env != NULL)
    {
        for (int i = 1; *(env + i); ++i)
        {
            char* NameValue = *(env + i);

            size_t size = 0;
            while (NameValue[size] != '=')
            {
                ++size;
            }
            ++size; // cater for null terminator;
            char *expression;
            expression = (char *)malloc(size * sizeof(char));
            strncpy_s(expression, size, NameValue, size-1);
            expression[size - 1] = 0;
            __gnat_unsetenv(expression);
            free(expression);
        }
    }
}

#ifdef __cplusplus
}
#endif

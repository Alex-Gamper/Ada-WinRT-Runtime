/****************************************************************************
 *                                                                          *
 *                         GNAT COMPILER COMPONENTS                         *
 *                                                                          *
 *                                S Y S D E P                               *
 *                                                                          *
 *                          C Implementation File                           *
 *                                                                          *
 *         Copyright (C) 1992-2018, Free Software Foundation, Inc.          *
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

#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>

#include "adaint.h"

/*
   Notes:

   (1) Opening a file with read mode fails if the file does not exist or
   cannot be read.

   (2) Opening a file with append mode causes all subsequent writes to the
   file to be forced to the then current end-of-file, regardless of
   intervening calls to the fseek function.

   (3) When a file is opened with update mode, both input and output may be
   performed on the associated stream.  However, output may not be directly
   followed by input without an intervening call to the fflush function or
   to a file positioning function (fseek, fsetpos, or rewind), and input
   may not be directly followed by output without an intervening call to a
   file positioning function, unless the input operation encounters
   end-of-file.

   The other target dependent declarations here are for the three functions
   __gnat_set_binary_mode, __gnat_set_text_mode and __gnat_set_mode:

      void __gnat_set_binary_mode (int handle);
      void __gnat_set_text_mode   (int handle);
      void __gnat_set_mode        (int handle, int mode);

   These functions have no effect in Unix (or similar systems where there is
   no distinction between binary and text files), but in DOS (and similar
   systems where text mode does CR/LF translation), these functions allow
   the mode of the stream with the given handle (fileno can be used to get
   the handle of a stream) to be changed dynamically. The returned result
   is 0 if no error occurs and -1 if an error occurs.

   Finally there is a boolean (character) variable

      char __gnat_text_translation_required;

   which is zero (false) in Unix mode, and one (true) in DOS mode, with a
   true value indicating that text translation is required on text files
   and that fopen supports the trailing t and b modifiers.

*/

const char __gnat_text_translation_required = 1;

#define WIN_SETMODE _setmode

void
__gnat_set_binary_mode (int handle)
{
  WIN_SETMODE (handle, O_BINARY);
}

void
__gnat_set_text_mode (int handle)
{
  WIN_SETMODE (handle, O_TEXT);
}

void
__gnat_set_mode (int handle, int mode)
{
  /*  the values here must be synchronized with
      System.File_Control_Block.Content_Encodding:

      None         = 0
      Default_Text = 1
      Text         = 2
      U8text       = 3
      Wtext        = 4
      U16text      = 5  */

 switch (mode) {
    case 0 : WIN_SETMODE (handle, _O_BINARY);          break;
    case 1 : WIN_SETMODE (handle, __gnat_current_ccs_encoding); break;
    case 2 : WIN_SETMODE (handle, _O_TEXT);            break;
    case 3 : WIN_SETMODE (handle, _O_U8TEXT);          break;
    case 4 : WIN_SETMODE (handle, _O_WTEXT);           break;
    case 5 : WIN_SETMODE (handle, _O_U16TEXT);         break;
 }
}

#include <windows.h>

int __gnat_is_windows_xp (void);

int
__gnat_is_windows_xp (void)
{
  static int is_win_xp=0, is_win_xp_checked=0;

  if (!is_win_xp_checked)
    {
      OSVERSIONINFO version;

      is_win_xp_checked = 1;

      memset (&version, 0, sizeof (version));
      version.dwOSVersionInfoSize = sizeof (version);

      is_win_xp = GetVersionEx (&version)
        && version.dwPlatformId == VER_PLATFORM_WIN32_NT
        && (version.dwMajorVersion > 5
            || (version.dwMajorVersion == 5 && version.dwMinorVersion >= 1));
    }
  return is_win_xp;
}

/* Get the bounds of the stack.  The stack pointer is supposed to be
   initialized to BASE when a thread is created and the stack can be extended
   to LIMIT before reaching a guard page.
   Note: for the main thread, the system automatically extend the stack, so
   LIMIT is only the current limit.  */

void
__gnat_get_stack_bounds (void **base, void **limit)
{
  NT_TIB *tib;

  /* We know that the first field of the TEB is the TIB.  */
  tib = (NT_TIB *)NtCurrentTeb ();

  *base = tib->StackBase;
  *limit = tib->StackLimit;
}


/* Return the name of the tty.   Under windows there is no name for
   the tty, so this function, if connected to a tty, returns the generic name
   "console".  */

char *
__gnat_ttyname (int filedes)
{
  if (_isatty (filedes))
    return "console";
  else
    return NULL;
}

#include <conio.h>  /* for getch(), kbhit() */

/* Implements the common processing for getc_immediate and
   getc_immediate_nowait. */

extern void getc_immediate (FILE *, int *, int *);
extern void getc_immediate_nowait (FILE *, int *, int *, int *);
extern void getc_immediate_common (FILE *, int *, int *, int *, int);

/* Called by Get_Immediate (Foo); */

void
getc_immediate (FILE *stream, int *ch, int *end_of_file)
{
  int avail;

  getc_immediate_common (stream, ch, end_of_file, &avail, 1);
}

/* Called by Get_Immediate (Foo, Available); */

void
getc_immediate_nowait (FILE *stream, int *ch, int *end_of_file, int *avail)
{
  getc_immediate_common (stream, ch, end_of_file, avail, 0);
}

/* Called by getc_immediate () and getc_immediate_nowait () */

void
getc_immediate_common (FILE *stream,
                       int *ch,
                       int *end_of_file,
                       int *avail,
                       int waiting)
{
  int fd = _fileno (stream);
  int char_waiting;
  int eot_ch = 4; /* Ctrl-D */

  if (_isatty (fd))
    {
      if (waiting)
	{
	  *ch = _getch ();

	  if (*ch == eot_ch)
	    *end_of_file = 1;
	  else
	    *end_of_file = 0;

	  *avail = 1;
	}
      else /* ! waiting */
	{
	  char_waiting = _kbhit();

	  if (char_waiting == 1)
	    {
	      *avail = 1;
	      *ch = _getch ();

	      if (*ch == eot_ch)
		*end_of_file = 1;
	      else
		*end_of_file = 0;
	    }
	  else
	    {
	      *avail = 0;
	      *end_of_file = 0;
	    }
	}
    }
  else
    {
      /* If we're not on a terminal, then we don't need any fancy processing.
	 Also this is the only thing that's left if we're not on one of the
	 supported systems; which means that for non supported systems,
         get_immediate may wait for a carriage return on terminals. */
      *ch = fgetc (stream);
      if (feof (stream))
        {
          *end_of_file = 1;
          *avail = 0;
        }
      else
        {
          *end_of_file = 0;
          *avail = 1;
        }
    }
}

/* The following definitions are provided in NT to support Windows based
   Ada programs.  */

#include <windows.h>

/* Provide functions to echo the values passed to WinMain (windows bindings
   will want to import these).  We use the same names as the routines used
   by AdaMagic for compatibility.  */

char *rts_get_hInstance (void);
char *rts_get_hPrevInstance (void);
char *rts_get_lpCommandLine (void);
int   rts_get_nShowCmd (void);

char *
rts_get_hInstance (void)
{
  return (char *)GetModuleHandleA (0);
}

char *
rts_get_hPrevInstance (void)
{
  return 0;
}

char *
rts_get_lpCommandLine (void)
{
  return GetCommandLineA ();
}

int
rts_get_nShowCmd (void)
{
  return 1;
}

/* This value is returned as the time zone offset when a valid value
   cannot be determined. It is simply a bizarre value that will never
   occur. It is 3 days plus 73 seconds (offset is in seconds). */

long __gnat_invalid_tzoff = 259273;

/* Definition of __gnat_localtime_r used by a-calend.adb */

extern void
__gnat_localtime_tzoff (const time_t *, const int *, long *);

static const unsigned long long w32_epoch_offset = 11644473600ULL;
void
__gnat_localtime_tzoff (const time_t *timer, const int *is_historic, long *off)
{
  TIME_ZONE_INFORMATION tzi;

  DWORD tzi_status;

  tzi_status = GetTimeZoneInformation (&tzi);

  /* Cases where we simply want to extract the offset of the current time
     zone, regardless of the date. A value of "0" for flag "is_historic"
     signifies that the date is NOT historic, see the
     body of Ada.Calendar.UTC_Time_Offset. */

  if (*is_historic == 0) {
    *off = tzi.Bias;

    /* The system is operating in the range covered by the StandardDate
       member. */
    if (tzi_status == TIME_ZONE_ID_STANDARD) {
       *off = *off + tzi.StandardBias;
    }

    /* The system is operating in the range covered by the DaylightDate
       member. */
    else if (tzi_status == TIME_ZONE_ID_DAYLIGHT) {
       *off = *off + tzi.DaylightBias;
    }

    *off = *off * -60;
  }

  /* Time zone offset calculations for a historic or future date */

  else {
    union
    {
      FILETIME ft_time;
      unsigned long long ull_time;
    } utc_time, local_time;

    SYSTEMTIME utc_sys_time, local_sys_time;
    BOOL status;

    /* First convert unix time_t structure to windows FILETIME format.  */
    utc_time.ull_time = ((unsigned long long) *timer + w32_epoch_offset)
                        * 10000000ULL;

    /* If GetTimeZoneInformation does not return a value between 0 and 2 then
       it means that we were not able to retrieve timezone information. Note
       that we cannot use here FileTimeToLocalFileTime as Windows will use in
       always in this case the current timezone setting. As suggested on MSDN
       we use the following three system calls to get the right information.
       Note also that starting with Windows Vista new functions are provided
       to get timezone settings that depend on the year. We cannot use them as
       we still support Windows XP and Windows 2003.  */

    status = (tzi_status >= 0 && tzi_status <= 2)
      && FileTimeToSystemTime (&utc_time.ft_time, &utc_sys_time)
      && SystemTimeToTzSpecificLocalTime (&tzi, &utc_sys_time, &local_sys_time)
      && SystemTimeToFileTime (&local_sys_time, &local_time.ft_time);

    /* An error has occurred, return invalid_tzoff */

    if (!status) {
      *off = __gnat_invalid_tzoff;
    }
    else {
      if (local_time.ull_time > utc_time.ull_time) {
        *off = (long) ((local_time.ull_time - utc_time.ull_time)
               / 10000000ULL);
      }
      else {
        *off = - (long) ((utc_time.ull_time - local_time.ull_time)
               / 10000000ULL);
      }
    }
  }
}

int
__gnat_is_file_not_found_error (int errno_val) {
   switch (errno_val) {
      case ENOENT:
         return 1;

      default:
        return 0;
   }
}

int
__gnat_name_case_equivalence ()
{
  /*  the values here must be synchronized with Ada.Directories.Name_Case_Kind:

      Unknown          = 0
      Case_Sensitive   = 1
      Case_Insensitive = 2
      Case_Preserving  = 3  */

  return 3;
}


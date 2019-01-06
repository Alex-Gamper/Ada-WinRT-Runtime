/****************************************************************************
 *                                                                          *
 *                         GNAT RUN-TIME COMPONENTS                         *
 *                                                                          *
 *                            T E R M I N A L S                             *
 *                                                                          *
 *                          C Implementation File                           *
 *                                                                          *
 *                     Copyright (C) 2008-2018, AdaCore                     *
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

#define ATTRIBUTE_UNUSED __attribute__((unused))

/* First all usupported platforms. Add stubs for exported routines. */

#if defined (VMS) || defined (__vxworks) || defined (__Lynx__) || defined (__ANDROID__) || defined (__PikeOS__) || defined(__DJGPP__) || defined(_WIN32)

void *
__gnat_new_tty (void)
{
  return (void*)0;
}

char *
__gnat_tty_name (void* t ATTRIBUTE_UNUSED)
{
  return (char*)0;
}

int
__gnat_interrupt_pid (int pid ATTRIBUTE_UNUSED)
{
  return -1;
}

int
__gnat_interrupt_process (void* desc ATTRIBUTE_UNUSED)
{
  return -1;
}

int
__gnat_setup_communication (void** desc ATTRIBUTE_UNUSED)
{
  return -1;
}

void
__gnat_setup_parent_communication (void *d ATTRIBUTE_UNUSED,
				   int *i ATTRIBUTE_UNUSED,
				   int *o ATTRIBUTE_UNUSED,
				   int *e ATTRIBUTE_UNUSED,
				   int *p ATTRIBUTE_UNUSED)
{
}

int
__gnat_setup_child_communication (void *d ATTRIBUTE_UNUSED,
				  char **n ATTRIBUTE_UNUSED,
				  int u ATTRIBUTE_UNUSED)
{
  return -1;
}

int
__gnat_terminate_process (void *desc ATTRIBUTE_UNUSED)
{
  return -1;
}

int
__gnat_terminate_pid (int pid ATTRIBUTE_UNUSED)
{
  return -1;
}

int
__gnat_tty_fd (void* t ATTRIBUTE_UNUSED)
{
  return -1;
}

int
__gnat_tty_supported (void)
{
  return 0;
}

int
__gnat_tty_waitpid (void *desc ATTRIBUTE_UNUSED)
{
  return 1;
}

void
__gnat_close_tty (void* t ATTRIBUTE_UNUSED)
{
}

void
__gnat_free_process (void** process ATTRIBUTE_UNUSED)
{
}

void
__gnat_reset_tty (void* t ATTRIBUTE_UNUSED)
{
}

void
__gnat_send_header (void* d ATTRIBUTE_UNUSED,
		    char h[5] ATTRIBUTE_UNUSED,
		    int s ATTRIBUTE_UNUSED,
		    int *r ATTRIBUTE_UNUSED)
{
}

void
__gnat_setup_winsize (void *desc ATTRIBUTE_UNUSED,
		      int rows ATTRIBUTE_UNUSED,
		      int columns ATTRIBUTE_UNUSED)
{
}

/* For Windows platforms. */

#elif defined(_WIN32_REMOVED)

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define MAXPATHLEN 1024

#define NILP(x) ((x) == 0)
#define Qnil 0
#define report_file_error(x, y) fprintf (stderr, "Error: %s\n", x);
#define INTEGERP(x) 1
#define XINT(x) x

struct TTY_Process {
  int pid;           /* Number of this process */
  PROCESS_INFORMATION procinfo;
  HANDLE w_infd, w_outfd;
  HANDLE w_forkin, w_forkout;
  BOOL usePipe;
};

/* Control whether create_child cause the process to inherit GPS'
   error mode setting.  The default is 1, to minimize the possibility of
   subprocesses blocking when accessing unmounted drives.  */
static int Vw32_start_process_inherit_error_mode = 1;

/* Control whether spawnve quotes arguments as necessary to ensure
   correct parsing by child process.  Because not all uses of spawnve
   are careful about constructing argv arrays, we make this behavior
   conditional (off by default, since a similar operation is already done
   in g-expect.adb by calling Normalize_Argument). */
static int Vw32_quote_process_args = 0;

static DWORD AbsoluteSeek(HANDLE, DWORD);
static VOID  ReadBytes(HANDLE, LPVOID, DWORD);

#define XFER_BUFFER_SIZE 2048

/* This tell if the executable we're about to launch uses a GUI interface. */
/* if we can't determine it, we will return true */
static int
is_gui_app (char *exe)
{
#if 0
  HANDLE hImage;

  DWORD  bytes;
  DWORD  iSection;
  DWORD  SectionOffset;
  DWORD  CoffHeaderOffset;
  DWORD  MoreDosHeader[16];
  CHAR   *file;
  size_t nlen;

  ULONG  ntSignature;

  IMAGE_DOS_HEADER      image_dos_header;
  IMAGE_FILE_HEADER     image_file_header;
  IMAGE_OPTIONAL_HEADER image_optional_header;
  IMAGE_SECTION_HEADER  image_section_header;

  /*
   *  Open the reference file.
  */
  nlen = strlen (exe);
  file = exe;
  if (nlen > 2) {
    if (exe[0] == '"') {
      /* remove quotes */
      nlen -= 2;
      file = malloc ((nlen + 1) * sizeof (char));
      memcpy (file, &exe[1], nlen);
      file [nlen] = '\0';
    }
  }
  hImage = CreateFile(file,
                      GENERIC_READ,
                      FILE_SHARE_READ,
                      NULL,
                      OPEN_EXISTING,
                      FILE_ATTRIBUTE_NORMAL,
                      NULL);

  if (file != exe) {
    free (file);
  }

  if (INVALID_HANDLE_VALUE == hImage)
    {
      report_file_error ("Could not open exe: ", Qnil);
      report_file_error (exe, Qnil);
      report_file_error ("\n", Qnil);
      CloseHandle (hImage);
      return -1;
    }

  /*
   *  Read the MS-DOS image header.
   */
  ReadBytes(hImage, &image_dos_header, sizeof(IMAGE_DOS_HEADER));

  if (IMAGE_DOS_SIGNATURE != image_dos_header.e_magic)
    {
      report_file_error("Sorry, I do not understand this file.\n", Qnil);
      CloseHandle (hImage);
      return -1;
    }

  /*
   *  Read more MS-DOS header.       */
  ReadBytes(hImage, MoreDosHeader, sizeof(MoreDosHeader));
   /*
   *  Get actual COFF header.
   */
  CoffHeaderOffset = AbsoluteSeek(hImage, image_dos_header.e_lfanew) +
                     sizeof(ULONG);
  if (CoffHeaderOffset < 0) {
    CloseHandle (hImage);
    return -1;
  }

  ReadBytes (hImage, &ntSignature, sizeof(ULONG));

  if (IMAGE_NT_SIGNATURE != ntSignature)
    {
      report_file_error ("Missing NT signature. Unknown file type.\n", Qnil);
      CloseHandle (hImage);
      return -1;
    }

  SectionOffset = CoffHeaderOffset + IMAGE_SIZEOF_FILE_HEADER +
    IMAGE_SIZEOF_NT_OPTIONAL_HEADER;

  ReadBytes(hImage, &image_file_header, IMAGE_SIZEOF_FILE_HEADER);

  /*
   *  Read optional header.
   */
  ReadBytes(hImage,
            &image_optional_header,
            IMAGE_SIZEOF_NT_OPTIONAL_HEADER);

  CloseHandle (hImage);

  switch (image_optional_header.Subsystem)
    {
    case IMAGE_SUBSYSTEM_UNKNOWN:
        return 1;

    case IMAGE_SUBSYSTEM_NATIVE:
        return 1;

    case IMAGE_SUBSYSTEM_WINDOWS_GUI:
        return 1;

    case IMAGE_SUBSYSTEM_WINDOWS_CUI:
        return 0;

    case IMAGE_SUBSYSTEM_OS2_CUI:
        return 0;

    case IMAGE_SUBSYSTEM_POSIX_CUI:
        return 0;

    default:
        /* Unknown, return GUI app to be preservative: if yes, it will be
           correctly launched, if no, it will be launched, and a console will
           be also displayed, which is not a big deal */
        return 1;
    }
#else
	HWND ConsoleWindow = GetConsoleWindow();
	if (ConsoleWindow == NULL)
		return 1;
	else
		return 0;
#endif
}

static DWORD
AbsoluteSeek (HANDLE hFile, DWORD offset)
{
    DWORD newOffset;

    newOffset = SetFilePointer (hFile, offset, NULL, FILE_BEGIN);

    if (newOffset == 0xFFFFFFFF)
      return -1;
    else
      return newOffset;
}

static VOID
ReadBytes (HANDLE hFile, LPVOID buffer, DWORD size)
{
  DWORD bytes;

  if (!ReadFile(hFile, buffer, size, &bytes, NULL))
    {
      size = 0;
      return;
    }
  else if (size != bytes)
    {
      return;
    }
}

static int
nt_spawnve (char *exe, char **argv, char *env, struct TTY_Process *process)
{
  STARTUPINFO start;
  SECURITY_ATTRIBUTES sec_attrs;
  SECURITY_DESCRIPTOR sec_desc;
  DWORD flags;
  char dir[ MAXPATHLEN ];
  int pid;
  int is_gui, use_cmd;
  char *cmdline, *parg, **targ;
  int do_quoting = 0;
  char escape_char;
  int arglen;

  /* we have to do some conjuring here to put argv and envp into the
     form CreateProcess wants...  argv needs to be a space separated/null
     terminated list of parameters, and envp is a null
     separated/double-null terminated list of parameters.

     Additionally, zero-length args and args containing whitespace or
     quote chars need to be wrapped in double quotes - for this to work,
     embedded quotes need to be escaped as well.  The aim is to ensure
     the child process reconstructs the argv array we start with
     exactly, so we treat quotes at the beginning and end of arguments
     as embedded quotes.

     Note that using backslash to escape embedded quotes requires
     additional special handling if an embedded quote is already
     preceded by backslash, or if an arg requiring quoting ends with
     backslash.  In such cases, the run of escape characters needs to be
     doubled.  For consistency, we apply this special handling as long
     as the escape character is not quote.

     Since we have no idea how large argv and envp are likely to be we
     figure out list lengths on the fly and allocate them.  */

  if (!NILP (Vw32_quote_process_args))
    {
      do_quoting = 1;
      /* Override escape char by binding w32-quote-process-args to
	 desired character, or use t for auto-selection.  */
      if (INTEGERP (Vw32_quote_process_args))
	escape_char = XINT (Vw32_quote_process_args);
      else
	escape_char = '\\';
    }

  /* do argv...  */
  arglen = 0;
  targ = argv;
  while (*targ)
    {
      char *p = *targ;
      int need_quotes = 0;
      int escape_char_run = 0;

      if (*p == 0)
	need_quotes = 1;
      for ( ; *p; p++)
	{
	  if (*p == '"')
	    {
	      /* allow for embedded quotes to be escaped */
	      arglen++;
	      need_quotes = 1;
	      /* handle the case where the embedded quote is already escaped */
	      if (escape_char_run > 0)
		{
		  /* To preserve the arg exactly, we need to double the
		     preceding escape characters (plus adding one to
		     escape the quote character itself).  */
		  arglen += escape_char_run;
		}
	    }
	  else if (*p == ' ' || *p == '\t')
	    {
	      need_quotes = 1;
	    }

	  if (*p == escape_char && escape_char != '"')
	    escape_char_run++;
	  else
	    escape_char_run = 0;
	}
      if (need_quotes)
	{
	  arglen += 2;
	  /* handle the case where the arg ends with an escape char - we
	     must not let the enclosing quote be escaped.  */
	  if (escape_char_run > 0)
	    arglen += escape_char_run;
	}
      arglen += strlen (*targ) + 1;
      targ++;
    }

  is_gui = is_gui_app (argv[0]);
  use_cmd = FALSE;

  if (is_gui == -1) {
    /* could not determine application type. Try launching with "cmd /c" */
    is_gui = FALSE;
    arglen += 7;
    use_cmd = TRUE;
  }

  cmdline = (char*)malloc (arglen + 1);
  targ = argv;
  parg = cmdline;

  if (use_cmd == TRUE) {
    strcpy (parg, "cmd /c ");
    parg += 7;
  }

  while (*targ)
    {
      char * p = *targ;
      int need_quotes = 0;

      if (*p == 0)
	need_quotes = 1;

      if (do_quoting)
	{
	  for ( ; *p; p++)
	    if (*p == ' ' || *p == '\t' || *p == '"')
	      need_quotes = 1;
	}
      if (need_quotes)
	{
	  int escape_char_run = 0;
	  char * first;
	  char * last;

	  p = *targ;
	  first = p;
	  last = p + strlen (p) - 1;
	  *parg++ = '"';
	  for ( ; *p; p++)
	    {
	      if (*p == '"')
		{
		  /* double preceding escape chars if any */
		  while (escape_char_run > 0)
		    {
		      *parg++ = escape_char;
		      escape_char_run--;
		    }
		  /* escape all quote chars, even at beginning or end */
		  *parg++ = escape_char;
		}
	      *parg++ = *p;

	      if (*p == escape_char && escape_char != '"')
		escape_char_run++;
	      else
		escape_char_run = 0;
	    }
	  /* double escape chars before enclosing quote */
	  while (escape_char_run > 0)
	    {
	      *parg++ = escape_char;
	      escape_char_run--;
	    }
	  *parg++ = '"';
	}
      else
	{
	  strcpy (parg, *targ);
	  parg += strlen (*targ);
	}
      *parg++ = ' ';
      targ++;
    }
  *--parg = '\0';

  memset (&start, 0, sizeof (start));
  start.cb = sizeof (start);

  if (process->usePipe == TRUE) {
    start.dwFlags = STARTF_USESTDHANDLES;
    start.hStdInput = process->w_forkin;
    start.hStdOutput = process->w_forkout;
    /* child's stderr is always redirected to outfd */
    start.hStdError = process->w_forkout;
  } else {
    start.dwFlags = STARTF_USESTDHANDLES;
    /* We only need to redirect stderr/stdout here. Stdin will be forced to
       the spawned process console by explaunch */
    start.hStdInput = NULL;
    start.hStdOutput = process->w_forkout;
    start.hStdError = process->w_forkout;
  }

  /* Explicitly specify no security */
  if (!InitializeSecurityDescriptor (&sec_desc, SECURITY_DESCRIPTOR_REVISION))
    goto EH_Fail;
  if (!SetSecurityDescriptorDacl (&sec_desc, TRUE, NULL, FALSE))
    goto EH_Fail;
  sec_attrs.nLength = sizeof (sec_attrs);
  sec_attrs.lpSecurityDescriptor = &sec_desc;
  sec_attrs.bInheritHandle = FALSE;

  /* creating a new console allow easier close. Do not use
     CREATE_NEW_PROCESS_GROUP as this results in disabling Ctrl+C */
  flags = CREATE_NEW_CONSOLE;
  if (NILP (Vw32_start_process_inherit_error_mode))
    flags |= CREATE_DEFAULT_ERROR_MODE;

  /* if app is not a gui application, hide the console */
  if (is_gui == FALSE) {
    start.dwFlags |= STARTF_USESHOWWINDOW;
    start.wShowWindow = SW_HIDE;
  }

  /* Set initial directory to null character to use current directory */
  if (!CreateProcess (NULL, cmdline, &sec_attrs, NULL, TRUE,
		      flags, env, NULL, &start, &process->procinfo))
    goto EH_Fail;

  pid = (int) process->procinfo.hProcess;
  process->pid=pid;

  return pid;

 EH_Fail:
  return -1;
}

/*************************
 ** __gnat_send_header ()
 *************************/

#define EXP_SLAVE_CREATE 'c'
#define EXP_SLAVE_KEY    'k'
#define EXP_SLAVE_MOUSE  'm'
#define EXP_SLAVE_WRITE  'w'
#define EXP_SLAVE_KILL   'x'

#define EXP_KILL_TERMINATE  0x1
#define EXP_KILL_CTRL_C     0x2
#define EXP_KILL_CTRL_BREAK 0x4

void
__gnat_send_header (struct TTY_Process* p, char header[5], int size, int *ret)
{
  if (p->usePipe == FALSE) {
    header[0] = EXP_SLAVE_WRITE;
    header[1] = size & 0xff;
    header[2] = (size & 0xff00) >> 8;
    header[3] = (size & 0xff0000) >> 16;
    header[4] = (size & 0xff000000) >> 24;
    *ret = 1;
  } else {
    *ret = 0;
  }
}

/**********************************
 **  __gnat_setup_communication ()
 **********************************/

int
__gnat_setup_communication (struct TTY_Process** process_out) /* output param */
{
  struct TTY_Process* process;

  process = (struct TTY_Process*)malloc (sizeof (struct TTY_Process));
  ZeroMemory (process, sizeof (struct TTY_Process));
  *process_out = process;

  return 0;
}

#define EXP_PIPE_BASENAME "\\\\.\\pipe\\ExpectPipe"

int
__gnat_setup_child_communication
  (struct TTY_Process* process,
   char** argv,
   int Use_Pipes)
{
  int cpid;
  HANDLE parent;
  SECURITY_ATTRIBUTES sec_attrs;
  char slavePath [MAX_PATH];
  char **nargv;
  int argc;
  int i;
  char pipeNameIn[100];
  HANDLE hSlaveInDrv = NULL; /* Handle to communicate with slave driver */

  parent = GetCurrentProcess ();

  /* Set inheritance for the pipe handles */
  sec_attrs.nLength = sizeof (SECURITY_ATTRIBUTES);
  sec_attrs.bInheritHandle = TRUE;
  sec_attrs.lpSecurityDescriptor = NULL;

  if (Use_Pipes) {
    /* Create in and out pipes */
    if (!CreatePipe (&process->w_forkin, &process->w_infd, &sec_attrs, 0))
      report_file_error ("Creation of child's IN handle", Qnil);
    if (!CreatePipe (&process->w_outfd, &process->w_forkout, &sec_attrs, 0))
      report_file_error ("Creation of child's OUT handle", Qnil);

    /* Do not inherit the parent's side of the pipes */
    SetHandleInformation (&process->w_infd, HANDLE_FLAG_INHERIT, 0);
    SetHandleInformation (&process->w_outfd, HANDLE_FLAG_INHERIT, 0);

    /* use native argv */
    nargv = argv;
    process->usePipe = TRUE;

  } else {
    static int pipeNameId = 0;

    process->w_infd = NULL;

    /* We create a named pipe for Input, as we handle input by sending special
       commands to the explaunch process, that uses it to feed the actual input
       of the process */
    //sprintf(pipeNameIn, "%sIn%08x_%08x", EXP_PIPE_BASENAME, GetCurrentProcessId(), pipeNameId);
	{
		LPCSTR		prefix = "%1!s!In%2!08x!_%3!08x!";
		DWORD_PTR	Arguments[] = { (DWORD_PTR) EXP_PIPE_BASENAME, (DWORD_PTR) GetCurrentProcessId(), (DWORD_PTR) pipeNameId };
		DWORD		Ok = FormatMessageA(FORMAT_MESSAGE_ARGUMENT_ARRAY | FORMAT_MESSAGE_FROM_STRING, (LPCVOID) prefix, 0, 0, (LPSTR) pipeNameIn, 100, (va_list*) Arguments);
	}

    pipeNameId++;

    hSlaveInDrv = CreateNamedPipe(pipeNameIn,
				  PIPE_ACCESS_OUTBOUND,
				  PIPE_TYPE_BYTE | PIPE_WAIT, 1, 8192, 8192,
				  20000, NULL);
    if (hSlaveInDrv == NULL)  goto end;

    if (!CreatePipe (&process->w_outfd, &process->w_forkout, &sec_attrs, 0))
      report_file_error ("Creation of child's OUT handle", Qnil);

    if (SearchPath (NULL, "explaunch.exe", NULL,
                    MAX_PATH, slavePath, NULL) == 0) goto end;

    for (argc=0; argv[argc] != NULL; argc++) ;
    nargv = (char **) malloc (sizeof (char*) * (argc + 3));
    nargv[0] = slavePath;
    nargv[1] = pipeNameIn;

    for (i = 0; i <= argc; i++) nargv[i + 2] = argv[i];
    process->usePipe = FALSE;
  }

  /* Spawn the child. */
  cpid = nt_spawnve (nargv[0], nargv, NULL, process);

  /* close the duplicated handles passed to the child */
  CloseHandle (process->w_forkout);

  if (process->usePipe == TRUE) {
    CloseHandle (process->w_forkin);

  } else {
    UCHAR buf[8];		/* enough space for child status info */
    DWORD count;
    BOOL bRet;
    DWORD dwRet;

    /*
     * Wait for connection with the slave driver
     */
    bRet = ConnectNamedPipe(hSlaveInDrv, NULL);
    if (bRet == FALSE) {
      dwRet = GetLastError();
      if (dwRet == ERROR_PIPE_CONNECTED) {
	;
      } else {
	goto end;
      }
    }

    process->w_infd = hSlaveInDrv;

    /*
     * wait for slave driver to initialize before allowing user to send to it
     */
    bRet = ReadFile(process->w_outfd, buf, 8, &count, NULL);
    if (bRet == FALSE) {
      cpid = -1;
    }

    dwRet = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
    if (dwRet != 0) {
      cpid = -1;
    }

    cpid = buf[4] | (buf[5] << 8) | (buf[6] << 16) | (buf[7] << 24);
    process->pid = cpid;
  }

  if (cpid == -1)
    /* An error occurred while trying to spawn the process.  */
    report_file_error ("Spawning child process", Qnil);

  return cpid;
 end:
  if (hSlaveInDrv != NULL)
    CloseHandle (hSlaveInDrv);
  return -1;
}

void
__gnat_setup_parent_communication
  (struct TTY_Process* process,
   int* in,
   int* out,
   int* err,
   int* pid)
{
  *in = _open_osfhandle ((long) process->w_infd, 0);
  *out = _open_osfhandle ((long) process->w_outfd, 0);
  /* child's stderr is always redirected to outfd */
  *err = *out;
  *pid = process->pid;
}

typedef struct _child_process
{
  HWND                 hwnd;
  PROCESS_INFORMATION *procinfo;
} child_process;

/* The major and minor versions of NT.  */
static int w32_major_version;
static int w32_minor_version;

/* Distinguish between Windows NT and Windows 95.  */
static enum {OS_UNKNOWN, OS_WIN95, OS_NT} os_subtype = OS_UNKNOWN;

/* Cache information describing the NT system for later use.  */
static void
cache_system_info (void)
{
  union
    {
      struct info
        {
          char  major;
          char  minor;
          short platform;
        } info;
      DWORD data;
    } version;

  /* Cache the version of the operating system.  */
  version.data = GetVersion ();
  w32_major_version = version.info.major;
  w32_minor_version = version.info.minor;

  if (version.info.platform & 0x8000)
    os_subtype = OS_WIN95;
  else
    os_subtype = OS_NT;
}

static BOOL CALLBACK
find_child_console (HWND hwnd, child_process * cp)
{
  DWORD thread_id;
  DWORD process_id;

  thread_id = GetWindowThreadProcessId (hwnd, &process_id);
  if (process_id == cp->procinfo->dwProcessId)
    {
      char window_class[32];

      GetClassName (hwnd, window_class, sizeof (window_class));
      if (strcmp (window_class,
                  (os_subtype == OS_WIN95)
                  ? "tty"
                  : "ConsoleWindowClass") == 0)
        {
          cp->hwnd = hwnd;
          return FALSE;
        }
    }
  /* keep looking */
  return TRUE;
}

int
__gnat_interrupt_pid(int pid);

int
__gnat_interrupt_process (struct TTY_Process* p)
{
  char buf[2];
  DWORD written;
  BOOL bret;

  if (p->usePipe == TRUE) {
    bret = FALSE;
  } else {
    buf[0] = EXP_SLAVE_KILL;
    buf[1] = EXP_KILL_CTRL_C;
    bret = WriteFile (p->w_infd, buf, 2, &written, NULL);
  }

  if (bret == FALSE) {
    return __gnat_interrupt_pid (p->procinfo.dwProcessId);
  }
  return 0;
}

int
__gnat_interrupt_pid (int pid)
{
  volatile child_process cp;
  int rc = 0;

  cp.procinfo = (LPPROCESS_INFORMATION) malloc (sizeof (PROCESS_INFORMATION));
  cp.procinfo->dwProcessId = pid;

  if (os_subtype == OS_UNKNOWN)
    cache_system_info ();

  /* Try to locate console window for process. */
  EnumWindows ((WNDENUMPROC) find_child_console, (LPARAM) &cp);

  if (cp.hwnd)
    {
      BYTE control_scan_code = (BYTE) MapVirtualKey (VK_CONTROL, 0);
      /* Retrieve Ctrl-C scancode */
      BYTE vk_break_code = 'C';
      BYTE break_scan_code = (BYTE) MapVirtualKey (vk_break_code, 0);
      HWND foreground_window;

      foreground_window = GetForegroundWindow ();
      if (foreground_window)
        {
          /* NT 5.0, and apparently also Windows 98, will not allow
             a Window to be set to foreground directly without the
             user's involvement. The workaround is to attach
             ourselves to the thread that owns the foreground
             window, since that is the only thread that can set the
             foreground window.  */
          DWORD foreground_thread, child_thread;

          foreground_thread =
            GetWindowThreadProcessId (foreground_window, NULL);
          if (foreground_thread == GetCurrentThreadId ()
              || !AttachThreadInput (GetCurrentThreadId (),
                                     foreground_thread, TRUE))
            foreground_thread = 0;

          child_thread = GetWindowThreadProcessId (cp.hwnd, NULL);
          if (child_thread == GetCurrentThreadId ()
              || !AttachThreadInput (GetCurrentThreadId (),
                                     child_thread, TRUE))
            child_thread = 0;

          /* Set the foreground window to the child.  */
          if (SetForegroundWindow (cp.hwnd))
            {
              /* Generate keystrokes as if user had typed Ctrl-Break or
                 Ctrl-C.  */
              keybd_event (VK_CONTROL, control_scan_code, 0, 0);
              keybd_event (vk_break_code, break_scan_code,
                (vk_break_code == 'C' ? 0 : KEYEVENTF_EXTENDEDKEY), 0);
              keybd_event (vk_break_code, break_scan_code,
                (vk_break_code == 'C' ? 0 : KEYEVENTF_EXTENDEDKEY)
                 | KEYEVENTF_KEYUP, 0);
              keybd_event (VK_CONTROL, control_scan_code, KEYEVENTF_KEYUP, 0);

              /* Sleep for a bit to give time for the main frame to respond
              to focus change events.  */
              Sleep (100);

              SetForegroundWindow (foreground_window);
            }
          /* Detach from the foreground and child threads now that
             the foreground switching is over.  */
          if (foreground_thread)
	    AttachThreadInput (GetCurrentThreadId (), foreground_thread, FALSE);
	  if (child_thread)
            AttachThreadInput (GetCurrentThreadId (), child_thread, FALSE);
        }
    }
  /* Ctrl-Break is NT equivalent of SIGINT.  */
  else if (!GenerateConsoleCtrlEvent
             (CTRL_BREAK_EVENT, cp.procinfo->dwProcessId))
    {
      errno = EINVAL;
      rc = -1;
    }

  free (cp.procinfo);
  return rc;
}

/* kill a process, as this implementation use CreateProcess on Win32 we need
   to use Win32 TerminateProcess API */
int
__gnat_terminate_process (struct TTY_Process* p)
{
  char buf[2];
  DWORD written;
  BOOL bret;

  if (p->usePipe == TRUE) {
    bret = FALSE;
  } else {
    buf[0] = EXP_SLAVE_KILL;
    buf[1] = EXP_KILL_TERMINATE;
    bret = WriteFile (p->w_infd, buf, 2, &written, NULL);
  }

  if (bret == FALSE) {
    if (!TerminateProcess (p->procinfo.hProcess, 1))
      return -1;
    else
      return 0;
  } else
    return 0;
}

typedef struct {
  DWORD dwProcessId;
  HANDLE hwnd;
} pid_struct;

static BOOL CALLBACK
find_process_handle (HWND hwnd, pid_struct * ps)
{
  DWORD thread_id;
  DWORD process_id;

  thread_id = GetWindowThreadProcessId (hwnd, &process_id);
  if (process_id == ps->dwProcessId)
    {
      ps->hwnd = hwnd;
      return FALSE;
    }
  /* keep looking */
  return TRUE;
}

int
__gnat_terminate_pid (int pid)
{
  pid_struct ps;

  ps.dwProcessId = pid;
  ps.hwnd = 0;
  EnumWindows ((WNDENUMPROC) find_process_handle, (LPARAM) &ps);

  if (ps.hwnd)
    {
      if (!TerminateProcess (ps.hwnd, 1))
	return -1;
      else
	return 0;
    }

  return -1;
}

/* wait for process pid to terminate and return the process status. This
   implementation is different from the adaint.c one for Windows as it uses
   the Win32 API instead of the C one. */

int
__gnat_tty_waitpid (struct TTY_Process* p)
{
  DWORD exitcode;
  DWORD res;
  HANDLE proc_hand = p->procinfo.hProcess;

  res = WaitForSingleObject (proc_hand, 0);
  GetExitCodeProcess (proc_hand, &exitcode);

  CloseHandle (p->procinfo.hThread);
  CloseHandle (p->procinfo.hProcess);

  /* No need to close the handles: they were closed on the ada side */

  return (int) exitcode;
}

/********************************
 **  __gnat_free_process ()
 ********************************/

void
__gnat_free_process (struct TTY_Process** process)
{
  free (*process);
  *process = NULL;
}

/* TTY handling */

typedef struct {
  int tty_fd;        /* descriptor for the tty */
  char tty_name[24]; /* Name of TTY device */
} TTY_Handle;

int
__gnat_tty_supported (void)
{
  return 0;
}

/* Return the tty name associated with p */

char *
__gnat_tty_name (TTY_Handle* t)
{
  return t->tty_name;
}

int
__gnat_tty_fd (TTY_Handle* t)
{
  return t->tty_fd;
}

TTY_Handle*
__gnat_new_tty (void)
{
  return (TTY_Handle*)0;
}

void
__gnat_reset_tty (TTY_Handle* t)
{
  return;
}

void
__gnat_close_tty (TTY_Handle* t)
{
  free (t);
}

void
__gnat_setup_winsize (void *desc, int rows, int columns)
{
}

#endif /* WIN32 */

/****************************************************************************
 *                                                                          *
 *                         GNAT COMPILER COMPONENTS                         *
 *                                                                          *
 *                              S E H - I N I T                             *
 *                                                                          *
 *                          C Implementation File                           *
 *                                                                          *
 *           Copyright (C) 2005-2018, Free Software Foundation, Inc.        *
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
#define ATTRIBUTE_NORETURN __attribute__((noreturn))

#include <windows.h>
#include <excpt.h>

#define xmalloc(S) malloc (S)

#include "raise.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Addresses of exception data blocks for predefined exceptions. */
extern struct Exception_Data constraint_error;
extern struct Exception_Data numeric_error;
extern struct Exception_Data program_error;
extern struct Exception_Data storage_error;
extern struct Exception_Data tasking_error;
extern struct Exception_Data _abort_signal;

#define Raise_From_Signal_Handler ada__exceptions__raise_from_signal_handler

extern void Raise_From_Signal_Handler (struct Exception_Data *, const char *)
  ATTRIBUTE_NORETURN;

/* Prototypes.  */
extern void _global_unwind2 (void *);

EXCEPTION_DISPOSITION __gnat_SEH_error_handler
(struct _EXCEPTION_RECORD*, void*, struct _CONTEXT*, void*) ATTRIBUTE_NORETURN;

struct Exception_Data *
__gnat_map_SEH (EXCEPTION_RECORD* ExceptionRecord, const char **msg);

/* Convert an SEH exception to an Ada one.  Return the exception ID and set
   MSG to the corresponding message.  */

struct Exception_Data *
__gnat_map_SEH (EXCEPTION_RECORD* ExceptionRecord, const char **msg)
{
  switch (ExceptionRecord->ExceptionCode)
    {
    case EXCEPTION_ACCESS_VIOLATION:
      /* If the failing address isn't maximally aligned or if the page before
	 the faulting page is not accessible, this is a program error.  */
      if ((ExceptionRecord->ExceptionInformation[1] & 3) != 0)
	  //|| IsBadCodePtr
	  //   ((FARPROC)(ExceptionRecord->ExceptionInformation[1] + 4096)))
	{
	  *msg = "EXCEPTION_ACCESS_VIOLATION";
	  return &program_error;
	}
      else
	{
	  /* Otherwise this is a stack overflow.  */
	  *msg = "stack overflow or erroneous memory access";
	  return &storage_error;
	}

    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
      *msg = "EXCEPTION_ARRAY_BOUNDS_EXCEEDED";
      return &constraint_error;

    case EXCEPTION_DATATYPE_MISALIGNMENT:
      *msg = "EXCEPTION_DATATYPE_MISALIGNMENT";
      return &constraint_error;

    case EXCEPTION_FLT_DENORMAL_OPERAND:
      *msg = "EXCEPTION_FLT_DENORMAL_OPERAND";
      return &constraint_error;

    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
      *msg = "EXCEPTION_FLT_DENORMAL_OPERAND";
      return &constraint_error;

    case EXCEPTION_FLT_INVALID_OPERATION:
      *msg = "EXCEPTION_FLT_INVALID_OPERATION";
      return &constraint_error;

    case EXCEPTION_FLT_OVERFLOW:
      *msg = "EXCEPTION_FLT_OVERFLOW";
      return &constraint_error;

    case EXCEPTION_FLT_STACK_CHECK:
      *msg = "EXCEPTION_FLT_STACK_CHECK";
      return &program_error;

    case EXCEPTION_FLT_UNDERFLOW:
      *msg = "EXCEPTION_FLT_UNDERFLOW";
      return &constraint_error;

    case EXCEPTION_INT_DIVIDE_BY_ZERO:
      *msg = "EXCEPTION_INT_DIVIDE_BY_ZERO";
      return &constraint_error;

    case EXCEPTION_INT_OVERFLOW:
      *msg = "EXCEPTION_INT_OVERFLOW";
      return &constraint_error;

    case EXCEPTION_INVALID_DISPOSITION:
      *msg = "EXCEPTION_INVALID_DISPOSITION";
      return &program_error;

    case EXCEPTION_NONCONTINUABLE_EXCEPTION:
      *msg = "EXCEPTION_NONCONTINUABLE_EXCEPTION";
      return &program_error;

    case EXCEPTION_PRIV_INSTRUCTION:
      *msg = "EXCEPTION_PRIV_INSTRUCTION";
      return &program_error;

    case EXCEPTION_SINGLE_STEP:
      *msg = "EXCEPTION_SINGLE_STEP";
      return &program_error;

    case EXCEPTION_STACK_OVERFLOW:
      *msg = "EXCEPTION_STACK_OVERFLOW";
      return &storage_error;

    default:
      *msg = NULL;
      return NULL;
    }
}

/*  On x86-64/Windows the EH mechanism is no more based on a chained list of
    handlers addresses on the stack.  Instead unwinding information is used
    to retrieve the exception handler (similar to DWARF2 unwinding).  So in
    order to register an exception handler, we need to put in the binary
    some unwinding information.  This information can be present statically
    in the image file inside the .pdata section or registered through the
    RtlAddFunctionTable API.  In the case where the GCC toolchain does not
    generate the .pdata information for each function, we don't really need
    to handle SEH exceptions except for signal handling, so we register a
    "fake" unwinding data that associates a SEH exception handler with the
    complete .text section.  As we never return from the handler, the system
    does not try to do the final unwinding using the .pdata information and
    the unwinding is handled by the runtime using the GNAT or GCC mechanism.

    Solutions based on SetUnhandledExceptionFilter have been discarded as this
    function is mostly disabled on latest Windows versions.

    Using AddVectoredExceptionHandler should also be discarded as it overrides
    all SEH exception handlers that might be present in the program itself and
    the loaded DLL; for example it results in unexpected behavior in the Win32
    subsystem.  */

/* Nothing to do, the handler is either not used or statically installed by
   the asm statement just above.  */
void __gnat_install_SEH_handler (void *eh ATTRIBUTE_UNUSED)
{
}

#ifdef __cplusplus
}
#endif

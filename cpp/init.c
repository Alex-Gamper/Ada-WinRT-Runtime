/****************************************************************************
 *                                                                          *
 *                         GNAT COMPILER COMPONENTS                         *
 *                                                                          *
 *                                 I N I T                                  *
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

/*  This unit contains initialization circuits that are system dependent.
    A major part of the functionality involves stack overflow checking.
    The GCC backend generates probe instructions to test for stack overflow.
    For details on the exact approach used to generate these probes, see the
    "Using and Porting GCC" manual, in particular the "Stack Checking" section
    and the subsection "Specifying How Stack Checking is Done".  The handlers
    installed by this file are used to catch the resulting signals that come
    from these probes failing (i.e. touching protected pages).  */

/* This file should be kept synchronized with s-init.ads, s-init.adb and the
   s-init-*.adb variants. All these files implement the required functionality
   for different targets.  */

#include <sys/stat.h>
#define xmalloc(S) malloc (S)

#include "adaint.h"
#include "raise.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void __gnat_raise_program_error (const char *, int);

/* Addresses of exception data blocks for predefined exceptions.  Tasking_Error
   is not used in this unit, and the abort signal is only used on IRIX.
   ??? Revisit this part since IRIX is no longer supported.  */
extern struct Exception_Data constraint_error;
extern struct Exception_Data numeric_error;
extern struct Exception_Data program_error;
extern struct Exception_Data storage_error;

/* For the Cert run time we use the regular raise exception routine because
   Raise_From_Signal_Handler is not available.  */
#ifdef CERT
#define Raise_From_Signal_Handler \
                      __gnat_raise_exception
extern void Raise_From_Signal_Handler (struct Exception_Data *, const char *);
#else
#define Raise_From_Signal_Handler \
                      ada__exceptions__raise_from_signal_handler
extern void Raise_From_Signal_Handler (struct Exception_Data *, const char *);
#endif

/* Global values computed by the binder.  Note that these variables are
   declared here, not in the binder file, to avoid having unresolved
   references in the shared libgnat.  */
int   __gl_main_priority                 = -1;
int   __gl_main_cpu                      = -1;
int   __gl_time_slice_val                = -1;
char  __gl_wc_encoding                   = 'n';
char  __gl_locking_policy                = ' ';
char  __gl_queuing_policy                = ' ';
char  __gl_task_dispatching_policy       = ' ';
char *__gl_priority_specific_dispatching = 0;
int   __gl_num_specific_dispatching      = 0;
char *__gl_interrupt_states              = 0;
int   __gl_num_interrupt_states          = 0;
int   __gl_unreserve_all_interrupts      = 0;
int   __gl_exception_tracebacks          = 0;
int   __gl_exception_tracebacks_symbolic = 0;
int   __gl_detect_blocking               = 0;
int   __gl_default_stack_size            = -1;
int   __gl_leap_seconds_support          = 0;
int   __gl_canonical_streams             = 0;
char *__gl_bind_env_addr                 = NULL;

/* This value is not used anymore, but kept for bootstrapping purpose.  */
int   __gl_zero_cost_exceptions          = 0;

/* Indication of whether synchronous signal handler has already been
   installed by a previous call to adainit.  */
int  __gnat_handler_installed      = 0;

/* HAVE_GNAT_INIT_FLOAT must be set on every targets where a __gnat_init_float
   is defined.  If this is not set then a void implementation will be defined
   at the end of this unit.  */
#undef HAVE_GNAT_INIT_FLOAT

/******************************/
/* __gnat_get_interrupt_state */
/******************************/

char __gnat_get_interrupt_state (int);

/* This routine is called from the runtime as needed to determine the state
   of an interrupt, as set by an Interrupt_State pragma appearing anywhere
   in the current partition.  The input argument is the interrupt number,
   and the result is one of the following:

       'n'   this interrupt not set by any Interrupt_State pragma
       'u'   Interrupt_State pragma set state to User
       'r'   Interrupt_State pragma set state to Runtime
       's'   Interrupt_State pragma set state to System  */

char
__gnat_get_interrupt_state (int intrup)
{
  if (intrup >= __gl_num_interrupt_states)
    return 'n';
  else
    return __gl_interrupt_states [intrup];
}

/***********************************/
/* __gnat_get_specific_dispatching */
/***********************************/

char __gnat_get_specific_dispatching (int);

/* This routine is called from the runtime as needed to determine the
   priority specific dispatching policy, as set by a
   Priority_Specific_Dispatching pragma appearing anywhere in the current
   partition.  The input argument is the priority number, and the result
   is the upper case first character of the policy name, e.g. 'F' for
   FIFO_Within_Priorities. A space ' ' is returned if no
   Priority_Specific_Dispatching pragma is used in the partition.  */

char
__gnat_get_specific_dispatching (int priority)
{
  if (__gl_num_specific_dispatching == 0)
    return ' ';
  else if (priority >= __gl_num_specific_dispatching)
    return 'F';
  else
    return __gl_priority_specific_dispatching [priority];
}

void
__gnat_install_handler (void)
{
  __gnat_handler_installed = 1;
}

/*********************/
/* __gnat_init_float */
/*********************/

/* This routine is called as each process thread is created, for possible
   initialization of the FP processor.  This version is used under INTERIX
   and WIN32.  */

void
__gnat_init_float (void)
{
  asm ("finit");
}

/***********************************/
/* __gnat_adjust_context_for_raise */
/***********************************/

void
__gnat_adjust_context_for_raise (int signo ,
				 void *ucontext )
{
  /* We used to compensate here for the raised from call vs raised from signal
     exception discrepancy with the GCC ZCX scheme, but this now can be dealt
     with generically in the unwinder (see GCC PR other/26208).  This however
     requires the use of the _Unwind_GetIPInfo routine in raise-gcc.c, which
     is predicated on the definition of HAVE_GETIPINFO at compile time.  Only
     the VMS ports still do the compensation described in the few lines below.

     *** Call vs signal exception discrepancy with GCC ZCX scheme ***

     The GCC unwinder expects to be dealing with call return addresses, since
     this is the "nominal" case of what we retrieve while unwinding a regular
     call chain.

     To evaluate if a handler applies at some point identified by a return
     address, the propagation engine needs to determine what region the
     corresponding call instruction pertains to.  Because the return address
     may not be attached to the same region as the call, the unwinder always
     subtracts "some" amount from a return address to search the region
     tables, amount chosen to ensure that the resulting address is inside the
     call instruction.

     When we raise an exception from a signal handler, e.g. to transform a
     SIGSEGV into Storage_Error, things need to appear as if the signal
     handler had been "called" by the instruction which triggered the signal,
     so that exception handlers that apply there are considered.  What the
     unwinder will retrieve as the return address from the signal handler is
     what it will find as the faulting instruction address in the signal
     context pushed by the kernel.  Leaving this address untouched looses, if
     the triggering instruction happens to be the very first of a region, as
     the later adjustments performed by the unwinder would yield an address
     outside that region.  We need to compensate for the unwinder adjustments
     at some point, and this is what this routine is expected to do.

     signo is passed because on some targets for some signals the PC in
     context points to the instruction after the faulting one, in which case
     the unwinder adjustment is still desired.  */
}

#ifdef __cplusplus
}
#endif

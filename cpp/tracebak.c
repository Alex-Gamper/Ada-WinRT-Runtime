/****************************************************************************
 *                                                                          *
 *                         GNAT RUN-TIME COMPONENTS                         *
 *                                                                          *
 *                            T R A C E B A C K                             *
 *                                                                          *
 *                          C Implementation File                           *
 *                                                                          *
 *            Copyright (C) 2000-2018, Free Software Foundation, Inc.       *
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

extern int __gnat_backtrace (void **, int, void *, void *, int);

#include <windows.h>

//#define IS_BAD_PTR(ptr) (IsBadCodePtr((FARPROC)ptr))

int
__gnat_backtrace (void **array,
                  int size,
                  void *exclude_min,
                  void *exclude_max,
                  int skip_frames)
{
    CONTEXT context;
    UNWIND_HISTORY_TABLE history;
    int i;

    /* Get the context.  */
#if 0
    RtlCaptureContext (&context);
    /* Setup unwind history table (a cached to speed-up unwinding).  */
    memset(&history, 0, sizeof(history));

    i = 0;
    while (1)
    {
        PRUNTIME_FUNCTION RuntimeFunction;
        KNONVOLATILE_CONTEXT_POINTERS NvContext;
        ULONG64 ImageBase;
        VOID *HandlerData;
        ULONG64 EstablisherFrame;

        /* Get function metadata.  */
        RuntimeFunction = RtlLookupFunctionEntry(context.Rip, &ImageBase, &history);

        if (!RuntimeFunction)
        {
            /* In case of failure, assume this is a leaf function.  */
            context.Rip = *(ULONG64 *)context.Rsp;
            context.Rsp += 8;
        }
        else
        {
            /* If the last unwinding step failed somehow, stop here.  */
            //if (IS_BAD_PTR(context.Rip))
            //  break;

            /* Unwind.  */
            memset(&NvContext, 0, sizeof(KNONVOLATILE_CONTEXT_POINTERS));
            RtlVirtualUnwind(0, ImageBase, context.Rip, RuntimeFunction,
                &context, &HandlerData, &EstablisherFrame,
                &NvContext);
        }

        /* 0 means bottom of the stack.  */
        if (context.Rip == 0)
            break;

        /* Skip frames.  */
        if (skip_frames > 1)
        {
            skip_frames--;
            continue;
        }

        /* Excluded frames.  */
        if ((void *)context.Rip >= exclude_min && (void *)context.Rip <= exclude_max)
            continue;

        array[i++] = (void *)(context.Rip - 2);
        if (i >= size)
            break;
    }
    return i;
#else
    //PVOID BackTrace[63];
    return RtlCaptureStackBackTrace(skip_frames, 63, array, NULL);
#endif

}

#ifdef __cplusplus
}
#endif

#include "Winrt.h"
#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

    extern int __stdio_common_vfprintf_s(int _Options, FILE* _Stream, char const* const _Format, _locale_t _Locale, va_list _ArgList);
    extern int __stdio_common_vfscanf(int _Options, FILE* _Stream, char const* const _Format, _locale_t _Locale, va_list _ArgList);
    extern int __local_stdio_printf_options();
    extern int __local_stdio_scanf_options();

    int WINRT_fprintf_s(FILE* _Stream, char const* const _Format, ...)
    {
        int _Result;
        va_list _ArgList;
        _crt_va_start(_ArgList, _Format);
        _Result = __stdio_common_vfprintf_s(__local_stdio_printf_options(), _Stream, _Format, NULL, _ArgList);
        _crt_va_end(_ArgList);
        return _Result;
    }

    int WINRT_fscanf_s(FILE* _Stream, char const* const _Format, ...)
    {
#define _CRT_INTERNAL_SCANF_SECURECRT                   (1ULL << 0)
#define _CRT_INTERNAL_SCANF_LEGACY_WIDE_SPECIFIERS      (1ULL << 1)
#define _CRT_INTERNAL_SCANF_LEGACY_MSVCRT_COMPATIBILITY (1ULL << 2)

        int _Result;
        va_list _ArgList;
        _crt_va_start(_ArgList, _Format);
        _Result = __stdio_common_vfscanf(__local_stdio_scanf_options() | _CRT_INTERNAL_SCANF_SECURECRT, _Stream, _Format, NULL, _ArgList);
        _crt_va_end(_ArgList);
        return _Result;
    }

#ifdef __cplusplus
}
#endif

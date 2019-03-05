#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

    int WINRT_fprintf_s(FILE* _Stream, char const* const _Format, ...);
    int WINRT_fscanf_s(FILE* _Stream, char const* const _Format, ...);

#ifdef __cplusplus
}
#endif

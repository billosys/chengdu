#ifndef RSS_H_INCLUDED
#define RSS_H_INCLUDED

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

size_t getPeakRSS(void);
size_t getCurrentRSS(void);

#ifdef __cplusplus
}
#endif

#endif

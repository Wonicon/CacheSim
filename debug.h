#ifndef DEBUG_H
#define DEBUG_H

#include <cstdio>

//#define DEBUG

#ifdef DEBUG

extern FILE *logfile;

#define log(fmt, ...) \
    fprintf(logfile, "[%s#%s:%d] " fmt "\n", __FILE__, __FUNCTION__, __LINE__, ## __VA_ARGS__)

#else

#define log(...)

#endif

#endif // DEBUG_H

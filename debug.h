#ifndef DEBUG_H
#define DEBUG_H

#include <cstdio>

//#define DEBUG

extern FILE *logfile;

extern int n_instr;

#ifdef DEBUG


#define log(fmt, ...) \
    fprintf(logfile, "[%5d] " fmt "\n", n_instr, ## __VA_ARGS__)

#else

#define log(...)

#endif

#endif // DEBUG_H

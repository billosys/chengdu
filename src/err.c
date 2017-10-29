/***
 * cpddl
 * -------
 * Copyright (c)2017 Daniel Fiser <danfis@danfis.cz>,
 * AI Center, Department of Computer Science,
 * Faculty of Electrical Engineering, Czech Technical University in Prague.
 * All rights reserved.
 *
 * This file is part of cpddl.
 *
 * Distributed under the OSI-approved BSD License (the "License");
 * see accompanying file BDS-LICENSE for details or see
 * <http://www.opensource.org/licenses/bsd-license.php>.
 *
 * This software is distributed WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the License for more information.
 */

#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <boruvka/timer.h>

#include "err.h"

#ifndef PDDL_ERR_MSG_MAXLEN
# define PDDL_ERR_MSG_MAXLEN 512
#endif /* PDDL_ERR_MSG_MAXLEN */

#ifndef PDDL_ERR_TRACE_DEPTH
# define PDDL_ERR_TRACE_DEPTH 16
#endif /* PDDL_ERR_TRACE_DEPTH */

struct pddl_err_trace {
    const char *filename;
    int line;
    const char *func;
};
typedef struct pddl_err_trace pddl_err_trace_t;

struct pddl_err {
    pddl_err_trace_t trace[PDDL_ERR_TRACE_DEPTH];
    int trace_depth;
    int trace_more;
    char msg[PDDL_ERR_MSG_MAXLEN];
    FILE *out;
    int err;

    int print_warn;
    FILE *warn_out;
    int print_info;
    FILE *info_out;
    bor_timer_t info_timer;
    int info_timer_init;
};
typedef struct pddl_err pddl_err_t;


static __thread pddl_err_t err = { 0 };

void _pddlErrReset(void)
{
    err.trace_depth = 0;
    err.trace_more = 0;
    err.msg[0] = 0;
    if (err.out == NULL)
        err.out = stderr;
    err.err = 0;
}

void pddlErrSetOutput(FILE *out)
{
    err.out = out;
}

void pddlErrSetWarnOutput(FILE *out)
{
    err.warn_out = out;
    err.print_warn = 1;
}

void pddlErrEnableWarn(int enable)
{
    err.print_warn = enable;
}

void pddlErrSetInfoOutput(FILE *out)
{
    err.info_out = out;
    err.print_info = 1;
}

void pddlErrEnableInfo(int enable)
{
    err.print_info = enable;
}

void pddlErrPrintTraceback(void)
{
    if (!err.err)
        return;
    if (err.out == NULL)
        err.out = stderr;

    for (int i = 0; i < err.trace_depth; ++i){
        for (int j = 0; j < i; ++j)
            fprintf(err.out, "  ");
        fprintf(err.out, "  ");
        fprintf(err.out, "%s:%d (%s)\n",
                err.trace[i].filename,
                err.trace[i].line,
                err.trace[i].func);
    }
    fflush(err.out);
}

void pddlErrPrint(void)
{
    if (!err.err)
        return;
    if (err.out == NULL)
        err.out = stderr;

    fprintf(err.out, "Error: %s\n", err.msg);
    fflush(err.out);
}

void pddlErrPrintWithTraceback(void)
{
    pddlErrPrint();
    pddlErrPrintTraceback();
}

void _pddlErr(const char *filename, int line, const char *func,
              const char *format, ...)
{
    va_list ap;

    _pddlErrReset();

    err.trace[0].filename = filename;
    err.trace[0].line = line;
    err.trace[0].func = func;
    err.trace_depth = 1;
    err.trace_more = 0;

    va_start(ap, format);
    vsnprintf(err.msg, PDDL_ERR_MSG_MAXLEN, format, ap);
    va_end(ap);
    err.err = 1;
}

void _pddlErrPrepend(const char *format, ...)
{
    va_list ap;
    char msg[PDDL_ERR_MSG_MAXLEN];
    int size;

    strcpy(msg, err.msg);
    va_start(ap, format);
    size = vsnprintf(err.msg, PDDL_ERR_MSG_MAXLEN, format, ap);
    snprintf(err.msg + size, PDDL_ERR_MSG_MAXLEN - size, "%s", msg);
    va_end(ap);

}

void _pddlTrace(const char *filename, int line, const char *func)
{
    if (err.trace_depth == PDDL_ERR_TRACE_DEPTH){
        err.trace_more = 1;
    }else{
        err.trace[err.trace_depth].filename = filename;
        err.trace[err.trace_depth].line = line;
        err.trace[err.trace_depth].func = func;
        ++err.trace_depth;
    }
}

void _pddlWarn(const char *filename, int line, const char *func,
               const char *format, ...)
{
    va_list ap;

    if (err.print_warn == 0)
        return;
    if (err.warn_out == NULL)
        err.warn_out = stderr;

    va_start(ap, format);
    fprintf(err.warn_out, "Warning: %s:%d [%s]: ", filename, line, func);
    vfprintf(err.warn_out, format, ap);
    va_end(ap);
    fprintf(err.warn_out, "\n");
    fflush(err.warn_out);
}

void _pddlInfo(const char *filename, int line, const char *func,
               const char *format, ...)
{
    struct rusage usg;
    long peak_mem = 0L;
    va_list ap;

    if (err.print_info == 0)
        return;
    if (err.info_out == NULL)
        err.info_out = stderr;
    if (!err.info_timer_init){
        borTimerStart(&err.info_timer);
        err.info_timer_init = 1;
    }

    if (getrusage(RUSAGE_SELF, &usg) == 0)
        peak_mem = usg.ru_maxrss / 1024L;
    va_start(ap, format);
    borTimerStop(&err.info_timer);
    fprintf(err.info_out, "[%.3fs %ldMB] ",
            borTimerElapsedInSF(&err.info_timer), peak_mem);
    vfprintf(err.info_out, format, ap);
    va_end(ap);
    fprintf(err.info_out, "\n");
    fflush(err.info_out);
}

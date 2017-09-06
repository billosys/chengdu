/***
 * cpddl
 * -------
 * Copyright (c)2016 Daniel Fiser <danfis@danfis.cz>,
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

#ifndef ___PDDL_ERR_H__
#define ___PDDL_ERR_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void _pddlErrReset(void);
void _pddlErr(const char *filename, int line, const char *func,
              const char *format, ...);
void _pddlErrPrepend(const char *format, ...);
void _pddlTrace(const char *filename, int line, const char *func);
void _pddlWarn(const char *filename, int line, const char *func,
               const char *format, ...);
void _pddlInfo(const char *filename, int line, const char *func,
               const char *format, ...);

#define TRACE _pddlTrace(__FILE__, __LINE__, __func__)
#define TRACE_RET(V) do { \
        TRACE; \
        return (V); \
    } while (0)

#define TRACE_UPDATE(format, ...) do { \
        _pddlErrPrepend(format, __VA_ARGS__); \
        TRACE; \
    } while (0)
#define TRACE_UPDATE_RET(V, format, ...) do { \
        _pddlErrPrepend(format, __VA_ARGS__); \
        TRACE_RET(V); \
    } while (0)

#define ERR(format, ...) do { \
        _pddlErr(__FILE__, __LINE__, __func__, format, __VA_ARGS__); \
    } while (0)
#define ERR2(msg) do { \
        _pddlErr(__FILE__, __LINE__, __func__, msg); \
    } while (0)
#define ERR_RET(V, format, ...) do { \
        ERR(format, __VA_ARGS__); \
        return (V); \
    } while (0)
#define ERR_RET2(V, msg) do { \
        ERR2(msg); \
        return (V); \
    } while (0)

#define ERR_LISP(N, format, ...) do { \
        ERR(format " on line %d.", __VA_ARGS__, (N)->lineno); \
    } while (0)
#define ERR_LISP2(N, msg) do { \
        ERR(msg " on line %d.", (N)->lineno); \
    } while (0)
#define ERR_LISP_RET(V, N, format, ...) do { \
        ERR_LISP(N, format, __VA_ARGS__); \
        return (V); \
    } while (0)
#define ERR_LISP_RET2(V, N, msg) do { \
        ERR_LISP2(N, msg); \
        return (V); \
    } while (0)


#define FATAL(format, ...) do { \
        fprintf(stderr, "Fatal Error: %s:%d [%s]: " format, \
                __FILE__, __LINE__, __func__, __VA_ARGS__); \
        exit(-1); \
    } while (0)
#define FATAL2(msg) do { \
        fprintf(stderr, "Fatal Error: %s:%d [%s]: " msg, \
                __FILE__, __LINE__, __func__); \
        exit(-1); \
    } while (0)

#define WARN(format, ...) do { \
        _pddlWarn(__FILE__, __LINE__, __func__, format, __VA_ARGS__); \
    } while (0)
#define WARN2(msg) do { \
        _pddlWarn(__FILE__, __LINE__, __func__, msg); \
    } while (0)

#define INFO(format, ...) do { \
        _pddlInfo(__FILE__, __LINE__, __func__, format, __VA_ARGS__); \
    } while (0)
#define INFO2(msg) do { \
        _pddlInfo(__FILE__, __LINE__, __func__, msg); \
    } while (0)

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* ___PDDL_ERR_H__ */

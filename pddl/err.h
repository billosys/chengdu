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

#ifndef __PDDL_ERR_H__
#define __PDDL_ERR_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Sets output stream for printing errors (by pddlErrPrint*() functions).
 * Default ouput stream in stderr.
 */
void pddlErrSetOutput(FILE *out);

/**
 * Sets ouput stream for the warnings -- this also automatically calls
 * pddlErrEnableWarn().
 * Default ouput stream in stderr.
 */
void pddlErrSetWarnOutput(FILE *out);
void pddlErrSetInfoOutput(FILE *out);

/**
 * Enables/disables printing out warnings.
 * Warning are disabled by default.
 */
void pddlErrEnableWarn(int enable);
void pddlErrEnableInfo(int enable);

/**
 * Prints error message.
 */
void pddlErrPrint(void);

/**
 * Prints traceback.
 */
void pddlErrPrintTraceback(void);

/**
 * Prints both error message and traceback.
 */
void pddlErrPrintWithTraceback(void);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __PDDL_ERR_H__ */

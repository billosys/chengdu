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

#ifndef __PDDL_UTIL_H__
#define __PDDL_UTIL_H__

#include <pddl/lisp.h>
#include <pddl/obj.h>
#include <pddl/param.h>
#include <pddl/cond.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Greatest common denominator.
 */
_bor_inline int pddlGCD(int x, int y)
{
    if (x == 0)
        return y;

    while (y != 0){
        if (x > y) {
            x = x - y;
        }else{
            y = y - x;
        }
    }

    return x;
}

/**
 * Least Common Multiple
 */
_bor_inline int pddlLCM(int x, int y)
{
    return (x * y) / pddlGCD(x, y);
}


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __PDDL_UTIL_H__ */

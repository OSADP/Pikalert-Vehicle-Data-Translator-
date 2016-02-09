#!/usr/bin/env python

"""Logical symbols"""

# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
# ** Copyright UCAR (c) 1992 - 2005
# ** University Corporation for Atmospheric Research(UCAR)
# ** National Center for Atmospheric Research(NCAR)
# ** Research Applications Program(RAP)
# ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA
# ** All rights reserved. Licenced use only.
# ** Do not copy or distribute without authorization.
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*


import numarray
import operator

(LT, LE, EQ, GT, GE, NE, OR, AND, COL, ALL) = range(10)
COND_REC_FUN_LIST = (operator.lt, operator.le, operator.eq, operator.gt, operator.ge, operator.ne, operator.or_, operator.and_)
COND_MAT_FUN_LIST = (numarray.less, numarray.less_equal, numarray.equal, numarray.greater, numarray.greater_equal, numarray.not_equal, numarray.logical_or, numarray.logical_and)
(RULE, SEL, VAL) = range(3)

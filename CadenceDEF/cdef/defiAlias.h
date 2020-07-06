/*
 * This  file  is  part  of  the  Cadence  LEF/DEF  Open   Source
 * Distribution,  Product Version 5.3, and is subject to the Cadence LEF/DEF
 * Open Source License Agreement.   Your  continued  use  of this file
 * constitutes your acceptance of the terms of the LEF/DEF Open Source
 * License and an agreement to abide by its  terms.   If you  don't  agree
 * with  this, you must remove this and any other files which are part of the
 * distribution and  destroy any  copies made.
 * 
 * For updates, support, or to become part of the LEF/DEF Community, check
 * www.openeda.org for details.
 */

#ifndef defiALIAS_h
#define defiALIAS_h
#include <stdio.h>
#include "defiKRDefs.h"

typedef struct defiAlias_itr_s {
  int             num_;
} defiAlias_itr;

EXTERN defiAlias_itr *
defiAlias_itr_Create
  PROTO_PARAMS((  ));

EXTERN void
defiAlias_itr_Init
  PROTO_PARAMS(( defiAlias_itr * this ));

EXTERN void
defiAlias_itr_Destroy
  PROTO_PARAMS(( defiAlias_itr * this ));

EXTERN void
defiAlias_itr_Delete
  PROTO_PARAMS(( defiAlias_itr * this ));

EXTERN int
defiAlias_itr_Next
  PROTO_PARAMS(( defiAlias_itr * this ));

EXTERN const char *
defiAlias_itr_Key
  PROTO_PARAMS(( defiAlias_itr * this ));

EXTERN const char *
defiAlias_itr_Data
  PROTO_PARAMS(( defiAlias_itr * this ));

EXTERN int
defiAlias_itr_Marked
  PROTO_PARAMS(( defiAlias_itr * this ));

#endif

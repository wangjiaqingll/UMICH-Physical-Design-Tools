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
#include <stdio.h>
#include <stdlib.h>
#include "defiAlias.h"

/*
 * defiAlias_itr
 *//* From FEF_stringhash.cpp  */
extern const char *defAliasKey(int num);

extern const char *defAliasData(int num);

extern int defAliasMarked(int num);

extern int defAliasLimit();

defiAlias_itr *
defiAlias_itr_CreateUninitialized()
{
  defiAlias_itr *this = (defiAlias_itr *) malloc(sizeof(defiAlias_itr));

  defiAlias_itr_Init(this);
  return (this);
}

void 
defiAlias_itr_Init(defiAlias_itr * this)
{
  this->num_ = -1;
}

void 
defiAlias_itr_Destroy(defiAlias_itr * this)
{
}

void
defiAlias_itr_Delete(defiAlias_itr * this)
{
  if (this == NULL)
    return;
  defiAlias_itr_Destroy(this);
  free(this);
}

int 
defiAlias_itr_Next(defiAlias_itr * this)
{
  while (1) {
    this->num_ += 1;
    if (this->num_ <= -1 || this->num_ >= defAliasLimit())
      return 0;
    if (defAliasKey(this->num_))
      break;
  }
  return 1;
}

const char *
defiAlias_itr_Key(defiAlias_itr * this)
{
  if (this->num_ <= -1 || this->num_ >= defAliasLimit())
    return 0;
  return defAliasKey(this->num_);
}

const char *
defiAlias_itr_Data(defiAlias_itr * this)
{
  if (this->num_ <= -1 || this->num_ >= defAliasLimit())
    return 0;
  return defAliasData(this->num_);
}

int 
defiAlias_itr_Marked(defiAlias_itr * this)
{
  if (this->num_ <= -1 || this->num_ >= defAliasLimit())
    return 0;
  return defAliasMarked(this->num_);
}

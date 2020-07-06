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
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include "defiPropType.h"
#include "defiDebug.h"
defiPropType *
defiPropType_CreateUninitialized()
{
  defiPropType *this = (defiPropType *) malloc(sizeof(defiPropType));

  defiPropType_Init(this);
  return (this);
}

void 
defiPropType_Init(defiPropType * this)
{
  this->numProperties_ = 0;
  this->propertiesAllocated_ = 0;
  this->propNames_ = 0;
  this->propTypes_ = 0;
}

void 
defiPropType_Clear(defiPropType * this)
{
  int     i;

  for (i = 0;
       i < this->numProperties_;
       i++) {
    free(this->propNames_[i]);
  }
  this->numProperties_ = 0;
  this->propertiesAllocated_ = 0;
}

void 
defiPropType_Destroy(defiPropType * this)
{
  defiPropType_Clear(this);
  if (this->propNames_)
    free(this->propNames_);
  if (this->propTypes_)
    free(this->propTypes_);
}

void
defiPropType_Delete(defiPropType * this)
{
  if (this == NULL)
    return;
  defiPropType_Destroy(this);
  free(this);
}

void 
defiPropType_setPropType(defiPropType * this,
			 const char *name,
			 const char type)
{
  int     len;

  if (this->numProperties_ == this->propertiesAllocated_)
    defiPropType_bumpProps(this);
  len = strlen(name) + 1;
  this->propNames_[this->numProperties_] = (char *) malloc(len);
  strcpy(this->propNames_[this->numProperties_], DEFCASE(name));
  this->propTypes_[this->numProperties_] = type;
  this->numProperties_ += 1;
}

void 
defiPropType_bumpProps(defiPropType * this)
{
  int     lim = this->propertiesAllocated_;

  int     news;

  char  **newpn;

  char   *newt;

  news = lim ? lim + lim : 2;
  newpn = (char **) malloc(sizeof(char *) * news);
  newt = (char *) malloc(sizeof(char) * news);
  lim = this->propertiesAllocated_ = news;
  if (lim > 2) {
    int     i;

    for (i = 0;
	 i < this->numProperties_;
	 i++) {
      newpn[i] = this->propNames_[i];
      newt[i] = this->propTypes_[i];
    }
    free((char *) (this->propNames_));
    free((char *) (this->propTypes_));
  }
  this->propNames_ = newpn;
  this->propTypes_ = newt;
}

const char 
defiPropType_propType(const defiPropType * this,
		      char *name)
{
  int     i;

  if (!name || *name == '\0')
    return ('N');
  for (i = 0;
       i < this->numProperties_;
       i++) {
    if (strcmp(name, this->propNames_[i]) == 0)
      return (this->propTypes_[i]);
  }
  return ('N');
}

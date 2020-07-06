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
#include "lefiLayer.h"
#include "lefiDebug.h"

/*
 * lefiLayerDensity
 */ lefiLayerDensity *
lefiLayerDensity_CreateUninitialized()
{
  lefiLayerDensity *this = (lefiLayerDensity *) malloc(sizeof(lefiLayerDensity));

  return (this);
}

void 
lefiLayerDensity_Init(lefiLayerDensity * this,
		      const char *type)
{
  int     len = strlen(type) + 1;

  this->type_ = (char *) malloc(len);
  strcpy(this->type_, CASE(type));
  this->oneEntry_ = 0;
  this->numFrequency_ = 0;
  this->frequency_ = 0;
  this->numWidths_ = 0;
  this->widths_ = 0;
  this->numTableEntries_ = 0;
  this->tableEntries_ = 0;
  this->numCutareas_ = 0;
  this->cutareas_ = 0;
}

void 
lefiLayerDensity_Destroy(lefiLayerDensity * this)
{
  if (this->type_)
    free((char *) (this->type_));
  if (this->frequency_)
    free((char *) (this->frequency_));
  if (this->widths_)
    free((char *) (this->widths_));
  if (this->tableEntries_)
    free((char *) (this->tableEntries_));
  if (this->cutareas_)
    free((char *) (this->cutareas_));
}

void
lefiLayerDensity_Delete(lefiLayerDensity * this)
{
  if (this == NULL)
    return;
  lefiLayerDensity_Destroy(this);
  free(this);
}

void 
lefiLayerDensity_setOneEntry(lefiLayerDensity * this,
			     double entry)
{
  this->oneEntry_ = entry;
}

void 
lefiLayerDensity_addFrequency(lefiLayerDensity * this,
			      int num,
			      double *frequency)
{
  this->numFrequency_ = num;
  this->frequency_ = frequency;
}

void 
lefiLayerDensity_addWidth(lefiLayerDensity * this,
			  int num,
			  double *width)
{
  this->numWidths_ = num;
  this->widths_ = width;
}

void 
lefiLayerDensity_addTableEntry(lefiLayerDensity * this,
			       int num,
			       double *entry)
{
  this->numTableEntries_ = num;
  this->tableEntries_ = entry;
}

void 
lefiLayerDensity_addCutarea(lefiLayerDensity * this,
			    int num,
			    double *cutarea)
{
  this->numCutareas_ = num;
  this->cutareas_ = cutarea;
}

int 
lefiLayerDensity_hasOneEntry(lefiLayerDensity * this)
{
  return this->oneEntry_ ? 1 : 0;
}

char   *
lefiLayerDensity_type(lefiLayerDensity * this)
{
  return this->type_;
}

double 
lefiLayerDensity_oneEntry(lefiLayerDensity * this)
{
  return this->oneEntry_;
}

int 
lefiLayerDensity_numFrequency(lefiLayerDensity * this)
{
  return this->numFrequency_;
}

double 
lefiLayerDensity_frequency(lefiLayerDensity * this,
			   int index)
{
  return this->frequency_[index];
}

int 
lefiLayerDensity_numWidths(lefiLayerDensity * this)
{
  return this->numWidths_;
}

double 
lefiLayerDensity_width(lefiLayerDensity * this,
		       int index)
{
  return this->widths_[index];
}

int 
lefiLayerDensity_numTableEntries(lefiLayerDensity * this)
{
  return this->numTableEntries_;
}

double 
lefiLayerDensity_tableEntry(lefiLayerDensity * this,
			    int index)
{
  return this->tableEntries_[index];
}

int 
lefiLayerDensity_numCutareas(lefiLayerDensity * this)
{
  return this->numCutareas_;
}

double 
lefiLayerDensity_cutArea(lefiLayerDensity * this,
			 int index)
{
  return this->cutareas_[index];
}

/*
 * lefiLayer
 */ lefiLayer *
lefiLayer_CreateUninitialized()
{
  lefiLayer *this = (lefiLayer *) malloc(sizeof(lefiLayer));

  lefiLayer_Init(this);
  return (this);
}

void 
lefiLayer_Init(lefiLayer * this)
{
  this->name_ = (char *) malloc(16);
  this->nameSize_ = 16;
  this->type_ = (char *) malloc(16);
  this->typeSize_ = 16;
  this->numSpacings_ = 0;
  this->spacingsAllocated_ = 0;
  this->numCurrentPoints_ = 0;
  this->currentPointsAllocated_ = 2;
  this->currentWidths_ = (double *) malloc(sizeof(double) * 2);
  this->current_ = (double *) malloc(sizeof(double) * 2);
  this->numResistancePoints_ = 0;
  this->resistancePointsAllocated_ = 2;
  this->resistanceWidths_ = (double *) malloc(sizeof(double) * 2);
  this->resistances_ = (double *) malloc(sizeof(double) * 2);
  this->numCapacitancePoints_ = 0;
  this->capacitancePointsAllocated_ = 2;
  this->capacitanceWidths_ = (double *) malloc(sizeof(double) * 2);
  this->capacitances_ = (double *) malloc(sizeof(double) * 2);
  this->numProps_ = 0;
  this->propsAllocated_ = 1;
  this->names_ = (char **) malloc(sizeof(char *));
  this->values_ = (char **) malloc(sizeof(char *));
  lefiLayer_clear(this);
  this->numAccurrents_ = 0;
  this->accurrentAllocated_ = 0;
  this->numDccurrents_ = 0;
  this->dccurrentAllocated_ = 0;
  this->numNums_ = 0;
  this->numAllocated_ = 0;
  this->lengthThreshold_ = 0;
}

void 
lefiLayer_Destroy(lefiLayer * this)
{
  lefiLayer_clear(this);
  free(this->name_);
  free(this->type_);
  if (this->spacing_)
    free((char *) (this->spacing_));
  if (this->spacingName_)
    free((char *) (this->spacingName_));
  free((char *) (this->currentWidths_));
  free((char *) (this->current_));
  free((char *) (this->resistanceWidths_));
  free((char *) (this->resistances_));
  free((char *) (this->capacitanceWidths_));
  free((char *) (this->capacitances_));
  free((char *) (this->names_));
  free((char *) (this->values_));
  if (this->nums_)
    free((char *) (this->nums_));
}

void
lefiLayer_Delete(lefiLayer * this)
{
  if (this == NULL)
    return;
  lefiLayer_Destroy(this);
  free(this);
}

void 
lefiLayer_clear(lefiLayer * this)
{
  int     i;

  lefiLayerDensity *p;

  if (this->name_)
    *(this->name_) = 0;
  if (this->type_)
    *(this->type_) = 0;
  this->hasPitch_ = 0;
  this->hasOffset_ = 0;
  this->hasWidth_ = 0;
  this->hasWireExtension_ = 0;
  this->hasSpacing_ = 0;
  this->hasDirection_ = 0;
  this->hasResistance_ = 0;
  this->hasCapacitance_ = 0;
  this->hasHeight_ = 0;
  this->hasThickness_ = 0;
  this->hasShrinkage_ = 0;
  this->hasCapMultiplier_ = 0;
  this->hasEdgeCap_ = 0;
  this->hasAntennaArea_ = 0;
  this->hasAntennaLength_ = 0;
  this->hasCurrentDensityPoint_ = 0;
  this->hasUseLengthThreshold_ = 0;
  for (i = 0;
       i < this->numSpacings_;
       i++)
    if (this->spacingName_[i])
      free(this->spacingName_[i]);
  for (i = 0;
       i < this->numProps_;
       i++) {
    free(this->names_[i]);
    free(this->values_[i]);
  }
  this->numProps_ = 0;
  this->numSpacings_ = 0;
  this->numCurrentPoints_ = 0;
  if (this->numAccurrents_) {
    for (i = 0;
	 i < this->numAccurrents_;
	 i++) {
      p = this->accurrents_[i];
      lefiLayerDensity_Destroy(p);
      free((char *) p);
    }
    this->numAccurrents_ = 0;
    this->accurrentAllocated_ = 0;
    free((char *) (this->accurrents_));
    this->accurrents_ = 0;
  }
  if (this->numDccurrents_) {
    for (i = 0;
	 i < this->numDccurrents_;
	 i++) {
      p = this->dccurrents_[i];
      lefiLayerDensity_Destroy(p);
      free((char *) p);
    }
    this->numDccurrents_ = 0;
    this->dccurrentAllocated_ = 0;
    free((char *) (this->dccurrents_));
    this->dccurrents_ = 0;
  }
  this->lengthThreshold_ = 0;
}

void 
lefiLayer_setName(lefiLayer * this,
		  const char *name)
{
  int     len = strlen(name) + 1;

  lefiLayer_clear(this);
  if (len > this->nameSize_) {
    free(this->name_);
    this->name_ = (char *) malloc(len);
    this->nameSize_ = len;
  }
  strcpy(this->name_, CASE(name));
}

void 
lefiLayer_setType(lefiLayer * this,
		  const char *typ)
{
  int     len = strlen(typ) + 1;

  if (len > this->typeSize_) {
    free(this->type_);
    this->type_ = (char *) malloc(len);
    this->typeSize_ = len;
  }
  strcpy(this->type_, CASE(typ));
}

void 
lefiLayer_setPitch(lefiLayer * this,
		   double num)
{
  this->hasPitch_ = 1;
  this->pitch_ = num;
}

void 
lefiLayer_setOffset(lefiLayer * this,
		    double num)
{
  this->hasOffset_ = 1;
  this->offset_ = num;
}

void 
lefiLayer_setWidth(lefiLayer * this,
		   double num)
{
  this->hasWidth_ = 1;
  this->width_ = num;
}

void 
lefiLayer_setWireExtension(lefiLayer * this,
			   double num)
{
  this->hasWireExtension_ = 1;
  this->wireExtension_ = num;
}

void 
lefiLayer_setSpacing(lefiLayer * this,
		     double dist,
		     double left,
		     double right)
{
  if (this->numSpacings_ == this->spacingsAllocated_) {
    double *nd;

    char  **nn;

    int     i, lim;

    if (this->spacingsAllocated_ == 0) {
      lim = this->spacingsAllocated_ = 2;
      nd = (double *) malloc(sizeof(double) * lim);
      nn = (char **) malloc(sizeof(char *) * lim);
    }
    else {
      lim = this->spacingsAllocated_ * 2;
      this->spacingsAllocated_ = lim;
      nd = (double *) malloc(sizeof(double) * lim);
      nn = (char **) malloc(sizeof(char *) * lim);
      lim /= 2;
      for (i = 0;
	   i < lim;
	   i++) {
	nd[i] = this->spacing_[i];
	if (this->spacingName_[i])
	  nn[i] = this->spacingName_[i];
      }
      free((char *) (this->spacing_));
      free((char *) (this->spacingName_));
    }
    this->spacing_ = nd;
    this->spacingName_ = nn;
  }
  this->hasSpacing_ = 1;
  this->spacing_[this->numSpacings_] = dist;
  this->spacingName_[this->numSpacings_] = 0;
  this->numSpacings_ += 1;
  this->left_ = left;
  this->right_ = right;
}

void 
lefiLayer_setSpacingName(lefiLayer * this,
			 const char *spacingName)
{
  if (spacingName) {
    int     len = strlen(spacingName) + 1;

    this->spacingName_[this->numSpacings_ - 1] = (char *) malloc(len);
    strcpy(this->spacingName_[this->numSpacings_ - 1], CASE(spacingName));
  }
}

void 
lefiLayer_setDirection(lefiLayer * this,
		       const char *dir)
{
  this->direction_ = (char *) dir;
  this->hasDirection_ = 1;
}

void 
lefiLayer_setResistance(lefiLayer * this,
			double num)
{
  this->hasResistance_ = 1;
  this->resistance_ = num;
}

void 
lefiLayer_setCapacitance(lefiLayer * this,
			 double num)
{
  this->hasCapacitance_ = 1;
  this->capacitance_ = num;
}

void 
lefiLayer_setHeight(lefiLayer * this,
		    double num)
{
  this->hasHeight_ = 1;
  this->height_ = num;
}

void 
lefiLayer_setThickness(lefiLayer * this,
		       double num)
{
  this->hasThickness_ = 1;
  this->thickness_ = num;
}

void 
lefiLayer_setShrinkage(lefiLayer * this,
		       double num)
{
  this->hasShrinkage_ = 1;
  this->shrinkage_ = num;
}

void 
lefiLayer_setCapMultiplier(lefiLayer * this,
			   double num)
{
  this->hasCapMultiplier_ = 1;
  this->capMultiplier_ = num;
}

void 
lefiLayer_setEdgeCap(lefiLayer * this,
		     double num)
{
  this->hasEdgeCap_ = 1;
  this->edgeCap_ = num;
}

void 
lefiLayer_setAntennaLength(lefiLayer * this,
			   double num)
{
  this->hasAntennaLength_ = 1;
  this->antennaLength_ = num;
}

void 
lefiLayer_setAntennaArea(lefiLayer * this,
			 double num)
{
  this->hasAntennaArea_ = 1;
  this->antennaArea_ = num;
}

void 
lefiLayer_setCurrentDensity(lefiLayer * this,
			    double num)
{
  this->hasCurrentDensityPoint_ = 1;
  this->currentDensity_ = num;
}

void 
lefiLayer_setCurrentPoint(lefiLayer * this,
			  double width,
			  double current)
{
  if (this->numCurrentPoints_ == this->currentPointsAllocated_) {
    int     max = this->numCurrentPoints_;

    int     len;

    int     i;

    double *nc;

    double *nw;

    len = this->currentPointsAllocated_ *= 2;
    nc = (double *) malloc(sizeof(double) * len);
    nw = (double *) malloc(sizeof(double) * len);
    for (i = 0;
	 i < max;
	 i++) {
      nc[i] = this->current_[i];
      nw[i] = this->currentWidths_[i];
    }
    free((char *) (this->current_));
    free((char *) (this->currentWidths_));
    this->current_ = nc;
    this->currentWidths_ = nw;
  }
  this->current_[this->numCurrentPoints_] = current;
  this->currentWidths_[this->numCurrentPoints_] = width;
  this->numCurrentPoints_ += 1;
}

void 
lefiLayer_setResistancePoint(lefiLayer * this,
			     double width,
			     double resistance)
{
  if (this->numResistancePoints_ == this->resistancePointsAllocated_) {
    int     max = this->numResistancePoints_;

    int     len;

    int     i;

    double *nc;

    double *nw;

    len = this->resistancePointsAllocated_ *= 2;
    nc = (double *) malloc(sizeof(double) * len);
    nw = (double *) malloc(sizeof(double) * len);
    for (i = 0;
	 i < max;
	 i++) {
      nc[i] = this->resistances_[i];
      nw[i] = this->resistanceWidths_[i];
    }
    free((char *) (this->resistances_));
    free((char *) (this->resistanceWidths_));
    this->resistances_ = nc;
    this->resistanceWidths_ = nw;
  }
  this->resistances_[this->numResistancePoints_] = resistance;
  this->resistanceWidths_[this->numResistancePoints_] = width;
  this->numResistancePoints_ += 1;
}

void 
lefiLayer_setCapacitancePoint(lefiLayer * this,
			      double width,
			      double capacitance)
{
  if (this->numCapacitancePoints_ == this->capacitancePointsAllocated_) {
    int     max = this->numCapacitancePoints_;

    int     len;

    int     i;

    double *nc;

    double *nw;

    len = this->capacitancePointsAllocated_ *= 2;
    nc = (double *) malloc(sizeof(double) * len);
    nw = (double *) malloc(sizeof(double) * len);
    for (i = 0;
	 i < max;
	 i++) {
      nc[i] = this->capacitances_[i];
      nw[i] = this->capacitanceWidths_[i];
    }
    free((char *) (this->capacitances_));
    free((char *) (this->capacitanceWidths_));
    this->capacitances_ = nc;
    this->capacitanceWidths_ = nw;
  }
  this->capacitances_[this->numCapacitancePoints_] = capacitance;
  this->capacitanceWidths_[this->numCapacitancePoints_] = width;
  this->numCapacitancePoints_ += 1;
}

int 
lefiLayer_hasType(const lefiLayer * this)
{
  return (this->type_[0] != '\0') ? 1 : 0;
}

int 
lefiLayer_hasPitch(const lefiLayer * this)
{
  return this->hasPitch_;
}

int 
lefiLayer_hasOffset(const lefiLayer * this)
{
  return this->hasOffset_;
}

int 
lefiLayer_hasWidth(const lefiLayer * this)
{
  return this->hasWidth_;
}

int 
lefiLayer_hasWireExtension(const lefiLayer * this)
{
  return this->hasWireExtension_;
}

int 
lefiLayer_hasSpacingNumber(const lefiLayer * this)
{
  return ((this->hasSpacing_ != 0) && (this->numSpacings_ > 0)) ? 1 : 0;
}

int 
lefiLayer_hasSpacingRange(const lefiLayer * this)
{
  return ((this->hasSpacing_ != 0) && (this->left_ != this->right_)) ? 1 : 0;
}

int 
lefiLayer_hasDirection(const lefiLayer * this)
{
  return this->hasDirection_;
}

int 
lefiLayer_hasResistance(const lefiLayer * this)
{
  return this->hasResistance_;
}

int 
lefiLayer_hasCapacitance(const lefiLayer * this)
{
  return this->hasCapacitance_;
}

int 
lefiLayer_hasHeight(const lefiLayer * this)
{
  return this->hasHeight_;
}

int 
lefiLayer_hasThickness(const lefiLayer * this)
{
  return this->hasThickness_;
}

int 
lefiLayer_hasShrinkage(const lefiLayer * this)
{
  return this->hasShrinkage_;
}

int 
lefiLayer_hasCapMultiplier(const lefiLayer * this)
{
  return this->hasCapMultiplier_;
}

int 
lefiLayer_hasEdgeCap(const lefiLayer * this)
{
  return this->hasEdgeCap_;
}

int 
lefiLayer_hasAntennaArea(const lefiLayer * this)
{
  return this->hasAntennaArea_;
}

int 
lefiLayer_hasAntennaLength(const lefiLayer * this)
{
  return this->hasAntennaLength_;
}

int 
lefiLayer_hasCurrentDensityPoint(const lefiLayer * this)
{
  return this->hasCurrentDensityPoint_;
}

int 
lefiLayer_hasCurrentDensityArray(const lefiLayer * this)
{
  return this->numCurrentPoints_ ? 1 : 0;
}

int 
lefiLayer_hasResistanceArray(const lefiLayer * this)
{
  return this->numResistancePoints_ ? 1 : 0;
}

int 
lefiLayer_hasCapacitanceArray(const lefiLayer * this)
{
  return this->numCapacitancePoints_ ? 1 : 0;
}

void 
lefiLayer_currentDensityArray(const lefiLayer * this,
			      int *numPoints,
			      double **widths,
			      double **current)
{
  *numPoints = this->numCurrentPoints_;
  *widths = this->currentWidths_;
  *current = this->current_;
}

void 
lefiLayer_resistanceArray(const lefiLayer * this,
			  int *numPoints,
			  double **widths,
			  double **res)
{
  *numPoints = this->numResistancePoints_;
  *widths = this->resistanceWidths_;
  *res = this->resistances_;
}

void 
lefiLayer_capacitanceArray(const lefiLayer * this,
			   int *numPoints,
			   double **widths,
			   double **cap)
{
  *numPoints = this->numCapacitancePoints_;
  *widths = this->capacitanceWidths_;
  *cap = this->capacitances_;
}

int 
lefiLayer_numSpacing(const lefiLayer * this)
{
  return this->numSpacings_;
}

char   *
lefiLayer_name(const lefiLayer * this)
{
  return this->name_;
}

const char *
lefiLayer_type(const lefiLayer * this)
{
  return this->type_;
}

double 
lefiLayer_pitch(const lefiLayer * this)
{
  return this->pitch_;
}

double 
lefiLayer_offset(const lefiLayer * this)
{
  return this->offset_;
}

double 
lefiLayer_width(const lefiLayer * this)
{
  return this->width_;
}

double 
lefiLayer_wireExtension(const lefiLayer * this)
{
  return this->wireExtension_;
}

double 
lefiLayer_spacing(const lefiLayer * this,
		  int index)
{
  return this->spacing_[index];
}

char   *
lefiLayer_spacingName(const lefiLayer * this,
		      int index)
{
  return this->spacingName_[index];
}

double 
lefiLayer_spacingRangeLeft(const lefiLayer * this)
{
  return this->left_;
}

double 
lefiLayer_spacingRangeRight(const lefiLayer * this)
{
  return this->right_;
}

const char *
lefiLayer_direction(const lefiLayer * this)
{
  return this->direction_;
}

double 
lefiLayer_currentDensityPoint(const lefiLayer * this)
{
  return this->currentDensity_;
}

double 
lefiLayer_resistance(const lefiLayer * this)
{
  return this->resistance_;
}

double 
lefiLayer_capacitance(const lefiLayer * this)
{
  return this->capacitance_;
}

double 
lefiLayer_height(const lefiLayer * this)
{
  return this->height_;
}

double 
lefiLayer_thickness(const lefiLayer * this)
{
  return this->thickness_;
}

double 
lefiLayer_shrinkage(const lefiLayer * this)
{
  return this->shrinkage_;
}

double 
lefiLayer_capMultiplier(const lefiLayer * this)
{
  return this->capMultiplier_;
}

double 
lefiLayer_edgeCap(const lefiLayer * this)
{
  return this->edgeCap_;
}

double 
lefiLayer_antennaLength(const lefiLayer * this)
{
  return this->antennaLength_;
}

double 
lefiLayer_antennaArea(const lefiLayer * this)
{
  return this->antennaArea_;
}

void 
lefiLayer_print(const lefiLayer * this,
		FILE * f)
{
  int     i, max;

  double *j;

  double *k;

  fprintf(f, "Layer %s:\n", lefiLayer_name(this));
  if (lefiLayer_hasType(this))
    fprintf(f, " type %s\n", lefiLayer_type(this));
  if (lefiLayer_hasPitch(this))
    fprintf(f, " pitch %lg\n", lefiLayer_pitch(this));
  if (lefiLayer_hasWireExtension(this))
    fprintf(f, " wireextension %lg\n", lefiLayer_wireExtension(this));
  if (lefiLayer_hasWidth(this))
    fprintf(f, " width %lg\n", lefiLayer_width(this));
  if (lefiLayer_hasSpacingNumber(this)) {
    for (i = 0;
	 i < lefiLayer_numSpacing(this);
	 i++)
      fprintf(f, " spacing %lg\n", lefiLayer_spacing(this,
						     i));
  }
  if (lefiLayer_hasSpacingRange(this))
    fprintf(f, " spacing %lg %lg\n", lefiLayer_spacingRangeLeft(this), lefiLayer_spacingRangeRight(this));
  if (lefiLayer_hasDirection(this))
    fprintf(f, " direction %s\n", lefiLayer_direction(this));
  if (lefiLayer_hasResistance(this))
    fprintf(f, " resistance %lg\n", lefiLayer_resistance(this));
  if (lefiLayer_hasResistanceArray(this)) {
    lefiLayer_resistanceArray(this,
			      &max,
			      &j,
			      &k);
    fprintf(f, " resistance PWL");
    for (i = 0;
	 i < max;
	 i++)
      fprintf(f, " %lg %lg", j[i], k[i]);
    fprintf(f, "\n");
  }
  if (lefiLayer_hasCapacitance(this))
    fprintf(f, " capacitance %lg\n", lefiLayer_capacitance(this));
  if (lefiLayer_hasCapacitanceArray(this)) {
    lefiLayer_capacitanceArray(this,
			       &max,
			       &j,
			       &k);
    fprintf(f, " capacitance PWL");
    for (i = 0;
	 i < max;
	 i++)
      fprintf(f, " %lg %lg", j[i], k[i]);
    fprintf(f, "\n");
  }
  if (lefiLayer_hasHeight(this))
    fprintf(f, " height %lg\n", lefiLayer_height(this));
  if (lefiLayer_hasThickness(this))
    fprintf(f, " thickness %lg\n", lefiLayer_thickness(this));
  if (lefiLayer_hasShrinkage(this))
    fprintf(f, " shrinkage %lg\n", lefiLayer_shrinkage(this));
  if (lefiLayer_hasCapMultiplier(this))
    fprintf(f, " cap muptiplier %lg\n", lefiLayer_capMultiplier(this));
  if (lefiLayer_hasEdgeCap(this))
    fprintf(f, " edge cap %lg\n", lefiLayer_edgeCap(this));
  if (lefiLayer_hasCurrentDensityPoint(this))
    fprintf(f, " currentden %lg\n", lefiLayer_currentDensityPoint(this));
  if (lefiLayer_hasCurrentDensityArray(this)) {
    lefiLayer_currentDensityArray(this,
				  &max,
				  &j,
				  &k);
    fprintf(f, " currentden PWL");
    for (i = 0;
	 i < max;
	 i++)
      fprintf(f, " %lg %lg", j[i], k[i]);
    fprintf(f, "\n");
  }
}

void 
lefiLayer_addProp(lefiLayer * this,
		  const char *name,
		  const char *value)
{
  int     len = strlen(name) + 1;

  if (this->numProps_ == this->propsAllocated_) {
    int     i;

    int     max;

    int     lim = this->numProps_;

    char  **nn;

    char  **nv;

    max = this->propsAllocated_ *= 2;
    nn = (char **) malloc(sizeof(char *) * max);
    nv = (char **) malloc(sizeof(char *) * max);
    for (i = 0;
	 i < lim;
	 i++) {
      nn[i] = this->names_[i];
      nv[i] = this->values_[i];
    }
    free((char *) (this->names_));
    free((char *) (this->values_));
    this->names_ = nn;
    this->values_ = nv;
  }
  this->names_[this->numProps_] = (char *) malloc(sizeof(char) * len);
  strcpy(this->names_[this->numProps_], name);
  len = strlen(value) + 1;
  this->values_[this->numProps_] = (char *) malloc(sizeof(char) * len);
  strcpy(this->values_[this->numProps_], value);
  this->numProps_ += 1;
}

int 
lefiLayer_numProps(const lefiLayer * this)
{
  return this->numProps_;
}

const char *
lefiLayer_propName(const lefiLayer * this,
		   int i)
{
  return this->names_[i];
}

const char *
lefiLayer_propValue(const lefiLayer * this,
		    int i)
{
  return this->values_[i];
}

void 
lefiLayer_addAccurrentDensity(lefiLayer * this,
			      const char *type)
{
  lefiLayerDensity *density;

  if (this->numAccurrents_ == this->accurrentAllocated_) {
    lefiLayerDensity **array;

    int     i;

    this->accurrentAllocated_ = this->accurrentAllocated_ ? this->accurrentAllocated_ * 2 : 2;
    array = (lefiLayerDensity **) malloc(sizeof(lefiLayerDensity *) * this->accurrentAllocated_);
    for (i = 0;
	 i < this->numAccurrents_;
	 i++)
      array[i] = this->accurrents_[i];
    if (this->accurrents_)
      free((char *) (this->accurrents_));
    this->accurrents_ = array;
  }
  density = this->accurrents_[this->numAccurrents_] = (lefiLayerDensity *) malloc(sizeof(lefiLayerDensity));
  this->numAccurrents_ += 1;
  lefiLayerDensity_Init(density,
			type);
}

void 
lefiLayer_setAcOneEntry(lefiLayer * this,
			double num)
{
  lefiLayerDensity *density;

  density = this->accurrents_[this->numAccurrents_ - 1];
  lefiLayerDensity_setOneEntry(density,
			       num);
}

void 
lefiLayer_addAcFrequency(lefiLayer * this)
{
  lefiLayerDensity *density;

  density = this->accurrents_[this->numAccurrents_ - 1];
  lefiLayerDensity_addFrequency(density,
				this->numNums_,
				this->nums_);
  this->numNums_ = 0;
  this->numAllocated_ = 0;
  this->nums_ = 0;
}

void 
lefiLayer_addAcTableEntry(lefiLayer * this)
{
  lefiLayerDensity *density;

  density = this->accurrents_[this->numAccurrents_ - 1];
  lefiLayerDensity_addTableEntry(density,
				 this->numNums_,
				 this->nums_);
  this->numNums_ = 0;
  this->numAllocated_ = 0;
  this->nums_ = 0;
}

void 
lefiLayer_addAcWidth(lefiLayer * this)
{
  lefiLayerDensity *density;

  density = this->accurrents_[this->numAccurrents_ - 1];
  lefiLayerDensity_addWidth(density,
			    this->numNums_,
			    this->nums_);
  this->numNums_ = 0;
  this->numAllocated_ = 0;
  this->nums_ = 0;
}

void 
lefiLayer_setDcOneEntry(lefiLayer * this,
			double num)
{
  lefiLayerDensity *density;

  density = this->dccurrents_[this->numDccurrents_ - 1];
  lefiLayerDensity_setOneEntry(density,
			       num);
}

void 
lefiLayer_addDccurrentDensity(lefiLayer * this,
			      const char *type)
{
  lefiLayerDensity *density;

  if (this->numDccurrents_ == this->dccurrentAllocated_) {
    lefiLayerDensity **array;

    int     i;

    this->dccurrentAllocated_ = this->dccurrentAllocated_ ? this->dccurrentAllocated_ * 2 : 2;
    array = (lefiLayerDensity **) malloc(sizeof(lefiLayerDensity *) * this->dccurrentAllocated_);
    for (i = 0;
	 i < this->numDccurrents_;
	 i++)
      array[i] = this->dccurrents_[i];
    if (this->dccurrents_)
      free((char *) (this->dccurrents_));
    this->dccurrents_ = array;
  }
  density = this->dccurrents_[this->numDccurrents_] = (lefiLayerDensity *) malloc(sizeof(lefiLayerDensity));
  this->numDccurrents_ += 1;
  lefiLayerDensity_Init(density,
			type);
}

void 
lefiLayer_addDcCutarea(lefiLayer * this)
{
  lefiLayerDensity *density;

  density = this->dccurrents_[this->numDccurrents_ - 1];
  lefiLayerDensity_addCutarea(density,
			      this->numNums_,
			      this->nums_);
  this->numNums_ = 0;
  this->numAllocated_ = 0;
  this->nums_ = 0;
}

void 
lefiLayer_addDcTableEntry(lefiLayer * this)
{
  lefiLayerDensity *density;

  density = this->dccurrents_[this->numDccurrents_ - 1];
  lefiLayerDensity_addTableEntry(density,
				 this->numNums_,
				 this->nums_);
  this->numNums_ = 0;
  this->numAllocated_ = 0;
  this->nums_ = 0;
}

void 
lefiLayer_addDcWidth(lefiLayer * this)
{
  lefiLayerDensity *density;

  density = this->dccurrents_[this->numDccurrents_ - 1];
  lefiLayerDensity_addWidth(density,
			    this->numNums_,
			    this->nums_);
  this->numNums_ = 0;
  this->numAllocated_ = 0;
  this->nums_ = 0;
}

void 
lefiLayer_addNumber(lefiLayer * this,
		    double num)
{
  if (this->numNums_ == this->numAllocated_) {
    double *array;

    int     i;

    this->numAllocated_ = this->numAllocated_ ? this->numAllocated_ * 2 : 2;
    array = (double *) malloc(sizeof(double) * this->numAllocated_);
    for (i = 0;
	 i < this->numNums_;
	 i++)
      array[i] = this->nums_[i];
    if (this->nums_)
      free((char *) (this->nums_));
    this->nums_ = array;
  }
  this->nums_[this->numNums_++] = num;
}

int 
lefiLayer_hasAccurrentDensity(const lefiLayer * this)
{
  return this->numAccurrents_ ? 1 : 0;
}

int 
lefiLayer_hasDccurrentDensity(const lefiLayer * this)
{
  return this->numDccurrents_ ? 1 : 0;
}

int 
lefiLayer_numAccurrentDensity(const lefiLayer * this)
{
  return (this->numAccurrents_);
}

int 
lefiLayer_numDccurrentDensity(const lefiLayer * this)
{
  return (this->numDccurrents_);
}

lefiLayerDensity *
lefiLayer_accurrent(const lefiLayer * this,
		    int index)
{
  if (index >= this->numAccurrents_)
    return 0;
  return (this->accurrents_[index]);
}

lefiLayerDensity *
lefiLayer_dccurrent(const lefiLayer * this,
		    int index)
{
  if (index >= this->numDccurrents_)
    return 0;
  return (this->dccurrents_[index]);
}

void 
lefiLayer_setSpacingUseLength(lefiLayer * this)
{
  this->hasUseLengthThreshold_ = 1;
}

void 
lefiLayer_setSpacingLength(lefiLayer * this,
			   double num)
{
  this->lengthThreshold_ = num;
  this->left_ = -1;
}

int 
lefiLayer_hasSpacingUseLengthThreshold(lefiLayer * this)
{
  return this->hasUseLengthThreshold_ ? 1 : 0;
}

int 
lefiLayer_hasSpacingLengthThreshold(lefiLayer * this)
{
  return this->lengthThreshold_ ? 1 : 0;
}

double 
lefiLayer_spacingLengthThreshold(const lefiLayer * this)
{
  return this->lengthThreshold_;
}

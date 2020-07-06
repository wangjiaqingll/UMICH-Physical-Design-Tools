/*
 *     This  file  is  part  of  the  Cadence  LEF/DEF  Open   Source
 *  Distribution,  Product Version 5.3, and is subject to the Cadence
 *  LEF/DEF Open Source License Agreement.   Your  continued  use  of
 *  this file constitutes your acceptance of the terms of the LEF/DEF
 *  Open Source License and an agreement to abide by its  terms.   If
 *  you  don't  agree  with  this, you must remove this and any other
 *  files which are part of the distribution and  destroy any  copies
 *  made.
 *
 *     For updates, support, or to become part of the LEF/DEF Community,
 *  check www.openeda.org for details.
 */

#ifndef lefiLayer_h
#define lefiLayer_h

#include <stdio.h>
#include "lefiKRDefs.hpp"

class lefiLayerDensity {
public:
  lefiLayerDensity();
  ~lefiLayerDensity();

  void Init(const char* type);
  void Destroy();
  void clear();

  void setOneEntry(double entry);
  void addFrequency(int num, double* frequency);
  void addWidth(int num, double* width);
  void addTableEntry(int num, double* entry);
  void addCutarea(int num, double* cutarea);

  char* type();
  int hasOneEntry();
  double oneEntry();
  int numFrequency();
  double frequency(int index);
  int numWidths();
  double width(int index);
  int numTableEntries();
  double tableEntry(int index);
  int numCutareas();
  double cutArea(int index);

protected:
  char* type_;
  double oneEntry_;
  int numFrequency_;
  double* frequency_;
  int numWidths_;
  double* widths_;
  int numTableEntries_;
  double* tableEntries_;
  int numCutareas_;
  double* cutareas_;
};

class lefiLayer {
public:
  lefiLayer();
  void Init();

  void Destroy();
  ~lefiLayer();

  void clear();
  void setName(const char* name); // calls clear to init
  void setType(const char* typ);
  void setPitch(double num);
  void setOffset(double num);
  void setWidth(double num);
  void setSpacing(double dist, double left, double right);
  void setSpacingName(const char* spacingName);
  void setDirection(const char* dir);
  void setResistance(double num);
  void setCapacitance(double num);
  void setHeight(double num);
  void setWireExtension(double num);
  void setThickness(double num);
  void setShrinkage(double num);
  void setCapMultiplier(double num);
  void setEdgeCap(double num);
  void setAntennaArea(double num);
  void setAntennaLength(double num);
  void setCurrentDensity(double num);
  void setCurrentPoint(double width, double current);
  void setResistancePoint(double width, double res);
  void setCapacitancePoint(double width, double cap);
  void addProp(const char* name, const char* value);
  void addAccurrentDensity(const char* type);
  void setAcOneEntry(double num);
  void addAcFrequency();
  void addAcTableEntry();
  void addAcWidth();
  void addDccurrentDensity(const char* type);
  void setDcOneEntry(double num);
  void addDcTableEntry();
  void addDcWidth();
  void addDcCutarea();
  void addNumber(double num);
  void setSpacingUseLength();
  void setSpacingLength(double num);

  int hasType() const ;
  int hasPitch() const ;
  int hasOffset() const ;
  int hasWidth() const ;
  int hasSpacingNumber() const ;
  int hasSpacingRange() const ;
  int hasDirection() const ;
  int hasResistance() const ;
  int hasResistanceArray() const ;
  int hasCapacitance() const ;
  int hasCapacitanceArray() const ;
  int hasHeight() const ;
  int hasThickness() const ;
  int hasWireExtension() const ;
  int hasShrinkage() const ;
  int hasCapMultiplier() const ;
  int hasEdgeCap() const ;
  int hasAntennaLength() const ;
  int hasAntennaArea() const ;
  int hasCurrentDensityPoint() const ;
  int hasCurrentDensityArray() const ;
  int hasAccurrentDensity() const;
  int hasDccurrentDensity() const;
  int hasSpacingUseLengthThreshold();
  int hasSpacingLengthThreshold();

  int numProps() const;
  const char* propName(int index) const;
  const char* propValue(int index) const;

  int numSpacing() const;

  char* name() const ;
  const char* type() const ;
  double pitch() const ;
  double offset() const ;
  double width() const ;
  double spacing(int index) const ;
  char*  spacingName(int index) const ;
  double spacingRangeLeft() const ;
  double spacingRangeRight() const ;
  const char* direction() const ;
  double resistance() const ;
  double capacitance() const ;
  double height() const ;
  double wireExtension() const ;
  double thickness() const ;
  double shrinkage() const ;
  double capMultiplier() const ;
  double edgeCap() const ;
  double antennaLength() const ;
  double antennaArea() const ;
  double currentDensityPoint() const ;
  void currentDensityArray(int* numPoints,
	   double** widths, double** current) const ;
  void capacitanceArray(int* numPoints,
	   double** widths, double** resValues) const ;
  void resistanceArray(int* numPoints,
	   double** widths, double** capValues) const ;

  int numAccurrentDensity() const;
  lefiLayerDensity* accurrent(int index) const;
  int numDccurrentDensity() const;
  lefiLayerDensity* dccurrent(int index) const;

  double spacingLengthThreshold() const;

  // Debug print
  void print(FILE* f) const;

protected:
  char* name_;
  int nameSize_;
  char* type_;
  int typeSize_;

  int hasPitch_;
  int hasOffset_;
  int hasWidth_;
  int hasSpacing_;
  int hasDirection_;
  int hasResistance_;
  int hasCapacitance_;
  int hasHeight_;
  int hasWireExtension_;
  int hasThickness_;
  int hasShrinkage_;
  int hasCapMultiplier_;
  int hasEdgeCap_;
  int hasAntennaArea_;
  int hasAntennaLength_;
  int hasCurrentDensityPoint_;
  int hasUseLengthThreshold_;

  double currentDensity_;
  double pitch_;
  double offset_;
  double width_;
  double wireExtension_;
  int numSpacings_;
  int spacingsAllocated_;
  double* spacing_;       // for Cut Layer, spacing is multiple
  char** spacingName_;
  double left_;
  double right_;
  char* direction_;
  double resistance_;
  double capacitance_;
  double height_;
  double thickness_;
  double shrinkage_;
  double capMultiplier_;
  double edgeCap_;
  double antennaArea_;
  double antennaLength_;

  int numCurrentPoints_;
  int currentPointsAllocated_;
  double* currentWidths_;
  double* current_;

  int numCapacitancePoints_;
  int capacitancePointsAllocated_;
  double* capacitanceWidths_;
  double* capacitances_;

  int numResistancePoints_;
  int resistancePointsAllocated_;
  double* resistanceWidths_;
  double* resistances_;

  int numProps_;
  int propsAllocated_;
  char** names_;
  char** values_;

  int numAccurrents_;                 // number of ACCURRENTDENSITY
  int accurrentAllocated_;
  lefiLayerDensity** accurrents_;
  int numDccurrents_;                 // number of DCCURRENTDENSITY
  int dccurrentAllocated_;
  lefiLayerDensity** dccurrents_;
  int numNums_;
  int numAllocated_;
  double* nums_;

  double lengthThreshold_;
};

#endif

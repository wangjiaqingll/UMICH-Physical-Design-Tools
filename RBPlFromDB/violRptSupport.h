/**************************************************************************
***    
*** Copyright (c) 1995-2000 Regents of the University of California,
***               Andrew E. Caldwell, Andrew B. Kahng and Igor L. Markov
*** Copyright (c) 2000-2002 Regents of the University of Michigan,
***               Saurabh N. Adya and Igor L. Markov
***
***  Contact author(s): abk@cs.ucsd.edu, imarkov@umich.edu
***  Original Affiliation:   UCLA, Computer Science Department,
***                          Los Angeles, CA 90095-1596 USA
***
***  Permission is hereby granted, free of charge, to any person obtaining 
***  a copy of this software and associated documentation files (the
***  "Software"), to deal in the Software without restriction, including
***  without limitation 
***  the rights to use, copy, modify, merge, publish, distribute, sublicense, 
***  and/or sell copies of the Software, and to permit persons to whom the 
***  Software is furnished to do so, subject to the following conditions:
***
***  The above copyright notice and this permission notice shall be included
***  in all copies or substantial portions of the Software.
***
*** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
*** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
*** OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
*** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
*** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
*** OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
*** THE USE OR OTHER DEALINGS IN THE SOFTWARE.
***
***
***************************************************************************/






//990112	aec	pulled from violationReport.cxx

#ifndef __VIOLRPT_SUPPORT_H__
#define __VIOLRPT_SUPPORT_H__

#include <functional>
#include "ABKCommon/abkcommon.h"
#include "Placement/placement.h"
#include "DB/DB.h"
#include "RBPlace/RBPlacement.h"
#include "violationReport.h"


/******** function signatures **************/

// The function below called by applyToRegion (therefore not a member of
// violationReport.
       void logError1 (unsigned&, unsigned, std::vector <Violations>&,
                      const Violations&, unsigned, const dbNet&, const char *);
 
static bool isLegal(const Symmetry& cellSym, const Orientation& cellOri);
 
static bool isLegal (const Orientation& rowOri,
                     const Symmetry& siteSym,
                     const Orientation& cellOri);
 
/********* function definitions *************/


/*
   This class lets us binary search on vectors of cell indices, to
   find those in an x-coordinate region, where the x-bound may
   not be the location of a known cell.
*/
class cellXcompare
{
public:  // these are public so mapping function can change them...
  const Placement* _pl;
  double _ref;
public:
  cellXcompare (const Placement* pl, double ref) { _pl = pl; _ref = ref; }
  bool operator() (unsigned index1, unsigned index2)
  {
    return 
      ((index1 != InvalidUnsigned) ? (*_pl)[index1].x : _ref)
        < ((index2 != InvalidUnsigned) ? (*_pl)[index2].x : _ref);
  }
};

template<class kind>
class unaryPredicate : public std::unary_function<kind, bool>
{
public:
  bool operator() (kind) {return true;}
};


class applyToRegionPred : public unaryPredicate<unsigned>
{
public:
  virtual bool operator() (unsigned) = 0;
};

class wireTestFunctor : public applyToRegionPred
{
protected:
  const dbLayerGeom& _lGeom;
  const DB& _db;
  unsigned _verbose;
  std::vector <Violations>& _violations;
  unsigned& _errors;
  const dbNet& _snet;
  const ViolationReport & _violReport;
  
public:
  
  wireTestFunctor (const DB& db, 
                   unsigned& errors,
                   unsigned verbose,
                   std::vector <Violations>& violations,
                   const dbLayerGeom& lGeom,
                   const dbNet& snet,
                   const ViolationReport& violReport)
    : _lGeom(lGeom), _db(db), _verbose(verbose), _violations(violations), 
      _errors(errors), _snet(snet), _violReport(violReport)
  {}
  
  bool operator() (unsigned cellID)
  {
    if (_violReport.checkOverlap(cellID, _lGeom))
      logError1(_errors, _verbose, _violations, Violations("wOverlap"),
               cellID, _snet, "Cell overlaps a special net");
    return true;
  }
};


#endif

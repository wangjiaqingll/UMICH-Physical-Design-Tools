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









// Created 970814 by Igor Markov, VLSI CAD ABKGROUP, UCLA/CS

// 970815 ilm  added bool dbSpatial::allCellsPlaced() const
// 970823 ilm  changed dbSpatial::orientations from vector<bool> to bit_vector

//======================================================================
//                          Spatial Package
//====================================================================== 
#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <vector>
using std::vector;
#include "ABKCommon/abkcommon.h"
#include "Placement/placement.h"
#include "Combi/mapping.h"
#include "Constraints/constraints.h"
#include "dbDefines.h"
#include "dbSpatial.h"

bool dbSpatial::isConsistent() const
{
    bool IamFine=true;
    if (locations.getSize()!=locationValid.size()  ||
        locations.getSize()!=orientations.size())          IamFine=false;
    abkwarn(IamFine,"Spatial section inconsistent : size mismatch\n");
    return IamFine;
}

void dbSpatial::reorderCells(const Permutation& perm)
{
   locations.reorder(perm);  
   reorderBitVector(locationValid,perm);  
   reorderVector(orientations,perm);  
   constraints.reorderSubPlWith(perm);
}

bool dbSpatial::allCellsPlaced() const
{
  for(unsigned k=0;k<locationValid.size();k++) 
     if (!locationValid[k]) return false;
  return true;
}


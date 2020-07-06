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









// Created 970716 by Igor Markov, VLSI CAD ABKGROUP, UCLA/CS
// 970814 ilm  moved implementations into dbSpatial.cxx
// 970823 ilm  changed dbSpatial::orientatiol from vector<bool> to bit_vector

//======================================================================
//                          Spatial Package
//====================================================================== 

#ifndef _DBSPATIAL_H_
#define _DBSPATIAL_H_

#include <vector>
using std::vector;
#if !defined(__GNUC__) && !defined(bit_vector)
#define bit_vector vector<bool>
#endif

#include "ABKCommon/abkcommon.h"
#include "Placement/placement.h"
#include "Placement/symOri.h"
#include "Combi/mapping.h"
#include "Constraints/constraints.h"
#include "dbDefines.h"

class dbSpatial;

//========================== CLASS INTERFACES =============================

class dbSpatial
{
    public:
        dbSpatial() {};
        // parser will directly populate most of the below
        // recommendation: create the vectors and then assign
        // them to private fields of dbContext;
 
        // copy-ctor and assignment are prohibited: see below
        
        //The next 3 fields are all indexed by cell index.
        //locationsValid[i] should be set true if the corresponding
        //DEF COMPONENTS stmt says PLACED, FIXED or COVER.
        //Furthermore, if it says FIXED or COVER then a corresponding
        //constraint should be added to the member constraints.

        Placement           locations;
        bit_vector          locationValid;  
        vector<Orientation> orientations; // orientations

        Constraints         constraints;  // defined in the Constraints package
                                          // each individual constraint within
                                          // this object will reference the
                                          // member "locations" defined above

        unsigned            getSize()   const  { return locations.getSize();}
        
        bool                isConsistent()  const;

        void                reorderCells(const Permutation&);

        bool                allCellsPlaced() const;

public: // this is what you can't do  --
        // default copy-ctor and assignment are wrong

        dbSpatial(const dbSpatial&)
                 { abkfatal(0,"Can't copy-construct dbSpatial"); }
        dbSpatial& operator=(const dbSpatial&) 
                 { abkfatal(0,"Can't assign dbSpatial"); return *this; }

};

#endif

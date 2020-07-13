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





/*
   1998.3.6  Devendra Vidhani

   A violationReport indicates illegalities detected for each
   cell in an RBPlacement.
*/


#ifndef _VIOLREPORT_H_
#define _VIOLREPORT_H_

#include <vector>
#include "ABKCommon/abkcommon.h"
#include "RBPlace/RBPlacement.h"
#include "RBPlace/violations.h"


class ViolationReport 
{
    const DB& 		   _db;
    RBPlacement*           _rbplace;
    const PlacementWOrient _pl;

    std::vector <Violations>    _violations;
    unsigned               _numViols;
    Verbosity              _verbosity;


    void 	_checkLegality();
    unsigned 	_isPlacementRowBased();

    void _checkHorizontalOverlap();
    void _checkCellOrientation ();
    void _checkOverlapLayer ();
    void _checkSpecialWiring ();
    void _recheckSpecialWiring ();
    bool _checkOverlapObstacles (unsigned lowerCell, unsigned upperCell) const;
    bool _checkOverlap(unsigned cellID, const dbNet& snet) const;

    void logError (const Violations&, unsigned, const RBPSubRow&, const char*);
    
    void logError (const Violations&,unsigned,unsigned,const RBPSubRow&,
		   const char*);

    void logError (const Violations&, unsigned, const Orientation &,
                   const Symmetry&, const Orientation&, const Symmetry&,
		   const char*);
    
    void logError (const Violations&, unsigned, const Symmetry&,
                   const Orientation&, const char *);
    void logError (const Violations&, unsigned, unsigned, const char *);
    void logError (const Violations&, unsigned, const dbNet&, const char *);
    void logError (const Violations&, unsigned, const char *);
    
public:

    ViolationReport (const DB& db, Verbosity verbosity = Verbosity("1_0_0"));

    ViolationReport (const DB& db, RBPlacement &rowBasedStructure,
                     Verbosity verbosity = Verbosity("1_0_0"));

    bool checkOverlap(unsigned cellID, const dbLayerGeom& lGeom) const;
  
    const Violations & operator[](unsigned cellId) const
    { return _violations[cellId]; }

    bool hasViolations () const { return (_numViols == 0); }

    unsigned getNumViols () const { return _numViols; }

    void clearCell (unsigned i)
    {
	if(_violations[i].hasViolations())
        {
            _violations[i].clearAll();
            _numViols--;
	}
    }
};

#endif

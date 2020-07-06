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











// Created by Igor Markov, Aug 17, 1997 (from dbPostProc.h)
//	CHANGES
// 970819  aec  added CompareNetsByDegree:: operator()
// 970819  mro  added #pragma disable
// 970825  ilm  merged in the static member initializers from dbDesign
// 970826  reworked dbNet::~dbNet()
// 970905  aec  added calculating _numSources to induceCellsFromPins
// 970906  aec  removed numSources, added numOut and numInOut
// 971208  mro  changed tie-breaker in CompareNetsByCellIndex
// 971209  mro  added CompareNetsByName class to sort special nets

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <map>
#include <functional>
#include "dbDefines.h"
#include "Placement/symOri.h"          // Symmetry and Orientation classes
#include "dbTypes.h"
#include "dbCell.h"
#include "dbPin.h"
#include "dbNet.h"
#include "dbContext.h"
#include "dbCommonPin.h"

char dbNet::_forAllCellsFlag = false;
char dbNet::_forAllPinsFlag  = false;

#ifdef _MSC_VER
#pragma warning (disable : 4786)
#endif
using std::map;
using std::less;

dbNet::~dbNet()
{
  delete[] _name;
  unsigned k,size=_wiring.size();
  for(k=0;k<size;k++) delete _wiring[k];
}

void dbNet::induceCellsFromPins()
{
        //to be called by the post-processor.  sets the vector of cell*s,
        //from the vector of pin*s, and sorts the cells..sources first.

    std::map<const dbCell*, int, less<const dbCell*> > sortCells;
    std::map<const dbCell*, int, less<const dbCell*> >::iterator  cellItr;
    itPinLocal     pinItr;
    const dbCell*  curCell;

    for(pinItr = pinsBegin() ; pinItr != pinsEnd(); pinItr++)
    {
        curCell = &( (*pinItr)->getCell() );
        if(sortCells.find(curCell) == sortCells.end() )
        {
            if( (*pinItr)->isDirInput() )
                sortCells[curCell] = 0;

            else if( (*pinItr)->isDirInOut() )
                sortCells[curCell] = 1;

            else if( (*pinItr)->isDirOutput() || 
		     (*pinItr)->isDirOutputTristate() )
                sortCells[curCell] = 2;
	    else	//unknown
		sortCells[curCell] = 3;

        } else {
                //this cell has 2+ pins on the net.
	    if(sortCells[curCell] == 3)
	    {	//go with whatever this is:
		if( (*pinItr)->isDirInput() )
                sortCells[curCell] = 0;
 
            	else if( (*pinItr)->isDirInOut() )
                    sortCells[curCell] = 1;
 
            	else if( (*pinItr)->isDirOutput() || 
                         (*pinItr)->isDirOutputTristate() )
                    sortCells[curCell] = 2; 
            	else        //unknown
                    sortCells[curCell] = 3;

	    }else if( (*pinItr)->isDirInput() && (sortCells[curCell] == 2) )
                sortCells[curCell] = 1; //make it a type inout.

	    else if(((*pinItr)->isDirOutput() || 
		     (*pinItr)->isDirOutputTristate()) && 
		    sortCells[curCell] == 0)
		sortCells[curCell] = 1;

            else if( (*pinItr)->isDirInOut() )
                sortCells[curCell] = 1;

            //otherwise, leave it as it is.
        }
    }

        //copy the map into the vector.  outputs, inouts, inputs, unknowns.
    _numOut = _numInOut = 0;
	
    for(cellItr = sortCells.begin(); cellItr != sortCells.end(); cellItr++)
        if( (*cellItr).second == 2 )
 	{
            _cells.push_back( (dbCell *)((*cellItr).first) );
	    _numOut++;
	}

    for(cellItr = sortCells.begin(); cellItr != sortCells.end(); cellItr++)
        if( (*cellItr).second == 1 )
	{
            _cells.push_back( (dbCell *)((*cellItr).first) );
	    _numInOut++;
	}
    for(cellItr = sortCells.begin(); cellItr != sortCells.end(); cellItr++)
        if( (*cellItr).second == 0 )
            _cells.push_back( (dbCell *)((*cellItr).first) );

    for(cellItr = sortCells.begin(); cellItr != sortCells.end(); cellItr++)
        if( (*cellItr).second == 3 )
            _cells.push_back( (dbCell *)((*cellItr).first) ); 
}

bool CompareNetsByCellIndex:: operator()( dbNet* n1, dbNet* n2) const
{
    if(n1->getNumPins() == 0)
    {
        if(n2->getNumPins() == 0)
            return (n1->getIndex() < n2->getIndex() );
        else
            return false;   //'empty' nets always go last
    }

    if(n2->getNumPins() == 0) return true;
 
    itCellLocal c1Itr, c2Itr;
 
    for(c1Itr = n1->cellsBegin(), c2Itr = n2->cellsBegin();
            (c1Itr != n1->cellsEnd()) && (c2Itr != n2->cellsEnd());
            c1Itr++, c2Itr++)
    {
        if( (*c1Itr)->getIndex() < (*c2Itr)->getIndex() ) return true;
        if( (*c1Itr)->getIndex() > (*c2Itr)->getIndex() ) return false;
    }       //if same..loop

    if( c1Itr == n1->cellsEnd() && c2Itr == n2->cellsEnd() )
             return(strcasecmp(n1->getName(),n2->getName())<0 );//tie-breaker
 
    if( c1Itr == n1->cellsEnd() ) return true;
        else return false;
}

bool CompareNetsByDegree:: operator()( dbNet* n1, dbNet* n2) const
{
    if(n1->getNumPins() > n2->getNumPins() )
	return true;

    else if(n2->getNumPins() > n1->getNumPins() )
	return false;

    else 
	return (n1->getIndex() < n2->getIndex() );
}
	
bool CompareNetsByName:: operator()( dbNet* n1, dbNet* n2) const
{
    int result = strcasecmp(n1->getName(),n2->getName());

    if (result<0) return true;
    else if (result>0) return false;
    else return strcmp(n1->getName(),n2->getName()) <0 ;
}	

bool CompareNetsByDirection:: operator()( dbNet* n1, dbNet* n2) const
{	//sort nets first by direction..then by index
	//what happens if a net is connected to the cell by more than on	
	//pin????

    const dbPin* p1 = _cell.commonPin(n1);
    const dbPin* p2 = _cell.commonPin(n2);

    if(p1->isDirOutput() || p1->isDirOutputTristate())
    {
	if(p2->isDirOutput() || p2->isDirOutputTristate())
	    return n1->getIndex() < n2->getIndex();
	else
	    return true;
    }
    else if(p2->isDirOutput() || p2->isDirOutputTristate())
	return false;

    if(p1->isDirInOut())
    {
	if(p2->isDirInOut())
	    return n1->getIndex() < n2->getIndex();
	else
	    return true;
    }
    else if(p2->isDirInOut() )
	return false;

    if(p1->isDirInput())
    {
	if(p2->isDirInput())
	    return n1->getIndex() < n2->getIndex();
	else
	    return true;
    }
    else if(p2->isDirInput() )
	return false;

    return n1->getIndex() < n2->getIndex();
}

void dbNet::removeDupPins()
{
    // it's possible to put the same pin on the net in
    // both the PINS section and the NETS section (e.g.
    // matsushita_blk1.  So here we remove dups.  001012 mro

    std::sort(_pins.begin(),_pins.end(),less<dbPin*>());
    std::vector<dbPin*>::iterator  dupItr =
        std::unique(_pins.begin(),_pins.end());
    _pins.erase(dupItr,_pins.end());
}

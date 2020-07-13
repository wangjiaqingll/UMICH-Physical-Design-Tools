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



// Created 970814 by Igor Markov, ABKGroup VLSI CAD UCLA/CS

//      CHANGES
//
// 970806 aec  added checking that each net on each cell is in the 'big' vector
//              changed output from cerr to abkwarn
// 970813 aec  added checking numSCs and numCCs
// 970815 aec  moved consist check from conchk.cxx
// 970818 ilm  LGC files now allow for comments
// 970819 aec  added loop functions for SNets
// 970819 aec  added halfPerimEval()
//	       changed postProcessor to remove degree1 nets.	
// 970820 ilm  slight correction to halfPerimEval()
// 970820 mro  added const_casts to _nets.erase() call if _MSC_VER
// 970822 aec  postProcessor populates _pinLib and _pins
//        aec  isConsistent checks that each pin is in _pins and
//		each M. Pin is in _pinLib
//        aec  sets into sorted vectors in isConsistent
// 970825 aec  added orientations to halfPerim private function
// 970525 ilm  merged in the static member initializers from dbDesign
// 970826 ilm  renamed halfPerimEval() into evalHalfPerimCost()
// 970826 ilm  added evalMSTCost()
// 970827 aec  added sorting of dbMasterCell::obst. to dbNetlist::postProcess
// 970827 ilm  moved the above into DBpostProc.cxx
// 970907 aec  moved findLEQ/EEQ functions here, from dbNetlist.h,
//		reworked these to use binary_search
// 970911 aec  added evalHalfPerim functions for single net, and one
//               without pinlocations.  Copied Maogang's pinlocation
//               code into HP and MST eval functions
// 970914 aec  changed processLGC to be case-insensative
// 970920 mro  changes to LEQ and EEQ classes. They do not include EQs of
//		size 1.
// 971115 ilm  added dbNetlist::evalWHalfPerimWireLength() 
//             for weighted wirelength
// 971130 ilm  dbNetlist::evalHalfPerimCost() for nets is now returning
//             half-perim as double, as opposite to the whole BBox
// 971130 ilm  corrected a dozen const-correctness bugs in 
//             wirelength cost evaluators in dbNetlist
// 980121 ilm  added optional thresholding to WL estimators
//             as well as IONetsOnly flag
// 980223 mro  added deletion of degree-1 nets scheduled to be erased
//             in postProcess().
// 980393 ilm  added #include <stdio.h> to remove it from the header
// 020811 ilm  ported to g++ 3.0
#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif


#include <stdio.h>
#include <fstream>
#include <set>
using std::set;
#include <vector>
using std::vector;
using std::bit_vector;
#include <algorithm>
using std::sort;
using std::binary_search;
#include <functional>
using std::less;
#include "ABKCommon/abkcommon.h"
#include "Stats/stats.h"
#include "dbDefines.h"
#include "dbTypes.h"
#include "dbCell.h"
#include "dbNet.h"
#include "dbPin.h"
#include "dbNetlist.h"

char dbNetlist::_forAllCellsFlag = false;
char dbNetlist::_forAllNetsFlag  = false;

dbNetlist::~dbNetlist()
{
    unsigned k;
    for(k=0;k<_cellLib.size();k++) delete _cellLib[k];
    for(k=0;k<_pinLib .size();k++) delete  _pinLib[k];
    for(k=0;k<_cells  .size();k++) delete  _cells [k];
    for(k=0;k<_pins   .size();k++) delete  _pins  [k];
    for(k=0;k<_nets   .size();k++) delete  _nets  [k];
    for(k=0;k<_sNets  .size();k++) delete  _sNets [k];
}

void dbNetlist::processLGC( ifstream &lgcf,const Verbosity &verbose)
{
    //for each entry in the LGC file, set that master cell's CoreClass.
    char MCname[100], MCclass[100];
    itMasterCell mcItr;
    bool found;
    bit_vector alreadyMatched(_cellLib.size(), false);
    unsigned mcIndex;

    int lineNo=1;

    while(!lgcf.eof()  && (lgcf.peek() != -1) && lgcf )
    {
        lgcf >> eathash(lineNo) >> MCname >> noeol(lineNo)
             >> MCclass >> skiptoeol; 
        lineNo++;
        found = false;

        //find the MC with name == MCname;
        for(mcIndex = 0, mcItr = _cellLib.begin(); mcItr != _cellLib.end(); 
		mcItr++, mcIndex++)
        {
            if(abk_strcasecmp((*mcItr)->getName(), MCname) == 0)
            {
                found = true;
                break;
            }
        }

        if(!found)
		{
            if (verbose.forActions>0)
				abkwarn2(false,MCname," not found in mastercell lib ");
		}
		else if(alreadyMatched[mcIndex])
		{
			if (verbose.forActions>0)
				abkwarn2(false,MCname,
				          " appears more than once in the LGC file ");
		}
        else 
			{
					(*mcItr)->_type.setCoreClass(MCclass);
				alreadyMatched[mcIndex] = true;
			}
    }
}

void dbNetlist::postProcess()
{
   //remove nets with <= 1 pin.
	//first, sort them to put such nets at the end.
//  itNetGlobal nItr;
    vector<dbNet*>::iterator nItr;
    sort(_nets.begin(), _nets.end(), CompareNetsByDegree());

    for(nItr = _nets.begin(); nItr != _nets.end(); nItr++)
    {
        dbNet &net=**nItr;
        net.removeDupPins();
        unsigned numPins=net.getNumPins();
        if (net.hasLogicalPin()) numPins++;
	if (numPins <= 1)
	    break;
    }

    if(nItr == _nets.end())
	cout<<"  No degree 1 or 0 nets found"<<endl;
    else
	cout<<"  Removing degree 1 and 0 nets ... "<<endl;

//  itNetGlobal tmpNItr;
    vector<dbNet*>::iterator tmpNItr;
    itPinLocal  pItr;

	//update the pins's _net ptrs for the degree 1 nets
    for(tmpNItr = nItr; nItr != _nets.end(); nItr++)
    {
	abkassert((*nItr)->getNumPins() <= 1, "Sorting By Degree Failed");

	if( (*nItr)->getNumPins() > 0 )
        {
		pItr = (*nItr)->pinsBegin();
		(*pItr)->_net = NULL;
	}

        delete (*nItr);  // we're going to remove this net from the vector
                         // in the following call to erase(), but we also
                         // need to make sure dbNet dtor be called.
    }
		
    _nets.erase(tmpNItr, _nets.end());
//  _nets.erase(const_cast<dbNet**> (tmpNItr),
//              const_cast<dbNet**> (_nets.end()));


   //for each cell, build its vector of nets.
    itCellGlobal cItr;
    for(cItr = cellsBegin(); cItr != cellsEnd(); cItr++)
        (*cItr)->induceNetsFromPins(&_sNets);

   //for each net, build its vector of cells.
    for(nItr = _nets.begin(); nItr != _nets.end(); nItr++)
        (*nItr)->induceCellsFromPins();

  //populate dbNetlist's NUMs
    _numIOPads = _numCoreCells = _numAllPads = _numIOPins = 0;
    _numSCs = _numCCs = 0;
    _coreCellArea = 0.0;

    for(cItr = cellsBegin(); cItr != cellsEnd(); cItr++)
    {
        //set the instance cell's type from it's MasterCell's type
        (*cItr)->setTypeFromMaster();

        if ((*cItr)->isIO())
        {
            _numAllPads++;
            if(!(*cItr)->isPOther() )
	    {
                _numIOPads++;
		itPinLocal lPItr;
		for(lPItr = (*cItr)->pinsBegin(); 
			lPItr != (*cItr)->pinsEnd(); lPItr++)
		{
		    if((*lPItr)->isConnected() && (*lPItr)->isUseSignal() ) 
	            	_numIOPins++;
		}
	    }
        }
        else if((*cItr)->isCore() )
        {
            _numCoreCells++;
            _coreCellArea += (*cItr)->getMaster().getHeight() *
                             (*cItr)->getMaster().getWidth();
            if((*cItr)->isSC() )
                _numSCs++;
            else if( (*cItr)->isCC() )
                _numCCs++;
        }
    }

   //populate dbNetlist::_pinLib


   if(_pinLib.size() == 0)
   {
	cout<<"  Populating _pinLib ..."<<endl;
 	ctainerMasterPinsGlobal	pinLibTemp;
   	itMasterPinLocal	mPItr;	
	itMasterCell		mCItr;

   	for(mCItr = cellLibBegin(); mCItr != cellLibEnd(); mCItr++)
   	{
	    for(mPItr=(*mCItr)->pinsBegin();mPItr !=(*mCItr)->pinsEnd();mPItr++)
	    	pinLibTemp.push_back( (*mPItr) );
        }
	_pinLib = pinLibTemp;
   }  

  //populate dbNetlist::_pins

   if( _pins.size() == 0 )
   {
	cout<<"  Populating _pins ..."<<endl;
	ctainerPinsGlobal	pinsTemp;

	for(cItr = cellsBegin(); cItr != cellsEnd(); cItr++)
	{
	    for(pItr = (*cItr)->pinsBegin(); pItr != (*cItr)->pinsEnd(); pItr++)
		pinsTemp.push_back( (*pItr) );
	}
	_pins = pinsTemp;
   }

}

bool dbNetlist::isConsistent() const
{
 
    bool         status = true;
    itCellGlobal        cItr;
    itMasterCell        mCItr;
    itMasterPinGlobal   mPItr;
    itPinGlobal         pItr;
    itNetGlobal         nItr;
    unsigned            ctr, ctrAllPads, ctrIOPads, ctrCoreCells;
    unsigned            ctrSCs, ctrCCs;
    char                warnings[300];
 
 
    vector<dbNet*> 	  sortNets;//makes for faster lookup.
    vector<dbCell*> 	  sortCells;
    vector<dbPin*> 	  sortPins;
    vector<dbMasterPin*>  sortMPins;
    vector<dbMasterCell*> sortMCells;
 
    sortNets	= _nets;
    sortCells 	= _cells;
    sortPins 	= _pins;
    sortMPins 	= _pinLib;
    sortMCells 	= _cellLib;

    sort(sortNets.begin(),   sortNets.end(),   less<dbNet*>()  );
    sort(sortCells.begin(),  sortCells.end(),  less<dbCell*>() );
    sort(sortPins.begin(),   sortPins.end(),   less<dbPin*>()  );
    sort(sortMPins.begin(),  sortMPins.end(),  less<dbMasterPin*>()  );
    sort(sortMCells.begin(), sortMCells.end(), less<dbMasterCell*>() );
 
//check that total corecell area is > 0;
    if(_coreCellArea <= 0)
    {
        sprintf(warnings, "Netlist: total core cell area is %d\n",
                static_cast<int>(floor(_coreCellArea)));
        abkwarn(0,warnings);
        status = false;
    }
 
//check that the cell id's are consistent..ie, the go from
        //0->numCells-1
 
    ctrSCs = ctrCCs = ctrAllPads = ctrIOPads = ctrCoreCells = 0;
    for(ctr = 0, cItr = cellsBegin(); cItr != cellsEnd(); cItr++, ctr++)
    {
        if(ctr != (*cItr)->getIndex())
        {
            sprintf(warnings, "Netlist: cell index %d is misplaced\n", ctr);
            abkwarn(0, warnings);
            status = false;
        }
 
        if((*cItr)->isIO())
        {
            ctrAllPads++;
            if(!(*cItr)->isPOther() )
                ctrIOPads++;
        } else if ((*cItr)->isCore() )
        {
            ctrCoreCells++;
            if((*cItr)->isSC() )
                ctrSCs++;
            else if ((*cItr)->isCC() )
                ctrCCs++;
        }
    }


    if(ctrAllPads  != _numAllPads)
    {
        sprintf(warnings, "Netlist: incorrect numAllPads (was %d)\n",
                                        _numAllPads);
        abkwarn(0, warnings);
        status = false;
    }
 
    if(ctrCoreCells  != _numCoreCells)
    {
        sprintf(warnings, "Netlist: incorrect numCoreCells (was %d)\n",
                                        _numCoreCells);
        abkwarn(0, warnings);
        status = false;
    }
 
    if(ctrIOPads != _numIOPads)
    {
        sprintf(warnings, "Netlist: incorrect numIOPads (was %d)\n",
                                        _numIOPads);
        abkwarn(0, warnings);
        status = false;
    }
 
    if(ctrSCs != _numSCs)
    {
        sprintf(warnings, "Netlist: incorrect numSCs (was %d)\n",
                                        _numSCs);
        abkwarn(0, warnings);
        status = false;
    }
 
    if(ctrCCs != _numCCs)
    {
        sprintf(warnings, "Netlist: incorrect numCCs (was %d)\n",
                                        _numCCs);
        abkwarn(0, warnings);
        status = false;
    }
//for each cell....
    for(cItr = cellsBegin(); cItr != cellsEnd(); cItr++)
    {
 
//check that the master is in the vector of master*s
 
        const dbMasterCell* mCell = &((*cItr)->getMaster());
        
        if(!binary_search( sortMCells.begin(), sortMCells.end(), 
				const_cast<dbMasterCell*>(mCell) ) )
        {       
            sprintf(warnings,
                "Netlist: cell %s 's master is not in the MasterCell v.",
                (*cItr)->getName() );
            abkwarn(0, warnings);
            status = false;
        }
 
//check that cell's info matched MasterCells info.
        if((*cItr)->getNumPins() != mCell->getNumPins())
        {
            sprintf(warnings,
                "Netlist: %s does not have the same number pins as its Master",
                (*cItr)->getName() );
            abkwarn(0, warnings);
            status = false;
        }
 
//check that each pin points to this cell, and that the pin is on a net
//in the cells net* vector
 
        for(pItr = (*cItr)->pinsBegin(); pItr != (*cItr)->pinsEnd(); pItr++)
        {
		//check that the pin is in dbNetlist::_pins
	    if(!binary_search(sortPins.begin(), sortPins.end(), *pItr ) )
	    {
		sprintf(warnings,
		    "Netlist: %s has a pin which is not in dbNetlist::_pins",
		    (*cItr)->getName() );
		abkwarn(0, warnings);
		status = false;
	    }
	
                //pin points to this cell
            if(&((*pItr)->getCell()) != (*cItr))
            {
                sprintf(warnings,
                    "Netlist: %s has a pin which does not refer to it",
                    (*cItr)->getName() );
                abkwarn(0, warnings);
                status = false;
            }
                //pin is on a net in this cell's vector<dbNet*>, or in _sNets
 
            if( !(*pItr)->isConnected() ) break;//done, as it's not on a net
 
            for(nItr = (*cItr)->netsBegin(); nItr != (*cItr)->netsEnd(); nItr++)                if((*nItr) == &(*pItr)->getNet() ) break;
 
            if( nItr == (*cItr)->netsEnd() )
            {   //net not found in cells vector of net*s, and is on SOME net
                //check the _sNets
                for(nItr = _sNets.begin(); nItr != _sNets.end(); nItr++)
                        if( (*nItr) == &(*pItr)->getNet() ) break;
 
                if(nItr == _sNets.end() )
                {
                    sprintf(warnings,
                        "Netlist: %s has a pin not on a net in its v. of nets",
                        (*cItr)->getName() );
                    abkwarn(0, warnings);
                    status = false;
                }
            }
        
        }//end, for each pin on each cell
 
//check that each net in the cell's vector of nets is in the design's vector
//of all nets.
 
        abkfatal( (*cItr)->getNumPins() >= (*cItr)->getNumNets(),
                "more nets than pins");
 
        for(nItr = (*cItr)->netsBegin(); nItr != (*cItr)->netsEnd(); nItr++)
        {
            if(!binary_search( sortNets.begin(), sortNets.end(), *nItr ) )
            {
                sprintf(warnings,
                   "    Netlist: %s is missing from the global nets\n",
                (*nItr)->getName() );
                abkwarn(0, warnings);
                status = false;
            }
        }
    }//end, for each cell in netlist
 
 
//for each net....
    unsigned nIdx;
    for(nItr = netsBegin(), nIdx = 0; nItr != netsEnd(); nItr++, nIdx++)
    {
//check the nets are in order, at least by their index.
        if((*nItr)->getIndex() != nIdx)
        {
            sprintf(warnings, "Net %d is in the wrong order\n",
                                (*nItr)->getIndex());
            abkwarn(0, warnings);
            status = false;
        }
 
//warning (but not fatal) for degree zero nets.
 
        if((*nItr)->getNumPins() == 0)
        {
            sprintf(warnings, "%s has no pins on it\n", (*nItr)->getName());
            //abkwarn(0, warnings);
            cerr<<warnings;
            abkfatal( (*nItr)->getNumCells() == 0, "Cells > Pins" );
        }
//check each pin on the net
        for(pItr = (*nItr)->pinsBegin(); pItr != (*nItr)->pinsEnd(); pItr++)
        {
//pin should point to the net.
            if((*pItr)->isConnected() && &((*pItr)->getNet()) != (*nItr))
            {
                sprintf(warnings,
                   "Netlist: net %s has a pin which does not refer to it",
                    (*nItr)->getName() );
                abkwarn(0, warnings);
                status = false;
            }
 
//pin should be on a cell in the net's vector<cell*>
            for(cItr=(*nItr)->cellsBegin(); cItr != (*nItr)->cellsEnd(); cItr++)                if((*cItr) == &((*pItr)->getCell()) ) break;
 
            if(nItr == (*cItr)->netsEnd() )
            {   //cell was not found in the vector of cells
                sprintf(warnings,
                   "Netlist: net %s has pin from a cell not in its v. of cells",                   (*nItr)->getName() );
                abkwarn(0, warnings);
                status = false;
            }
        }//end, for each pin on each net
 
//check that each cell on the net is in the design's vector of all cells

        for(cItr = (*nItr)->cellsBegin(); cItr != (*nItr)->cellsEnd(); cItr++)
        {
            if(!binary_search( sortCells.begin(), sortCells.end(), *cItr ) )
            {
                sprintf(warnings,
                    "Netlist: %s has a cell not in the v. of all cells",
                    (*nItr)->getName() );
                abkwarn(0, warnings);
                status = false;
            }
        }
 
    }//end, for each net in netlist
 
//for each masterCell....
 
    for(mCItr = cellLibBegin(); mCItr != cellLibEnd(); mCItr++)
    {
        //check has a name
        if( (*mCItr)->getName() == "")
        {
            sprintf(warnings, "Netlist: MasterCell with no name");
            abkwarn(0, warnings);
            status = false;
        }
    //check each pin on the MCell
        for(mPItr=(*mCItr)->pinsBegin(); mPItr != (*mCItr)->pinsEnd(); mPItr++)
        {
            if(&((*mPItr)->getCell()) != (*mCItr))
            {
                sprintf(warnings,
                    "Netlist: MC %s has a pin which does not refer to it",
                    (*mCItr)->getName() );
                abkwarn(0, warnings);
                status = false;
            }
	   //check that the MPin is in _pinLib
	    if(!binary_search( sortMPins.begin(), sortMPins.end(), *mPItr ) )
	    {
		sprintf(warnings,
		    "Netlist: MC %s has a pin which is not in _pinLib",
		    (*mCItr)->getName() );
		abkwarn(0, warnings);
		status = false;
	    }
        }

    }
        
    return status;
}


double dbNetlist::evalHalfPerimCost(const Placement &pl, 
		const vector<Orientation> *orientations,
                unsigned threshold, bool IONetsOnly) const
{
    itNetGlobal nItr;
    double totalPerim = 0.0;

    for(nItr = netsBegin(); nItr != netsEnd(); nItr++)
      if (!threshold || (*nItr)->getNumPins() < threshold)
	totalPerim += evalHalfPerimCost(*nItr, pl, 
                                        orientations, IONetsOnly);

    return totalPerim;
}

double dbNetlist::evalHalfPerimCost(dbNet *net, const Placement &pl, 
			const vector<Orientation> *orientations,
			bool IONetsOnly) const
{
    BBox       netBox;
    itPinLocal pItr;
    unsigned   cellId;
    Point      pinOffset;
    bool       IONet=false;


    for(pItr = net->pinsBegin(); pItr != net->pinsEnd(); pItr++)
    {
        if (IONetsOnly && (*pItr)->getCell().isIO()) IONet=true;

	cellId = (*pItr)->getCell().getIndex();
	pinOffset = (*pItr)->getMaster().getCenter();

    	if(orientations != NULL)
	{
            double h = (*pItr)->getCell().getMaster().getHeight();
            double w = (*pItr)->getCell().getMaster().getWidth();
 
            if(!(*orientations)[cellId].isFaceUp() )
                pinOffset.x = w - pinOffset.x;
 
            double tmp;
            switch((*orientations)[cellId].getAngle())
            {
            	case   0:   //( x,  y)
                    break;
                case  90:   //( y, -x)
                    tmp = pinOffset.x;
                    pinOffset.x = pinOffset.y;
                    pinOffset.y = w - tmp;
                    break;
                case 180:   //(-x, -y)
                    pinOffset.x = w - pinOffset.x;
                    pinOffset.y = h - pinOffset.y;
                    break;
                case 270:   //(-y,  x)
		    tmp = pinOffset.y;
		    pinOffset.y = pinOffset.x;
		    pinOffset.x = h - tmp;
		    break;
                default: abkfatal(0," Orientation corrupted ");
            }
 
   	} 
 
        abkassert(cellId < pl.getSize(),"placement index out of range");
        netBox += (pl[cellId ] + pinOffset);
    }

    if (!IONetsOnly || IONet) return netBox.getHalfPerim();
    else return 0;
} 

#define NumNetCostFactors 51

static double NetCostFactorTable[NumNetCostFactors]=
{
                0.0,
                1.000000, 1.000000, 1.000000, 1.082797, 1.153598,
                1.220592, 1.282322, 1.338501, 1.399094, 1.449260,
                1.497380, 1.545500, 1.593619, 1.641739, 1.689859,
                1.730376, 1.770893, 1.811410, 1.851927, 1.892444,
                1.928814, 1.965184, 2.001553, 2.037923, 2.074293,
                2.106117, 2.137941, 2.169766, 2.201590, 2.233414,
                2.264622, 2.295830, 2.327038, 2.358246, 2.389454,
                2.418676, 2.447898, 2.477119, 2.506341, 2.535563,
                2.560954, 2.586346, 2.611737, 2.637129, 2.662520,
                2.688684, 2.714848, 2.741013, 2.767177, 2.793341
};

static double getNetCostFactor(unsigned numPins)
{
   if (numPins<NumNetCostFactors) return NetCostFactorTable[numPins];
        else return NetCostFactorTable[NumNetCostFactors-1];
}

double dbNetlist::evalWHalfPerimCost(const Placement &pl, 
		const vector<Orientation> *orientations,
                unsigned threshold, bool IONetsOnly) const
{
    itNetGlobal nItr;
    double totalPerim = 0.0;

    for(nItr = netsBegin(); nItr != netsEnd(); nItr++)
      if (!threshold || (*nItr)->getNumPins() < threshold)
	totalPerim += getNetCostFactor((*nItr)->getNumPins()) *
                      evalHalfPerimCost(*nItr, pl, orientations, IONetsOnly);

    return totalPerim;
}


double 
dbNetlist::evalHalfPerimCostNoPins(dbNet *net, const Placement &pl, 
                                   bool IONetsOnly) const
{
    BBox       netBox;
    itPinLocal pItr;
    unsigned   cellId;
//  Point      pinOffset;
    bool       IONet=false;
 
    for(pItr = net->pinsBegin(); pItr != net->pinsEnd(); pItr++)
    {
        cellId = (*pItr)->getCell().getIndex();
        netBox += pl[cellId];
        if (IONetsOnly && (*pItr)->getCell().isIO()) IONet = true;
    }

    if (!IONetsOnly || IONet) return netBox.getHalfPerim();
    else return 0;
}


double dbNetlist::evalMSTCost(const Placement &pl, 
		const vector<Orientation> *orientations) const
{
    itNetGlobal nItr;
    itPinLocal  pItr;
    double totalPerim = 0.0;
//  Point cellCenter;
    Point pinOffset;
    unsigned cellId;

    for(nItr = netsBegin(); nItr != netsEnd(); nItr++)
    {
        unsigned netDegree=(*nItr)->getNumPins();
        abkfatal(netDegree<3000," MST evaluator failed: netDegree > 3000\n");
                               // likely to overflow
	Placement netPl(netDegree);
        unsigned pinCount=0;
	for(pItr = (*nItr)->pinsBegin(); pItr != (*nItr)->pinsEnd(); pItr++)
	{

	  if(orientations == NULL) //assume all cells facing up and angle==0
            netPl[pinCount++]=pl[ (*pItr)->getCell().getIndex() ];

	  else
	  { 
        	cellId = (*pItr)->getCell().getIndex();
        	pinOffset = (*pItr)->getMaster().getCenter();
 
                double h = (*pItr)->getCell().getMaster().getHeight();
            	double w = (*pItr)->getCell().getMaster().getWidth();
 
            	if(!(*orientations)[cellId].isFaceUp() )
                    pinOffset.x = w - pinOffset.x;
 
            	double tmp;
            	switch((*orientations)[cellId].getAngle())
            	{
                    case   0:   //( x,  y)
                    	break;
                    case  90:   //( y, -x)
                    	tmp = pinOffset.x;
                    	pinOffset.x = pinOffset.y;
                    	pinOffset.y = w - tmp;
                    	break;
                    case 180:   //(-x, -y)
                    	pinOffset.x = w - pinOffset.x;
                    	pinOffset.y = h - pinOffset.y;
                        break;
                    case 270:   //(-y,  x)
                    	tmp = pinOffset.y;
                    	pinOffset.y = pinOffset.x;
                    	pinOffset.x = h - tmp;
                    	break;
                    abkfatal(0," Orientation corrupted ");
		}
 
                netPl[pinCount++]= pl[cellId ] + pinOffset;
	  }

	}	//end, for all pins
        double  netMSTCost =0; // = netPl.getMSTCost();
        abkfatal(0," MST cost computation unavailable "); 
        abkfatal(netMSTCost>=0.0,
           " MSTCost evaluator failed: overflow (net and scale too large)\n");
           // MST cost evaluator uses integers : they can easily overflow
	totalPerim += netMSTCost;

    }//end, for all nets

    return totalPerim;
}


eqClassMasterCells dbNetlist::getCellLEQClass(dbMasterCell* memberCell)
    const
{ 
    equivClassesMasterCells::const_iterator LEQItr;
    
    for(LEQItr = _cellLEQs.begin(); LEQItr != _cellLEQs.end(); LEQItr++)
    {
        if(binary_search( (*LEQItr).begin(), (*LEQItr).end(), memberCell) )
                return(*LEQItr);
    }
    eqClassMasterCells retval;
    retval.push_back(memberCell);
    return retval;
}   
 
eqClassMasterCells dbNetlist::getCellEEQClass(dbMasterCell* memberCell)
    const
{  
    equivClassesMasterCells::const_iterator EEQItr;
    
    for(EEQItr = _cellEEQs.begin(); EEQItr != _cellEEQs.end(); EEQItr++)
    {
        if(binary_search( (*EEQItr).begin(), (*EEQItr).end(), memberCell) )
                return(*EEQItr);
    }
       
    eqClassMasterCells retval;
    retval.push_back(memberCell);
    return retval;
}
 
eqClassMasterPins dbNetlist::getPinLEQClass(dbMasterPin* memberPin)
    const
{
    equivClassesMasterPins::const_iterator LEQItr;
    
    for(LEQItr = _pinLEQs.begin(); LEQItr != _pinLEQs.end(); LEQItr++)
    {
        if(binary_search( (*LEQItr).begin(), (*LEQItr).end(), memberPin) )
                return(*LEQItr);
    }
 
    eqClassMasterPins retval;
    retval.push_back(memberPin);
    return retval;
}      

void dbNetlist::printNetStats(ostream& out) const
{
    itNetGlobal itNet=netsBegin();
    unsigned n, nNets=getNumNets();
    vector<unsigned> netSizes(nNets);

    for(n=0;n!=nNets;n++,itNet++)
       netSizes[n]=(*itNet)->getNumCells();
         
    out << endl << " Net sizes (cells) : " << TrivialStats(netSizes)
        << CumulativeFrequencyDistribution(netSizes,20) << endl;
}


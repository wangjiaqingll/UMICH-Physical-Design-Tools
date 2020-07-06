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




// Created July 29, 1997 by Igor Markov, VLSI CAD ABKGROUP, UCLA/CS

// CHANGES
// 970805   aec  added processing of LGC file
// 970812   aec  added easyAccess.populate
// 970814   aec  design.postProc added an argument, spatial
// 970814   aec  removed above argument.
//		 	moved cell/net sorting from netlist.postproc to here 
// 970814   mro  changed _MSC_VER branch including const_cast
// 970827   ilm  moved in sorting of dbMasterCell::_obstacles from dbNetlist.cxx
// 970829   ilm     added a hack with ABKDEBUG as a workaround for
//                     a bug in SunCC4.2 iropt
// 970905   aec  added sorting of pins on nets, cells, and in dbNetlist
// 970907   aec  added sorting of LEQ/EEQs
// 970919   pat  added spatial to easyAccess populate parameters
// 971205   ilm  pins are now reindexed after being sorted
// 971209   mro  special nets sorted by name then reindexed.
// 020811   ilm  ported to g++ 3.0
#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <fstream>
#include <stdlib.h>

#ifndef  ABKDEBUG
#define  ABKDEBUG
#endif

#include "ABKCommon/abkcommon.h"
#include "DB.h"
#include "dbTemporal.h"

using std::sort;
using std::less;

// not inlined, specially to defeat a bug in SunPro CC optimizer
void sortDesignCellsByName(ctainerCellsGlobal::iterator begin,
     ctainerCellsGlobal::iterator end, unsigned nFixed, unsigned nFixedX, 
     unsigned nFixedY);

// not inlined, specially to defeat a bug in SunPro CC optimizer
void sortPins(dbNetlist& design);

// not inlined, specially to defeat a bug in SunPro CC optimizer
void sortObstacles(dbNetlist& design);

// not inlined, specially to defeat a bug in SunPro CC optimizer
void sortEqClasses(dbNetlist& design);


void DB::postProcessor(const char * LGCFileName)
{
    Timer tm;

    // LGCFileName defaults to NULL
    if (LGCFileName)
    {
        cout << "Opening " << LGCFileName << endl;
        ifstream lgcf(LGCFileName);
        abkfatal(lgcf,"Can\'t open LGC file");
        cout<<"Parsing LGC file ... "<<endl;
	design.processLGC(lgcf,_params.verbose);
    }

    if (_params.fixPlacedPads)
    {
        vector<unsigned> placedPads; 
        for (itCellGlobal iC=design.cellsBegin();iC!=design.cellsEnd();iC++)
        {
           typedef dbCell const &rDbCell;
           rDbCell cell(*(*iC));
           unsigned cellIdx = cell.getIndex();
	   if(!cell.getMaster().isCore())
	   {

             if (spatial.locationValid[cellIdx])
                 placedPads.push_back(cellIdx);
             else
  	     abkwarn3(0, "Non-core cell",cell.getName()," w/o valid location");

	   }
        }
        if (placedPads.size())
        {
           cout << "Fixing " << placedPads.size()
                << " noncore components  ..." << endl;
           Mapping plPadsMap(placedPads.size(),spatial.locations.getSize(),
                              placedPads);
           Placement fixedLocs(plPadsMap,spatial.locations);
           FixedGroupConstraint fxPads(spatial.locations, plPadsMap, fixedLocs);
           spatial.constraints.add (fxPads);
        }
    }

    // this needs to be done _before cell reordering 
    cout<<"Compactifying constraints ... "<<endl;
    spatial.constraints.compactify(); 
  
    cout<<"Starting postprocessor ... " << endl;
    design.postProcess();

  //_______________________________________ 
  //sort the nets and cells in design
  //_______________________________________

   Timer sortTm;
   itCellGlobal cItr;
// itNetGlobal  nItr;
   ctainerNetsGlobal::iterator nItr;

    cout<<"  Sorting: "<< endl;

    sortObstacles(design);

    cout<<"          - master cells by name ... " << endl;

    sort(design._cellLib.begin(), design._cellLib.end(), CompareMCsByName());

   //sort the global vector of cells by cell Names.
    cout<<"          - cells by name ... "<< endl;

    CompareCellsByName sortByCellName; // used many times below
    sort(design._cells.begin(), design._cells.end(), sortByCellName);
    //  assign new cell indexes
    Permutation cellPerm(design._cells.size() );
    unsigned cIdx;
 
    for(cItr = design._cells.begin(), cIdx=0; cItr != design._cells.end();
                        cItr++, cIdx++)
    {
        cellPerm[(*cItr)->getIndex() ] = cIdx;
        (*cItr)->_index = cIdx;
    }
    //pass the permutation to the spatial class
    spatial.reorderCells(cellPerm);

   //sort each net's cells by their index.
    cout<<"          - each net's cells ... " << endl;

    for(nItr = design._nets.begin(); nItr != design._nets.end(); nItr++)
        (*nItr)->sortCellsByIndex();
 
   //sort the global vector of nets by the index's of the cells on them.
   // put nets w. degree 0 at the end
    cout<<"          - nets ... " << endl;

    sort(design._nets.begin(), design._nets.end(), CompareNetsByCellIndex());
    cout<<"          - special nets ... " << endl;
    sort(design._sNets.begin(),design._sNets.end(),CompareNetsByName());

    // now assign new indices for special nets
    unsigned nIdx;
    for(nItr = design._sNets.begin(), nIdx = 0; nItr != design._sNets.end(); 
			nItr++, nIdx++)
    {
        (*nItr)->_index = nIdx + snetOffset;
    }

 
  //remove degree0 nets & assign the new indexes
  // mro question 001012:  haven't degree-0 nets already been
  // removed by the line design.postProcess() above?
    for(nItr = design._nets.begin(), nIdx = 0; nItr != design._nets.end(); 
			nItr++, nIdx++)
    {
        dbNet &net=**nItr;
        if( net.getNumPins() == 0 && !net.hasLogicalPin())
                break;
        net._index = nIdx;
    }

    sortPins(design);

    sortEqClasses(design);
 
    design._nets.erase(nItr, design._nets.end());
//  design._nets.erase(const_cast<dbNet**>(nItr),
//                     const_cast<dbNet**>(design.netsEnd()));
 
  //resort each net's cells by their direction (source first).
    cout<<"          - each net's cells by direction ... " << endl;
    for(nItr = design._nets.begin(); nItr != design._nets.end(); nItr++)
        (*nItr)->sortCellsByDir();

  //re-sort the global vector of cells by constraints, then alpha
 
  cout<<"          - each cell by placement status and type ... " << endl;

  if(!spatial.constraints.isEmpty())
  {
    Permutation newIndeces;
    spatial.constraints.getPermutation(newIndeces);
    reorderVector(design._cells, newIndeces);
 
    unsigned numFixed, numFixedX, numFixedY;
    if (spatial.constraints.hasFGC() )
        numFixed = spatial.constraints.getFGC().getSize();
    else
        numFixed = 0;
 
    if(spatial.constraints.hasFXGC())
        numFixedX = spatial.constraints.getFXGC().getSize();
    else
        numFixedX = 0;
 
    if (spatial.constraints.hasFYGC())
        numFixedY = spatial.constraints.getFYGC().getSize();
    else
        numFixedY = 0;

    // SunPro CC optimizer bug workaround
    sortDesignCellsByName(design._cells.begin(),design._cells.end(),
                          numFixed, numFixedX, numFixedY);
   } 
   else //no constraints..just an alpha sort will do.
      sort(design._cells.begin(), design._cells.end(), sortByCellName);

	//sort the nets on cells.
   cout<<"          - nets on cells... " << endl;
   for(cItr = design._cells.begin(); cItr != design._cells.end(); cItr++)
   	(*cItr)->sortNets(); 

   sortTm.stop();
   cout<<"  Sorting took "<< sortTm <<endl;
 
 
 //construct the permutation of cell indexes (conversion from old to new)
 //and assign new indexes.
 
    cout<<"  Reordering locations, constraints etc ... "<<endl;
 
    for(cItr = design._cells.begin(), cIdx=0; cItr != design._cells.end(); 
			cItr++, cIdx++)
    {
        cellPerm[(*cItr)->getIndex() ] = cIdx;
                //cellPerm[<oldIndex>] = <newIndex>
        (*cItr)->_index = cIdx;
    }
 
  //pass the permutation to the spatial class
    spatial.reorderCells(cellPerm);

    Timer eaTm;
    easyAccess.populate(context, design, spatial, getFileNames(), getDirName());
    easyAccess._coreCellArea = design._coreCellArea;
    eaTm.stop();
    cout<< "  Populating EasyAccess took " << eaTm << endl;

    if( timing )
    {
    // call the timing postprocessor
        cout << "Calling timing post processor..." << endl;
        Timer timingTm;
        timing->postProcess( design );
        timingTm.stop();
        cout << "Timing post processor took " << timingTm << endl;
    }
    
    tm.stop();
    cout << "Postprocessor took " << tm << endl;
}

// not inlined, especially to defeat a bug in SunPro CC optimizer
void sortDesignCellsByName(ctainerCellsGlobal::iterator begin,
     ctainerCellsGlobal::iterator end, unsigned numFixed, unsigned numFixedX, 
     unsigned numFixedY)
{
    CompareCellsByName byName;     

    sort(begin,begin+numFixed, byName);

    sort(begin+numFixed, begin+(numFixed+numFixedX), byName);
 
    sort(begin+(numFixed+numFixedX),
                begin+(numFixed+numFixedX+numFixedY), byName);
    sort(begin+(numFixed+numFixedX+numFixedY), end, byName);
}

// not inlined, especially to defeat a bug in SunPro CC optimizer
void sortPins(dbNetlist& design)
{
   itCellGlobal cItr;
   itNetGlobal  nItr;
   itMasterCell mCItr;

  //sort the PinLib by the master cell a MPin is on, direction, then name.
    cout<<"          - masterpins ... "<<endl;

    sort(design._pinLib.begin(), design._pinLib.end(),
			ComparePinsByCellIndexAndDir());

  //sort the Pins by the cell a pin is on, direction, then MPin name.
    cout<<"          - pins ... "<< endl;	

    sort(design._pins.begin(), design._pins.end(), 
			ComparePinsByCellIndexAndDir());

    unsigned pIdx=0;
    for(itPinGlobal PItr=design._pins.begin();PItr !=design._pins.end();PItr++)
      (*PItr)->_index=pIdx++;

  //sort the MPins on a MCell by direction
    cout<<"          - each mastercell's masterpins ... "<<endl;

    for(mCItr= design._cellLib.begin(); mCItr != design._cellLib.end(); mCItr++)
    {
	sort( (*mCItr)->_pins.begin(), (*mCItr)->_pins.end(), 
			ComparePinsByDirAndName());
    }

  //sort the pins on a cell by direction
    cout<<"          - each cell's pins ... "<< endl;
 
    for(cItr = design._cells.begin(); cItr != design._cells.end(); cItr++)
    {
	sort((*cItr)->_pins.begin(), (*cItr)->_pins.end(), 
			ComparePinsByDirAndName() );
    }

  //sort the pins on a net by direction
    cout<<"          - each net's pins ... "<<endl;
 
    for(nItr = design._nets.begin(); nItr != design._nets.end(); nItr++)
    {
	sort((*nItr)->_pins.begin(), (*nItr)->_pins.end(), 
			ComparePinsByDirAndName() );
    }
}



// not inlined, to defeat a bug in SunPro CC optimizer
void sortEqClasses(dbNetlist& design)
{
    cout<<"          - masterCell/Pin Equiv. Classes ... "<<endl;
   
    equivClassesMasterCells::iterator eqCItr;
    equivClassesMasterPins:: iterator eqPItr;
   //sort each EQClass by the pointers (allows for bin_search lookup)
 
    for(eqCItr = design._cellLEQs.begin(); eqCItr != design._cellLEQs.end();
                eqCItr++)
        sort((*eqCItr).begin(), (*eqCItr).end(), less<dbMasterCell* const>());
 
    for(eqCItr = design._cellEEQs.begin(); eqCItr != design._cellEEQs.end();
                eqCItr++)
        sort((*eqCItr).begin(), (*eqCItr).end(), less<dbMasterCell* const>());
 
    for(eqPItr = design._pinLEQs.begin(); eqPItr != design._pinLEQs.end();
                eqPItr++)
        sort((*eqPItr).begin(), (*eqPItr).end(), less<dbMasterPin* const>());
}

// not inlined, to defeat a bug in SunPro CC optimizer
void sortObstacles(dbNetlist& design)
{
    cout<<"          - obstacles in master cells by xMin ... " << endl;

    for(itMasterCell mCItr =  design.cellLibBegin();
                    mCItr != design.cellLibEnd();    mCItr++)
    {
       sort((*mCItr)->_obstacles.begin(), (*mCItr)->_obstacles.end(),
                CompareObstaclesByXmin() );
    }
}

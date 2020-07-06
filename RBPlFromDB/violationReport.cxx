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





// 97.9.30  Paul Tucker
// 980212   dv -- Modified from pat's code of Legalizer. Improved interfaces
//             -- for violation code and oriented row-based placement.
// 980306  dv  removed orientedRBPlacement class
// 980310 pat  added violation marking for both cells when an overlap
//             is detected; originally only reported one.
/*
   Constructor for violationReport, including methods for
   checking an entire RBPlacement at once.
*/
#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <algorithm>
#include "ABKCommon/abkcommon.h"
#include "Placement/placement.h"
#include "DB/DB.h"
#include "RBPlaceFromDB.h"
#include "violationReport.h"
#include "violRptSupport.h"

#if (defined(_MSC_VER) && !defined(rint))
#define rint(a) floor((a)+0.5)
#endif


ViolationReport::ViolationReport (const DB& db, Verbosity verbosity)
  : _db(db), _pl(db.spatial.locations,db.spatial.orientations),
    _violations(1+db.spatial.locations.getSize()), _numViols(0),
    _verbosity(verbosity)
{
    abkfatal(db.spatial.allCellsPlaced()," Not all cells are placed \n");

    Timer tm;

    RBPlaceFromDB tempRBCP(_db, RBPlaceParams());
    _rbplace = & tempRBCP;

    _checkLegality();
    _rbplace = NULL;

    tm.stop();

    if (_verbosity.forSysRes)
        cout << "Generating ViolationReport took " << tm << endl;
    else if (_verbosity.forActions  || _verbosity.forMajStats )
        cout << "ViolationReport computed " << endl;
}


ViolationReport::ViolationReport (const DB& db, RBPlacement & RBP,
                                  Verbosity verbosity)
#ifdef __GNUC__
  : _db(db), _rbplace(&RBP), _pl(static_cast<const PlacementWOrient&>(RBP)), 
    _violations(RBP.getNumCells()), _numViols(0), _verbosity(verbosity)
#else
  : _db(db), _rbplace(&RBP), _pl(static_cast<PlacementWOrient>(RBP)), 
    _violations(RBP.getNumCells()), _numViols(0), _verbosity(verbosity)
#endif
{
    Timer tm;
    _checkLegality();
    tm.stop();

    if (_verbosity.forSysRes)
        cout << "Generating ViolationReport took " << tm << endl;
    else if (_verbosity.forActions  || _verbosity.forMajStats )
        cout << "ViolationReport computed " << endl;
}

/*
   97.9.29  Paul Tucker
   Functions for checking legality of an oriented placement.
   Most of this stuff was taken in original form from Maogang's mdplace
   package.
*/

/******** function signatures **************/

// The function below called by applyToRegion (therefore not a member of
// violationReport.
       void logError1(unsigned&, unsigned, vector <Violations>&,
                      const Violations&, unsigned, const dbNet&, const char *);

static bool isLegal(const Symmetry& cellSym, const Orientation& cellOri);

static bool isLegal (const Orientation& rowOri,
                     const Symmetry& siteSym,
                     const Orientation& cellOri);

/********* function definitions *************/

void ViolationReport::_checkLegality ()
/*
   Adapted from Maogang's RuleChecker function.
   
   It is assumed that the RBPlacement object is well-formed,
   in particlar, the cell indices in every row are ordered
   by increasing X coordinate, and identify cells with identical
   Y coordinate.  (If suspect, this should be verified by the
   isConsistent() function for RBPlacement.)
   
   A placement is considered legal if:
   1. No cells in the same row overlap
   2. Every cell's orientation agrees with the row/site orientation
   3. If a cells overlaps with another in an adjacent row, none of their
   obstacles overlap
   4. No special net element intersects a cell obstacle or port
*/
{
  if ((_verbosity.forActions || _verbosity.forSysRes)
      &&!_verbosity.forMajStats)
    cout << "Running ViolationReport..." << endl;
  
  if (_verbosity.forMajStats)
      cout << "Checking placement of cells into rows..." << endl;
/*
  if (_rbplace->isPopulated())
  {
      if (!_rbplace->allCellsPlaced())
          unsigned cellsNotInRows = _isPlacementRowBased();
  }
  else
  {
      _rbplace->resetPlacement(_pl);
      if (!_rbplace->allCellsPlaced())
          unsigned cellsNotInRows = _isPlacementRowBased();
  }
*/
  if (_verbosity.forMajStats)
      cout<<"Checking for horizontal overlap..."<<endl;
  _checkHorizontalOverlap ();

  if (_verbosity.forMajStats)
      cout<<"Checking cell orientations..."<<endl;
  _checkCellOrientation ();

  if (_verbosity.forMajStats)
      cout<<"Checking overlap layer..."<<endl;
  _checkOverlapLayer ();

  if (_verbosity.forMajStats)
      cout<<"Checking special wiring..."<<endl;
  _checkSpecialWiring ();

  //_recheckSpecialWiring ();

  if (_verbosity.forMajStats) 
  {
    if (_numViols > 0)
      cout << "Found " << _numViols << " Errors" << endl;
    else
      cout << "No violations found; placement is legal." << endl;
    cout << "Finished ViolationReport..." << endl;
  }
}


void ViolationReport::_checkHorizontalOverlap()
// Tests for horizontal overlap, subrow violations, 
// and being at a legal site
{
  unsigned cellID, nextID;
  double width;

  itRBPCoreRow cr;
  for(cr = _rbplace->coreRowsBegin(); cr != _rbplace->coreRowsEnd(); cr++)
  {
    for(unsigned j=0; j < cr->getNumSubRows(); j++)
    {
      const RBPSubRow& SR = (*cr)[j];
      if (SR.getNumCells()>0)
      {
        double spacing = SR.getSpacing();
        for(unsigned k=0; k < SR.getNumCells(); k++)
        {
          cellID = SR[k];
          width = _db.easyAccess.getCellWidths()[cellID];
          double offset = _pl[cellID].x - SR.getXStart();
          int quanta = static_cast<int>(rint(offset/spacing));
          if (((offset / spacing) - static_cast<double>(quanta))
              != 0)
            logError(Violations("offSite"), cellID, SR,
                     "Cell not aligned with a site.");
          if (_pl[cellID].x < SR.getXStart())
            logError(Violations("subRow"), cellID, SR,
                     "Cell off left end of subRow.");
          if (_pl[cellID].x + width > SR.getXEnd())
            logError(Violations("subRow"), cellID, SR,
                     "Cell off right end of subRow.");
          if (k < SR.getNumCells()-1)
          {
            nextID = SR[k+1];
            if (_pl[cellID].x + width > _pl[nextID].x)
              logError(Violations("hOverlap"), cellID, nextID, SR,
                       "Cell overlaps next cell.");
          }
        }
      }
    }
  }
} 


void ViolationReport::_checkCellOrientation ()
{
  itRBPCoreRow cr;
  for(cr = _rbplace->coreRowsBegin(); cr != _rbplace->coreRowsEnd(); cr++)
  {
    const Orientation& rowOri = cr->getOrientation();
    for(unsigned j=0; j < cr->getNumSubRows(); j++)
    {
      const RBPSubRow& SR = (*cr)[j];
      const Symmetry& siteSym = SR.getSite().symmetry;
      for(unsigned k=0; k < SR.getNumCells(); k++)
      {
        unsigned cellID = SR[k];
        const Symmetry& cellSym = 
          _db.getNetlist().getCellByIdx(cellID).getMaster().getSymmetry();
        const Orientation& cellOri =  _pl.getOrient(cellID);
        if (!isLegal(rowOri, siteSym, cellOri))
          logError(Violations("rOrient"), cellID, rowOri, siteSym, cellOri,
                   cellSym, "Illegal cell orientation w.r.t. row.");
        if (!isLegal(cellSym, cellOri))
          logError(Violations("cOrient"), cellID, cellSym, cellOri, 
                   "Illegal cell orientation w.r.t. own symmetry");
      }
    }
  }
}


void ViolationReport::_checkOverlapLayer ()
{
  itRBPCoreRow cr; 
  for(cr = _rbplace->coreRowsBegin(); cr != _rbplace->coreRowsEnd(); cr++)
  {
    if(cr+1 == _rbplace->coreRowsEnd()) continue;

    double rowHeight = (*cr)[0].getSite().height;
    if(cr->getYCoord()+rowHeight <= (cr+1)->getYCoord()) continue;
        // Rows do not overlap
    
    const RBPCoreRow& CR1 = *cr;
    const RBPCoreRow& CR2 = *(cr+1);
    
    for(unsigned j1=0; j1<CR1.getNumSubRows() ; j1++)
    {
      const RBPSubRow& SR1 = CR1[j1];
      for(unsigned k1=0; k1<SR1.getNumCells(); k1++)
      {
        unsigned cid1 = SR1[k1];
        double w1 = _db.easyAccess.getCellWidths()[cid1];
        for(unsigned j2=0; j2<CR2.getNumSubRows(); j2++)
        {
          const RBPSubRow& SR2 = CR2[j2];
          for(unsigned k2=0; k2<SR2.getNumCells(); k2++)
          {
            unsigned cid2 = SR2[k2];
            double w2 = _db.easyAccess.getCellWidths()[cid2];
            if((_pl[cid1].x + w1 > _pl[cid2].x) &&
	       (_pl[cid2].x + w2 > _pl[cid1].x))
            {
              if (_checkOverlapObstacles(cid1, cid2))
                logError(Violations("vOverlap"), cid1, cid2,
                         "Overlap in Y dimension");
            }
          }
        }
      }
    }
  }
}



void ViolationReport::_recheckSpecialWiring ()
/*
   This version is very slow, and only exists for confirming 
   results of the other version.
*/
{
  const dbNetlist& netlist = _db.getNetlist();
  unsigned numCells = netlist.getNumCells();

  unsigned cellID;
  for (cellID = 0; cellID<numCells; cellID++)
  {
    if (netlist.getCellByIdx(cellID).isCore())
    {
      unsigned netID;
      for (netID=snetOffset;
           netID < netlist.getNumSNets() + snetOffset;
           netID++)
      {  
        const dbNet& snet = netlist.getSNetByIdx(netID);
        if (_checkOverlap(cellID, snet))
          logError(Violations("wOverlap"), cellID, snet,
                   "Cell overlaps a special net");
      }
    }
  }
}


bool applyToRegion(applyToRegionPred &F, BBox region, const Placement& pl,
                   const RBPlacement* rbplace)
/*
   Here's a basic utility function for legalization.
   
   Given a BBox region, it applies the function argument F
   to every cell overlapping that region until F returns false.

   If the function F changes the placement, it should probably
   quit immediately...I'm not sure if this will be robust in
   the face of a changing placement.
   
   This function returns true iff F is applied to anything.
*/
{
// pair<const RBPCoreRow*, const RBPCoreRow*> CRitr;
  pair<itRBPCoreRow, itRBPCoreRow> CRitr;
  pair<itRBPSubRow,  itRBPSubRow>  SRitr;
  pair<vector<unsigned>::const_iterator, vector<unsigned>::const_iterator> Citr;
  cellXcompare Xcmp(&pl, region.xMin);
  bool didApply = false;
  
  // Find the coreRow containing the lower left corner of the region
  RBPCoreRow CRdummy(region.yMin);
  CRitr = std::equal_range(rbplace->coreRowsBegin(),
                      rbplace->coreRowsEnd(), CRdummy,
                      compareCoreRowY());
  // Now CRitr.first identifies the first coreRow that starts
  // at or above yMin.  CRitr.second identifies the first
  // coreRow that is above yMin. 
  // We probably want to look at the first row before CRitr.first.
  //cout << "CRitrs: " << CRitr.first << " " << CRitr.second << endl;
  if (CRitr.first != rbplace->coreRowsEnd())
  {
    itRBPCoreRow CR = CRitr.first;

    // See if we really want to start at the row below
    if (CRitr.first != rbplace->coreRowsBegin())
    {
      const RBPCoreRow& CR2 = *(CRitr.first - 1);
      if( (CR2.getNumSubRows() > 0)
          &&((CR2.getYCoord() + CR2[0].getSite().height) > region.yMin))
        CR--;
    }
    
    while (CR->getYCoord() <= region.yMax)
    {
      // find the first subrow containing the lefthand edge of the region.
      RBPSubRow SRdummy(region.xMin, region.xMin);
      SRitr = std::equal_range(CR->subRowsBegin(), CR->subRowsEnd(), SRdummy, 
			compareSubRowX());

      if (SRitr.first != CR->subRowsEnd())
      {
        itRBPSubRow SR = SRitr.first;

        if((SR->getXStart() > region.xMin)
			 && (SRitr.first != CR->subRowsBegin()))
          SR--;

        // Now we've got the subrow that's supposed to contain the
        // lefthand edge of the region.  Find the first cell that
        // overlaps it.  If it's the first cell in the subRow,
        // then also check the last cell in the prior subRow,
        // because that cell may extend over the boundary.
        Citr = std::equal_range(SR->cellIdsBegin(), SR->cellIdsEnd(), 
				InvalidUnsigned, Xcmp);
        vector<unsigned>::const_iterator CII = Citr.first;
        if((CII != SR->cellIdsBegin()) &&((CII == SR->cellIdsEnd())
               ||(pl[*CII].x > region.xMin)))
          CII--;
        
        // First take a quick look back
        if((CII == SR->cellIdsBegin()) &&
           (SRitr.first != CR->subRowsBegin()) &&
           ((SRitr.first - 1)->getNumCells() > 0))
        {
          const RBPSubRow& prev = *(SRitr.first - 1);
          didApply = true;
          if (!F( *(prev.cellIdsEnd() - 1)))
            return true;
        }
        
        // Now march across the coreRow to the next bound
        while ((CII != SR->cellIdsEnd()) && (pl[*CII].x < region.xMax))
        {
          didApply = true;
          if (!F(*CII))
            return true;
          CII++;
          if (CII == SR->cellIdsEnd())
          {
            SR++;
            if (SR == CR->subRowsEnd())
              break;   // out of iteration in this coreRow
            CII = SR->cellIdsBegin();
          }
        }  // while iterating across subRow
              
      } // If found a candidate starting subRow

      CR++;  // try next coreRow
      //cout << "Next coreRow y: " << CR->y << endl;
      if (  (CR == rbplace->coreRowsEnd())
          ||(CR->getYCoord() > region.yMax))
        return didApply;
      
    } // while CR overlaps the region
      
  } // if found a candidate starting coreRow

  return false;
}


void ViolationReport::_checkSpecialWiring ()
/*
   The algorithm used here:

   Some nested loops iterate once over each wire element in
   any special net.
   For each such element we then calculate points of interest,
   where it may overlap a cell.
   For each point of interest we do binary search on the
   RBPlacement object to retrieve any cell overlapping such
   a point, and then test that cell against the wire element.

   Correctness depends on generating an adequate set of interesting
   points, while speed depends on not generating too many.

*/
{
  const dbNetlist& netlist = _db.getNetlist();
  
  for (unsigned netID = snetOffset;
       netID < netlist.getNumSNets() + snetOffset;
       netID++)
  {  
    const dbNet& snet = netlist.getSNetByIdx(netID);
    for (unsigned wireIndex=0; 
         wireIndex<snet.getNumWiringLayerGeoms(); 
         wireIndex++)
    {
      const dbLayerGeom& lGeom = snet.getWiringLayerGeom(wireIndex);
      wireTestFunctor F(_db, _numViols, _verbosity.forMajStats, _violations, lGeom, snet, *this);
      BBox wire(lGeom);

      applyToRegion(F, wire, _pl, _rbplace);
    } // for wire boxes on net
  } // for special net
}


void ViolationReport::logError (const Violations& vCode, unsigned cellID,
                                const RBPSubRow& SR, const char* msg)
{
  _numViols++;
  if (_violations.size() > 0)
    _violations[cellID] |= vCode;
  if (_verbosity.forMajStats >= 2)
  {
    const dbMasterCell& master = _db.getNetlist().getCellByIdx(cellID).getMaster();
    BBox bounds = BBox(0,0,master.getWidth(),master.getHeight());
    master.reOrientRec(bounds,_pl.getOrient(cellID));
    bounds.TranslateBy(_pl[cellID]);
    cout << "Error " << _numViols << ": CellID: " << cellID
        << " " << msg << endl;
    if (_verbosity.forMajStats >= 10)
    {
        cout << " cellID: " << cellID << " subRow(y,xStart,xEnd):("
               << SR.getYCoord() << "," << SR.getXStart() << "," << SR.getXEnd() << ")" << endl;
         cout << " cell bounds: " << bounds;
    }
  }
}

void ViolationReport::logError (const Violations& vCode, unsigned cellID,
                                unsigned nextID, const RBPSubRow& SR,
                                const char* msg)
{
  _numViols++;
  if (_violations.size() > 0)
  {
    _violations[cellID] |= vCode;
    _violations[nextID] |= vCode;
  }
  if (_verbosity.forMajStats >= 2)
  {
    const dbMasterCell& master = _db.getNetlist().getCellByIdx(cellID).getMaster();
    BBox bounds = BBox(0,0,master.getWidth(),master.getHeight());
    master.reOrientRec(bounds,_pl.getOrient(cellID));
    bounds.TranslateBy(_pl[cellID]);
    
    const dbMasterCell& master2 = _db.getNetlist().getCellByIdx(nextID).getMaster();
    BBox bounds2 = BBox(0,0,master2.getWidth(),master2.getHeight());
    master2.reOrientRec(bounds2,_pl.getOrient(nextID));
    bounds2.TranslateBy(_pl[nextID]);
    
    cout << "Error " << _numViols << ": CellID: " << cellID
        << " " << msg << endl;
    if (_verbosity.forMajStats >= 10)
    {
        cout << " cellID: " << cellID << " subRow(y,xStart,xEnd):("
              << SR.getYCoord() << "," << SR.getXStart() << "," << SR.getXEnd() << ")" << endl;
        cout << " cell " << cellID << " bounds: " << bounds;
        cout << " cell " << nextID << " bounds: " << bounds2;
    }
  }
}

void ViolationReport::logError (const Violations& vCode, unsigned cellID,
                                const Orientation& rowOri,
                                const Symmetry& siteSym,
                                const Orientation& cellOri,
                                const Symmetry& cellSym,
                                const char* msg)
{
  _numViols++;
  if (_violations.size() > 0)
    _violations[cellID] |= vCode;

  if (_verbosity.forMajStats >= 2)
        cout << "Error " << _numViols << ": CellID: " << cellID
            << " " << msg << endl;
  if (_verbosity.forMajStats >= 10)
  {
    cout << "cell: " << cellID;
    cout << " row orient: " << rowOri;
    cout << "site sym: " << siteSym;
    cout << "cell orient: " << cellOri 
      << " and sym: " << cellSym << endl;
  }
}

void ViolationReport::logError (const Violations& vCode, unsigned cellID,
                                const Symmetry& cellSym,
                                const Orientation& cellOri,
                                const char* msg)
{
  _numViols++;
  if (_violations.size() > 0)
    _violations[cellID] |= vCode;
  if (_verbosity.forMajStats >= 2)
        cout << "Error " << _numViols << ": CellID: " << cellID
            << " " << msg << endl;
  if (_verbosity.forMajStats >= 10)
  {
    cout << "cell symmetry: " << cellSym;
    cout << "cell orientation: " << cellOri;
  }
}

void ViolationReport::logError (const Violations& vCode, unsigned cellID1,
                                unsigned cellID2, const char* msg)
{
  _numViols++;
  if (_violations.size() > 0)
  {
    _violations[cellID1] |= vCode;
    _violations[cellID2] |= vCode;
  }
  if (_verbosity.forMajStats >= 2)
        cout << "Error " << _numViols << ": CellID1: " << cellID1 <<
            " and CellID2 " << cellID2 << " " << msg << endl;
  if (_verbosity.forMajStats >= 10)
    cout << "Cell 1: " << cellID1 << " Cell 2: " << cellID2 << endl;
}

void ViolationReport::logError (const Violations& vCode, unsigned cellID,
                                const dbNet& snet, const char* msg)
{
  _numViols++;
  if (_violations.size() > 0)
    _violations[cellID] |= vCode;
  if (_verbosity.forMajStats >= 2)
        cout << "Error " << _numViols << ": CellID: " << cellID
            << " " << msg << endl;
  if (_verbosity.forMajStats >= 10)
    cout << "Cell: " << cellID << " special net: " << snet.getName() << endl;
}

void ViolationReport::logError (const Violations& vCode,
                                unsigned cellID,
                                const char* msg)
{
  _numViols++;
  if (_violations.size() > 0)
    _violations[cellID] |= vCode;
  if (_verbosity.forMajStats >= 2)
      cout << "Error " << _numViols << ": CellID: " << cellID
          << " " << msg << endl;
}

       void logError1 (unsigned& errors,
                      unsigned verbose,
                      vector <Violations>& violations,
                      const Violations& vCode,
                      unsigned cellID,
                      const dbNet& snet,
                      const char* msg)
{
  errors++;
  if (violations.size() > 0)
    violations[cellID] |= vCode;
  if (verbose == 2)
    cout << "Error " << errors << ": CellID: " << cellID << " "
        << msg << endl;
  if (verbose == 10)
    cout << "Cell: " << cellID << " special net: " << snet.getName() << endl;
}


static bool isLegal(const Symmetry& cellSym, const Orientation& cellOri)
{
  if (unsigned(cellSym)==0) {                // sym none.
    if ((cellOri.getAngle()==0) && (cellOri.isFaceUp())) return true;
    else return false;
  }
  else if (unsigned(cellSym)==2)  {        // sym Y
    if (cellOri.getAngle()==0) return true;
    else return false;
  }
  else if (unsigned(cellSym)==1)  {        // sym X
    if ((cellOri.getAngle()==0 && cellOri.isFaceUp()) ||
        (cellOri.getAngle()==180 && !cellOri.isFaceUp())) return true;
    else return false;
  }
  else if (unsigned(cellSym)==3)  {    // sym X Y
    if (cellOri.getAngle()==0 || cellOri.getAngle()==180) return true;
    else return false;
  }
  else if (unsigned(cellSym)==4)  {    // sym ROT
    if (cellOri.isFaceUp()) return true;
    else return false;
  }
  else return true;
}


static bool isLegal (const Orientation& rowOri,
                     const Symmetry& siteSym,
                     const Orientation& cellOri)
/* 
   Ugly....clean up later
*/
{
  if (unsigned(siteSym)==2) { // siteSym = Y
    if (rowOri.getAngle()==0 && rowOri.isFaceUp()) {   // rowOri=N
      if(cellOri.getAngle()==0) return true;
      else return false;
    }
    else if (rowOri.getAngle()==180 && !rowOri.isFaceUp()) {   // rowOri=FS
      if(cellOri.getAngle()==180) return true;
      else return false;
    }
    else {
      printf("ERROR! Unknown orientation of the row! Exit!\n");
      cout<<"rowOri="<<rowOri<<", siteSym="<<siteSym<<", cellOri="
          <<cellOri<<endl;
      exit(0);}
  }
  if (unsigned(siteSym)==1) { // siteSym = X
    if (rowOri.getAngle()==0 && rowOri.isFaceUp()) {   // rowOri=N
      if((cellOri.getAngle()==0 && cellOri.isFaceUp()) ||
         (cellOri.getAngle()==180 && !cellOri.isFaceUp())) return true;
      else return false;
    }
    else if (rowOri.getAngle()==180 && !rowOri.isFaceUp()) {   // rowOri=FS
      if((cellOri.getAngle()==0 && cellOri.isFaceUp()) ||
         (cellOri.getAngle()==180 && !cellOri.isFaceUp())) return true;
      else return false;
    }
    else {
      printf("ERROR! Unknown orientation of the row! Exit!\n");
      cout<<"rowOri="<<rowOri<<", siteSym="<<siteSym<<", cellOri="
          <<cellOri<<endl;
      exit(0);}
  }
  if (unsigned(siteSym)==0) { // siteSym = none 
    if (rowOri.getAngle()==0 && rowOri.isFaceUp()) {   // rowOri=N
      if(cellOri.getAngle()==0 && cellOri.isFaceUp()) return true;
      else return false;
    }
    else if (rowOri.getAngle()==180 && !rowOri.isFaceUp()) {   // rowOri=FS
      if(cellOri.getAngle()==180 && !cellOri.isFaceUp()) return true;
      else return false;
    }
    else {
      printf("ERROR! Unknown orientation of the row! Exit!\n");
      cout<<"rowOri="<<rowOri<<", siteSym="<<siteSym<<", cellOri="
          <<cellOri<<endl;
      exit(0);}
  }
  if (unsigned(siteSym)==3) { // siteSym = X Y
    if (rowOri.getAngle()==0 && rowOri.isFaceUp()) {   // rowOri=N
      if(cellOri.getAngle()==0 || cellOri.getAngle()==180) return true;
      else return false;
    }
    else if (rowOri.getAngle()==180 && !rowOri.isFaceUp()) {   // rowOri=FS
      if(cellOri.getAngle()==0 || cellOri.getAngle()==180) return true;
      else return false;
    }
    else {
      printf("ERROR! Unknown orientation of the row! Exit!\n");
      cout<<"rowOri="<<rowOri<<", siteSym="<<siteSym<<", cellOri="
          <<cellOri<<endl;
      exit(0);}
  }
  if (unsigned(siteSym)==4) { // siteSym = ROT90
    if (rowOri.getAngle()==0 && rowOri.isFaceUp()) {   // rowOri=N
      if(cellOri.isFaceUp()) return true;
      else return false;
    }
    else if (rowOri.getAngle()==180 && !rowOri.isFaceUp()) {   // rowOri=FS
      if(!cellOri.isFaceUp()) return true;
      else return false;
    }
    else {
      printf("ERROR! Unknown orientation of the row! Exit!\n");
      cout<<"rowOri="<<rowOri<<", siteSym="<<siteSym<<", cellOri="
          <<cellOri<<endl;
      exit(0);}
  }
  else {         // siteSym = X Y ROT90
    return true;
  }

return bool(UINT_MAX); //satisfy compiler

}


bool ViolationReport::_checkOverlapObstacles (unsigned lowerCell,
                                              unsigned upperCell)
const
/*
   True iff some pair of obstacles on the two cells intersect.
*/
{
  const dbNetlist& netlist = _db.getNetlist();
  const dbMasterCell& master1 = netlist.getCellByIdx(lowerCell).getMaster();
  const dbMasterCell& master2 = netlist.getCellByIdx(upperCell).getMaster();
  const ctainerLayerGeoms& obs1 = master1.getObstacles();
  const ctainerLayerGeoms& obs2 = master2.getObstacles();
  BBox outline1 = master1.reOrientRec(BBox(0,0,
                                           master1.getWidth(),
                                           master1.getHeight()),
                                      _pl.getOrient(lowerCell));
  outline1.TranslateBy(_pl[lowerCell]);
  BBox outline2 = master2.reOrientRec(BBox(0,0,
                                           master2.getWidth(),
                                           master2.getHeight()),
                                      _pl.getOrient(lowerCell));
  outline2.TranslateBy(_pl[upperCell]);
  
  if (!outline1.intersects(outline2))
    return false;

  // Iterate over obstacles on lower cell
  for (unsigned obs1i = 0; obs1i < obs1.size(); obs1i++)
  {
    // For some reason, we only care about obstacles on a type 2 layer.
    // Is this correct???
    if ((int)obs1[obs1i]->getLayer().getType()!=2)
      continue;
    BBox obstacle1 = master1.reOrientRec(BBox(*obs1[obs1i]),
                                         _pl.getOrient(lowerCell));
    obstacle1.TranslateBy(_pl[lowerCell]);

    if (!obstacle1.intersects(outline2))
      continue;

    // Iterate over obstacles on upper cell
    for (unsigned obs2i=0; obs2i < obs2.size(); obs2i++)
    {
      if ((int)obs2[obs2i]->getLayer().getType() != 2)
        continue;
      BBox obstacle2 = master2.reOrientRec(BBox(*obs2[obs2i]),
                                           _pl.getOrient(upperCell));
      obstacle2.TranslateBy(_pl[upperCell]);

      if (obstacle1.intersects(obstacle2))
      {
        /*
        cout << "Overlap int between\n" << obstacle1 << " on layer " <<
          obs1[obs1i]->getLayer().getIndex() << " and " << obstacle2 << " on layer "
            << obs2[obs2i]->getLayer().getIndex() << endl;
        cout << "cell " << lowerCell << " at " << _pl[lowerCell] << " w " 
          << master1.getWidth() << " h " << master1.getHeight() << " orien "
          << _pl.getOrient(lowerCell)<<" obs at "<< BBox(*obs1[obs1i]) << endl;
        cout << "cell " << upperCell << " at " << _pl[upperCell] << " w "
          << master2.getWidth() << " h " << master2.getHeight() << " orien "
          << _pl.getOrient(upperCell)<<" obs at "<<BBox(*obs2[obs2i]) << endl;
        */
        return true;
      }
    } // for upper cell obstacles
  } // for lower cell obstacles
  return false;
}


bool ViolationReport::checkOverlap(unsigned cellID,
                                   const dbLayerGeom& lGeom)
const
/*
   Check whether any obstacle or port of the placed cell intersects
   with the lGeom feature.
*/
{
  const dbCell& cell = _db.getNetlist().getCellByIdx(cellID);
  const dbMasterCell& master = cell.getMaster();
  //double width = master.getWidth();
  //double height = master.getHeight();
  unsigned layerIndex = lGeom.getLayer().getIndex();
  BBox wire(lGeom);

  // cout << "Presumption of obs intersection for cell " << cellID << endl;
  // Check wire against all cell obstacles
  const ctainerLayerGeoms& obstacles = master.getObstacles();
  for (unsigned obsIndex = 0; obsIndex < obstacles.size(); obsIndex++)
  {
    if (obstacles[obsIndex]->getLayer().getIndex() != layerIndex)
      continue;
    BBox obsBox = master.reOrientRec(*obstacles[obsIndex],
                                     _pl.getOrient(cellID));
    obsBox.TranslateBy(_pl[cellID]);
    if (wire.intersects(obsBox))
      return true;
  }
  
  // Check wire against all cell ports
  itMasterPinLocal pItr;
  for (pItr = master.pinsBegin(); pItr!=master.pinsEnd(); pItr++) 
  {
    const ctainerLayerGeoms& ports=(*pItr)->getPorts();
    for (unsigned portIndex=0; portIndex<ports.size(); portIndex++)
    {
      if (ports[portIndex]->getLayer().getIndex() != layerIndex)
        continue;
      BBox portBox = master.reOrientRec(BBox(*ports[portIndex]),
                                        _pl.getOrient(cellID));
      portBox.TranslateBy(_pl[cellID]);
      if (wire.intersects(portBox))
        return true;
    }
  } // for all ports
  
  return false;
} // checkOverlap


bool ViolationReport::_checkOverlap(unsigned cellID, 
                                    const dbNet& snet)
const
/*
   Check whether any obstacle or port of the placed cell intersects
   with any wiring element of the special net.
   */
{
  const dbCell& cell = _db.getNetlist().getCellByIdx(cellID);
  const dbMasterCell& master = cell.getMaster();
  double width = master.getWidth();
  double height = master.getHeight();
  
  // Iterate over all wire rectangles in special net
  for (unsigned wireIndex=0; wireIndex<snet.getNumWiringLayerGeoms(); wireIndex++)
  {
    const dbLayerGeom& lGeom = snet.getWiringLayerGeom(wireIndex);
    unsigned layerIndex = lGeom.getLayer().getIndex();
    BBox wire = BBox(lGeom);
    BBox cellBounds = BBox(_pl[cellID].x,
                           _pl[cellID].y, 
                           _pl[cellID].x + width,
                           _pl[cellID].y + height);
    
    // Quick test whether wire intersects interior of cell bounds
    if (!wire.intersects(cellBounds))
      continue;
    
    //cout << "Warning; wire does intersect bounds of cell "
    //  << cellID << " which are " << cellBounds;
    // Check wire against all cell obstacles
    const ctainerLayerGeoms& obstacles = master.getObstacles();
    for (unsigned obsIndex = 0; obsIndex < obstacles.size(); obsIndex++)
    {
      if (obstacles[obsIndex]->getLayer().getIndex() != layerIndex)
      {
        //cout << "wire on layer " << layerIndex << " obs on layer "
        //  << obstacles[obsIndex]->getLayer().getIndex() << endl;
        continue;
      }
      BBox obsBox = master.reOrientRec(*obstacles[obsIndex],
                                       _pl.getOrient(cellID));
      obsBox.TranslateBy(_pl[cellID]);
      //cout << "Testing boxes:\n" << wire << obsBox;
      if (wire.intersects(obsBox))
        return true;
    }
    
    // Check wire against all cell ports
    itMasterPinLocal pItr;
    for (pItr = master.pinsBegin(); pItr!=master.pinsEnd(); pItr++) 
    {
      const ctainerLayerGeoms& ports=(*pItr)->getPorts();
      for (unsigned portIndex=0; portIndex<ports.size(); portIndex++)
      {
        if (ports[portIndex]->getLayer().getIndex() != layerIndex)
          continue;
        BBox portBox = master.reOrientRec(BBox(*ports[portIndex]),
                                          _pl.getOrient(cellID));
        portBox.TranslateBy(_pl[cellID]);
        if (wire.intersects(portBox))
          return true;
      }
    } // for all ports
    
  } // for all wire elements
  
  return false;
} // _checkOverlap


unsigned ViolationReport::_isPlacementRowBased ()
{
  unsigned numCoreCells = 0;
  const RBPlacement & rbp = *_rbplace;
  unsigned i;
  unsigned cellsInRows = 0;
  unsigned cellsNotInRows = 0;
  const dbNetlist& netlist = _db.getNetlist();
  abkassert(rbp.getNumCells() <= netlist.getNumCells(),
        "More points in placement than database");

  for (i=0; i<rbp.getNumCells(); i++)
  {
    bool found = false;
    pair<itRBPCoreRow, itRBPCoreRow> CRindex;
    pair<itRBPSubRow,  itRBPSubRow>  SRindex;
    Point   pt(_pl[i]);
    const dbCell& cell = netlist.getCellByIdx(i);

//    cout<<"checking cell "<<i<<"  at "<<pt<<endl;

    if (cell.isCore())
    { // Try to put the cell in a core subrow, by binary search
      numCoreCells++;
      RBPCoreRow CR(pt.y);
      CRindex = std::equal_range(rbp.coreRowsBegin(), rbp.coreRowsEnd(),  
		CR,
                compareCoreRowY());
//      cout<<"  done w/ b-search"<<endl;
      if (CRindex.first < CRindex.second)
      {
//	cout<<"found range"<<endl;
        RBPSubRow SR(pt.x, pt.x);
        SRindex = std::equal_range(CRindex.first->subRowsBegin(),
                              CRindex.first->subRowsEnd(),
                              SR,
                              compareSubRowX());
        if (SRindex.first < SRindex.second)
          found = true;
      }
      // At the moment, we don't try to do anything with
      // non-core cells.
      if (!found)
      {
        cellsNotInRows++;
//	cout<<"not in a row"<<endl;
        logError (Violations("offSite"), i, "Cell not in any Row");
      }
      else cellsInRows++;
    }
  }
  //if (cellsNotInRows)
  //  cout << " RBPlacement : " << cellsNotInRows
  //    << " cell(s) not in rows "<< endl;
  abkfatal((cellsInRows + cellsNotInRows) == numCoreCells++,
       "Populate cell count doesn't match.");
  // Finally(?), sort the cell ids on each subrow by their x coordinate,
  // so cells are left to right in each subrow.
  return cellsNotInRows;
}


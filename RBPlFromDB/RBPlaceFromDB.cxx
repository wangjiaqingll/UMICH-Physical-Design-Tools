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

















//Created on 990529 by Andrew Caldwell
//split from RBPlacement.cxx

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <algorithm>
#include "DB/DB.h"
#include "NetTopology/hgWDimsFromDB.h"
#include "RBPlaceFromDB.h"
#include "RBRowsFromDB.h"

#ifdef _MSC_VER
#ifndef rint
#define rint(a) floor((a)+0.5)
#endif
#endif

using std::vector;
// 初始化参数列表
// 
RBPlaceFromDB::RBPlaceFromDB (const DB& db, RBPlacement::Parameters params) 
	: RBPlacement(db.getNetlist().getNumCells(), db.spatial.locations,
		      db.spatial.orientations, params), 
	  _db(db), _netlist(db.getNetlist())
{
    _hgWDims = new HGraphWDimensionsFromDB(_db);

    constructRows();
    constructCellStats(); 
    splitRowsOnNets();
    splitRowsOnFixedCells();
    setCellsInRows();
    populateCC();
    abkassert(isConsistent(), "inconsistent at end of rows");
    cout<<params<<endl;
    cout<<_params<<endl;
    if(params.remCongestion)
      RBPlBudget();

    //copy the constraints now
    Constraints * thisConstraints = new Constraints(_db.spatial.constraints);
    constraints = *thisConstraints;
    initBBox();
    //initialize the group constraints in RBPlace
    initGroupRegionConstr();
    initRegionUtilization();
}

void RBPlaceFromDB::constructRows()
{
    abkfatal(_coreRows.size() == 0, "can't populate rows more than once");
    unsigned numRows = _db.getLayout().getNumRows();

  // Organize all the rows according to type and y origin
    vector<const dbRow*> rawCoreRows;
    for(unsigned rowIndex = 0; rowIndex < numRows; rowIndex++)
    {
    	const dbRow& row = _db.getLayout().getRowByIdx(rowIndex);
    	if(row.getSite().getType() != dbSite::PAD)
    	{
	    abkassert(row.getDirection() == dbRow::HORIZ,
		"core row without horizontal orientation");
            rawCoreRows.push_back(&row);
    	}
    }
    _coreRows.reserve(rawCoreRows.size());

  // Sort the raw core rows by X coordinate (minor) and Y (major)
    std::stable_sort(rawCoreRows.begin(), rawCoreRows.end(), dbRowXlt());
    std::stable_sort(rawCoreRows.begin(), rawCoreRows.end(), dbRowYlt());

  // Build one RBPCoreRow out of all rows with same y-coordinate
  for(unsigned i=0; i<rawCoreRows.size(); i++)
  {
    Point origin = rawCoreRows[i]->getOrigin();
    double Y = origin.y;
    const dbSite* site = & rawCoreRows[i]->getSite();
    double spacing = rawCoreRows[i]->getSpacing();

    _coreRows.push_back(
		RBPCoreRow(Y,rawCoreRows[i]->getOrientation(),
			   RBPSite(site->getHeight(), site->getWidth(),
				   site->getSymmetry()),_placement,spacing));

    double subRowLen = spacing*(rawCoreRows[i]->getNumSites()-1)+
			site->getWidth();

    _coreRows.back().appendNewSubRow(origin.x, origin.x+subRowLen);

    while((i < rawCoreRows.size()-1) && (rawCoreRows[i+1]->getOrigin().y == Y))
    {
      i++;
      Point origin2 = rawCoreRows[i]->getOrigin();
      subRowLen = spacing*(rawCoreRows[i]->getNumSites()-1)+site->getWidth();
      _coreRows.back().appendNewSubRow(origin2.x, origin2.x+subRowLen);
    }
  }
  unsigned newLength = _coreRows.size()-_params.numRowsToRemove;
  _coreRows.erase(_coreRows.begin()+newLength, _coreRows.end());
}

void RBPlaceFromDB::constructCellStats()
{
    abkfatal(_coreRows.size() > 0,
	"must call constructRows before constructCellStats()");

    const dbSpatial& spatial = _db.spatial;

    double siteHeight = _coreRows[0].getHeight();

    _db.spatial.constraints.getFGCbitVec(_isFixed);

    itCellGlobal dbC;
    for(dbC = _netlist.cellsBegin(); dbC != _netlist.cellsEnd(); dbC++)
    {
	unsigned cellId = (*dbC)->getIndex();

	if((*dbC)->isIO()) 
	{   
	    abkfatal(spatial.locationValid[cellId], "unplaced pad cell"); 
	    abkwarn(_isFixed[cellId], "Unfixed pad....RBPL is fixing it");
	}
	else if((*dbC)->isCore())
	{ 
	    _isCoreCell[(*dbC)->getIndex()] = true; 
	    abkwarn((*dbC)->getHeight() <= siteHeight,
		     "Overtall core cell");
	    if(_isFixed[(*dbC)->getIndex()])
	    {
	        if(_params.verb.forMajStats > 4)
	  	    cout<<"Fixed core cell "<<(*dbC)->getIndex()<<endl;
	        abkfatal(spatial.locationValid[cellId], "unplaced fixed cell");
	    }
	}
	else if((*dbC)->isBlock() || (*dbC)->isOther())
	{      //it is assumed that all blocks are fixed
	    abkfatal(spatial.locationValid[cellId], "unplaced block");

	    if(_params.verb.forMajStats > 4)
	    {
	      cout<<"Fixed Block: "<<cellId<<" at "<<_placement[cellId]<<endl;
	      cout<<**dbC<<endl;
	    }
	}
	else
	{
	    cout<<"Cell with type neither io, block, other, or core."<<endl;
	    cout<<**dbC<<endl;
	    abkfatal(0, "cell of type not yet handled");
	}
    }

    abkfatal(_storElt.size()==_isFixed.size(),
        "RBPlaceFromDB: _storElt.size() != _isFixed.size()\n ");
    unsigned j=0, numStor=0;
    for(j=0; j!=_isFixed.size(); j++)
    {
      const dbCell&        cell=_db.getNetlist().getCellByIdx(j);
      const dbMasterCell& mcell=cell.getMaster();
      if (mcell.isCore())
//  if (mcell.isFlipFlop() && mcell.isLatch()) 
      if (cell.isSC()) { _storElt[j]=true; numStor++; }
    }
    cout << " Found " << numStor << " store elements " << endl;
}

void RBPlaceFromDB::splitRowsOnNets()
{ // Split subRows on specialNet boundaries.

    const unsigned numSNets = _netlist.getNumSNets();

    bit_vector libLayers(_db.getLayout().getNumLayers(), false);
  	//libLayers[x] is true if the cell lib uses that layer

    for(itMasterCell mc=_netlist.cellLibBegin(); mc!=_netlist.cellLibEnd();mc++)
    {
      if(!(*mc)->isCore())  continue;
      const ctainerLayerGeoms& obstacles = (*mc)->getObstacles();
      for(unsigned i = 0; i < obstacles.size(); i++)
	libLayers[obstacles[i]->getLayer().getIndex()] = true;
    }

    itMasterPinGlobal mp;
    for(mp=_netlist.pinLibBegin(); mp != _netlist.pinLibEnd(); mp++)
    {
      if(!(*mp)->getCell().isCore())  continue;
      const ctainerLayerGeoms& pinGeoms = (*mp)->getPorts();
      for(unsigned i = 0; i < pinGeoms.size(); i++)
	libLayers[pinGeoms[i]->getLayer().getIndex()] = true;
    }

    for(unsigned o = 0; o < libLayers.size(); o++)
    	if(libLayers[o]) cout<<" Cell Lib uses layer "<<o<<endl;


    unsigned snetIndex, geoIndex;
      
    for(snetIndex=snetOffset; snetIndex<numSNets+snetOffset; snetIndex++)
    {
      const dbNet& snet = _netlist.getSNetByIdx(snetIndex);

      for(geoIndex=0; geoIndex<snet.getNumWiringLayerGeoms(); geoIndex++)
      {
        const dbLayerGeom& lg = snet.getWiringLayerGeom(geoIndex);
        const dbLayer&     layer = lg.getLayer();
        Rectangle obstacle = Rectangle(lg);

		//this wire is not on a layer used by the cell lib, so
		//it will not invalidate any sites
	if(!libLayers[layer.getIndex()])  continue;

	if(layer.getType() == dbLayer::MASTERSLICE ||	//poly
	   layer.getType() == dbLayer::CUT) 		//vias
		continue; //this is assumed not to invalidate placement sites



    	for(unsigned i=0; i<_coreRows.size(); i++)
    	{
      	  RBPCoreRow& cr = _coreRows[i];

      	  double coreRowYMin = cr.getYCoord();
      	  double coreRowYMax = cr.getYCoord() + cr.getHeight();

		//a specialNet is assumed only to invalidate sites if
		//it crosses the height of the row (otherwise it is likely
		//to be a power/ground rail, which is fine
          if((obstacle.yMin > coreRowYMin) || 
	     (obstacle.yMax < coreRowYMax))  continue;

          splitRowByObstacle(cr, obstacle);
	
        }
      }
    } 

	//doing this screws up the coreRows pointers inside the sub rows...
    unsigned crId;
    for(crId = 0; crId < _coreRows.size(); crId++)
    if(_coreRows[crId].getNumSubRows() == 0)
    {
	_coreRows.erase(_coreRows.begin() + crId);
	crId--;
    }
	//...so we have to reset them manually	
    for(crId = 0; crId < _coreRows.size(); crId++)
    {
      RBPCoreRow& cr = _coreRows[crId];
      for(unsigned srId = 0; srId < cr.getNumSubRows(); srId++)
	cr._subRows[srId]._coreRow = &cr;	
    }
}

void RBPlaceFromDB::splitRowsOnFixedCells()
{
    unsigned c;
    for(c = 0; c < getNumCells(); c++)
    {
      if(_isFixed[c])
      {
	const dbCell& block = _netlist.getCellByIdx(c);

        Rectangle obstacle(_placement[c]);
	/* by Xiaojian,ported by sadya for macro-rotation problem  
	   obstacle.xMax = obstacle.xMin + block.getWidth();
	   obstacle.yMax = obstacle.yMin + block.getHeight();*/

	Orient nodeOrient = _placement.getOrient(c);
        if(nodeOrient == Orient("N") || nodeOrient == Orient("S") || 
	   nodeOrient == Orient("FN") || nodeOrient == Orient("FS"))
	  {
	    obstacle.xMax = obstacle.xMin + block.getWidth();
	    obstacle.yMax = obstacle.yMin + block.getHeight();
	  }
        else
	  {
	    obstacle.xMax = obstacle.xMin + block.getHeight();
	    obstacle.yMax = obstacle.yMin + block.getWidth();
	  }
	/* end by Xiaojian */
	
	for(unsigned crIdx = 0; crIdx < _coreRows.size(); crIdx++)
	  {
	  RBPCoreRow& cr = _coreRows[crIdx];

	  if(obstacle.yMin <= cr.getYCoord() &&
	     obstacle.yMax >  cr.getYCoord())
          	splitRowByObstacle(cr, obstacle);
        }
      }
    } 

	//doing this screws up the coreRows pointers inside the sub rows...
    unsigned crId;
    for(crId = 0; crId < _coreRows.size(); crId++)
    if(_coreRows[crId].getNumSubRows() == 0)
    {
	_coreRows.erase(_coreRows.begin() + crId);
	crId--;
    }
	//...so we have to reset them manually	
    for(crId = 0; crId < _coreRows.size(); crId++)
    {
      RBPCoreRow& cr = _coreRows[crId];
      unsigned srId;
      for(srId = 0; srId < cr.getNumSubRows(); srId++)
	cr._subRows[srId]._coreRow = &cr;	
    }
}


//added by sadya to relieve congestion by blowing up cells

void RBPlaceFromDB::RBPlBudget()
{
  itRoutedNet routedNet;
  vector<double> nodeBudgets;
  nodeBudgets.resize(getNumCells());
  std::fill(nodeBudgets.begin(), nodeBudgets.end(), 0);

  for(routedNet = _db.routedNets.routedNetsBegin();
      routedNet != _db.routedNets.routedNetsEnd(); ++routedNet)
    {
      if(!(*routedNet)->isInDB())
	continue;

      unsigned netIdx = (*routedNet)->getNetIdx();
      const HGFEdge net = _hgWDims->getEdgeByIdx(netIdx);
      unsigned netDegree = net.getDegree();

      if(netDegree > 3) continue;

      double netHPWL = oneNetHPWL(netIdx, 1);
      double netRouteWL = (*routedNet)->getTotalLength();
      double netBudget = netRouteWL - netHPWL;
      double budget = netBudget/netDegree;
      for(itHGFNodeLocal node=net.nodesBegin(); node!=net.nodesEnd(); ++node)
	{
	  unsigned nodeIdx=(*node)->getIndex();
	  nodeBudgets[nodeIdx] += budget;
	}
    }

  double sumBudget=0;
  for(unsigned i=0; i<nodeBudgets.size(); ++i)
    sumBudget += nodeBudgets[i];

  vector<Budget> budgets;
  for(unsigned i=0; i<nodeBudgets.size();++i)
    {
      Budget tempBudget;
      tempBudget.index = i;
      tempBudget.budget = nodeBudgets[i];
      budgets.push_back(tempBudget);
    }

  std::sort(budgets.begin(), budgets.end(),sort_budgets());

  //calculate the total no: available white space available
  unsigned numDeadSites=0;

  itRBPCoreRow itc;
  itRBPSubRow its;
  itRBPCellIds itn;
  double siteWidth=1;

  for(itc=coreRowsBegin();itc!=coreRowsEnd();++itc)
    {
      for(its=itc->subRowsBegin();its!=itc->subRowsEnd();++its)
	{
	  siteWidth = its->getSpacing();
	  for(itn=its->cellIdsBegin();itn!=its->cellIdsEnd();++itn)
	    {
	      if(itn!=its->cellIdsEnd()-1)
		{
		  if(_placement[*(itn+1)].x >
		     _placement[*itn].x+_hgWDims->getNodeWidth(*itn))
		  {
		    double whiteSpaceWidth = _placement[*(itn+1)].x -
		      _placement[*itn].x+_hgWDims->getNodeWidth(*itn);
		    numDeadSites += int(whiteSpaceWidth/siteWidth);
		  }
		}
	    }
	}
    }
  unsigned availDeadSites = numDeadSites/2; //make 50 % available
  double budgetPerWS = sumBudget/availDeadSites;
  
  for(int i=budgets.size()-1;i >= 0;--i)
    {
      int sitePerCell = int(budgets[i].budget/budgetPerWS);
      if(sitePerCell <= 0 )
	break;

      double oldWidth = _hgWDims->getNodeWidth(budgets[i].index);
      double bloatWidth=0;
      if(sitePerCell*siteWidth >= oldWidth*0.25)  //limit increase in size to
	                                       //0.25 times original size
	bloatWidth = oldWidth*0.25;
      else
	bloatWidth = sitePerCell*siteWidth;

      double newWidth = oldWidth + bloatWidth;
      _hgWDims->setNodeWidth(newWidth, budgets[i].index);

//      cout<<"sitePerCell "<<sitePerCell<<" addition "<<bloatWidth<<endl;

      availDeadSites -= sitePerCell;
      if(availDeadSites <= 0 )
	break;
    }

}





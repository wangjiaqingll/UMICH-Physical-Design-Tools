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
   97.8.8  Paul Tucker

   Defintions of large member functions for dbEasyAccess class.
*/
// CHANGES
// 970813 mro moved struct equnsigned to file scope otherwise can't
//            use in templates in WinNT.
// 970813 mro added pragma to turn off warning 4786
// 970813 pat/aec isConsistent function added
// 970814 aec changed isConsistent to output to file easyAccess.out
//		added checking that cells w/0 width are ExtPin
// 970814 pat/aec added numLayers check to isConsistent
// 970815 aec  added code to set _totalCellWidth to populateCellWidths
// 970822 aec  updated isConsistent and populateNetIds to deal with
//		 pins not on a net.
// 970829 ilm  reworked stream output so that it does not confuse
//               diff (and human readers)   
// 970829 ilm  added support for coreCellsEmbedding
//        ilm  reworked some stream output yet again
// 970917 pat  set coreCells data on example easyAccess structure
// 970919 pat  added spatial to populate parameters,
//             added orienation handling to whitespace and
//             portlocation populating functions
// 971006 ilm  added dbEasyAccess::_pin2CellIdx to cache numbers of 
//                containg cells for all pins. 
//                Note: this is not a Mapping (not injective)
//        ilm  added dbEasyAccess::populatePin2CellIdx() which is now
//                called by dbEasyAccess::populate()
// 971129 pat  redid populatePortLocations; added seed argument to
//             the random version of populate; small changes to i/o
//             in agreement with these other changes
// 971205 pat  changed mistaken masterPin to mPin in pop.port.loc.
// 971209 mro  liberalized check of _netIds to allow for special nets

#ifdef  _MSC_VER
#pragma warning (disable:4786)
#endif

#include <iomanip>
#include <ABKCommon/sgi_hash_map.h>
#include <utility>
using std::pair;

#include "ABKCommon/abkcommon.h"
#include "Geoms/interval.h" 
#include "DB.h"
#include "dbEasyAccess.h"
#include "dbContext.h"

class CompareByStart
{
public:
    CompareByStart(){}

    bool operator() (const Interval i1, const Interval i2) const
    { 
	if(i1.low < i2.low)
		return true;
	else if(i1.low > i2.low)
		return false;
	else if(i1.high < i2.high)
		return true;
	else if(i1.high > i2.high)
		return false;
	else //they're identical
	return (&i1 < &i2);
    }
};


void dbEasyAccess::populate (const dbContext& context,
			     const dbNetlist& netlist,
			     const dbSpatial& spatial,
                             const vector<char*>& fileNames, const char* dir)
{
  populateCellWidths(netlist);
  populateCellAreas(netlist);
  populateWhitespace(context, netlist, spatial);
  populateNetIds(netlist);
//populatePortLocations(context, netlist, spatial);
  populateCoreCellEmbedding(netlist);
  populateCoreBBox(context);
  populatePin2CellIdx(netlist); 
  populateNameHashTables(netlist);

  unsigned k;
  char rcFileName[1024], *ptr=NULL;
  for(k=0; k!=fileNames.size(); k++)
  {
     if (!strstr(fileNames[k],".rc") && !strstr(fileNames[k],".RC"))
        continue;
     cout << " Found potential RC-file name "   << fileNames[k] << "...\n";
     if(fileNames[k][0] == '/')   // absolute path
     {
        strcpy(rcFileName, fileNames[k]);
        ptr=rcFileName;
     }
     else
     {
        cout << " will assume it is in directory " << dir << endl;
        strcpy(rcFileName,dir);
        ptr=rcFileName+strlen(dir);
        strcpy(ptr,fileNames[k]);
     }
  }

  if (!ptr) { cout << " No RC-file name found in DB" << endl; }
  else populateSimpleTiming(netlist,rcFileName);
  
}

ostream& operator<< (ostream& out, const dbEasyAccess& arg)
{
  unsigned i;

  abkfatal(arg.getCellWidths().size()==arg.getCellAreas().size(),
           " Size mismatch in cell information stored in dbEasyAccess\n");
  abkfatal(arg.getCoreCells().getDestSize()==arg.getCellAreas().size(),
           " Size mismatch in cell information stored in dbEasyAccess\n");

  out << "--- EasyAccess object ------------- " << endl << endl
      << "    Total cells : " << arg.getCellWidths().size() << endl 
      << "    Core  cells : " << arg.getCoreCells().getSourceSize()
                                                                 << endl
      << "    Whitespace layers : " << arg.getWhitespace().size() << endl
      << "    Total  pins : " << arg.getNetIds().size() << endl 
      << "    Port layers : " << arg.getNumPortLocations() << endl
      << "    Core BBox   : " << arg._coreBBox << endl;
	
  out << endl << "  Core cell indices : ";
 
  for (i=0; i<arg.getCoreCells().getSourceSize(); i++)
  {
    if ( i % 13 == 0 ) out << endl << "  " ;
    out << setw(5) << arg.getCoreCells()[i] << " " ;
  }
 
  out << endl << endl << "  cellWidths: ";

  for (i=0; i<arg.getCellWidths().size(); i++)
  {
    if ( i % 5 == 0 ) out << endl << "   " ;
    out << setw(13) << arg.getCellWidths()[i] << " " ;
  }
  out << endl << endl << "  cellAreas:  ";
  for (i=0; i<arg.getCellAreas().size(); i++)
  {
    if ( i % 5 == 0 )  out<<endl<<"   ";
    out << setw(13) << arg.getCellAreas()[i] << " " ;
  }
  out << endl << endl;
  out << "  Whitespace:  ";
  for (i=0; i<arg.getWhitespace().size(); i++)
  {
    out << endl << "    Layer " << i << ": " <<endl;
    const IntervalSeqLayer& layer = arg.getWhitespace()[i];
    unsigned j;
    for (j=0; j<layer.size(); j++)
          out << "      " << layer[j] ;
  }
  out << endl << "  Net Ids:   ";
  for (i=0; i<arg.getNetIds().size(); i++)
  {
    if ( i % 12 == 0 ) out<<endl<<"  ";
    if(arg.getNetIds()[i] == (unsigned)(-1) )
        out << setw(5) << -1 << " ";
    else 
        out << setw(5) <<arg.getNetIds()[i] << " ";
  }
  out << endl << endl;
  out << "  Port locations: " << endl;
  for (i=0; i<arg.getNumPortLocations(); i++)
  {
    out << endl << "    Layer " << i << ": " <<endl;
    const ISPairLayer& layer = arg.getPortLocationByIdx(i);
    unsigned j;
    for (j=0; j<layer.size(); j++)
      out << "     Top:" << layer[j].first
	<< "     Bot:" << layer[j].second;
  }
  out << endl;
  return out;
}

void dbEasyAccess::populateCellWidths (const dbNetlist& netlist)
{
  unsigned numCells = netlist.getNumCells();
  unsigned i;
  _totalCellWidth = 0.0;

  _cellWidths = vector<double>(numCells);
  
  for(i=0; i < numCells; i++)
  {
    _cellWidths[i] = netlist.getCellByIdx(i).getMaster().getWidth();
    _totalCellWidth += _cellWidths[i];
  }
}


void dbEasyAccess::populateCellAreas (const dbNetlist& netlist)
{
  unsigned numCells = netlist.getNumCells();
  unsigned i;

  _cellAreas = vector<double>(numCells);
  
  for(i=0; i < numCells; i++)
  {
    const dbMasterCell& master = netlist.getCellByIdx(i).getMaster();
    _cellAreas[i] = master.getWidth() * master.getHeight();
  }
}


void dbEasyAccess::populateNetIds (const dbNetlist& netlist)
{
  unsigned numPins = netlist.getNumPins();
  unsigned i;
  
  _netIds = vector<unsigned>(numPins);

  for (i=0; i < numPins; i++)
  {
	if(netlist.getPinByIdx(i).isConnected() )
	    _netIds[i] = netlist.getPinByIdx(i).getNet().getIndex();
	else
	    _netIds[i] = (unsigned)(-1);
  }
}

struct equnsigned
  {
    bool operator()(unsigned a1, unsigned a2) const
    {
      return a1 == a2;
    }
  };  

void dbEasyAccess::populateWhitespace (const dbContext& context,
				       const dbNetlist& netlist,
				       const dbSpatial& spatial)
/* Cell obstructions are stored on the mastercell as
   a container of dbLayerGeom objects, which are rectangles
   assigned to some layer.  For each cell, we initially model it
   as a single whitespace interval over its entire length,
   then iterate over the obstructions, splitting and/or trimming
   the remaining whitespace intervals until all obstruction
   intervals have been removed.
*/
{
  unsigned numCells = netlist.getNumCells();
//unsigned numLayers = context.getNumLayers();
  hash_map<unsigned, unsigned, hash<unsigned>, equnsigned> 
           relevantLayers;
  unsigned i,j;
  unsigned numRelLayers = 0;
  itLayer itr;

  // Figure out which layers need whitespace representation

  for (itr = context.beginLayers(); itr != context.endLayers(); itr++)
  {
    if (  ((*itr)->getType() == dbLayer::ROUTING)
	&&((*itr)->getDirection() == dbLayer::VERT))
    {
      relevantLayers[(*itr)->getIndex()] = numRelLayers++;
    }
  }

  _whitespace = vector<IntervalSeqLayer>(numRelLayers);
  
  for (i = 0; i < numRelLayers; i++)
  {
    // initialize the layer to have numCells IntervalSeqs
    _whitespace[i] = IntervalSeqLayer(numCells);
    for (j = 0; j < numCells; j++)
    {
      // Initialize each IntervalSeq with a single interval covering
      // the width of the cell in its current orientation
      const dbMasterCell& master = netlist.getCellByIdx(j).getMaster();
      const Orientation& orien = spatial.orientations[j];
      unsigned angle = orien.getAngle();
      double w;
      
      if ((angle == 90) || (angle == 270))
	w = master.getHeight();
      else
	w = master.getWidth();
      
      _whitespace[i][j].push_back(Interval(0,w));
    }
  }

  // Iterate over all cells, creating the whitespace representation.
  // Each obstacle is blanked out of the initial whitespace field,
  // in current orientation.
  // This could be made faster someday by only calculating for
  // mastercells, then copying for individual cells.
// ATTN: should ports be wiped out too?

  for (i=0; i<numCells; i++)
  {
    const dbMasterCell& master = netlist.getCellByIdx(i).getMaster();
    const ctainerLayerGeoms& layerGeoms = master.getObstacles();
    
    for (j=0; j<layerGeoms.size(); j++) 
    {
      unsigned globLayerIdx = layerGeoms[j]->getLayer().getIndex();
      if (relevantLayers.find(globLayerIdx) != relevantLayers.end())
      {
	IntervalSeq& IS = _whitespace[relevantLayers[globLayerIdx]][i];
	Rectangle rect = master.reOrientRec(*layerGeoms[j],
					    spatial.orientations[i]);
	IS.blankInterval(Interval(rect.xMin, rect.xMax));
      }
    }
  }
}

  struct eqlkey 
  {
    bool operator() (const pair<const dbMasterPin*, Orient> e1,
		     const pair<const dbMasterPin*, Orient> e2)
      const
      {
	if (  (e1.first == e2.first)
	    ||(e1.second == e2.second))
	  return true;
	else
	  return false;
      }
  };

  struct keyHash 
  {
    bool operator() (pair<const dbMasterPin*, Orient> p) const
    {
      hash<unsigned> H;
//    unsigned K = reinterpret_cast<unsigned>(p.first);
// the code above is not 64-bit safe; its replacement by Xiaojian is below
      unsigned long K1= reinterpret_cast<unsigned long>(p.first);
      unsigned K = (unsigned)K1;

      K += p.second.getAngle() + p.second.isFaceUp();
      return H(K);
    }
  };

  typedef  hash_map < pair<const dbMasterPin*, Orient>, 
                      unsigned, keyHash, eqlkey>
	    PinTable;

void dbEasyAccess::populatePortLocations (const dbContext& context,
					  const dbNetlist& netlist,
					  const dbSpatial& spatial)
/* 
   The portLocations entry of dbEasyAccess contains, for every 
   pin on a core cell, a pair of IntervalSeqs that describe
   the unobstructed port image visible from above (higher Y coord)
   and below the cell.  The images for a Y axis flipped cell are
   obtained just by reversing these seqs.

   For relatively efficient generation, we only generate images on
   demand, for each mastercell/orientation encountered in the core.
   Each time we build an image, we record in the hashtable the
   mastercell and orientation used, and the pin index.  Then,
   if another pin instance has the same master and orientation,
   we copy it.
*/
{
  unsigned numPins = netlist.getNumPins();
  unsigned pinIndex = 0, i;
  
  PinTable pinTable;
  
//unsigned numCells = netlist.getNumCells();
//unsigned numLayers = context.getNumLayers();
  typedef   hash_map<unsigned, unsigned, hash<unsigned>, equnsigned> 
         LayerIndexTable;
  LayerIndexTable relevantLayers;
  unsigned numRelLayers = 0;

  // Find the vertical routing layers, and assign indices

  for (itLayer itr = context.beginLayers(); itr != context.endLayers(); itr++)
  {
    if (  ((*itr)->getType() == dbLayer::ROUTING)
	&&((*itr)->getDirection() == dbLayer::VERT))
    {
      relevantLayers[(*itr)->getIndex()] = numRelLayers++;
    }
  }
  
  _portLocations = vector<ISPairLayer>(numRelLayers);
  for (i=0; i<numRelLayers; i++)
    _portLocations[i] = 
      ISPairLayer(numPins, ISeqPair(IntervalSeq(),IntervalSeq()));
  
  for (pinIndex=0; pinIndex < numPins; pinIndex++)
  {
    const dbPin& pin = netlist.getPinByIdx(pinIndex);
    const dbMasterPin& masterPin = pin.getMaster();
    const dbCell& cell = pin.getCell();
    
    // Determine whether this pin is on a core cell; we
    // only care about core cell pins.
    if (!cell.isCore()) continue;

    unsigned cellIdx = cell.getIndex();
    Orient orient = spatial.orientations[cellIdx];
    if (pinTable.find(pair<const dbMasterPin*, Orient>(&masterPin, orient))
	== pinTable.end())
    { 
      // Process all the pins and ports on the master cell
      // as a group.
      const dbMasterCell& masterCell = cell.getMaster();

      // Iterate over every pin on the instance cell, and look
      // up its master (assumed to be on the master cell)
      for (itPinLocal instPin = cell.pinsBegin();
	   instPin != cell.pinsEnd();
	   ++instPin)
      {
	unsigned instPinIdx = (*instPin)->getIndex();
	const dbMasterPin& mPin = (*instPin)->getMaster();
	abkassert(instPinIdx < numPins,
		  "Out of range pin index on cell pins list");

	// Iterate over every port on the pin
	const ctainerLayerGeoms& ports = mPin.getPorts();
	unsigned portIndex;
	for (portIndex = 0; portIndex < ports.size(); portIndex++)
	{    
	  //const dbLayer layer = ports[portIndex]->getLayer();

	  // Iterate over the vertical routing layers;
	  LayerIndexTable::const_iterator layerItr;
	  for (layerItr = relevantLayers.begin();
	       layerItr != relevantLayers.end();
	       ++layerItr)
	  {
	    unsigned realLayerIdx = (*layerItr).first;
	    unsigned RelLayerIdx = (*layerItr).second;
	    Rectangle rect =
	      masterCell.reOrientRec(*ports[portIndex], orient);
	    abkfatal(rect.xMin < rect.xMax, "Rectangle geometry invalid");
	    IntervalSeq above, below;
	    above.push_back(Interval(rect.xMin, rect.xMax));
	    below.push_back(Interval(rect.xMin, rect.xMax));
	    
	    // Now do an inner loop, for all _other_ pins and obstructions
	    // on the master cell, blanking out the obstructed intervals.
	    for (itMasterPinLocal omPin = masterCell.pinsBegin();
		 omPin != masterCell.pinsEnd();
		 ++omPin)
	    {
	      if (*omPin == &mPin) continue;
	      
	      const ctainerLayerGeoms& oPorts = (*omPin)->getPorts();
	      unsigned oPortIndex;
	      for (oPortIndex = 0; oPortIndex < oPorts.size(); oPortIndex++)
	      {
		if (oPorts[oPortIndex]->getLayer().getIndex()
		    != realLayerIdx)
		  continue;   // no obstruction 
		Rectangle oRect = 
		  masterCell.reOrientRec(*oPorts[oPortIndex], orient);
		
		if (oRect.yMax >= rect.yMax)
		  above.blankInterval(Interval(oRect.xMin, oRect.xMax));
		if (oRect.yMin <= rect.yMin)
		  below.blankInterval(Interval(oRect.xMin, oRect.xMax));
	      }
	    } // for all other ports

	    const ctainerLayerGeoms& obstacles = masterCell.getObstacles();
	    for (unsigned oIdx=0; oIdx < obstacles.size(); ++oIdx)
	    {
	      if (obstacles[oIdx]->getLayer().getIndex() != realLayerIdx)
		continue;
	      Rectangle oRect =
		masterCell.reOrientRec(*obstacles[oIdx], orient);
	      
	      if (oRect.yMax >= rect.yMax)
		above.blankInterval(Interval(oRect.xMin, oRect.xMax));
	      if (oRect.yMin <= rect.yMin)
		below.blankInterval(Interval(oRect.xMin, oRect.xMax));
	    } // for all obstructions
	    
	    // Now put above and below in the proper place.
	    above.canonicalize();
	    below.canonicalize();
	    _portLocations[RelLayerIdx][instPinIdx].first.merge(above);
	    _portLocations[RelLayerIdx][instPinIdx].second.merge(below);
	    
	  } // for every vertical routing layer
	  
	} // for every port
	
	// Record that this pin has a good image
	pinTable[pair<const dbMasterPin*, Orient>(&masterPin, orient)]
	  = instPinIdx;
	
      } // for every pin on mastercell
      
    } // if pin not found in hashtable
    
    else
    {
      // copy image from the recorded instance
      unsigned goodPinIdx = pinTable[pair<const dbMasterPin*, Orient>
				     (&masterPin, orient)];
      unsigned j;
      for (j=0; j<numRelLayers; j++)
      {
	_portLocations[j][pinIndex] =
	  _portLocations[j][goodPinIdx];
      }
    }
    
  } // for every pin in database

} // populatePortLocations

      

void dbEasyAccess::populateCoreCellEmbedding(const dbNetlist& netlist)
{
  vector<unsigned>   coreCellIndices;
  unsigned           idx=0;
  itCellGlobal cItr;
  for(cItr = netlist.cellsBegin(); cItr != netlist.cellsEnd(); cItr++,idx++)
     if ((*cItr)->isCore()) coreCellIndices.push_back(idx);

  _coreCellEmbedding=
       Subset (coreCellIndices.size(),netlist.getNumCells(),coreCellIndices);
}

void dbEasyAccess::populateNameHashTables(const dbNetlist& netlist)
{
    for(itCellLocal cellIt = netlist.cellsBegin();
        cellIt != netlist.cellsEnd(); cellIt++)
        _cellNameTable[(*cellIt)->getName()] = *cellIt;

    for(itNetLocal netIt = netlist.netsBegin();
        netIt != netlist.netsEnd(); netIt++)
        _netNameTable[(*netIt)->getName()] = *netIt;

}

void dbEasyAccess::populateCoreBBox(const dbLayout& context)
// make sure _coreBox is filled correctly
{
  unsigned numRows = context.getNumRows();
  unsigned rowIndex;
  for (rowIndex = 0; rowIndex < numRows; rowIndex++)
  {
    const dbRow& row = context.getRowByIdx(rowIndex);
    if (row.getSite().getType() == dbSite::CORE)
    {
       abkwarn(row.getDirection() == dbRow::HORIZ,
          "dbEasyAccess: non-horizontal core row found ");
       _coreBBox += row.getOrigin();
       double xEnd= row.getOrigin().x + row.getLength();
       if (xEnd > _coreBBox.xMax) _coreBBox.xMax = xEnd;
    }
  }
}

void dbEasyAccess::populate (dbEasyAccess::Examples ex,
			     unsigned numCells,
			     unsigned numNets,
			     unsigned numPins,
			     unsigned seed)
/*
   Create pseudo-random definitions that can be used by
   other example constructors.

   Currently supported examples are:

   1) IntRandom: all values are non-neg integers (in double format),
     with clock-set random seed.
   2) IntLessRandom: all values are non-neg integers with random seed
     set to a fixed value, for repeatability.
*/
{
  Timer tm;
  unsigned seed1,seed2,seed3,seed4;
  unsigned i;

  switch (ex)
  {
  case IntRandom:       // for stochastic testing
    //ranCellWidth.CPUseed(tm.getUnixTime());
    //ranNetId.CPUseed(tm.getUnixTime());
    //ranWhite.CPUseed(tm.getUnixTime());
    //ranPort.CPUseed(tm.getUnixTime());
    seed1 = static_cast<unsigned>(tm.getUnixTime());
    seed2 = static_cast<unsigned>(tm.getUnixTime()+1);
    seed3 = static_cast<unsigned>(tm.getUnixTime()+2);
    seed4 = static_cast<unsigned>(tm.getUnixTime()+3);
    break;

  case IntLessRandom:   // for consistent results
    //ranCellWidth.seed(0);
    //ranNetId.seed(0);
    //ranWhite.seed(0);
    //ranPort.seed(0);
    seed1=seed;
    seed2=seed + 1;
    seed3=seed + 2;
    seed4=seed + 3;
    break;

  default: abkfatal(false,"Unknown placement example");
  }

  RandomUnsigned ranCellWidth(8,20,seed1);
  RandomUnsigned ranNetId(0,numNets,seed2);
  RandomIntervalSeqGenerator ranWhite(0,1,3,8,24,2,seed3);
  RandomIntervalSeqGenerator ranPort(0,1,2,8,24,4,seed4);
  vector<unsigned> coreCellIndices;

  _cellWidths = vector<double>(numCells);
  _cellAreas = vector<double>(numCells);
  _whitespace = vector<IntervalSeqLayer>(1);
  _whitespace[0] = IntervalSeqLayer(numCells);
  _netIds = vector<unsigned>(numPins);
  _portLocations = vector<ISPairLayer>(1, ISPairLayer(numPins));

  for (i=0; i<numCells; i++) 
  {
    _cellWidths[i] = unsigned(ranCellWidth);
    _cellAreas[i] = _cellWidths[i];    // Assume unit height
    _whitespace[0][0] = IntervalSeq(0);
    ranWhite.setInt(_whitespace[0][i], static_cast<int>(rint(_cellWidths[i])));
    coreCellIndices.push_back(i);
  }

  _coreCellEmbedding =
    Subset (coreCellIndices.size(),numCells,coreCellIndices);

  for (i=0; i<numPins; i++)
  {
    _netIds[i] = unsigned(ranNetId);
    // Observe that there is no connection between pins and
    // cells, so there is no connection between port locations
    // and width of any particular cell
    ranPort.setInt(_portLocations[0][i].first, ranCellWidth);
    _portLocations[0][i].second = _portLocations[0][i].first;
  }
  
}

void dbEasyAccess::populatePin2CellIdx(const dbNetlist& netlist)
// populates _pin2CellIdx by putting the number of containing cell for each pin
// note _pin2CellIdx is not a map
{
    itPinGlobal pItr=netlist.pinsBegin();
    unsigned    n,nPins=netlist.getNumPins();

    _pin2CellIdx=vector<unsigned>(nPins,unsigned(-1));

    for(n=0; n!=nPins; pItr++, n++)
       _pin2CellIdx[n]= (*pItr)->getCell().getIndex();
}

const dbCell& dbEasyAccess::getCellByName(const char* name) const
{
    hash_map<const char*, const dbCell *, hash<const char*>, 
     CompareNameEqual>::const_iterator cellIt = _cellNameTable.find(name);
    abkfatal(cellIt!= _cellNameTable.end(),"cell is not found in the hash_map");
    return *(*cellIt).second;
}

const dbNet& dbEasyAccess::getNetByName(const char* name) const
{
    hash_map<const char*, const dbNet *, hash<const char*>, 
     CompareNameEqual>::const_iterator netIt = _netNameTable.find(name);
    abkfatal(netIt != _netNameTable.end(),"net is not found in the hash table");
    return *(*netIt).second;
}

bool dbEasyAccess::isConsistent (const dbContext& context,
                              const dbNetlist& netlist) const
/*
   Does some simple self checking on data structures
   after they have been populated.
   Returns true if things appear OK, false otherwise.
*/

{
  bool looksOK = true;
  bool status = true;
  unsigned numLayers = context.getNumLayers();
  unsigned i;
  
  looksOK = (_cellWidths.size() == netlist.getNumCells() );
  if(!looksOK)
  {
      cerr<<"Size of _cellWidths is "<<_cellWidths.size()<<endl;
      abkwarn(0, "EasyAccess: cellWidths has incorrect number of items");
      status = false;
  }

  if (_cellWidths.size()!=_cellAreas.size())
  {
    abkwarn(0, " Size mismatch in cell information stored in dbEasyAccess\n");
    status = false;
  }

  if (_coreCellEmbedding.getDestSize()!=_cellAreas.size())
  {
    abkwarn(0," Size mismatch in cell information stored in dbEasyAccess\n");
    status = false;
  }
 
  // Are 0 width and area cells legit?  They occur in our test
  // cases, so they're permitted here, but maybe they should
  // not be allowed.   pat 97.8.13
  //  cell widths/areas of 0 should only occur for extPins..
  //	added a check for this.  In all other cases, it's an error. (-AEC ?)

  if (looksOK)
  {
    for (i=0; i<_cellWidths.size(); i++)
    {
      if(netlist.getCellByIdx(i).getMaster().isExtPin() )
	looksOK = (looksOK && (_cellWidths[i] == 0));
		//extpins should have width==0
      else 
      	looksOK = (looksOK && (_cellWidths[i] > 0));
    }
  }

  if(!looksOK)
  {
      abkwarn(0, "EasyAccess failed cellWidths Check");
      status = false;
  }
 
  looksOK = (_cellAreas.size() == netlist.getNumCells());
  if(!looksOK)
  {
	abkwarn(0, "EasyAccess: cellAreas has incorrect number of items");
	status = false;
  }

  if (looksOK)
  {
    for (i=0; i<_cellAreas.size(); i++)
    {
      if(netlist.getCellByIdx(i).getMaster().isExtPin() )
	looksOK = (looksOK && (_cellAreas[i] == 0));
      else
      	looksOK = (looksOK && (_cellAreas[i] > 0));
    }
  }
 
  if(!looksOK)
  {
        abkwarn(0, "EasyAccess fails CellAreas check");
	status = false;
  }

  looksOK = (_netIds.size() == netlist.getNumPins());
  if(!looksOK)
  {
	abkwarn(0, "EasyAccess: netIds has incorrect number of items");
	status = false;
  }
  
  if (looksOK)
  {
    for (i=0; i<_netIds.size(); i++)
      looksOK = looksOK && ( (_netIds[i] < netlist.getNumNets()) ||
			      (_netIds[i] == (unsigned)(-1)) ||
                  ((_netIds[i]>=snetOffset) && (_netIds[i]-snetOffset < 
                                             netlist.getNumSNets())) );
  }
 
  if(!looksOK)
  {
        abkwarn(0, "EasyAccess fails netIds check");
	status = false;
  }
 
  looksOK = ((_whitespace.size() > 0) && (_whitespace.size() <= numLayers));
  
  if(!looksOK)
  {
        abkwarn(0, "EasyAccess fails whitespace check");
	status = false;
  }
 
  if(!status)
  {
     cerr<<"Easy access output was saved to easyAccess.out"<<endl;
     ofstream easyError("easyAccess.out");
     abkfatal(easyError, "Can\'t open easyAccess.out");
     easyError << (*this);
  }

  looksOK = ( looksOK
		&& (_coreCellArea > 0)
		&& (_totalCellWidth > 0));

  if(!looksOK)
     	abkwarn(0, "CellAreas and totalCellWidth are 0");
 
  return(looksOK);
}

void dbEasyAccess::populateSimpleTiming(const dbNetlist& netlist,
                                        const char* rcFileName)
{
  cout << " Opening file " << rcFileName << " ... " << flush << endl;  
  ifstream rcFile(rcFileName);
  if (!rcFile) { cout << "failed " << endl; return; }
  char buf1[1024];
  bool foundCint_c=false,foundCint_k=false,foundRint_c=false,foundRint_k=false;
  int  lineNo=0;
  while(rcFile && (!foundCint_c || !foundCint_k || !foundRint_c ||!foundRint_k))
  {
     rcFile >> eathash(lineNo);
     rcFile >> buf1;
     if (strcasecmp(buf1,"Rint_c")==0)
     {
        rcFile >> _simpleTiming.rInt_c;
        abkfatal(_simpleTiming.rInt_c!=DBL_MAX,"Failed to read Rint_c");
        foundRint_c=true;
     }
     else
     if (strcasecmp(buf1,"Rint_k")==0)
     {
        rcFile >> _simpleTiming.rInt_k;
        abkfatal(_simpleTiming.rInt_k!=DBL_MAX, "Failed to read Rint_k");
        foundRint_k=true;
     }
     else
     if (strcasecmp(buf1,"Cint_c")==0)
     {
        rcFile >> _simpleTiming.cInt_c;
        abkfatal(_simpleTiming.cInt_c!=DBL_MAX, "Failed to read Cint_c");
        foundCint_c=true;
     }
     else
     if (strcasecmp(buf1,"Cint_k")==0)
     {
        rcFile >> _simpleTiming.cInt_k;
        abkfatal(_simpleTiming.cInt_k!=DBL_MAX, "Failed to read Cint_k");
        foundCint_k=true;
     }
     rcFile >> skiptoeol;
  }
  abkfatal(rcFile," Did not find [some of] the four averaged delay coeffs ");
  cout <<" Found 4 averaged coeffs for per-unit interconnect cap and res"
       << endl;
  

  unsigned numMasters=0, numPins=0;
  char masterCellName[1024]="", pinName[1024]="";
  dbSimpleTiming::PinRecord tpr;
  while(rcFile)
  {
     rcFile >> eathash(lineNo);
     rcFile >> buf1;
     bool haveRorP=false;
     if (strcasecmp(buf1,"C")==0) 
     {
       numMasters++;
       rcFile >> masterCellName; 
     }
     else
     if (strcasecmp(buf1,"R")==0) 
     {
        rcFile >> tpr.drivRes;
        tpr.sinkCap=DBL_MAX;
        strcpy(pinName,"DRIVER-PIN__");
        haveRorP=true;
     }
     else
     if (strcasecmp(buf1,"P")==0) 
     {
        rcFile >> pinName >> tpr.sinkCap;
        tpr.drivRes=DBL_MAX;
        haveRorP=true;
     }

     if (haveRorP)
     {
        abkfatal(strcasecmp(masterCellName,"")," Missing master cell name ");
        char *combinedName=new char[strlen(masterCellName)+strlen(pinName)+3];
        sprintf(combinedName,"%s\t%s",masterCellName,pinName);
//      cout << " Combined name : " << combinedName << endl;
        _simpleTiming.pinInfo[combinedName]=tpr;
        numPins++;
     }
     rcFile >> skiptoeol;
  }
  cout << " Read delay info for " << numPins << " pins in " 
                                  << numMasters << " master cells " << endl;
  cout << "    (the netlist has " << netlist.getNumMasterPins() << " pins in " 
       << netlist.getNumMasterCells()<<" master cells) " << endl;

  if (netlist.getNumMasterCells()==numMasters)
     { abkwarn(netlist.getNumMasterPins() ==numPins,   " Missing pin info "); }
  else { abkwarn(0,                               " Missing master cell info");}

  _simpleTiming.populated=true;
}

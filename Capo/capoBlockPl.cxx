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








// Created: Nov 30, 1999 by Andrew Caldwell

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <algorithm>
#include "ABKCommon/abkcommon.h"
#include "ABKCommon/sgi_hash_map.h"
#include "PlaceEvals/placeEvals.h"
#include "Stats/trivStats.h"
#include "Placement/xyDraw.h"
#include "RBPlace/RBPlacement.h"
#include "capoPlacer.h"
#include "capoBlock.h"

CapoPlacer::CapoPlacer(RBPlacement& rbplace, const char* hclFileName, 
               const char* blockPLFileName, const CapoParameters& params)
	: _params(params), _rbplace(rbplace), 
          _hgraphOfNetlist(rbplace.getHGraph()),
	  _coreBBox(/*BBoxFromRBPlace*/), 
	  _placement(static_cast<PlacementWOrient>(rbplace)), 
	  _kPlacement(static_cast<PlacementWOrient>(rbplace)), 
          _placed(rbplace.getFixed()),
          _pinLocCalc(rbplace.getHGraph(), _placement),
	  _layerNum(0), 
	  _cellToBlockMap(_hgraphOfNetlist.getNumNodes(), 
			  static_cast<const CapoBlock*>(NULL)),
	  _nodeSeen(_hgraphOfNetlist.getNumNodes()), 
	  _edgeSeen(_hgraphOfNetlist.getNumEdges()),
	  _numOrigNets(100,0), _numEssentialNets(100,0),
	  _origNetPins(100,0), _essentialNetPins(100,0),
	  _numProblemsOfSize(100,0),
	  _netHasBeenCut(_hgraphOfNetlist.getNumEdges(), false),
	  _numNotSolved(0), _numSmallPartProbs(0),
	  constraints(_rbplace.constraints),
	  regionUtilization(_rbplace.regionUtilization),
	  groupRegionConstr(_rbplace.groupRegionConstr),
	  cellToGrpMapping(_rbplace.cellToGrpMapping)   //, _altCellNames(0)
{
    Timer  capoTimer;

    if(_params.verb.forActions > 0)
	cerr<<endl<<"Launching UCLA's CapoPlacer "
	          <<" from a Floorplanning"<<endl;

    setupAndCheck();

    readBlocksFile(blockPLFileName, hclFileName);
    _layerNum = static_cast<unsigned>(ceil(sqrt(_curLayout->size())));
    cout<<"Starting at layer: "<<_layerNum<<endl;


    if(_curLayout == &_layout[0])
    {
	_layout[1] = vector<CapoBlock*>(_layout[0].size());
	std::copy(_layout[0].begin(), _layout[0].end(), _layout[1].begin());
    }
    else if(_curLayout == &_layout[1])
    {
	_layout[0] = vector<CapoBlock*>(_layout[1].size());
	std::copy(_layout[1].begin(), _layout[1].end(), _layout[0].begin());
    }
    else
	    abkfatal(0, "_curLayout does not equal either layout");

    unsigned maxBlocks = _params.stopAtBlocks ? _params.stopAtBlocks : UINT_MAX;

    bool haveUnplacedBlocks = true;
    bool doVCuts = true;
    while(haveUnplacedBlocks && _curLayout->size() < maxBlocks )
    {
	AllowedPartDir partDir;
	if(_layerNum < 3) 
	     partDir = HAndV;
	else if(_layerNum == 3 || doVCuts)
	{
	     partDir = VOnly;
	     doVCuts = false;	//next time will be horizontal
	}
	else 
	{
	     partDir = HOnly;
	     doVCuts = true;
	}

	//we have no idea at this point which 'layout' will be choosen..
	//and which will be cleared..so they are made to be both the same.


	haveUnplacedBlocks  = doOneLayer(partDir);
    }

    if(_params.replaceSmallBlocks == AtTheEnd)
    {
	Timer replaceTimer;
	cout<<" Replacing Small Blocks"<<endl;
   	replaceSmallBlocks();
	replaceTimer.stop();
	cout<<" Replacing Small Blocks took: "<<
		replaceTimer.getUserTime()<<endl;
    }


    capoTimer.stop();

    if(_params.verb.forMajStats > 0)
        cout << "   CapoPlacer took : " << capoTimer << endl;

    rbplace.resetPlacementWOri(_placement);
}

struct eqstr
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) == 0;
  }
};

void CapoPlacer::readBlocksFile(const char* blockPlName, const char* hclName)
{
    ifstream blocksFile(blockPlName);
    abkfatal2(blocksFile, "Could not open block file ",blockPlName);

    CapoBlock* topLvlBlock = (*_curLayout)[0];
    _curLayout->clear();

    hash_map<const char*, unsigned, hash<const char*>, eqstr> nameToBlockIdMap;

    blocksFile>>needword("BoundingBox")>>skiptoeol;
    char  buffer[1024];
    blocksFile>>buffer;
   
    while(strcasecmp(buffer, "Name")){ blocksFile>>buffer;}

    while(!blocksFile.eof())
    {
	abkfatal(!strcasecmp(buffer, "Name"), 
			"expected 'Name' at the start of the line");

	blocksFile>>buffer;
	Point origin;
  	blocksFile>>needword("location")>>needword("(")
	          >>origin.x>>needword(",")>>origin.y>>needword(")");
	BBox blockBBox(origin);

	blocksFile>>needword("ori")>>needword("N");
	blocksFile>>needword("width")>>origin.x
		  >>needword("height")>>origin.y>>skiptoeol;

	blockBBox += (Point(blockBBox.xMin+origin.x, blockBBox.yMin+origin.y));

	vector<unsigned>   emptyCellIds;
        vector<CapoBlock*> emptyNeighbors;

	_curLayout->push_back( 
		new CapoBlock(emptyCellIds, _hgraphOfNetlist, 
			      topLvlBlock->_coreRows, blockBBox, 
			      emptyNeighbors, emptyNeighbors,
			      emptyNeighbors, emptyNeighbors, this,
			      buffer));

	nameToBlockIdMap[_curLayout->back()->getName()]=_curLayout->size()-1;
	blocksFile>>buffer;
    }
    blocksFile.close();

    ifstream hclFile(hclName);
    abkfatal2(hclFile, "Could not open hcl file ",hclName);

    hclFile>>needword("<hcl>")>>skiptoeol;
    hclFile>>buffer;
    while(strcmp(buffer, "<cluster")){ hclFile>>buffer;}

    char blockName[1024];
    char nodeName[1024];
    vector<vector<unsigned> > cellIds(_curLayout->size());

    while(strcmp(buffer, "</body>"))
    {
	hclFile>>buffer;
	strcpy(nodeName, strchr(buffer,'\"')+1);
					//first char after the \"
	*strchr(nodeName, '\"') = static_cast<char>(NULL);	
					//null terminate the node name

	hclFile>>buffer;
	strcpy(blockName, strchr(buffer,'\"')+1);//first char after the \"
	*strchr(blockName, '\"') = static_cast<char>(NULL); 	 
					//null terminate block name

	if(!strcasecmp(nodeName, blockName)) 
	{ //the root
	    hclFile>>skiptoeol;
    	    hclFile>>needcaseword("<cluster");
	    continue;
	}

	unsigned blockId = nameToBlockIdMap[blockName];
	unsigned nId = _hgraphOfNetlist.getNodeByName(nodeName).getIndex();
	cellIds[blockId].push_back(nId);

	hclFile>>skiptoeol;
	hclFile>>buffer;
    }
    hclFile.close();

    double totalCap = 0;
    for(unsigned b = 0; b < _curLayout->size(); b++)
    {
	(*_curLayout)[b]->resetCellIds(cellIds[b]);

	cout<<*(*_curLayout)[b]<<endl;
	totalCap += (*_curLayout)[b]->getCapacity();
    }

    cout<<"Total Block Cap: "<<totalCap<<endl;
    cout<<"Orig Block  Cap: "<<topLvlBlock->getCapacity()<<endl;

    delete topLvlBlock;
}


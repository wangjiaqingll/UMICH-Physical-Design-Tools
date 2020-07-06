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













// Created: Sep 11, 1997 by Igor Markov & Andy Caldwell

//CHANGES
//971113 aec added class CapoPlacer::Parameters
//971118 aec added SA multi-start and multi-level to Parameters
//980313 ilm added CapoPlacer::Parameters ctor from argc, argv
//           reworked CapoPlacer::Parameters and its output
//           now the original argc, argv are saved in the class
//980325 ilm added n,m and partFuzziness to CapoPlacer::Parameters
//980401 ilm added savePartProb parameter
//980401 ilm added capoPlacer::splitBlock()
//980402 aec added capoPlacer::runSAPlace()
//980609 ilm changed ClusterTree to ClusteringFromDB
//980619 aec updated to ClusterTreeFromDB
//990317 ilm block is not passed as const to constructNewBlocks 
//	     cause its neighbors are unlinked

#ifndef __CAPOPLACER_H__
#define __CAPOPLACER_H__

#include "capoBlock.h"
#include "MLPart/mlPart.h"
#include "HGraph/subHGraph.h"
#include "HGraphWDims/hgWDims.h"
#include "RBPlace/pinLocCalc.h"
#include "SmallPlacers/baseSmallPl.h"
#include "baseBlkSplitter.h"
#include "capoParams.h"
#include "Constraints/constraints.h"

class PartitioningProblemForCapo;
class CapoSplitterParams;
class CongestionMaps;

class CapoPlacer
{
    friend class LookAheadSplitter;
    friend class SplitRowOfBlocks;

  private:

    CapoParameters	   _params;

    RBPlacement&	   _rbplace;
    const HGraphWDimensions& _hgraphOfNetlist;

    BBox		   _coreBBox;
    PlacementWOrient	   _placement;  //Capo does not populate the
					//RBPlacement incrementally..but
					//rather, copies this placement into
					//the RBPlace all at once at the end	
    Placement 		   _kPlacement;

    //added by sadya to include CongestionMaps
    CongestionMaps*        _congestionMap;

    bit_vector		   _placed;     //has the node been assigned a location

    PinLocCalculator       _pinLocCalc;


    vector<CapoBlock*>	   _layout[2]; 
    vector<CapoBlock*>*    _curLayout;
    vector<CapoBlock*>     _placedBlocks;
    vector<CapoBlock*>     _saveBlocks;

    unsigned		   _layerNum;


    vector<const CapoBlock*> _cellToBlockMap;
		//contains a pointer to the block containing each cell.
		//cells not in a block have the pointer == NULL

	//useful bit-vectors (used when traversing nets/edges).
	//users must 'clear' the vectors before use
    bit_vector		_nodeSeen;
    bit_vector		_edgeSeen;


	//Data tracking structures

    vector<double>      _estBBoxWLPerLayer;    //est BBox WL at each level
    vector<unsigned>    _totCutPerLayer;       //total cut at each level

    vector<unsigned>    _nodesInEachBlock;     //used for calculating rent #s
    vector<unsigned>    _terminalsToEachBlock;

    vector<unsigned>	_numOrigNets;
    vector<unsigned>	_numEssentialNets;
    vector<unsigned>	_origNetPins;
    vector<unsigned>	_essentialNetPins;
    vector<unsigned>	_numProblemsOfSize;


       //'cut density' data storage
	  //'Stats' are #cut/length of cut
    vector<vector<pair<unsigned, double> > >   _vCutStats;
    vector<vector<pair<unsigned, double> > >   _hCutStats;
	  //'Ratios' are #newly cut/num cut
    vector<vector<pair<unsigned, unsigned> > > _vCutRatios;
    vector<vector<pair<unsigned, unsigned> > > _hCutRatios;

    bit_vector		_netHasBeenCut;

    unsigned		_numNotSolved; //number of smallPart problems
					//not solved exactly
    unsigned            _numSmallPartProbs;

    double		_totalLayoutArea;
    double		_totalOverfill;
    double		_maxOverfill;

//  const vector<char*>* _altCellNames;  // Capo does not own this pointer

  public:
    class capoTimer
    {
      public:
      static double FMTime;
      static double MLTime;
      static double BBTime;
      static double SmTime;
      static double SetupTime;
    } capoTimes;


    CapoPlacer(RBPlacement& rbplace, const CapoParameters& params,
	       CongestionMaps* congestionMap = NULL);
//             const vector<char*>* altCellNames=0);
    CapoPlacer(RBPlacement& rbplace, const char* hclFileName,
	       const char* blockPLFileName, const CapoParameters& params);

    ~CapoPlacer()
    {
       unsigned k;
       for(k=0; k!=_layout[0].size(); k++)
       {
          if (_layout[0][k]) delete _layout[0][k];
	  _layout[0][k] = NULL;
       }
       for(k=0; k!=_layout[1].size(); k++)
       {
          if (_layout[1][k]) delete _layout[1][k];
	  _layout[1][k] = NULL;
       }
    }

    double estimateWL();

    const vector<double>&   getBBoxPerLayer() const 
		{ return  _estBBoxWLPerLayer; }
    const vector<unsigned>& getCutPerLayer()  const
		{ return  _totCutPerLayer;    }

    const vector<unsigned>& getNodesInEachBlock() const
		{ return _nodesInEachBlock; }
    const vector<unsigned>& getTerminalsToEachBlock() const
		{ return _terminalsToEachBlock; }
    const vector<const CapoBlock*>& getCellToBlockMap() const
		{ return _cellToBlockMap;}

    Point getPinLocation(unsigned cellId, unsigned netId) const;

    const HGraphWDimensions& getNetlistHGraph() const
		{ return _hgraphOfNetlist;}

    const CapoParameters& getParams() const { return _params;}

    const PlacementWOrient& getPlacement() const { return _placement;}

//  const vector<char*>* getAltCellNames() const { return _altCellNames; }

    const RBPlacement& getRBP() const {return _rbplace; }

   //by sadya in SYNP
    const Constraints & constraints;//referenced from constraints in RBlacement
    vector< pair<BBox, double> >& regionUtilization; //by sadya for region 
                                 //utilization constraints. added while at SYNP
    vector< pair<BBox, vector<unsigned> > >& groupRegionConstr;
    vector<unsigned>& cellToGrpMapping;//do we really need this?? TO DO

  private:

    void setupAndCheck(); 
		  //sets up the _coreCells
		  //checks that Pads are not inside, and core cells are
		  //not outside, the coreBBox.
		  //Creates the first(top-level) CapoBlock, and places it
		  //in _layout[0]. sets _curLayout to _layout[0].

    void readBlocksFile(const char* blkFileName, const char* hclFileName);

    enum AllowedPartDir { HOnly, VOnly, HAndV};

	//one layer of block refinment. Returns true if any 
	//block remains unplaced
    bool doOneLayer(AllowedPartDir partDir);
    void doVOnlyLayer(vector<CapoBlock*>& curLayout,
                              vector<CapoBlock*>& newLayout,
                              unsigned layerNum);
    void doHOnlyLayer(vector<CapoBlock*>& curLayout,
                              vector<CapoBlock*>& newLayout,
                              unsigned layerNum);
    void doHAndVLayer(vector<CapoBlock*>& curLayout,
                              vector<CapoBlock*>& newLayout,
                              unsigned layerNum);

	//refineBlock returns true iff block was partitioned,
	//and its children appended to newLayout
    bool refineBlock(CapoBlock& block, vector<CapoBlock*>& newLayout, 
			unsigned layerNum, AllowedPartDir partDir,
			unsigned minNumCellsToPart, 
		        bool useCongestionInfo=false);

    void doOverlapping(vector<CapoBlock*>& layout);

    void plotBlocks(unsigned layerNum, vector<CapoBlock*>& layout);
         //saves a gnuplot script capoBlocks-layerX.gp

    void saveFloorplanProblem(const char* baseFileName);
	//saves all capoBlocks in the layout as a floorplanning instance.
	//also saves a clustering file, which acts as a mapping between
	//the netlist cells, and the blocks.

    Point getWeberLocation(const CapoBlock& block);
    Point getWeberLocation(unsigned nodeIdx);
    BBox  getWeberRegion  (const CapoBlock& block);
    BBox  getWeberRegion  (unsigned nodeIdx);
    unsigned nodesToMove(const CapoBlock& blk);

    void preSeedPartitioning(const CapoBlock& block,
		PartitioningProblemForCapo& probToSeed);
	//NOTE: preSeedPart has a side-effect.  It changes the partitionings
	//in probToSeed

	//sets _cellToBlockMap and placement
    void updateInfoAboutBlock(const CapoBlock* block);

    //only update placement info
    void updatePlInfoAboutBlock(const CapoBlock* block);

    //update only mapping info, not placement. used for capoNE
    void updateMapInfoAboutBlock(const CapoBlock* block);

    void replaceSmallBlocks();

    void repartitionBlocks(CapoBlock* block0, CapoBlock* block1);

    void repartitionBlocks(CapoBlock* block0, CapoBlock* block1, 
			   double relCongestionBlk0, double relCongestionBlk1);

   public:

    void printCutDensityStats();
    void printSmallProblemStats();
    void printNetlistStats();

    void getNetCutInfo(vector<unsigned>& externalCutNets,
                       vector<unsigned>& containedNets) const;
    unsigned getTotalNetCut() const;

    void getBlockMembership(vector<int>& nodeBlock) const;
    //use the given blocks to generate membership
    void getBlockMembership(vector<int>& nodeBlock, 
			    vector<CapoBlock *> blocks) const;
    void printBlockMembership(ostream& os) const;
    void printBlockMembership(const char* fileName) const;

    void getHierCellNames(vector<const char*>& cellNames) const;
    void printHierCellNames(ostream& os) const;
    void printHierCellNames(const char* fileName) const;

    void saveSmallProblem(const PartitioningProblemForCapo& problem,
                                  const CapoBlock& block);


    friend ostream& operator<< (ostream& os, vector<CapoBlock*>& layout);

    //added by sadya to save the blocks info as a floorplannning instance
    void saveBlocksAsFloorplan(char* fileName, vector<CapoBlock *> blocks);
    //save current snapshot of blocks in a copy
    void saveBlocksCopy();
    void solveQuadraticMinSoln();

    //get number of cells with region constraints in a block
    unsigned getNumRegionedCellsInBlock(CapoBlock * block);

    void boost(vector<BBox>& bboxes);
    int probeBlock(CapoBlock& block);
    void boostNets(CapoBlock & block, vector<BBox>&);
    void deboostNets(CapoBlock&);
    double squareCost();
    void printNetStat();
};

#endif


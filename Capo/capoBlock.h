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

















//created by Andy Caldwell on 09/14/97  caldwell@cs.ucla.edu

//CHANGES:
//mm  12/10/97 added const netsBegin(), netsEnd() mem fns
//ilm 980304   added const getNets(), netsEnd() method
//ilm 980316   added unsigned getNumSubrootChildren()
//ilm 980324   split clSubTreeH.h from this file (sim. for the .cxx)

//aec removed subRoot.  Block is now just a vector of leaf clusters.
//aec 981118   re-worked CapoBlocks.  All Cluster classes are removed.
//aec 990203   moved to RBPlace Rows
//aec 990223   added getWhitespace and getRelativeWhitespace
//ilm 990319   blocks now maintain a reflexive 4-directed neighborhood relation

#ifndef __CAPOBLOCK_H__
#define __CAPOBLOCK_H__

#include <vector>
using std::vector;
#include <algorithm>
using std::remove;
#include "ABKCommon/abkcommon.h"
#include "HGraphWDims/hgWDims.h"
#include "RBPlace/RBRows.h"

//see Capo/DOCS/CapoBlock.doc for a description of class CapoBlock

typedef pair<unsigned, unsigned> CROffset;
class junkForCompilerCROffset : public CROffset {};

typedef std::vector<const RBPCoreRow*>::const_iterator itCBCoreRow;
typedef std::vector<CROffset>::const_iterator itCBRowOffset;


class CapoBlock
{ 
    friend class CapoPlacer;
    friend class BaseBlockSplitter;
    friend class CapoBlockMatchRec;
    friend class LookAheadSplitter;
 
    vector<const RBPCoreRow*> 	_coreRows; //core rows (at least partly) 
					   //contained in this block
					   //the first coreRow is the lowest
					   //(smallest yCoord)

    vector<CROffset>		_startOffsets;
					   //for each CoreRow, the # of
					   //subRows which are left of the
					   //block (same as index of 1st
					   //subRow intersecting the block),
					   //and # of sites (in left-most
					   //intersecting subRow) left of the
					   //block.
					   //A start offset <i,j> means
					   //that the jth cell of the ith
					   //sub row of that CoreRow is the
					   //first site actually in this block
    vector<CROffset>		_endOffsets;//for each CoreRow, the
					   //index of the last subRow
					   //intersecting the block, and
					   //of the last site in the block.
				 	   //An end offset of <i,j> means
				  	   //that the jth cell of the ith
					   //subRow is the last site actually
					   //in the block.
				//IMPORTANT: note that unlike STL begin/end
				//iterators where 'end' iterators are outside
				//the collection, the subrows/sites referenced
				//by 'endOffsets' ARE actually in the block.
				//iteration is difficult otherwise...
				//(this means that start/endOffsets are
				//completely symetric.	

    unsigned			_numSites;

    double                      _capacity; 
					//sum of the sites on the rows, within
					//the interval this block covers

    BBox			_bbox;  //defines the layout boundries of
					//this block.  The top and bottom of
					//the bbox corespond to the top of the
					//top-most row, and the bottom of the
					//bottom-most row, respecively

    vector<unsigned>		_cellIds;//the Ids of the cells (movable, 
					//and otherwise) contained in this 
					//block.  Capo has a single netlist-
					//level HGraph, which these Ids 
					//corespond to.

    double			_totalCellArea; //total area of all cells
					//in this block.
    double                      _avgRowSpacing; // vertical spacing between rows
    double                      _avgCellWidth;
    double                      _avgCellHeight;

    double			_largestCellArea; //the area of the largest
					//cell in this block.

    char*			_name;  //the name of this block.  This is
					//used when saving problems

    unsigned			_index; //CapoBlocks are uniquely identified

    static unsigned		_nextBlockNum; //id of the next block to create

    const HGraphWDimensions&	_hgraph;// the global HGraph

    const CapoPlacer* 		_capo; //this is the capo placer object 
                                       //associated with this block, this
                                       //block does not own the _capo object.

    void  resetCellIds(const vector<unsigned>& newCellIds);
                                  // rips existing cells out of the block
                                  // and repopulates the block with newCellsIds

    bool			_canSplitBlock;
    bool			_isNewBlock;

  protected:
    vector<CapoBlock*>         _neighborsAbove;
    vector<CapoBlock*>         _neighborsBelow;
    vector<CapoBlock*>         _leftNeighbors;
    vector<CapoBlock*>         _rightNeighbors;

    CapoBlock*                 _sibling;

    bool isSibling(CapoBlock * candidate) const { return _sibling==candidate; }

    void _addNeighborAbove(CapoBlock* neighbor) 
        { _neighborsAbove.push_back(neighbor);}
    void _addNeighborBelow(CapoBlock* neighbor) 
        { _neighborsBelow.push_back(neighbor);}
    void _addLeftNeighbor (CapoBlock* neighbor) 
        { _leftNeighbors.push_back(neighbor); }
    void _addRightNeighbor(CapoBlock* neighbor) 
        { _rightNeighbors.push_back(neighbor);}

    void __removeFrom(vector<CapoBlock*>& v, CapoBlock* ptr)
    { 
      unsigned sizeBefore=v.size();
      if (sizeBefore==0) return;

      std::vector<CapoBlock*>::iterator newEnd=remove(v.begin(),v.end(),ptr);
      v.erase(newEnd, v.end()); 
      if (sizeBefore<=v.size())
      { 
          cerr << endl << "  Block [" << getIndex() << "] does not know about "
               " its neighboring block [" << ptr->getIndex() << "] ";
          abkfatal(0,"");
      }
    }

    void _removeNeighborAbove(CapoBlock* neighbor) 
         { __removeFrom(_neighborsAbove, neighbor); }
    void _removeNeighborBelow(CapoBlock* neighbor) 
         { __removeFrom(_neighborsBelow, neighbor); }
    void _removeLeftNeighbor(CapoBlock* neighbor) 
         { __removeFrom(_leftNeighbors, neighbor);  }
    void _removeRightNeighbor(CapoBlock* neighbor) 
         { __removeFrom(_rightNeighbors, neighbor);  }

    void setIndexToNextIndex() { _index = _nextBlockNum++;}

	//populates coreRows and start/endOffsets with the intersection
	//of candidateRows and boundary
    void compStartAndEndOffsets(
                const BBox&                 boundary,
                const vector<const RBPCoreRow*>&  candidateRows,
                vector<const RBPCoreRow*>&        coreRows,
                vector<CROffset>&           startOffsets,
                vector<CROffset>&           endOffsets);
  
  public:

    CapoBlock(	const vector<unsigned>&  cellIds,
	       	const HGraphWDimensions& hgraph,
		itCBCoreRow rowsBegin,
		itCBCoreRow rowsEnd,
		const vector<CROffset>& startOffsets,
		const vector<CROffset>& endOffsets,
                const vector<CapoBlock*>& neighbAbove,
                const vector<CapoBlock*>& neighbBelow,
                const vector<CapoBlock*>& leftNeighb,
                const vector<CapoBlock*>& rightNeighb,
                const CapoPlacer * capo = NULL,
		char* name = NULL
             );

	//this ctor calculates the included rows & their start/end offsets.
	//it includs all sites within the intersection of candidateRows 
	//and boundary
    CapoBlock(	const vector<unsigned>&  cellIds,
	       	const HGraphWDimensions& hgraph,
		const vector<const RBPCoreRow*>& candidateRows,
		const BBox& boundary,
                const vector<CapoBlock*>& neighbAbove,
                const vector<CapoBlock*>& neighbBelow,
                const vector<CapoBlock*>& leftNeighb,
                const vector<CapoBlock*>& rightNeighb,
                const CapoPlacer * capo = NULL,
		char* name = NULL
             );

    CapoBlock(vector<CapoBlock*>& src);

    //by sadya
    CapoBlock(CapoBlock* srcBlock);

    ~CapoBlock() 
    { if (_name != NULL) delete _name; _name = NULL;}

    unsigned	   getNumCells()   const {return _cellIds.size(); }

    std::vector<unsigned>::const_iterator cellIdsBegin() const 
		{return _cellIds.begin();}
    std::vector<unsigned>::const_iterator cellIdsEnd()   const 
		{return _cellIds.end();  }
    const vector<unsigned>& getCellIds() const { return _cellIds; }


    const vector<const RBPCoreRow*>&          getRows()   const 
					 { return _coreRows; }
    unsigned	   getNumRows()    const { return _coreRows.size(); }
    itCBCoreRow    rowsBegin()     const { return _coreRows.begin();}
    itCBCoreRow    rowsEnd()       const { return _coreRows.end();}
    const RBPCoreRow& getRow(unsigned r) const { return *_coreRows[r];}

    pair<unsigned,unsigned> getStartOffset(unsigned r) const
		{ return _startOffsets[r];}
    pair<unsigned,unsigned> getEndOffset(unsigned r) const
		{ return _endOffsets[r];}

    double   	   getCapacity()   const { return _capacity; }
    double   	   getNumSites()   const { return _numSites; }
    double	   getWhitespace() const { return _capacity - _totalCellArea;}
    double	   getRelativeWhitespace() const 
			{ return getWhitespace() / _capacity;}
    double	   getOverfill()   const { return max(0.0, -getWhitespace());}
    double	   getRelativeOverfill() const
			{ return getOverfill() / _capacity;}

    double 	   getContainedAreaInCoreRow(unsigned r) const;
    unsigned 	   getContainedSitesInCoreRow(unsigned r) const;
    double         getContainedLeftEdge (unsigned r) const;
    double         getContainedRightEdge(unsigned r) const;
    double 	   getContainedAreaInBBox(const BBox& rect) const;//by sadya in SYNP

    const BBox&	   getBBox()	   const { return _bbox;}
    double	   getHeight()     const { return _bbox.yMax - _bbox.yMin;}
    double	   getWidth()      const { return _bbox.xMax - _bbox.xMin;}
    Point 	   getCenter()     const { return _bbox.getGeomCenter();}

    double   getTotalCellArea()    const { return _totalCellArea;  }
    double   getAvgCellWidth()     const { return _avgCellWidth;   }
    double   getAvgCellHeight()    const { return _avgCellHeight;  }
    double   getAvgRowSpacing()    const { return _avgRowSpacing;  }
    double   getFactor()           const 
            {
	     if (_avgCellWidth + _avgCellHeight <0.1) return 1.0;
	     return _avgRowSpacing/(_avgCellWidth + (1.0/3) * _avgCellHeight);
	    }
    double   getLargestCellArea()  const { return _largestCellArea;}
    double   getWidestCellWidth()  const;

    double   getMedianCellArea()   const;

    unsigned 	   getIndex()  	   const { return _index; }

    unsigned getNumAdjacentCells() const;
    unsigned getNumAdjacentCellsWithDuplicates() const;

	//returns the x-line s.t. as close as possible to p0Cap 
	//site area is left of the x-line.
    double	   findXSplit(double c0, double c1, double roundPct,
				vector<double>& partCaps) const;
    double findXSplitUsingSkyline(double c0, double c1,
                                        vector<double>& partCaps) const;

    //in SYNP
    //finds split direction and point. tries to align the cut line to 
    //boundary of a region. if no regions in the given block or one region
    //occupies entire block, then returns false. proceed with normal flow
    //returns desired splitPt
    bool findSplitPtDir(bool desiredDir, 
		   vector< pair<BBox, vector<unsigned> > >& groupRegionConstr,
		   bool &splitDir, double& splitPt);
    
    void   computePartAreas(double xSplit, vector<double>& partCaps) const;

    void     linkNeighbors();
    void     unLinkNeighbors();

    const char* getName() const { return _name;}

    bool     canSplitBlock() const { return _canSplitBlock;}
    bool     isNewBlock()    const { return _isNewBlock;}

    friend ostream& operator<<(ostream& os, const CapoBlock& ct);

    void     decrementIdx() { --_nextBlockNum; }
      
};

ostream& operator<<(ostream& os, const CapoBlock& ct);

class CompareCellIdsByArea
{
  const HGraphFixed& _hgraph;

  public:
    CompareCellIdsByArea(const HGraphFixed& hgraph)
	: _hgraph(hgraph)
	{}

    bool operator()(unsigned cellId1, unsigned cellId2)
    {	if(_hgraph.getNodeByIdx(cellId1).getWeight() < 
	   _hgraph.getNodeByIdx(cellId2).getWeight())
		return true;
	return false;
    }
};


struct CompareSrcBlocksByX
{
    CompareSrcBlocksByX(){}
    bool operator()(const CapoBlock* b1, const CapoBlock* b2)
    {   return (b1->getBBox().xMin < b2->getBBox().xMin);  }
};


#endif

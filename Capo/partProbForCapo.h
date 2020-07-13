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











#ifndef __CAPO_PART_PROB_H__
#define __CAPO_PART_PROB_H__

#include <iostream>
#include <vector>
#include "Partitioning/partProb.h"

using std::bit_vector;

class Partitioning;
class CapoBlock;
class SubHGraph;


class PartitioningProblemForCapo : public PartitioningProblem
{
    friend class CapoPlacer;

  protected:

    vector<const CapoBlock*> 	_blocks;
    const HGraphFixed&		_hgraphOfNetlist;
    const vector<const CapoBlock*>&	_cellToBlockMap;
    const CapoPlacer&	        _capoPl;

    unsigned			_nDim;
    unsigned			_mDim;
    bool			_shouldBeRepartitioned;

    vector<int>			_yRows;

    SubHGraph*			_subHG; //IMPORTANT: this points to the
				//same object as the _hgraph value 
				//inherited from the base. But, it's
				//'subHG' properties are necessary, so this
				//allows for using them.

    static bit_vector		_edgesVisited;


    void	setTolerances();
    void	setBuffer();
    void        setPartDims();
    void        setPartDims(const vector<double>& partCaps);


    void	buildHGraph(bool honourGrpConstr=false);
    void	buildHGraphNE(bool honourGrpConstr=false);

    unsigned			_numOrigEdges;
    unsigned			_numEssentialEdges;
	//these are total degree (sum over all edges)
    unsigned			_origEdgeDegree;
    unsigned			_essentialEdgeDegree;

    unsigned			_hCutWSMethod;
    double			_optimisticAdj;

    Verbosity			_verb;

  public:

	//simpler method used by the 'SplitSmallBlock' methods.
	//does not set tolerances, etc.
    PartitioningProblemForCapo(
		const CapoBlock&	block,
		const CapoPlacer&       capo,
		      bool              isHCut,
		      double            splitPt,
		Verbosity verb = Verbosity("1 1 1"),
		bool honourGrpConstr=false);

	//build a problem to repartition 2 blocks
    PartitioningProblemForCapo(
		const vector<const CapoBlock*>&	blocks,
		const HGraphFixed& 		hgraphOfNetlist, 
		const CapoPlacer&		capo,
		const vector<const CapoBlock*>& cellToBlockMap,	
		Verbosity verb = Verbosity("1 1 1"));

	//build a problem to repartition 2 blocks given congestion info
    PartitioningProblemForCapo(
		const vector<const CapoBlock*>&	blocks,
		const HGraphFixed& 		hgraphOfNetlist, 
		const CapoPlacer&		capo,
		const vector<const CapoBlock*>& cellToBlockMap,
		const vector<double>& partCaps,
		Verbosity verb = Verbosity("1 1 1"));


    virtual ~PartitioningProblemForCapo();

    unsigned getNDim() const { return _nDim;}
    unsigned getMDim() const { return _mDim;}

    SubHGraph& getSubHGraph() const { return *_subHG;}

    unsigned	getNumOrigEdges()       const { return _numOrigEdges;       }
    unsigned	getNumEssentialEdges()  const { return _numEssentialEdges;  }
    unsigned    getOrigEdgePins()       const { return _origEdgeDegree;     }
    unsigned	getEssentialEdgePins()  const { return _essentialEdgeDegree;}

    void        setPartition(unsigned partNum, const BBox& newBBox)
        { (*_partitions)[partNum] = newBBox;}

    void        setCapacity(unsigned partNum, double newCap)
	{ (*_capacities)[partNum][0] = newCap;}

	//resetTolerance is to be used when partitioning a single
	//block, after the initial partitioning
    void        resetTolerance(double s0, double s1);

    bool        shouldBeRepartitioned() const 
		{ return _shouldBeRepartitioned;}

    void        setTolerance(double tolPct);	//`10' == 10%
    double      getTolerance(unsigned wtNum = 0) const;
    void        setPaperMethodTolerance(unsigned splitRow=UINT_MAX);
    void        setVBlockTolerance(double xSplit, double minTol=20);
};

#endif

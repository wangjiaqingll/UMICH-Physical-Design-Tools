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














//created by Andrew Caldwell on 10/17/99 caldwell@cs.ucla.edu

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <algorithm>
#include "splitLarge.h"
#include "partProbForCapo.h"
#include "MLPart/mlPart.h"
#include "FMPart/fmPart.h"
#include "ABKCommon/abktimer.h"

SplitLargeBlockVertically::SplitLargeBlockVertically
	(      CapoBlock&  blockToSplit,
	 const CapoPlacer& capo,
	       CapoSplitterParams  params,
	       PlacementWOrient* placement)
	: BaseBlockSplitter(blockToSplit,capo,params)
{
    if(_params.verb.forActions > 2)
	cout<<"Splitting a large block Vertically"<<endl;
   
    double   capacity  = _block.getCapacity();
    double   targetCap0, targetCap1;
    targetCap0 = capacity/2.0;
    targetCap1 = targetCap0;
    vector<double> partCaps;

    if(params.fillerCellsPartCaps)
      {
	vector<double> targetCellAreas(2,_block.getTotalCellArea()/2.0);
	bool changeCaps = partAreasForFillerCells(1, targetCellAreas);
	if(changeCaps)
	  {
	    targetCap0 = capacity*targetCellAreas[0]/(targetCellAreas[0]+targetCellAreas[1]);
	    targetCap1 = capacity*targetCellAreas[1]/(targetCellAreas[0]+targetCellAreas[1]);
	  }
      }

    _xSplit = _block.findXSplit(targetCap0,targetCap1,0,partCaps);

    Timer tm22;
    PartitioningProblemForCapo problem(_block, capo, false, _xSplit, 
				       Verbosity("1 1 1"), false);
    tm22.stop();
    CapoPlacer::capoTimer::SetupTime+=tm22.getUserTime();

    bool changeDefaultFlow=false;
    if(_params.useRgnConstr)
      {
	vector<double> targetPartCaps(2);
	changeDefaultFlow=vPartCapsFrmRgnConstr(_xSplit, targetPartCaps);
	if(changeDefaultFlow)
	  {
	    double p0Target = targetPartCaps[0]*_block.getTotalCellArea();
	    double p1Target = _block.getTotalCellArea()-p0Target;
	    problem.setCapacity(0, p0Target);
	    problem.setCapacity(1, p1Target);
	  }
	else
	  {
	    double p0Target = (partCaps[0]/_block.getCapacity())*
	      _block.getTotalCellArea();
	    double p1Target = _block.getTotalCellArea()-p0Target;
	    problem.setCapacity(0, p0Target);
	    problem.setCapacity(1, p1Target);
	  }
      }
    else
      {
	double p0Target = (partCaps[0]/_block.getCapacity())*
	  _block.getTotalCellArea();
	double p1Target = _block.getTotalCellArea()-p0Target;
	problem.setCapacity(0, p0Target);
	problem.setCapacity(1, p1Target);
      }
 
    if(_params.useRgnConstr && changeDefaultFlow)
      callPartitioner(problem,placement,true);
    else
      callPartitioner(problem,placement,false);

    createVSplitBlocks(problem, _xSplit);
}

SplitLargeBlockVertically::SplitLargeBlockVertically
	(      CapoBlock&  blockToSplit,
	       const CapoPlacer& capo,
	       double xSplit,
	       CapoSplitterParams  params,
	       PlacementWOrient* placement)
	: BaseBlockSplitter(blockToSplit,capo,params)
{
    if(_params.verb.forActions > 2)
	cout<<"Splitting a large block Vertically"<<endl;
   
    BBox blkBBox = _block.getBBox();
    if(xSplit < blkBBox.xMin || xSplit > blkBBox.xMax)
      {
	abkfatal(0, "xSplit out of block BBox while splitting verticall\n");
      }

    BBox p0BBox = blkBBox;
    p0BBox.xMax = xSplit;
    BBox p1BBox = blkBBox;
    p1BBox.xMin = xSplit;
    double   capacity  = _block.getCapacity();
    double p0Capacity =  _block.getContainedAreaInBBox(p0BBox);
    //double p1Capacity = capacity - p0Capacity;
    double totalCellArea = _block.getTotalCellArea();

    _xSplit = xSplit;

    Timer tm23;
    PartitioningProblemForCapo problem(_block, capo, false, _xSplit, 
				       Verbosity("1 1 1"), true);
    tm23.stop();
    CapoPlacer::capoTimer::SetupTime+=tm23.getUserTime();

    bool changeDefaultFlow=false;
    if(_params.useRgnConstr)
      {
	vector<double> targetPartCaps(2);
	changeDefaultFlow=vPartCapsFrmRgnConstr(_xSplit, targetPartCaps);
	if(changeDefaultFlow)
	  {
	    double p0Target = targetPartCaps[0]*_block.getTotalCellArea();
	    double p1Target = _block.getTotalCellArea()-p0Target;
	    problem.setCapacity(0, p0Target);
	    problem.setCapacity(1, p1Target);
	  }
	else
	  {
	    double capRatio = p0Capacity / capacity;
	    double p0CellArea = capRatio*totalCellArea;
	    problem.setCapacity(0, p0CellArea);
	    problem.setCapacity(1, totalCellArea - p0CellArea);
	  }
      }
    else
      {
	double capRatio = p0Capacity / capacity;
	double p0CellArea = capRatio*totalCellArea;
	problem.setCapacity(0, p0CellArea);
	problem.setCapacity(1, totalCellArea - p0CellArea);
      }

    callPartitioner(problem, placement, true);

    if(_params.verb.forActions > 4)
      cout<<"Requested xSplit "<<xSplit<<" Final xSplit "<<_xSplit<<endl;

    createVSplitBlocks(problem, _xSplit);
}
    
void SplitLargeBlockVertically::callPartitioner
		(PartitioningProblemForCapo& problem, 
		 PlacementWOrient* placement, bool splitPtFixed)
{
    bool foundGoodSplit = false;
    unsigned numTries   = 0;
   
    while(!foundGoodSplit && numTries++ < 2)
    {
	foundGoodSplit = callMLPart(problem, placement, splitPtFixed);
      
	if(foundGoodSplit)
	{
	    if(_params.verb.forActions > 9)
	      {
		cout<<" Success.  Found a good split"<<endl;
	      }
	}
	else if(numTries < 2)
	{
	  if(_params.verb.forActions > 9)
	    cout<<"  Failed. clearing part buffers"<<endl;

	  PartitioningBuffer& buff = problem.getSolnBuffers();
	  unsigned numTerms=problem.getHGraph().getNumTerminals();

	  unsigned sol;
	  for(sol = buff.beginUsedSoln(); sol != buff.endUsedSoln(); sol++)
  	  {
	    Partitioning& part = buff[sol];
	    for(unsigned i = numTerms; i < part.size(); i++)
		part[i].setToAll(2);
	  }

	  //consider changing the problem here to attempt to
	  //get a better solution	
	}
	else if(_params.verb.forActions > 9)
	    cout<<"  Failed too many times. Taking the previous soln."<<endl;
    }
}

bool SplitLargeBlockVertically::callMLPart
		(PartitioningProblemForCapo& problem, 
		 PlacementWOrient* placement, bool splitPtFixed)
{
    double blockWS = _block.getRelativeWhitespace();
    bool repartSmallWS = false;
    if(_params.repartSmallWS && blockWS < 0.02)
      repartSmallWS = true;
    
    if(_params.useWSTolMethod && !_params.uniformWS)
      problem.setVBlockTolerance(_xSplit, _params.constantTolerance);
    else
      problem.setTolerance(_params.constantTolerance);

    const HGraphFixed& hgraph = problem.getHGraph();

    if(_params.verb.forMajStats > 4)
    {
	cout<<" First MLPart"<<endl;
	cout<<"Targets:  "<<problem.getCapacities()[0][0]<<"/"
			  <<problem.getCapacities()[1][0]<<endl;
	cout<<"Max:      "<<problem.getMaxCapacities()[0][0]<<"/"
			  <<problem.getMaxCapacities()[1][0]<<endl;
	cout<<"Min:      "<<problem.getMinCapacities()[0][0]<<"/"
			  <<problem.getMinCapacities()[1][0]<<endl;
    }
    
    MLPart::Parameters mlParams(_capo.getParams().mlParams);

    mlParams.verb.forActions  = _params.verb.forActions/2;
    mlParams.verb.forMajStats = _params.verb.forMajStats/2;
    mlParams.verb.forSysRes   = _params.verb.forSysRes  /2;

    if(hgraph.getNumNodes() < 500)
        mlParams.clParams.sizeOfTop = 100;

    problem.reserveBuffers(2);	//sets are pairs of runs+a VC
    PartitioningBuffer  refinedSolns(hgraph.getNumNodes(), _params.numMLSets);
    vector<double>      partAreas(2); //in the BSF soln
    unsigned		bestSet;
    double 		origCut, finalCut;
    origCut = DBL_MAX;

    vector<const char*> nodeNames;
    if(_params.useNameHierarchy)
    {
      if(_params.verb.forMajStats > 2)
	cout<<"Using name hierarchy to construct clustering"<<endl;

      const HGraphFixed& netlistHG = _capo.getNetlistHGraph();
      std::vector<unsigned>::const_iterator cItr;

      for(cItr=_block.cellIdsBegin(); cItr != _block.cellIdsEnd(); cItr++)
	nodeNames.push_back(netlistHG.getNodeByIdx(*cItr).getName());
    }
    FillableHierarchyParameters fillHParams;

    unsigned setNum;
    for(setNum = 0; setNum < _params.numMLSets; setNum++)
    {
      MLPart*   partitioner1;

      if(_params.useNameHierarchy)
      {
        FillableHierarchy fillH(nodeNames, _params.delimiters, 
				problem.getHGraph(), fillHParams);
        Timer tm10;
        partitioner1 = new MLPart(problem, mlParams, &fillH);
        tm10.stop();
        CapoPlacer::capoTimer::MLTime+=tm10.getUserTime();
      }
      else if(_params.useQuadCluster && hgraph.getNumNodes() > 200)
      {
        Timer tm11;
	partitioner1 = new MLPart(problem, mlParams, placement);
        tm11.stop();
        CapoPlacer::capoTimer::MLTime+=tm11.getUserTime();
      }
      else
      {
        Timer tm12;
	partitioner1 = new MLPart(problem, mlParams);
        tm12.stop();
        CapoPlacer::capoTimer::MLTime+=tm12.getUserTime();
      }

      refinedSolns [setNum] = problem.getBestSoln();
      if(partitioner1->getBestSolnCost() < origCut)
      {
	origCut = partitioner1->getBestSolnCost();
	bestSet = setNum;
	partAreas[0] = partitioner1->getPartitionArea(0);
	partAreas[1] = partitioner1->getPartitionArea(1);
      }

      PartitionIds clearId;
      clearId.setToAll(2);
      Partitioning& buff0 = problem.getSolnBuffers()[0];
      Partitioning& buff1 = problem.getSolnBuffers()[1];
      std::fill(buff0.begin()+hgraph.getNumTerminals(),buff0.end(), clearId);
      std::fill(buff1.begin()+hgraph.getNumTerminals(),buff1.end(), clearId);

      delete partitioner1;
    }

    problem.reserveBuffers(_params.numMLSets);
    for(setNum = 0; setNum < _params.numMLSets; setNum++)
	problem.getSolnBuffers()[setNum] = refinedSolns[setNum];
 
    problem.setBestSolnNum(bestSet); 

    vector<double> siteAreas(2,0.0);

    if(!splitPtFixed)
      if ( blockWS < 0.5 || _params.uniformWS)
	_xSplit  = _block.findXSplit(partAreas[0], partAreas[1], 
						 0, siteAreas);
    
    if(!_params.doRepartitioning && !repartSmallWS && !splitPtFixed) //we're done
    {
        finalCut = origCut;
        if(_params.verb.forMajStats > 4)
	  cout<<" Best Solution had cut "<<finalCut<<endl;
	return true;
    }
    
    double idealXSplit = _xSplit;
    if(!splitPtFixed)
      if(blockWS < 0.5 || _params.uniformWS)
        _xSplit   = _block.findXSplit(partAreas[0], partAreas[1], 
						  10, siteAreas);

    if(_xSplit == idealXSplit && !repartSmallWS && !splitPtFixed)//no change...
    {
          finalCut = origCut;
          if(_params.verb.forMajStats > 4)
	    cout<<" Best Solution had cut "<<finalCut<<endl;
	  return true;
    }

    //cutline was shifted, repartition w/ the new
    //targets and new tolerance.

    if(!splitPtFixed)
      {
	  double p0Target = (siteAreas[0]/_block.getCapacity())*
	    _block.getTotalCellArea();
	  problem.setCapacity(0, p0Target);
	  problem.setCapacity(1, _block.getTotalCellArea() -p0Target);
      }

    
    if(repartSmallWS)
      {
	double medianCellSize =  _block.getMedianCellArea()/2.0;
	const vector<vector<double> > caps = problem.getCapacities();
	double minTol = 100*max(medianCellSize/caps[0][0], 
				medianCellSize/caps[1][0]);
	double tol = max(0.0, 100*blockWS);
	tol = max(minTol, tol);
	problem.setTolerance(tol);
      }
    else if (splitPtFixed)
      problem.setTolerance(5);
    else if (_params.useWSTolMethod && !_params.uniformWS)
      problem.setVBlockTolerance(_xSplit, 1);
    else
      problem.setTolerance(1);

    if(_params.verb.forMajStats > 4)
    {
	cout<<" Repartitioning due to non-ideal xSplit"<<endl;
	cout<<"Targets:  "<<problem.getCapacities()[0][0]<<"/"
			  <<problem.getCapacities()[1][0]<<endl;
	cout<<"Max:      "<<problem.getMaxCapacities()[0][0]<<"/"
			  <<problem.getMaxCapacities()[1][0]<<endl;
	cout<<"Min:      "<<problem.getMinCapacities()[0][0]<<"/"
			  <<problem.getMinCapacities()[1][0]<<endl;
    }

    FMPartitioner::Parameters fmParams;
    fmParams.useEarlyStop = false;
    fmParams.useClip = false;
    fmParams.verb.forActions  = _params.verb.forActions/2;
    fmParams.verb.forMajStats = _params.verb.forMajStats/2;
    fmParams.verb.forSysRes   = _params.verb.forSysRes  /2;

    Timer tm9;
    FMPartitioner partitioner2(problem, fmParams);
    tm9.stop();
    CapoPlacer::capoTimer::FMTime+=tm9.getUserTime();

    finalCut = partitioner2.getBestSolnCost();

    double cutDelta = finalCut - origCut;
    if(cutDelta > origCut/10 && cutDelta >= 5 && !splitPtFixed)
    {
	if(_params.verb.forActions > 9)
	  cout<<" ->FM Failed.  Repartitioning increased "
	      <<"the cut too much"<<endl;
 	return false;
    }	

    if(_params.verb.forMajStats > 4)
	cout<<" Best Solution had cut "<<finalCut<<endl;
  
    return true;
}


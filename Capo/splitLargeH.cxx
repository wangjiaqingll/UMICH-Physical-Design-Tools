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

SplitLargeBlockHorizontally::SplitLargeBlockHorizontally
	(      CapoBlock&  blockToSplit,
	 const CapoPlacer& capo,
	       CapoSplitterParams  params,
	       PlacementWOrient* placement)
	: BaseBlockSplitter(blockToSplit,capo,params)
{

    abkfatal(_block.getNumRows() > 1,	
	"can't split a 1 row block horizontally");

    if(_params.verb.forActions > 2)
	cout<<"Splitting a large block horizontally"<<endl;
   
    double   capacity  = _block.getCapacity();
    double totalCellArea = _block.getTotalCellArea();
    vector<double>   targetCellAreas(2,totalCellArea/2.0);
    vector<double>   actualCaps(2,0);

    if(params.fillerCellsPartCaps)
      partAreasForFillerCells(0, targetCellAreas);

    _splitRow  = findBestSplitRow(targetCellAreas, actualCaps);
    const vector<const RBPCoreRow*>& rows = _block.getRows();

    if(_params.verb.forActions > 4)
        cout<<"P0/P1 rows: "<<rows.size()-_splitRow<<"/"<<_splitRow<<endl;

    Timer tm20;
    PartitioningProblemForCapo problem(_block, capo, true, 
					rows[_splitRow]->getYCoord());
    tm20.stop();
    CapoPlacer::capoTimer::SetupTime+=tm20.getUserTime();

    bool changeDefaultFlow=false;
    if(_params.useRgnConstr)
      {
	vector<double> partCaps(2);
	changeDefaultFlow=hPartCapsFrmRgnConstr(_splitRow, partCaps);
	if(changeDefaultFlow)
	  {
	    double p0Target = partCaps[0]*_block.getTotalCellArea();
	    double p1Target = _block.getTotalCellArea()-p0Target;
	    problem.setCapacity(0, p0Target);
	    problem.setCapacity(1, p1Target);
	  }
	else
	  {
	    double capRatio = actualCaps[0] / capacity;
	    double p0CellArea = capRatio*_block.getTotalCellArea();
	    problem.setCapacity(0, p0CellArea);
	    problem.setCapacity(1, _block.getTotalCellArea() - p0CellArea);
	  }
      }
    else
      {
	double capRatio = actualCaps[0] / capacity;
	double p0CellArea = capRatio*_block.getTotalCellArea();
	problem.setCapacity(0, p0CellArea);
	problem.setCapacity(1, _block.getTotalCellArea() - p0CellArea);
      }

    if(_params.useRgnConstr && changeDefaultFlow)
      callPartitioner(problem, true, placement);
    else
      callPartitioner(problem, false, placement);
    
    if((_params.useRgnConstr && changeDefaultFlow) || !_params.uniformWS)
      createHSplitBlocks(problem, _splitRow);
    else
      createHSplitBlocks(problem);
}
  
//by sadya in SYNP to take care of region constraints
//splits a block at the given ySplit. also pre-fixes any cells
//in group constraints that are relevant to this block
SplitLargeBlockHorizontally::SplitLargeBlockHorizontally
	(      CapoBlock&  blockToSplit,
	       const CapoPlacer& capo,
	       double ySplit,
	       CapoSplitterParams  params,
	       PlacementWOrient* placement)
	: BaseBlockSplitter(blockToSplit,capo,params)
{
    abkfatal(_block.getNumRows() > 1,	
	"can't split a 1 row block horizontally");

    BBox blkBBox = _block.getBBox();
    if(ySplit < blkBBox.yMin || ySplit > blkBBox.yMax)
      {
	abkfatal(0, "ySplit out of block BBox while splitting horizontally\n");
      }

    if(_params.verb.forActions > 2)
	cout<<"Splitting a large block horizontally"<<endl;

    const vector<const RBPCoreRow*>& coreRows = _block.getRows();
    double actYSplit;
    unsigned r;
    for(r = 0; r < _block.getNumRows()-1; r++)
      {
	double y0 = coreRows[r]->getYCoord();
	double y1 = coreRows[r+1]->getYCoord();
	if(ySplit >= y0 && ySplit <= y1)
	  {
	    if((ySplit-y0) <= (y1 - ySplit))
	      {
		actYSplit = y0;
		_splitRow = r;
	      }
	    else
	      {
		actYSplit = y1;
		_splitRow = r+1;
	      }
	    break;
	  }
      }
    if(r ==  (_block.getNumRows()-1))
      {
	if(actYSplit >= coreRows[r]->getYCoord() && 
	   actYSplit <= blkBBox.yMax)
	  {
	    actYSplit = coreRows[r]->getYCoord();
	    _splitRow = r;
	  }
	else
	  abkfatal(0,"could not find a row corresponding to given ySplit while splitting block horizontally\n");
      }

    BBox p0BBox = blkBBox;
    p0BBox.yMin = actYSplit;
    BBox p1BBox = blkBBox;
    p1BBox.yMax = actYSplit;
    double   capacity  = _block.getCapacity();
    double p0Capacity =  _block.getContainedAreaInBBox(p0BBox);
    //double p1Capacity = capacity - p0Capacity;

    if(_params.verb.forActions > 4)
        cout<<"P0/P1 rows: "<<coreRows.size()-_splitRow<<"/"<<_splitRow<<endl;

    Timer tm21;
    PartitioningProblemForCapo problem(_block, capo, true, 
				       coreRows[_splitRow]->getYCoord(), 
				       Verbosity("1 1 1"), true);
    tm21.stop();
    CapoPlacer::capoTimer::SetupTime+=tm21.getUserTime();

    bool changeDefaultFlow=false;
    if(_params.useRgnConstr)
      {
	vector<double> partCaps(2);
	changeDefaultFlow=hPartCapsFrmRgnConstr(_splitRow, partCaps);
	if(changeDefaultFlow)
	  {
	    double p0Target = partCaps[0]*_block.getTotalCellArea();
	    double p1Target = _block.getTotalCellArea()-p0Target;
	    problem.setCapacity(0, p0Target);
	    problem.setCapacity(1, p1Target);
	  }
	else
	  {
	    double capRatio = p0Capacity / capacity;
	    double p0CellArea = capRatio*_block.getTotalCellArea();
	    problem.setCapacity(0, p0CellArea);
	    problem.setCapacity(1, _block.getTotalCellArea() - p0CellArea);
	  }
      }
    else
      {
	double capRatio = p0Capacity / capacity;
	double p0CellArea = capRatio*_block.getTotalCellArea();
	problem.setCapacity(0, p0CellArea);
	problem.setCapacity(1, _block.getTotalCellArea() - p0CellArea);
      }

    callPartitioner(problem, true, placement);

    if(_params.verb.forActions > 4)
      cout<<"Requested ySplit "<<ySplit<<"Final ySplit "<<coreRows[_splitRow]->getYCoord()<<endl;

    createHSplitBlocks(problem, _splitRow);
}
    
void SplitLargeBlockHorizontally::callPartitioner
                (PartitioningProblemForCapo& problem, bool splitPtFixed,
		 PlacementWOrient* placement)
{
    bool foundGoodSplit = false;
    unsigned numTries   = 0;
   
    while(!foundGoodSplit && numTries++ < 2)
    {
      foundGoodSplit = callMLPart(problem,splitPtFixed,placement);

      if(!foundGoodSplit && numTries < 2)
      {
	PartitioningBuffer& buff = problem.getSolnBuffers();
	unsigned numTerms=problem.getHGraph().getNumTerminals();
	unsigned sol;
        for(sol = buff.beginUsedSoln(); 
	    sol != buff.endUsedSoln(); sol++)
	{
	    Partitioning& part = buff[sol];
 	    for(unsigned i = numTerms; i < part.size(); i++)
		part[i].setToAll(2);
	}

	//consider changing the problem here to attempt to
	//get a better solution	
      }
    }
}


bool SplitLargeBlockHorizontally::callMLPart
                (PartitioningProblemForCapo& problem, bool splitPtFixed,
		 PlacementWOrient* placement)
{
    double blockWS = _block.getRelativeWhitespace();
    bool repartSmallWS = false;

    if(_params.repartSmallWS && blockWS < 0.02)
      repartSmallWS = true;

    if(_params.doRepartitioning || repartSmallWS || splitPtFixed)
      problem.setTolerance(_params.constantTolerance);
    else
    {
	if(!_params.useWSTolMethod)
	    problem.setTolerance(_params.constantTolerance);	
	else
	{
	  if(!splitPtFixed)
	    {
	      vector<double>   actualCaps(2,_block.getCapacity()/2.0);
	      vector<double>   partAreas (2,_block.getTotalCellArea()/2.0);
	      if(_params.fillerCellsPartCaps)
		partAreasForFillerCells(0, partAreas);
	      _splitRow  = findBestSplitRow(partAreas, actualCaps);
	    }
	  if(_params.useRgnConstr)
	    problem.setTolerance(_params.constantTolerance);	
	  else
    	    problem.setPaperMethodTolerance(_splitRow);
	}
    }

    const HGraphFixed& hgraph = problem.getHGraph();

    if(_params.verb.forActions > 4)
    {
	cout<<" First MLPart"<<endl;
	cout<<" Targets: "<<problem.getCapacities()[0][0]<<"/"
		          <<problem.getCapacities()[1][0]<<endl;
	cout<<" Max    : "<<problem.getMaxCapacities()[0][0]<<"/"
		          <<problem.getMaxCapacities()[1][0]<<endl;
	cout<<" Min    : "<<problem.getMinCapacities()[0][0]<<"/"
		          <<problem.getMinCapacities()[1][0]<<endl;
    }


    MLPart::Parameters mlParams(_capo.getParams().mlParams);

    mlParams.verb.forActions  = _params.verb.forActions/2;
    mlParams.verb.forMajStats = _params.verb.forMajStats/2;
    mlParams.verb.forSysRes   = _params.verb.forSysRes/2;

    if(hgraph.getNumNodes() < 500)
	mlParams.clParams.sizeOfTop = 100;

    problem.reserveBuffers(2);  //sets are pairs of runs+a VC
    PartitioningBuffer  refinedSolns(hgraph.getNumNodes(), _params.numMLSets);
    vector<double>      partAreas(2); //in the BSF soln
    unsigned            bestSet;
    double              origCut, finalCut;
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
      MLPart* partitioner1;

      if(_params.useNameHierarchy)
      {
        FillableHierarchy fillH(nodeNames, _params.delimiters,
                                problem.getHGraph(), fillHParams);
        Timer tm6;
        partitioner1 = new MLPart(problem, mlParams, &fillH);
        tm6.stop();
        CapoPlacer::capoTimer::MLTime+=tm6.getUserTime();
      }
      else if(_params.useQuadCluster && hgraph.getNumNodes() > 200)
      {
        Timer tm7;
	partitioner1 = new MLPart(problem, mlParams, placement);
        tm7.stop();
        CapoPlacer::capoTimer::MLTime+=tm7.getUserTime();
      }
      else
      {
        Timer tm8;
	partitioner1 = new MLPart(problem, mlParams);
        tm8.stop();
        CapoPlacer::capoTimer::MLTime+=tm8.getUserTime(); 
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

    vector<double>   actualCaps(2,0);
    if(!splitPtFixed)
      {
	if(blockWS < 0.5 || _params.uniformWS)
	  _splitRow  = findBestSplitRow(partAreas, actualCaps);
      }

    if(_params.verb.forActions > 4 )
        cout<<"final P0/P1 rows: "<<_block.getNumRows()-_splitRow<<"/"
			    <<_splitRow<<endl;

    if(_params.doRepartitioning || repartSmallWS || splitPtFixed)
    {
      cout<<"Doing Repartitioning phase"<<endl;
      //this sets the capacities and the tolerances too
      if(!splitPtFixed)
	{
	  double p0Target = (actualCaps[0]/_block.getCapacity())*
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
      else if(splitPtFixed)
	problem.setTolerance(5);
      else if(_params.useWSTolMethod)
      	problem.setPaperMethodTolerance(_splitRow);
      else
	problem.setTolerance(_params.constantTolerance);

      if(_params.verb.forActions > 4)
      {
	cout<<" Second FMPart"<<endl;
	cout<<" Targets: "<<problem.getCapacities()[0][0]<<"/"
		          <<problem.getCapacities()[1][0]<<endl;
	cout<<" Max    : "<<problem.getMaxCapacities()[0][0]<<"/"
		          <<problem.getMaxCapacities()[1][0]<<endl;
	cout<<" Min    : "<<problem.getMinCapacities()[0][0]<<"/"
		          <<problem.getMinCapacities()[1][0]<<endl;
      }

      FMPartitioner::Parameters fmParams;
      fmParams.useEarlyStop = false;
      fmParams.useClip = false;
      fmParams.verb.forActions  = _params.verb.forActions/2;
      fmParams.verb.forMajStats = _params.verb.forMajStats/2;
      fmParams.verb.forSysRes   = _params.verb.forSysRes/2;

      unsigned bestSolnNum = problem.getBestSolnNum();
      problem.getSolnBuffers().setBeginUsedSoln(bestSolnNum);
      problem.getSolnBuffers().setEndUsedSoln  (bestSolnNum+1);

      Timer tm5;
      FMPartitioner partitioner2(problem, fmParams);
      tm5.stop();
      CapoPlacer::capoTimer::FMTime+=tm5.getUserTime();

      finalCut = partitioner2.getBestSolnCost();

      double cutDelta = finalCut - origCut;
      if(cutDelta > origCut/10 && cutDelta >= 5 && !splitPtFixed)
      {
	if(_params.verb.forActions > 1)
	  cout<<" ->MLPart Failed.  Repartitioning increased "
	      <<"the cut from "<<origCut<<" to "<<finalCut<<endl;
 	return false;
      }

      bestSolnNum = problem.getBestSolnNum();
      if(_params.verb.forMajStats > 4)
	cout<<" Best Solution had cut "<<finalCut<<endl;
    } 

    //    else
    //cout<<"Skipping Repartitioning phase"<<endl;
 
    if(problem.getViolations()[problem.getBestSolnNum()])
    {
	if(_params.verb.forActions > 1)
	  cout<<" ->MLPart Failed.  No legal solutions"<<endl;
 	return false;
    }

    return true;
}


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



#include <vector>
using std::vector;

//extern vector<bool> found;




//created by Andrew Caldwell on 10/17/99 caldwell@cs.ucla.edu

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include "splitSmall.h"
#include "partProbForCapo.h"
#include "MLPart/mlPart.h"
#include "FMPart/fmPart.h"
#include "ShellPart/bbFMPart.h"

SplitSmallBlockVertically::SplitSmallBlockVertically
	(      CapoBlock&  blockToSplit,
	 const CapoPlacer& capo,
	       CapoSplitterParams params)
	: BaseBlockSplitter(blockToSplit,capo,params)
{

    if(_params.verb.forActions > 2)
	cout<<"Splitting a small block Vertically"<<endl;

    //1)construct the partitioning problem
    //2)while(!foundAGoodSplit)
    //    decide which partitioner to use, and partition the block
    //    if needed, change tolerances and repartition
    //	  see if this is a good split (legal, etc)
    //3)construct new blocks

    double   capacity  = _block.getCapacity();
    double   targetCap0, targetCap1;
    targetCap0 = capacity/2.0;
    targetCap1 = targetCap0;

    vector<double> partCaps;

    _xSplit = _block.findXSplit(targetCap0,targetCap1,0,partCaps);

    Timer tm25;
    PartitioningProblemForCapo problem(_block, capo, false, _xSplit);
    tm25.stop();
    CapoPlacer::capoTimer::SetupTime+=tm25.getUserTime();
    
    double p0Target = (partCaps[0]/_block.getCapacity())*
      _block.getTotalCellArea();
    double p1Target = _block.getTotalCellArea()-p0Target;
    problem.setCapacity(0, p0Target);
    problem.setCapacity(1, p1Target);
    
    //by sadya
    //problem.setVBlockTolerance(_xSplit);

    /*
    if(strcmp(_block.getName(), "BLOCKV0_H1_") == 0)
      {
        problem.saveAsNodesNets("problem");
        exit(0);
      }
    */
    callPartitioner(problem);


    callPartitioner(problem);

    if(_params.verb.forActions > 8)
	cout<<"Done partitioning...creating blocks"<<endl;

    createVSplitBlocks(problem, _xSplit);
}
 
    
void SplitSmallBlockVertically::callPartitioner
		(PartitioningProblemForCapo& problem)
{
    bool foundGoodSplit = false;
    unsigned numTries   = 0;
   
    while(!foundGoodSplit && numTries++ < 2)
    {
	if(_block.getNumCells() <= _capo.getParams().smallPartThreshold)
	{
	    if(_params.verb.forActions > 8)
		cout<<"Using BBPart"<<endl;
	    foundGoodSplit = callBBFM(problem);
	}
	else
	{
	    if(_params.verb.forActions > 8)
		cout<<"Using FMPart"<<endl;
	    foundGoodSplit = callFMPart(problem);

	    if(foundGoodSplit)
	    {
	 	if(_params.verb.forActions > 9)
		    cout<<"  Success.  Found a good partitioning"<<endl;
	    }
	    else if(numTries < 2)
	    {
	 	if(_params.verb.forActions > 9)
		    cout<<"  Failed. clearing part buffers"<<endl;
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
	    else if(_params.verb.forActions > 9)
		cout<<"  Failed too many times. Taking the best soln"<<endl;
	}
    }
}

bool SplitSmallBlockVertically::callBBFM
		(PartitioningProblemForCapo& problem)
{
    if(_params.useWSTolMethod && !_params.uniformWS)
      problem.setVBlockTolerance(_xSplit, 20.0);
    else
      problem.setTolerance(20.0);

    BBFMPart::Parameters bbfmparams;
    bbfmparams.fmStarts = 5;
    bbfmparams.bbParams.pushLimit = 1000000; //~2 seconds
    bbfmparams.verb = _params.verb;
    bbfmparams.verb.forActions  /= 4;
    bbfmparams.verb.forMajStats /= 4;
    bbfmparams.verb.forSysRes   /= 4;

    problem.reserveBuffers(1);
    problem.getSolnBuffers().setBeginUsedSoln(0);
    problem.getSolnBuffers().setEndUsedSoln(1);

    Timer tm17;
    BBFMPart partitioner1(problem, bbfmparams);
    tm17.stop();
    CapoPlacer::capoTimer::BBTime+=tm17.getUserTime();

    vector<double> partAreas(2,0.0);
    vector<double> siteAreas(2,0.0);
    const Partitioning& soln = problem.getBestSoln();
    for(unsigned c = 0; c < problem.getHGraph().getNumNodes(); c++)
    {
	unsigned part    = soln[c].isInPart(0) ? 0 : 1;
	partAreas[part] += problem.getHGraph().getNodeByIdx(c).getWeight();
    }
    
    double blockWS = _block.getRelativeWhitespace();

    if(blockWS < 0.5)
      _xSplit = _block.findXSplit(partAreas[0], partAreas[1], 10, siteAreas);

    //really ought to be more pro-active here.......
    if(  blockWS > 0 &&
      ( (siteAreas[0] < partAreas[0] || siteAreas[1] < partAreas[1])))
      {
	_xSplit = _block.findXSplit(partAreas[0], partAreas[1], 0, siteAreas);
      }

    return true; 
}

bool SplitSmallBlockVertically::callFMPart
		(PartitioningProblemForCapo& problem)
{
    double blockWS = _block.getRelativeWhitespace();
    bool repartSmallWS = false;
    if(_params.repartSmallWS && blockWS < 0.02)
      repartSmallWS = true;

    if(_params.useWSTolMethod  && !_params.uniformWS)
      problem.setVBlockTolerance(_xSplit, 20.0);
    else
      problem.setTolerance(20.0);
    //problem.setTolerance(10.0);

    FMPartitioner::Parameters fmParams;
    fmParams.useEarlyStop = false;
    fmParams.useClip = (_block.getNumCells() > 50) ? 1 : 0;
    fmParams.verb = _params.verb;
    fmParams.verb.forActions  /= 4;
    fmParams.verb.forMajStats /= 4;
    fmParams.verb.forSysRes   /= 4;

    unsigned numFMStarts = 4;
    problem.reserveBuffers(numFMStarts);
    problem.getSolnBuffers().setBeginUsedSoln(0);
    problem.getSolnBuffers().setEndUsedSoln(numFMStarts);
   
    Timer tm1;
    FMPartitioner partitioner1(problem, fmParams);
    tm1.stop();
    CapoPlacer::capoTimer::FMTime+=tm1.getUserTime();

    vector<double> partAreas(2,0.0);
    vector<double> siteAreas(2,0.0);
    const Partitioning&  soln = problem.getBestSoln();
    const HGraphFixed& hgraph = problem.getHGraph();

    for(unsigned c = hgraph.getNumTerminals(); c < hgraph.getNumNodes(); c++)
    {
	unsigned part    = soln[c].isInPart(0) ? 0 : 1;
	partAreas[part] += hgraph.getNodeByIdx(c).getWeight();
    }

		//for small blocks this isn't really the best
		//way to do this.  Really, we ought to compute the
		//range of splits that give us some min ws% in
		//each new block, and then pick the best xSplit
		//within that range.
    double idealXSplit = _block.findXSplit(partAreas[0], partAreas[1], 
						0, siteAreas);
    if(blockWS < 0.5)
      _xSplit            = _block.findXSplit(partAreas[0], partAreas[1], 
					     5, siteAreas);

    if(_params.verb.forMajStats > 9)
    {
	cout<<" CellAreas: "<<partAreas[0]<<"/"<<partAreas[1]<<endl;
	cout<<" Total:     "<<_block.getTotalCellArea()<<endl;
	cout<<" SiteAreas: "<<siteAreas[0]<<"/"<<siteAreas[1]<<endl;
    }

		//cutline was shifted
    if((_xSplit != idealXSplit && _params.doRepartitioning) || repartSmallWS)
    {
      double origCut = 
	problem.getCosts()[problem.getBestSolnNum()];
      
      double p0Target = (siteAreas[0]/_block.getCapacity())*
	_block.getTotalCellArea();
      problem.setCapacity(0, p0Target);
      problem.setCapacity(1, _block.getTotalCellArea() -p0Target);

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
      else if(_params.useWSTolMethod  && !_params.uniformWS)
	problem.setVBlockTolerance(_xSplit, 2.0);
      else
	problem.setTolerance(2.0);

      if(_params.verb.forMajStats > 4)
      {
        cout<<" Repartitioning FMPart"<<endl;
        cout<<"Targets:  "<<problem.getCapacities()[0][0]<<"/"
                          <<problem.getCapacities()[1][0]<<endl;
        cout<<"Max:      "<<problem.getMaxCapacities()[0][0]<<"/"
                          <<problem.getMaxCapacities()[1][0]<<endl;
        cout<<"Min:      "<<problem.getMinCapacities()[0][0]<<"/"
                          <<problem.getMinCapacities()[1][0]<<endl;
      }        

      fmParams.useClip = false;
     Timer tm2;
      FMPartitioner partitioner2(problem, fmParams);
    tm2.stop();
    CapoPlacer::capoTimer::FMTime+=tm2.getUserTime();

      double newCut = 
	problem.getCosts()[problem.getBestSolnNum()];

      double cutDelta = newCut - origCut;
      if(cutDelta > (origCut/10) && cutDelta >= 5)
      {
	if(_params.verb.forActions > 9)
	  cout<<" ->FM Failed.  Repartitioning increased "
	      <<"the cut too much"<<endl;
 	return false;
      }	
    }
    else
	_xSplit = idealXSplit;

    unsigned bestSolnNum = problem.getBestSolnNum(); 
    if(_params.verb.forMajStats > 4)
	cout<<" Best Solution had cut "<<problem.getCosts()[bestSolnNum]<<endl;
  
    if(problem.getViolations()[bestSolnNum])
    {
	if(_params.verb.forActions > 9)
	  cout<<" ->FM Failed.  No legal solutions"<<endl;
	_xSplit = idealXSplit;
 	return false;
    }

    return true;
}


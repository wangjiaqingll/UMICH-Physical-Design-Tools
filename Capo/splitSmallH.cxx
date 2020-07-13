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

SplitSmallBlockHorizontally::SplitSmallBlockHorizontally
	(      CapoBlock&  blockToSplit,
	 const CapoPlacer& capo,
	       CapoSplitterParams params)
	: BaseBlockSplitter(blockToSplit,capo,params)
{
    abkfatal(_block.getNumRows() > 1,	
	"can't split a 1 row block horizontally");

    double blockWS = _block.getRelativeWhitespace();
    bool repartSmallWS = false;
    if(_params.repartSmallWS && blockWS < 0.02)
      repartSmallWS = true;
    
    if(_params.verb.forActions > 2)
	cout<<"Splitting a small block horizontally"<<endl;
   
   	//notes:

	//shifting the cutline will be difficult on such small
	//problems, so we won't allow as much tolerance.
	//also, with the small number of rows small problems have,
	//shifting by even 1 row would mean a large change in
	//tolerance

	//P0 is the top partition

    const vector<const RBPCoreRow*>& rows = _block.getRows();
    double totalCellArea = _block.getTotalCellArea();
    vector<double>   targetCellAreas(2,totalCellArea/2.0);
    vector<double>   actualCaps(2,0);

    _splitRow  = findBestSplitRow(targetCellAreas, actualCaps);

    if(_params.verb.forActions > 4)
        cout<<"P0/P1 rows: "<<rows.size()-_splitRow<<"/"<<_splitRow<<endl;

    Timer tm24;
    PartitioningProblemForCapo problem(_block, capo, true, 
					rows[_splitRow]->getYCoord());
    tm24.stop();
    CapoPlacer::capoTimer::SetupTime+=tm24.getUserTime();
    
    double capRatio = actualCaps[0] / _block.getCapacity();
    double p0CellArea = capRatio*_block.getTotalCellArea();
    problem.setCapacity(0, p0CellArea);
    problem.setCapacity(1, _block.getTotalCellArea() - p0CellArea);

    if(_params.doRepartitioning || _params.useWSTolMethod || repartSmallWS)
	problem.setPaperMethodTolerance(_splitRow);
    else
	problem.setTolerance(_params.constantTolerance);
    
    /*
     if(strcmp(_block.getName(), "BLOCKV0_V1_") == 0)
       {
       problem.saveAsNodesNets("problem");
       exit(0);
       }
     */

   callPartitioner(problem);

    if(_params.verb.forActions > 8)
	cout<<"Done partitioning...creating blocks"<<endl;

    createHSplitBlocks(problem, _splitRow);
}
 
    
void SplitSmallBlockHorizontally::callPartitioner
		(PartitioningProblemForCapo& problem)
{
    bool foundGoodSplit = false;
    unsigned numTries   = 0;
   
    while(!foundGoodSplit && numTries++ < 2)
    {
	if(_block.getNumCells() <= _capo.getParams().smallPartThreshold)
	{
	  /*
	  if(_block.getNumCells() < 6)
	    {
	      problem.saveAsNodesNets("problemH");
	      exit(0);
	    }
	  */

	    if(_params.verb.forActions > 8)
		cout<<"Using BBPart"<<endl;
	    foundGoodSplit = callBBFM(problem);
	}
	else
	{
	    if(_params.verb.forActions > 8)
		cout<<"Using FMPart"<<endl;
	    foundGoodSplit = callFMPart(problem);

	    if(!foundGoodSplit && numTries < 2)
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
	}
    }
}

bool SplitSmallBlockHorizontally::callBBFM
		(PartitioningProblemForCapo& problem)
{
    problem.reserveBuffers(2);

    BBFMPart::Parameters bbfmparams;
    bbfmparams.fmStarts = 4;
    bbfmparams.bbParams.pushLimit = 1000000; //~2 seconds
    bbfmparams.verb = _params.verb;
    bbfmparams.verb.forActions  /= 4;
    bbfmparams.verb.forMajStats /= 4;
    bbfmparams.verb.forSysRes   /= 4;

    problem.getSolnBuffers().setBeginUsedSoln(0);
    problem.getSolnBuffers().setEndUsedSoln(1);

    Timer smallPartTimer;
    BBFMPart partitioner1(problem, bbfmparams);
    smallPartTimer.stop();
    CapoPlacer::capoTimer::BBTime+=smallPartTimer.getUserTime();

    //if even # rows or hard problem, take this soln
    if((_block.getNumRows()%2==0)/* || smallPartTimer.getUserTime() > 0.1*/)
	return true;

    unsigned origSplitRow = _splitRow;

	//otherwise, try rounding the split the other way..
    if(_splitRow <= floor(_block.getNumRows()/2.0))
	_splitRow++;
    else
	_splitRow--;

    abkfatal(_splitRow != 0 && _splitRow < _block.getNumRows(),
               "new blk has no rows");

    problem.setPaperMethodTolerance(_splitRow);
    problem.getSolnBuffers().setBeginUsedSoln(1);
    problem.getSolnBuffers().setEndUsedSoln(2);
    
    Timer tm16;
    BBFMPart partitioner2(problem, bbfmparams);
    tm16.stop();
    CapoPlacer::capoTimer::BBTime+=tm16.getUserTime();

		//note to self...could include area-balances
	 	//and pin-balances in this decision as well
    if(problem.getCosts()[0] <= problem.getCosts()[1])
    {
	problem.setBestSolnNum(0);
	_splitRow = origSplitRow;
    }
    else
	problem.setBestSolnNum(1);
	    
    return true;
}

bool SplitSmallBlockHorizontally::callFMPart
		(PartitioningProblemForCapo& problem)
{
    double blockWS = _block.getRelativeWhitespace();
    bool repartSmallWS = false;
    if(_params.repartSmallWS && blockWS < 0.02)
      repartSmallWS = true;

    if(_params.doRepartitioning || repartSmallWS)
      problem.setTolerance(10.0);
    
    //otherwise, the tolerance was already set correctly and
    //will not change

    if(_params.verb.forActions > 4)
    {
	cout<<" Targets: "<<problem.getCapacities()[0][0]<<"/"
		          <<problem.getCapacities()[1][0]<<endl;
	cout<<" Max    : "<<problem.getMaxCapacities()[0][0]<<"/"
		          <<problem.getMaxCapacities()[1][0]<<endl;
	cout<<" Min    : "<<problem.getMinCapacities()[0][0]<<"/"
		          <<problem.getMinCapacities()[1][0]<<endl;
	cout<<endl;
    }

    FMPartitioner::Parameters fmParams;
    fmParams.useEarlyStop = false;
    fmParams.useClip = (_block.getNumCells() > 75) ? 1 : 0;
    fmParams.verb = _params.verb;
    fmParams.verb.forActions  /= 4;
    fmParams.verb.forMajStats /= 4;
    fmParams.verb.forSysRes   /= 4;

    unsigned numFMStarts = 4;
    problem.reserveBuffers(numFMStarts);
    problem.getSolnBuffers().setBeginUsedSoln(0);
    problem.getSolnBuffers().setEndUsedSoln(numFMStarts);
  
    Timer tm3;
    FMPartitioner partitioner1(problem, fmParams);
    tm3.stop();
    CapoPlacer::capoTimer::FMTime+=tm3.getUserTime(); 

    double origCut = 
	problem.getCosts()[problem.getBestSolnNum()];

    if(_params.doRepartitioning || repartSmallWS)
    {	
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
      else if(_params.useWSTolMethod)
          problem.setPaperMethodTolerance(_splitRow);
      else
          problem.setTolerance(_params.constantTolerance);

      if(_params.verb.forActions > 4)
      {
	cout<<" Second (smaller tol) FMPart"<<endl;
	cout<<" Targets: "<<problem.getCapacities()[0][0]<<"/"
		          <<problem.getCapacities()[1][0]<<endl;
	cout<<" Max    : "<<problem.getMaxCapacities()[0][0]<<"/"
		          <<problem.getMaxCapacities()[1][0]<<endl;
	cout<<" Min    : "<<problem.getMinCapacities()[0][0]<<"/"
		          <<problem.getMinCapacities()[1][0]<<endl;
	cout<<endl;
      }

      fmParams.useClip = false;
      Timer tm4;
      FMPartitioner partitioner2(problem, fmParams);
      tm4.stop();
      CapoPlacer::capoTimer::FMTime+=tm4.getUserTime();

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

    unsigned bestSolnNum = problem.getBestSolnNum(); 
    if(_params.verb.forMajStats > 4)
	cout<<" Best Solution had cut "<<problem.getCosts()[bestSolnNum]<<endl;
  
    if(problem.getViolations()[bestSolnNum])
    {
	if(_params.verb.forActions > 9)
	  cout<<" ->FM Failed.  No legal solutions"<<endl;
 	return false;
    }

    return true;
}


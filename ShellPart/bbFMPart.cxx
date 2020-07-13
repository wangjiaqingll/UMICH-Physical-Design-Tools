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




//////////////////////////////////////////////////////////////////////
//
// Created by Mike Oliver on Apr 24, 1999
// BBFMPart.cxx: implementation of the BBFMPart class.
//
//////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include "bbFMPart.h"
#include "SmallPart/bbPart.h"
#include "FMPart/fmPart.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BBFMPart::BBFMPart(PartitioningProblem& problem, Parameters params)
	: _problem(problem),_params(params)
{
    unsigned numSol=problem.getSolnBuffers().endUsedSoln()-
                    problem.getSolnBuffers().beginUsedSoln();

    abkfatal3(numSol==1, "BBFMPart produces only one solution;",numSol,
             " requested\n");
    
    _params.bbParams.verb = _params.verb;
    _params.fmParams.verb = _params.verb;

    BBPart bbPart(_problem,_params.bbParams);

    _solvedExactly = true;

    if (bbPart.timedOut())
    {
	cout << "============== BB part timed out! ==========" << endl;
	_solvedExactly = false;

        PartitioningBuffer const &oldBuf = _problem.getSolnBuffers();
        unsigned oldBestNo = _problem.getBestSolnNum();

        // FM will fill in a solution for every available space
        // in the buffer.
        PartitioningBuffer newBuf(oldBuf.getNumModulesUsed(),
                                 _params.fmStarts);

        unsigned firstNo = newBuf.beginUsedSoln();

        //Now we copy the soln from BBPart into the first slot
        //in the new buffer
        newBuf[firstNo] = _problem.getBestSoln();

        //Now we temporarily use the new buffer to call fm
        PartitioningBuffer *pOldBuf=_problem.swapOutSolnBuffers(&newBuf);
        abkfatal(pOldBuf==&oldBuf,"Unexpected error");
        FMPartitioner fmPart(_problem,_params.fmParams);

        //Now we copy the best solution from fm into the old buffer
        (*pOldBuf)[oldBestNo] = _problem.getBestSoln();

        //And put the old buffer back into the problem
        _problem.swapOutSolnBuffers(pOldBuf);
        _problem.setBestSolnNum(oldBestNo);
    }
}

BBFMPart::~BBFMPart()
    {
    
    }

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


















//   created by Igor Markov, April 30, 1999

#ifndef __SMALLPL_PROB_H__
#define __SMALLPL_PROB_H__

#include "Ctainers/bitBoard.h"
#include "Placement/placeWOri.h"
#include "baseSmallPlPr.h"

class SmallPlacementProblem : public BaseSmallPlacementProblem
{
  static BitBoard _seenNets;
  static BitBoard _essentialNets;
  static BitBoard _mobileCells;
//added by sadya to represent clustered cells
  static BitBoard _clusteredCells;
	
  protected:

   SmallPlacementProblem(unsigned numCells, Verbosity verb=Verbosity())
        : BaseSmallPlacementProblem(numCells,verb) {}

  public:

   SmallPlacementProblem(const char* auxFile,Verbosity verb=Verbosity())
	: BaseSmallPlacementProblem(auxFile,verb) {}

   SmallPlacementProblem(const HGraphWDimensions& hgraph, 
	  const PlacementWOrient& placement,  
	  const bit_vector& placed,
          const vector<unsigned>& movables, const SmallPlacementRow&, 
          Verbosity verb=Verbosity("1 1 1"));

//this constructor is added by saurabh adya to take care of fake cells(WS)
//in the smallplacement problem
   SmallPlacementProblem(const HGraphWDimensions& hgraph,
	  const PlacementWOrient& placement,
	  const bit_vector& placed,
	  const bit_vector& fixed,
	  const vector<unsigned>& movables, 
	  const vector<SmallPlacementRow>&,
	  const vector<double>& whiteSpaceWidths,
          Verbosity verb=Verbosity("1 1 1"));

//this constructor is added by saurabh adya to allow clustering of cells
//in the smallplacement problem
   SmallPlacementProblem(const HGraphWDimensions& hgraph,
	  const PlacementWOrient& placement,
	  const bit_vector& placed,
	  const bit_vector& fixed,
	  const vector< vector<unsigned> >& movables, 
	  const vector<SmallPlacementRow>&,
	  const vector<double>& whiteSpaceWidths,
          Verbosity verb=Verbosity("1 1 1"));

	//for each node in hgraph, placement gives you the node's location,
	//orientations gives you its orientation, and placed tells you
	//if the cell has actually been placed or not.  If it has not
	//the nodes placed location will be used as the location of all
	//pins on the node.
};

#endif

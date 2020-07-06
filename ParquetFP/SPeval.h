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








#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <math.h>
#include <stdlib.h>
using namespace std;

namespace parquetfp
{
class SPeval
{
 private:
  //buffers go in here
  vector<unsigned> _match;
  vector<double> _LL;
  vector<unsigned> _reverseSeq;
  vector<unsigned> _XX;
  vector<unsigned> _YY;
  vector<double> _heights;
  vector<double> _widths;
  map<unsigned , double> _BST;  //for the O(nlogn) algo
  vector<unsigned> _reverseSeq2; //only for O(nlog n) algo

  vector< vector<bool> > _TCGMatrixHoriz;
  vector< vector<bool> > _TCGMatrixVert;

  double _lcsCompute(const vector<unsigned>& X,
		     const vector<unsigned>& Y,
		     const vector<double>& weights,
		     vector<unsigned>& match,
		     vector<double>& P,
		     vector<double>& L
		     );

  double _lcsReverseCompute(const vector<unsigned>& X,
			    const vector<unsigned>& Y,
			    const vector<double>& weights,
			    vector<unsigned>& match,
			    vector<double>& P,
			    vector<double>& L
			    );
  
  double _lcsComputeCompact(const vector<unsigned>& X,
			    const vector<unsigned>& Y,
			    const vector<double>& weights,
			    vector<unsigned>& match,
			    vector<double>& P,
			    vector<double>& L,
			    vector<double>& oppLocs,
			    vector<double>& oppWeights
			    );
  
  //fast are for the O(nlog n) algo
  double _findBST(unsigned index); //see the paper for definitions
  void _discardNodesBST(unsigned index, double length);

  double _lcsComputeFast(const vector<unsigned>& X,
		     const vector<unsigned>& Y,
		     const vector<double>& weights,
		     vector<unsigned>& match,
		     vector<double>& P,
		     vector<double>& L
		     );


  bool _TCGMatrixInitialized;
  void _initializeTCGMatrix(unsigned size);
  bool _paramUseFastSP;

 public:
  vector<double> xloc;
  vector<double> yloc;
  double xSize;
  double ySize;
  vector<double> xSlacks;
  vector<double> ySlacks;
  vector<double> xlocRev;
  vector<double> ylocRev;


  SPeval(const vector<double> heights, const vector<double> widths,
	 bool paramUseFastSP);

  
  void evaluate(vector<unsigned>& X, vector<unsigned>& Y);
  double xEval();
  double yEval();
  void evalSlacks(vector<unsigned>& X, vector<unsigned>& Y);
  double xEvalRev();
  double yEvalRev();

  void evaluateCompact(vector<unsigned>& X, vector<unsigned>& Y, 
		       bool whichDir);
  double xEvalCompact();
  double yEvalCompact();
  void computeConstraintGraphs();
  void removeRedundantConstraints(bool knownDir);
  void computeSPFromCG();

  //following are for evaluating with the O(nlog n) scheme
  void evaluateFast(vector<unsigned>& X, vector<unsigned>& Y);
  double xEvalFast();
  double yEvalFast();
  void evalSlacksFast(vector<unsigned>& X, vector<unsigned>& Y);
  double xEvalRevFast();
  double yEvalRevFast();

  //miscelleneous functions
  void changeWidths(vector<double>& widths);
  void changeHeights(vector<double>& heights);
  void changeNodeWidth(unsigned index, double width);
  void changeNodeHeight(unsigned index, double height);
  void changeOrient(unsigned index);
};
}
//using namespace parquetfp;

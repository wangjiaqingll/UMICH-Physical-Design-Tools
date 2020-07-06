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


/*Authors Matt Guthaus and Saurabh Adya*/

#ifndef _SIMANNEAL_H_
#define _SIMANNEAL_H_

#include "DB/DB.h"
#include "RBPlace/RBPlacement.h"

class SimAnnealParameters
{
 public:
};

class SimAnneal
{
 private:
  void _generate();
  double _cost();
  bool _accept(double newCost, double oldCost, double curTemp);
  void _update(double& curTemp);
  void _calibrate(); //warm up and calibrate the cost function, temperature etc
  void _anneal(); //performs actual annealing

  double _windowfactor;
  double _xspan,_yspan;
  
  BBox _layoutBBox;
  double _layoutXSize,_layoutYSize,_layoutArea;
  RBPlacement &_rb;
  unsigned _maxIter;
  unsigned _calibMaxIter;
  double _initTemp;
  double _curTemp;
  double _stopTemp;

  // costs

  bool _greedy;  //if true then only greedy annealing
  double _hpwl,_avghpwl,_totalhpwl;
  double _overlap,_avgoverlap,_totaloverlap;
  double _oldCost,_newCost;
  double _lambda;
  double _lambdacost;

  double _initHPWLPerNet; //used for scaling

  vector<unsigned> _movables; //moved cellist, cleared for every generated move
  vector<Point> _oldPlace; // old locations of movable cells 
  vector<Point> _newPlace; // new locations of movable cells
  
  // stats
  double _posAcceptCount, _negAcceptCount, _totalCount;
  unsigned _itCount;

 public:
  SimAnneal(RBPlacement& rbplace, bool greedy = false);
  //~SimAnneal();
};

#endif

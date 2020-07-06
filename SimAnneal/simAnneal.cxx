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


/* Authors Matt Guthaus and Saurabh Adya*/

#include "ABKCommon/abkcommon.h"
#include "RBPlace/RBPlacement.h"
#include "DB/DB.h"
#include "SimAnneal/simAnneal.h"

#define DEBUG 0
#define directedMoves 0

/*******************************************************************************
 * Constructor. Performs the main annealing algorithm.
 *******************************************************************************/
SimAnneal::SimAnneal(RBPlacement& rbplace, bool greedy)
	: _rb(rbplace), _calibMaxIter(500),
                _greedy(greedy), 
		_oldCost(0), _newCost(0),_lambda(1),
		_lambdacost(1), _oldPlace(0), _newPlace(0)
{
  //  abkassert(_rb.checkCC(),"All core cells not in valid rows. Bad RBPlacement\n");

  _negAcceptCount=_posAcceptCount=0;
  _layoutBBox = _rb.getBBox(false);
  _layoutXSize = _layoutBBox.getWidth();
  _layoutYSize = _layoutBBox.getHeight();
  _layoutArea = _layoutXSize*_layoutYSize;
  cout << "Placement area: " << _layoutXSize << " x " << _layoutYSize << endl;
  cout << "Num cells: " << _rb.getNumCells() << endl;
  
  _hpwl=_rb.evalHPWL(true);
  _overlap=_rb.calcOverlap();
  _oldCost = _cost();

  _initHPWLPerNet = _hpwl/_rb.getHGraph().getNumEdges();
  /********************************************************
   * Initialize the temp, max iterations and move window
   *********************************************************/
  //_initTemp=_curTemp=pow(_rb.getNumCells(),4.0/3); // from Sechen's Timberwolf
  _initTemp = _curTemp = _initHPWLPerNet;
  _stopTemp = _curTemp/100;
  _maxIter = _rb.getNumCells();

  cout << "Initial:\t Temp: " << _curTemp << " Iter: " << _maxIter 
       << " HWPL: " << _rb.evalHPWL(true) << " Over: " << _rb.calcOverlap() 
       << " Cost: " << _oldCost << endl;
  
  // at a minimum, the window looks 2 rows up/down, and 2 sites left/right
  // assumes all rows have same pitch and site width

  double areaPerCell = _layoutArea/_rb.getNumCells();
  double layoutAR = _layoutXSize/_layoutYSize;
  double widthPerCell = sqrt(areaPerCell*layoutAR);
  double heightPerCell = widthPerCell/layoutAR;
  
  // at a minimum, the window looks 2 rows up/down, and 2 sites left/right
  // assumes all rows have same pitch
  double minyspan = 2*(_rb.getCoreRow(1).getYCoord()-_rb.getCoreRow(0).getYCoord());
  // assumes all rows have same site width	
  double minxspan = 5*widthPerCell; 

  //for moves controlled by "window"
  _windowfactor=log10(_curTemp/_stopTemp)/log10(_initTemp/_stopTemp);
  const double scale=15;
  _xspan=_windowfactor*scale*widthPerCell;
  _yspan=_windowfactor*scale*heightPerCell;
  
  if(_xspan < minxspan)
    _xspan = minxspan;
  if(_yspan < minyspan)
    _yspan = minyspan;
  
  if(!_greedy)  //calibrate only if not greedy
    _calibrate();
  //return;
  _initTemp = _curTemp;
  _stopTemp = _curTemp/100;
  
  _anneal();

  cout << "Final:\t HWPL: " << _rb.evalHPWL() << " Over: " << _rb.calcOverlap()
       << endl;
}


/*******************************************************************************
 * generates a set of movable cells and locations (_movables, _newPlace)
 *******************************************************************************/
void
SimAnneal::_generate()
{
  _movables.clear();
  _oldPlace.clear();
  _newPlace.clear();

  // select random cell
  unsigned randIdx1;
  RandomUnsigned randcell(0,_rb.getNumCells());

  while (!_rb.isCoreCell(randIdx1=randcell) || !_rb.isInSubRow(randIdx1));
  _movables.push_back(randIdx1);
  _oldPlace.push_back(_rb[randIdx1]);

  double cellWidth1 = _rb.getHGraph().getNodeWidth(randIdx1);

  unsigned whichMove = RandomUnsigned(0,10);
  unsigned directedMove = RandomUnsigned(0,10);

  if (directedMove < 5) //totally random moves
    {
      if (whichMove < 4) // displace cell to random location
	{
	  unsigned crow=RandomUnsigned(0,_rb.getNumCoreRows());
	  const RBPCoreRow &cr = _rb.getCoreRow(crow);
	  unsigned srow=RandomUnsigned(0,cr.getNumSubRows());
	  unsigned site=RandomUnsigned(0,cr[srow].getNumSites());
	  Point randLoc = Point(cr[srow].getXStart()+site*cr[srow].getSiteWidth(),cr[srow].getYCoord());
	  
	  Point newLoc = randLoc;
	  bool foundWS = _rb.findClosestWS(randLoc, newLoc, cellWidth1);
	  if(foundWS)
	    {
	      _newPlace.push_back(newLoc);
	    }
	  else
	    {
	      _movables.pop_back();
	      _oldPlace.pop_back();
	    }

	  if (DEBUG) cout << "displace: " << randIdx1 << "->" <<  crow << "," << srow << "," << site << endl;
	}
      else
	{
	  unsigned randIdx2;
	  
	  while (!_rb.isCoreCell(randIdx2=randcell) || randIdx2 == randIdx1
		 || !_rb.isInSubRow(randIdx2));
	  // pair it to swap
	  _movables.push_back(randIdx2);
	  _oldPlace.push_back(_rb[randIdx2]);
	  // push them back backwards
	  _newPlace.push_back(_rb[randIdx2]);
	  _newPlace.push_back(_rb[randIdx1]);
	  if (DEBUG) {
	    cout << "swap:";
	    cout << "\t" << randIdx1 << ":" << _rb[randIdx1] << "->" << _rb[randIdx2] << endl;
	    cout << "\t" << randIdx2 << ":" << _rb[randIdx2] << "->" << _rb[randIdx1] << endl;
	  }
	}
    }
  else  //directed quadratic moves 
    {
      // starting cell location
      const RBPCoreRow *cr = _rb.findCoreRow(_rb[randIdx1]);
      abkassert(cr!=NULL,"Invalid core row");

      // assumes all rows are spaced the same
      double rowpitch =_rb.getCoreRow(1).getYCoord()-
	_rb.getCoreRow(0).getYCoord();
      double sitepitch = cr->getSpacing();//+cr->getSiteWidth();
      double rowHeight = _rb.getCoreRow(0).getHeight();
      // convert from center of cell to lower-left coordinates?
      double ycellcoordoffset = _layoutBBox.yMin;
      double xcellcoordoffset = _layoutBBox.xMin;
      
      Point curLoc=_rb[randIdx1];
      
      if (whichMove < 3)
	{
	  // find the median location
	  Point optLoc = _rb.calcMeanLoc(randIdx1);
	  
	  // put it at the optimal site ignoring overlap
	  optLoc.x = (floor((optLoc.x-xcellcoordoffset)/sitepitch))*sitepitch+
	    xcellcoordoffset;
	  optLoc.y=(floor((optLoc.y-ycellcoordoffset)/rowpitch))*rowpitch+
	    ycellcoordoffset;
	  
	  // make sure optloc is within the die area
	  optLoc.x=(optLoc.x>=_layoutBBox.xMax)?_layoutBBox.xMax:optLoc.x;
	  optLoc.x=(optLoc.x<=_layoutBBox.xMin)?_layoutBBox.xMin:optLoc.x;
	  optLoc.y=(optLoc.y>=_layoutBBox.yMax-rowHeight)?_layoutBBox.yMax-rowHeight:optLoc.y;
	  optLoc.y=(optLoc.y<=_layoutBBox.yMin)?_layoutBBox.yMin:optLoc.y;
	  
	  Point newLoc = optLoc;
	  bool foundWS = _rb.findClosestWS(optLoc, newLoc, cellWidth1);
	  if(foundWS)
	    {
	      _newPlace.push_back(newLoc);
	    }
	  else
	    {
	      _movables.pop_back();
	      _oldPlace.pop_back();
	    }	  

	  if (DEBUG) cout << "optimal:  " << randIdx1 << ": " << curLoc << "->" <<  optLoc << endl;
	}
      else if (whichMove < 6)
	{
	  Point newLoc;
	  // find random y position in window around curLoc
	  double ymin = ((curLoc.y-_yspan)<=_layoutBBox.yMin)?_layoutBBox.yMin:curLoc.y-_yspan;
	  double ymax = ((curLoc.y+_yspan)>=_layoutBBox.yMax-rowHeight)?_layoutBBox.yMax-rowHeight:curLoc.y+_yspan;
	  newLoc.y=RandomDouble(ymin,ymax);
	  
	  // find random x position in window around curLoc
	  double xmin = ((curLoc.x-_xspan)<=_layoutBBox.xMin)?_layoutBBox.xMin:curLoc.x-_xspan;
	  double xmax = ((curLoc.x+_xspan)>=_layoutBBox.xMax)?_layoutBBox.xMax:curLoc.x+_xspan;
	  newLoc.x=RandomDouble(xmin,xmax); 
	  
	  // make xloc multiple of site width
	  newLoc.x = (floor((newLoc.x-xcellcoordoffset)/sitepitch))*sitepitch+
	    xcellcoordoffset;
	  // make yloc multiple of row height+spacing
	  newLoc.y=(floor((newLoc.y-ycellcoordoffset)/rowpitch))*rowpitch +
	    ycellcoordoffset;

	  // checks that we have a valid core and sub row
	  RBRowPtrs rowPtrs = _rb.findBothRows(newLoc);
	  abkassert(rowPtrs.first != NULL, "Invalid core row");
	  abkassert(rowPtrs.second != NULL, "Invalid sub row");
	  
	  Point actLoc = newLoc;
	  bool foundWS = _rb.findClosestWS(newLoc, actLoc, cellWidth1);
	  if(foundWS)
	    {
	      _newPlace.push_back(actLoc);
	    }
	  else
	    {
	      _movables.pop_back();
	      _oldPlace.pop_back();
	    }	  
	  if (DEBUG) cout << "displace " << randIdx1 << ": " << curLoc << "->" <<  newLoc << endl;
	}
      else
	{
	  // swap with cell in window of optimal location (could be with an empty site though)
	  // find the new location for the cell
	  Point newLoc;
	  // find random y position in window around curLoc
	  double ymin = (curLoc.y-_yspan<=_layoutBBox.yMin)?_layoutBBox.yMin:curLoc.y-_yspan;
	  double ymax = (curLoc.y+_yspan>=_layoutBBox.yMax-rowHeight)?_layoutBBox.yMax-rowHeight:curLoc.y+_yspan;
	  newLoc.y=RandomDouble(ymin,ymax);
	  
	  // find random x position in window around curLoc
	  double xmin = (curLoc.x-_xspan<=_layoutBBox.xMin)?_layoutBBox.xMin:curLoc.x-_xspan;
	  double xmax = (curLoc.x+_xspan>=_layoutBBox.xMax)?_layoutBBox.xMax:curLoc.x+_xspan;
	  newLoc.x=RandomDouble(xmin,xmax); 
	  
	  // make xloc multiple of site width
	  newLoc.x = (floor((newLoc.x-xcellcoordoffset)/sitepitch))*sitepitch+
	    xcellcoordoffset;
	  // make yloc multiple of row height+spacing
	  newLoc.y=(floor((newLoc.y-ycellcoordoffset)/rowpitch))*rowpitch+
	    ycellcoordoffset;
	  
	  // checks that we have a valid core and sub row
	  RBRowPtrs rowPtrs = _rb.findBothRows(newLoc);
	  abkassert(rowPtrs.first != NULL, "Invalid core row");
	  abkassert(rowPtrs.second != NULL, "Invalid sub row");
	  
	  // get index of cell at (xloc,yloc)
	  unsigned randIdx2 = _rb.findCellIdx(newLoc);
	  
	  // if we found a cell within the window then swap it
	  if (randIdx2<_rb.getNumCells()&&_rb.isCoreCell(randIdx2))
	    {
	      // pair it to swap
	      _movables.push_back(randIdx2);
	      _oldPlace.push_back(_rb[randIdx2]);
	      // push them back backwards
	      _newPlace.push_back(_rb[randIdx2]);
	      _newPlace.push_back(_rb[randIdx1]);
	      if (DEBUG) 
		{
		  cout << "swap:";
		  cout << "\t" << randIdx1 << ":" << _rb[randIdx1] << "->" << _rb[randIdx2] << endl;
		  cout << "\t" << randIdx2 << ":" << _rb[randIdx2] << "->" << _rb[randIdx1] << endl;
		}
	    }
	  // if no cell was there, just displace the cell!
	  else
	    {
	      _newPlace.push_back(newLoc);
	      if (DEBUG) cout << "displace2 " << randIdx1 << ": " << curLoc << "->" <<  newLoc << endl;
	    }
	}
    }
}

/*******************************************************************************
 * _cost() calculates the cost function INCREMENTALLY. It finds the HPWL
 * of the vector of cells being moved (_movables) and the overlap of them too.
 *******************************************************************************/
double
SimAnneal::_cost()
{
  // find HPWL for each net attached to moved cells
  _hpwl += _rb.calcInstHPWL(_movables);
  // find overlap from introduced cells
  _overlap += _rb.calcInstOverlap(_movables);
  
  double cost;
  //cost = hpwl+overlap/log10(_curTemp/stopTemp);
  //cost = hpwl+10*pow(overlap,2.0);

  cost = (_hpwl+_lambda*_overlap);
  return(cost);
}


/*******************************************************************************
 * Move acceptance function.
 *******************************************************************************/
bool
SimAnneal::_accept(double _newCost, double _oldCost, double _curTemp)
{
  double delta = _newCost-_oldCost;
  RandomDouble randIdx(0,1);
  if (delta <= 0)
    {
      _posAcceptCount++;
      return(true);
    }

  if(_greedy)
    return(false);  

  //delta /= _initHPWLPerNet;
  //delta *= 50;

  //cout<<"Random  "<<randIdx<<" lambdaCost "<<_lambdacost<<" delta "<<delta<<"  cost  "<<_lambdacost*exp(-fabs(delta)/_curTemp)<<endl;

  if (randIdx < _lambdacost*exp(-fabs(delta)/_curTemp))
    {
      _negAcceptCount++;
      return(true);
    }
  return(false);	
}


/*******************************************************************************
 * Temperature update function.
 *******************************************************************************/
void
SimAnneal::_update(double& _curTemp)
{
  // old simple schedule (like in Dragon annealer)
  if (_curTemp > 0.66*_initTemp)
    _curTemp *=0.8;
  else if (_curTemp > 0.33*_initTemp)
    _curTemp *= 0.85;
  else if(_curTemp > 0.1*_initTemp)
    _curTemp *= 0.90;
  else
    _curTemp *= 0.96;
  
  //figure out alpha so that we have 120 temperatures like NRG
  //double alpha=pow(10,log10(stopTemp/_initTemp)/120);
  //_curTemp *= alpha;
  
  double areaPerCell = _layoutArea/_rb.getNumCells();
  double layoutAR = _layoutXSize/_layoutYSize;
  double widthPerCell = sqrt(areaPerCell*layoutAR);
  double heightPerCell = widthPerCell/layoutAR;
  
  // at a minimum, the window looks 2 rows up/down, and 2 sites left/right
  // assumes all rows have same pitch
  double minyspan = 2*(_rb.getCoreRow(1).getYCoord()-_rb.getCoreRow(0).getYCoord());
  // assumes all rows have same site width	
  double minxspan = 5*widthPerCell; 

  // moves controlled by "window"
  _windowfactor=log10(_curTemp/_stopTemp)/log10(_initTemp/_stopTemp);
  const double scale=15;
  _xspan=_windowfactor*scale*widthPerCell;
  _yspan=_windowfactor*scale*heightPerCell;

  if(_xspan < minxspan)
    _xspan = minxspan;
  if(_yspan < minyspan)
    _yspan = minyspan;

  cout << _windowfactor << ": " << _xspan << "," << _yspan << endl;
}

  /********************************************************
   * Perform a "warm up" that doesn't actually make any moves 
   * or change temperature to calibrate the cost function
   *********************************************************/
void SimAnneal::_calibrate()
{
  cout << "Performing a warm-up run to calibrate acceptance rates..." << endl;
  cout << setw(10) << "Iter"
       << setw(10) << "\tTemp"
       << setw(10) << "\tNAR"
       << setw(10) << "\tAR"
       << setw(10) << "\tlambda"
       << setw(10) << "\tCost(HPWL+OV)"<<endl; 

  double oldTemp = DBL_MAX;
  int it=0;
  int maxIter = 500;

  while(fabs(oldTemp-_curTemp)/oldTemp > 0.01 && it < maxIter)
    {
      ++it;
      oldTemp = _curTemp;
      _negAcceptCount=_posAcceptCount=0;
      _totaloverlap=_totalhpwl=0;
      _itCount = 0;
      cout << "[ ";
      
      while (_itCount++ <= _calibMaxIter) 
	{
	  _totalhpwl+=_hpwl;
	  _totaloverlap+=_overlap;
	  if (!(_itCount%(_calibMaxIter/5)))
	    {
	      cout << ".";
	      cout.flush();
	    }
	  if (_itCount==1)
	    {
	      _avghpwl=_totalhpwl/_itCount;
	      _avgoverlap=_totaloverlap/_itCount;
	      // overlap should never be scaled DOWN
	      if (_avgoverlap == 0) 
		_lambda=1;//_hpwl/_rb.getHGraph().getNumEdges();
	      else
		_lambda=(_avghpwl>_avgoverlap)?_avghpwl/_avgoverlap:1;
	      _oldCost = _hpwl+_lambda*_overlap;
	    }
	  if (!(_itCount%(_calibMaxIter/5))||_itCount==1)
	    {
//            double acceptRate = (_posAcceptCount+_negAcceptCount)/_itCount;
	      double negAcceptRate = _negAcceptCount/_itCount;
	      //_lambdacost *= (1-(negAcceptRate-0.01)/10);
	      _curTemp *= (1-(negAcceptRate-0.01)/1);
	      //cout<<"acceptrate "<<acceptRate<<" curTemp  "<<_curTemp<<" lambdacost  "<<_lambdacost<<endl;
	    }

	  _totalCount++;
  
	  _generate();
	  
	  _hpwl -= _rb.calcInstHPWL(_movables); 
	  _overlap -= _rb.calcInstOverlap(_movables);
	  _rb.updateCells(_movables, _newPlace); 
	  _newCost = _cost();

	  // check if the move is good
	  _accept(_newCost, _oldCost, _curTemp);

	  // this undoes the moves
	  _hpwl -= _rb.calcInstHPWL(_movables); 
	  _overlap -= _rb.calcInstOverlap(_movables);
	  _rb.updateCells(_movables, _oldPlace);
	  _oldCost = _cost();
	}
      
      cout << " ]";
      
      double negAcceptRate = _negAcceptCount/_calibMaxIter;
      double acceptRate = (_posAcceptCount+_negAcceptCount)/_calibMaxIter;
      cout << setw(10) << it
	   << setw(10) << _curTemp 
	   << "\t" << setw(10) << negAcceptRate 
	   << "\t" << setw(10) << acceptRate  
	   << "\t" << setw(10) << _lambdacost
	   << "\t" << setw(10) << _oldCost 
	   << "(" << _hpwl << "+" << _lambda << "*" << _overlap <<")" 
	   << endl;
    } 
}

/********************************************************
 * The real annealing algorithm
 *********************************************************/
void SimAnneal::_anneal()
{
  _movables.clear();
  _oldPlace.clear();
  _newPlace.clear();
  _hpwl=_rb.evalHPWL(true);
  _overlap=_rb.calcOverlap();
  _oldCost = _cost();
  _lambda = 1;

  cout << "Beginning temperature decline..." << endl;
  // for a certain number of iterations
  cout << "Temp\tNAR\tAR\tCost(HPWL+OV)"<<endl; 
  while (_curTemp > _stopTemp)
    {
      _negAcceptCount = _posAcceptCount = 0;
      _totaloverlap = _totalhpwl=0;
      
      cout << "[ ";
      // for the maximum number of configs at this iteration
      _itCount = 0;
      double avgCost=0,totalCost=0;

      while (_itCount++ <= _maxIter)
	{
	  // keep a running average of hpwl and overlap to scale them
	  _totalhpwl += _hpwl;
	  _totaloverlap += _overlap;
	  if (!(_itCount%(_maxIter/100)))
	    {
	      cout << ".";
	      cout.flush();
	    }
	  if (!(_itCount%(_maxIter/20)))
	    {
	      _avghpwl=_totalhpwl/_itCount;
	      _avgoverlap=_totaloverlap/_itCount;
	      // overlap should never be scaled DOWN
	      if (_avgoverlap==0) 
		_lambda=1;//_hpwl/rb.getHGraph().getNumEdges();
	      else
		_lambda=(_avghpwl>_avgoverlap)?_avghpwl/_avgoverlap:1;
	      //_lambda = 1;
	      _oldCost = _hpwl + _lambda*_overlap;
	    }
	  
	  _totalCount++;
	  
	  _generate();
	  
	  _hpwl -= _rb.calcInstHPWL(_movables); 
	  _overlap -= _rb.calcInstOverlap(_movables);
	  _rb.updateCells(_movables, _newPlace); 
	  _newCost = _cost();
	  
	  // calc averages
	  totalCost+=_oldCost;
	  avgCost=totalCost/_itCount;
	  
	  if (!_accept(_newCost, _oldCost, _curTemp))
	    {
	      // this undoes the moves
	      _hpwl -= _rb.calcInstHPWL(_movables); 
	      _overlap -= _rb.calcInstOverlap(_movables);
	      _rb.updateCells(_movables, _oldPlace);
	      _oldCost = _cost();
	    }
	  else
	    {
	      _oldCost = _newCost;
	    }
	}
      
      if (_itCount<_maxIter)
	cout << " X" << endl;
      else
	cout << " ]" << endl;
      
      double negAcceptRate = _negAcceptCount/_maxIter;
      double acceptRate = (_posAcceptCount+_negAcceptCount)/_maxIter;
      cout << setw(10) << _curTemp 
	   << "\t" << setw(10) << negAcceptRate 
	   << "\t" << setw(10) << acceptRate  
	   << "\t" << setw(10) << _oldCost 
	   << "(" << _hpwl << "+" << _lambda << "*" << _overlap <<")" 
	   << endl;
      _update(_curTemp);
    }
}

//SimAnneal::~SimAnneal() { }

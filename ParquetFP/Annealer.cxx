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









#include "FPcommon.h"
#include "PlToSP.h"
#include "Annealer.h"
#include "CommandLine.h"
using namespace parquetfp;

#define VANILLA

Annealer::Annealer(Command_Line* params, DB* db) : 
          _db(db), _params(params)
{
//set the random seed for each invokation of the Annealer
  int rseed;
  if(_params->getSeed)
    rseed = int(time((time_t *)NULL));
  else
    rseed = _params->seed;

  srand(rseed);        //seed for rand function
  srand48(rseed);      //seed for random_shuffle function
  cout<<"The random seed for this run is: "<<rseed<<endl;
								  
  char* baseName = _params->inFileName;
  strcpy(_baseFileName, baseName);

  Nodes * nodes = _db->getNodes();
  vector<double> heights = nodes->getNodeHeights();
  vector<double> widths = nodes->getNodeWidths();
  vector<double> xloc = nodes->getXLocs();
  vector<double> yloc = nodes->getYLocs();

  _sp = new SeqPair(_db->getNumNodes());
  _spEval = new SPeval(heights, widths, _params->useFastSP);
  _analSolve = new AnalSolve(_params, _db);
}

Annealer::~Annealer()
{
 if(_sp)  delete _sp;
 if(_spEval) delete _spEval;
 if(_analSolve) delete _analSolve;
}

void Annealer::solveQP()  //get a Quadratic minimum soln and update db
{
 _analSolve->solveSOR();
 _db->updatePlacement(_analSolve->getXLocs(), _analSolve->getYLocs());
}

void Annealer::takeSPfromDB()  //converts the present placement to a sequence pair
{
 vector<double> heights = _db->getNodeHeights();
 vector<double> widths = _db->getNodeWidths();
 vector<double> xloc = _db->getXLocs();
 vector<double> yloc = _db->getYLocs();

 PL2SP_ALGO useAlgo = TCG_ALGO;
 Pl2SP pl2sp(xloc, yloc, widths, heights, useAlgo);
 _sp->putX(pl2sp.getXSP());
 _sp->putY(pl2sp.getYSP());
 _spEval->changeWidths(widths);
 _spEval->changeHeights(heights);
}

void Annealer::go()
{
  Timer T;
  T.stop();
  T.start(0.0);
  anneal();
  T.stop();
  double timeReqd = T.getUserTime();
  double realTime = T.getRealTime();

  _spEval->evaluate(_sp->getX(), _sp->getY());
  _db->updatePlacement(_spEval->xloc, _spEval->yloc);

  if(_params->softBlocks)
    {
      //cout<<"Before area is "<<_spEval->xSize*_spEval->ySize<<endl;
      packSoftBlocks(100);
      _spEval->evaluate(_sp->getX(), _sp->getY());
      _db->updatePlacement(_spEval->xloc, _spEval->yloc);
    }
  if(0/*_params->minWL*/)
    {
      Point offset = _analSolve->getDesignOptLoc();
      cout<<"offset.x "<<offset.x<<" offset.y "<<offset.y<<endl;

      double initHPWL = _db->evalHPWL();
      cout<<"initHPWL "<<initHPWL<<endl;
      _db->shiftDesign(offset);
      double afterHPWL = _db->evalHPWL();
      cout<<"afterHPWL "<<afterHPWL<<endl;

      if(afterHPWL > initHPWL)
	{
	  offset.x *= -1;
	  offset.y *= -1;
	  _db->shiftDesign(offset);
	}
    }
  double blocksArea = _db->getNodesArea();
  double currArea =  _spEval->xSize*_spEval->ySize;
  double currAR = _spEval->xSize/_spEval->ySize;
  double whiteSpace = 100*(currArea - blocksArea)/currArea;
  double HPWL = _db->evalHPWL();
  
  _spEval->evalSlacks(_sp->getX(), _sp->getY());
  _db->updateSlacks(_spEval->xSlacks, _spEval->ySlacks);

  cout<<"realTime:"<<realTime<<"\tuserTime:"<<timeReqd<<endl;
  cout<<"Final Area: "<<currArea<<" WhiteSpace "<<whiteSpace<<"%"<<" AR "
      <<currAR<<" HPWL "<<HPWL<<endl;
}

void Annealer::updatePlacement(void)
{
  _db->updatePlacement(_spEval->xloc, _spEval->yloc);
}

void Annealer::eval(void)
{
  _spEval->evaluate(_sp->getX(), _sp->getY());
  _db->updatePlacement(_spEval->xloc, _spEval->yloc);
}

void Annealer::evalSlacks(void)
{
  _spEval->evalSlacks(_sp->getX(), _sp->getY());
  _db->updateSlacks(_spEval->xSlacks, _spEval->ySlacks);
}

void Annealer::evalCompact(bool whichDir)
{
  _spEval->evaluateCompact(_sp->getX(), _sp->getY(), whichDir);
  _db->updatePlacement(_spEval->xloc, _spEval->yloc);
}

void Annealer::anneal()
{ 
  bool budgetTime = _params->budgetTime;
  double seconds = _params->seconds;
  
  vector<unsigned> tempX, tempY;

  bool minWL = _params->minWL;
  double wireWeight = _params->wireWeight;
  double areaWeight = _params->areaWeight;
  double ARWeight = 1 - areaWeight - wireWeight;
  if(ARWeight < 0)
    ARWeight = 0;

  double tempArea = 0;
  double tempHeight = 0;
  double tempWidth = 0;
  double tempAR=0;
  //double bestHPWL = 1e100;
  //double bestArea = 1e100;
  double currArea = 1e100;
  double deltaArea = 0, deltaAR, delta;
  double blocksArea = _db->getNodesArea();
  double currHeight = 0;
  double currWidth = 0;
  double currTime = _params->startTime;
  double size = _sp->getX().size();
  double alpha = 0, r;
  double reqdAR = _params->reqdAR, currAR;
  int move = 0, count=0, ran;
  unsigned timeChangeCtr = 0;
  unsigned indexOrient = 0, moveSelect, iter, masterMoveSel=0; 
  ORIENT newOrient = N; 
  ORIENT oldOrient = N;
  double oldWidth=0, oldHeight=0, newWidth=0, newHeight=0;
  bool moveAccepted=0;
 
  double reqdArea = (1+(_params->maxWS/100.0))*blocksArea;
  double reqdWidth = sqrt(reqdArea*reqdAR);
  double reqdHeight = reqdWidth/reqdAR;

  bool brokeFromLoop;

  double tempHPWL=1e100;
  double currHPWL=1e100;
  double deltaHPWL=0;
  
  Timer looptm;
  looptm.stop();
  double unit=0, total=seconds, percent=1;
  unsigned moves=2000;

  while(currTime>_params->timeCool || budgetTime)
    {
      brokeFromLoop = 0;
      iter=0;
      do
	{
	  if (budgetTime)
	  {
	    if (count==0)
	    {
		    looptm.start(0.0);
	    }
	    else if (count==1000)
	    {  
		    looptm.stop();
		    unit=looptm.getUserTime()/1000;
		    if(unit == 0)
		     {
		       unit = 10e-6;
		     }
		    seconds-=looptm.getUserTime();
		    cout<<int(seconds/unit)<<" moves left with "<<
			unit*1000000<<" micro seconds per move.\n";
		    moves=unsigned(seconds/unit/125);//moves every .08 degree
	    }
	    else if (count > 1000)
	    {
		    seconds-=unit;
		    if (seconds <= 0)
			    return;
	    }
          }
	  
	  ++count; 
	  ++iter;

	  tempX = _sp->getX();
	  tempY = _sp->getY();

	  //select the types of moves here
	  if(_params->softBlocks && currTime < 50)
	    masterMoveSel = rand()%500;
	  moveSelect = rand()%1000;

	  currAR = currWidth/currHeight;
	  move = -1;
	  
#ifdef VANILLA
	  if(_params->softBlocks && masterMoveSel == 1)
	    move = packSoftBlocks(2);
	  else if(_params->softBlocks && masterMoveSel > 950)
	    move = makeSoftBlMove(tempX, tempY, indexOrient, newWidth, 
				  newHeight);
	  else if(((reqdAR-currAR)/reqdAR > 0.00005 || 
		   (currAR-reqdAR)/reqdAR > 0.00005) && 
		  (timeChangeCtr%4)==0 && reqdAR != -9999)
	    move = makeARMove(tempX, tempY, currAR);
	  else if(moveSelect < 150 && minWL == 1 && reqdAR != -9999)
	    move = makeARWLMove(tempX, tempY, currAR);
	  else if(moveSelect < 300 && minWL == 1)
	    move = makeHPWLMove(tempX, tempY);
	  else if(moveSelect < 500)
	    move = makeMoveOrient(indexOrient, oldOrient, newOrient);
	  else
	    move = makeMove(tempX, tempY);
#else
	  if(_params->softBlocks && masterMoveSel == 1)
	    move = packSoftBlocks(2);
	  else if(_params->softBlocks && masterMoveSel > 950)
	    move = makeSoftBlMove(tempX, tempY, indexOrient, newWidth, 
				  newHeight);
	  else if(((reqdAR-currAR)/reqdAR > 0.00007 || 
		   (currAR-reqdAR)/reqdAR >0.00005) 
	     && (timeChangeCtr%2)==0 && reqdAR != -9999)
	    move = makeARMove(tempX, tempY, currAR);
	  else if(moveSelect < 150 && minWL == 1 && reqdAR != -9999)
	    move = makeARWLMove(tempX, tempY, currAR);
	  else if(moveSelect < 300 && minWL == 1)
	    move = makeHPWLMove(tempX, tempY);
	  else if(moveSelect < 400)
	    move = makeMoveSlacks(tempX, tempY);
	  else if(moveSelect < 600)
	    move = makeMoveSlacksOrient(tempX, tempY, indexOrient, oldOrient, 
	    	                        newOrient);
	  else if(moveSelect < 700)
	    move = makeMoveOrient(indexOrient, oldOrient, newOrient);
	  else
	    move = makeMove(tempX, tempY);
#endif
	  //for orientation moves
	  if(move == 10)
	    {
	      if(oldOrient%2 != newOrient%2)
	        _spEval->changeOrient(indexOrient);

	      if(minWL)
		_db->getNodes()->changeOrient(indexOrient, newOrient,
					      *(_db->getNets())); 
	    }
	  if(move == 11)  //softBlocks move
	    {
	      oldHeight = _db->getNodes()->getNodeHeight(indexOrient);
	      oldWidth = _db->getNodes()->getNodeWidth(indexOrient);
	      _spEval->changeNodeWidth(indexOrient, newWidth);
	      _spEval->changeNodeHeight(indexOrient, newHeight);
	    }

	  _spEval->evaluate(tempX, tempY);
	  tempHeight = _spEval->ySize;
	  tempWidth = _spEval->xSize;
	  tempArea = tempHeight*tempWidth;
	  tempAR = tempWidth/tempHeight;

	  /*area objective*/
	  
	  if(currTime>30)
	    deltaArea=((tempArea-currArea)*1.2*_params->timeInit)/blocksArea;
	  else
	    deltaArea=((tempArea-currArea)*1.5*_params->timeInit)/blocksArea;
	    
	  /* x-viol + y-viol objective
	  deltaArea=0.5*1.3*_params->timeInit*((tempHeight-currHeight)/(reqdHeight)+(tempWidth-currWidth)/(reqdWidth));
	  */

	  /*max(x-viol, y-viol) objective
	   if((tempHeight-currHeight)>(tempWidth-currWidth))
	     deltaArea=((tempHeight-currHeight)*1.3*_params->timeInit)/(reqdHeight);
	   else
	     deltaArea=((tempWidth-currWidth)*1.3*_params->timeInit)/(reqdWidth);
	  */
	  
	  delta = deltaArea;

	  if(minWL)
	    {
	      _db->updatePlacement(_spEval->xloc, _spEval->yloc);
	      tempHPWL = _db->evalHPWL();
              if(currTime>30)
	        deltaHPWL=((tempHPWL-currHPWL)*1.2*_params->timeInit)/currHPWL;
  	      else
	        deltaHPWL=((tempHPWL-currHPWL)*1.5*_params->timeInit)/currHPWL;
	    }

	  if(reqdAR != -9999)
	    {
	      deltaAR = ((tempAR - reqdAR)*(tempAR - reqdAR) - (currAR - reqdAR)*(currAR - reqdAR))*1.2*_params->timeInit;
	      //deltaAR = 0;
	      if(minWL)
		delta = areaWeight*deltaArea + wireWeight*deltaHPWL +
		  ARWeight*deltaAR;
	      else
		delta = (areaWeight+wireWeight/2.0)*deltaArea + 
		  (ARWeight+wireWeight/2.0)*deltaAR;

	    }
	  else if(minWL)
	    {
	      delta = (areaWeight+ARWeight/2.0)*deltaArea + 
		(wireWeight+ARWeight)*deltaHPWL;
	    }
	  else
	    delta = deltaArea;

	  //delta = deltaArea;

	  if(delta<0 || move == -1)
	    moveAccepted = 1;
	  else if(currTime>_params->timeCool) 
	                                  //become greedy below time>timeCool
	    {
	      ran=rand()%10000;
	      r=double(ran)/9999;
	      if(r<exp(-1*delta/currTime))
		moveAccepted = 1;
	      else
		moveAccepted = 0;
	    }
	  else
	    moveAccepted = 0;
	  
	  if(moveAccepted)
	    {
	      currHeight=tempHeight;
	      currWidth=tempWidth;
	      currArea=tempArea;
	      currHPWL=tempHPWL;
	      _sp->putX(tempX);
	      _sp->putY(tempY);

	      if(move == 10)
		_db->getNodes()->changeOrient(indexOrient, newOrient,
					      *(_db->getNets())); 
	      else if(move == 11)
		{
		  _db->getNodes()->putNodeWidth(indexOrient, newWidth);
		  _db->getNodes()->putNodeHeight(indexOrient, newHeight);
		}
	    }
	  else
	    {
	      if(move == 10) //if move not accepted then put back orient
		{
	          if(oldOrient%2 != newOrient%2)
		    _spEval->changeOrient(indexOrient);

		  if(minWL)
		    _db->getNodes()->changeOrient(indexOrient, oldOrient,
						  *(_db->getNets())); 
		}
	      else if(move == 11) //if move not accepted then put back old HW's
		{
		  _spEval->changeNodeWidth(indexOrient, oldWidth);
		  _spEval->changeNodeHeight(indexOrient, oldHeight);
		}
	    }

	  //cout<<"currTime "<<currTime<<" currHPWL "<<currHPWL<<" deltaHPWL "<<deltaHPWL<<" deltaArea "<<deltaArea<<" delta "<<delta<<" param "<<exp(-1*delta/currTime)<<endl;

	  if(minWL/* && _params->startTime > 100*/)//for lowT anneal don't have this condition
	    {
	      if(currArea <= reqdArea && currHeight <= reqdHeight && 
		 currWidth <= reqdWidth && reqdAR != -9999 && currTime < 5)
		return;
	    }
	  else
	    {
	      if(currArea <= reqdArea && currHeight <= reqdHeight && 
		 currWidth <= reqdWidth && reqdAR != -9999)
		return;
	    }
	  
           if (iter>=moves)
		    break;

	}
      while(iter<size*1.5 || budgetTime);
      

      /*
      if(reqdAR != -9999)
	{
	  if(minWL)
	    if(currArea <= reqdArea && currHeight <= reqdHeight && 
	       currWidth <= reqdWidth && bestHPWL > currHPWL)
	      {
		_db->saveInBestCopy();
		bestHPWL = currHPWL;
	      }
	  else
	    {
	      if(currArea <= reqdArea && currHeight <= reqdHeight && 
		 currWidth <= reqdWidth && bestArea > currArea)
		{
		  _db->saveInBestCopy();
		  bestArea = currArea;
		}
	    }
	}
      */

      ++timeChangeCtr;

      if (budgetTime)
      {
	percent=seconds/total;
	
        if(percent<.66666 && percent>.33333)
	  alpha=0.9;
        else if(percent<.33333 && percent>.16666)
	  alpha=0.95;
        else if(percent<.16666 && percent>.06666)
	  alpha=0.96;
        else if(percent<.06666 && percent>.00333)
	  alpha=0.8;
        else if(percent<.00333 && percent>.00003)
	  alpha=0.98;
        else
	  alpha=0.85;
      }
      else
      {
        if(currTime<2000 && currTime>1000)
	  alpha=0.9;
        else if(currTime<1000 && currTime>500)
	  alpha=0.95;
        else if(currTime<500 && currTime>200)
	  alpha=0.96;
        else if(currTime<200 && currTime>10)
	  alpha=0.8;
        else if(currTime<15 && currTime>0.1)
	  alpha=0.98;
        else
	  alpha=0.85;
      }
      currTime = alpha*currTime;

      if(brokeFromLoop == 1)
	break;

      //cout<<currTime<<endl;
    }
}




void Annealer::sortSlacks(vector<Point>& sortedXSlacks, 
			  vector<Point>& sortedYSlacks)
{
  sortedXSlacks.erase(sortedXSlacks.begin(), sortedXSlacks.end());
  sortedYSlacks.erase(sortedYSlacks.begin(), sortedYSlacks.end());
  Point tempPoint;

  for(unsigned i=0;i<_spEval->xSlacks.size();++i)
    {
      tempPoint.x = _spEval->xSlacks[i];
      tempPoint.y = i;
      sortedXSlacks.push_back(tempPoint);
      tempPoint.x = _spEval->ySlacks[i];
      tempPoint.y = i;
      sortedYSlacks.push_back(tempPoint);
    }
  std::sort(sortedXSlacks.begin(),sortedXSlacks.end(),sort_slacks());
  std::sort(sortedYSlacks.begin(),sortedYSlacks.end(),sort_slacks());
}


int Annealer::makeMove(vector<unsigned>& A, vector<unsigned>& B)
{
  int elem1,elem2,i=0;
  int size = A.size();
  unsigned temporary;
  int moverand = rand()%1000;
  int movedir;
  elem1=rand()%size;
  elem2=rand()%size;
  std::vector<unsigned>::iterator itb;

  moverand=rand()%600;

  if(moverand<75)
    {
      temporary=A[elem1];
      A[elem1]=A[elem2];
      A[elem2]=temporary;
      return 1;
    }
  else if(moverand>75 && moverand<150)
    {
      temporary=B[elem1];
      B[elem1]=B[elem2];
      B[elem2]=temporary;
      return 2;
    }
  else if(moverand>150 && moverand<200)
    {
      temporary=A[elem1];
      A[elem1]=A[elem2];
      A[elem2]=temporary;
      elem1=rand()%size;
      elem2=rand()%size;
      temporary=B[elem1];
      B[elem1]=B[elem2];
      B[elem2]=temporary;
      return 3;
    }
  else if(moverand>200 && moverand<400)
    {
      movedir=rand()%100;
      if(movedir<50)
	{
	  i=0;
	  for(itb=A.begin();i!=elem1;++itb,++i)
	    { }
	  temporary=*itb;
	  A.erase(itb);
	  i=0;
	  for(itb=A.begin();i!=elem2;++itb,++i)
	    { }
	  A.insert(itb,1,temporary);
	}
      else
	{
	  i=0;
	  for(itb=B.begin();i!=elem1;++itb,++i)
	    { }
	  temporary=*itb;
	  B.erase(itb);
	  i=0;
	  for(itb=B.begin();i!=elem2;++itb,++i)
	    {
	    }
	  B.insert(itb,1,temporary);	
	}
      return 4;
    }
  else if(moverand>400 && moverand<600)
    {
      elem2=rand()%(int(ceil(size/4.0)));
      movedir=rand()%100;
      if(movedir<50)
	{
	  if((elem1-elem2)<0)
	    elem2=elem1+elem2;
	  else
	    elem2=elem1-elem2;
	}
      else
	{
	  if((elem1+elem2)>size-1)
	    elem2=elem1-elem2;
	  else
	    elem2=elem1+elem2;
	}
      if(moverand<500)
	{
	  temporary=A[elem1];
	  A[elem1]=A[elem2];
	  A[elem2]=temporary;
	}
      else
	{
	  temporary=B[elem1];
	  B[elem1]=B[elem2];
	  B[elem2]=temporary;
	}
      return 5;
    }
  return -1;
}

int Annealer::makeMoveOrient(unsigned& index, ORIENT& oldOrient,
			     ORIENT& newOrient)
{
  index = rand()%_sp->getSize();
  oldOrient = _db->getNodes()->getNode(index).getOrient();
  newOrient = ORIENT(rand()%8);   //one of the 8 orientations
  
  return 10;
}

int Annealer::makeMoveSlacksOrient(vector<unsigned>& A, vector<unsigned>& B,
			  unsigned& index, ORIENT& oldOrient, ORIENT& newOrient)
{
  unsigned size = A.size();
  _spEval->evalSlacks(A, B);

  sortSlacks(sortedXSlacks, sortedYSlacks);
  unsigned elem1=0;
  unsigned movedir = rand()%100;
  index = elem1;
  if(movedir<50)
    {
      for(unsigned i=0;i<size;++i)
	{
	  elem1 = unsigned(sortedXSlacks[i].y);
	  if(_db->getNodes()->getNodeWidth(elem1) >
	     _db->getNodes()->getNodeHeight(elem1))
	    break;
	}
      index = elem1;
    }
  else
    {
      for(unsigned i=0;i<size;++i)
	{
	  elem1 = unsigned(sortedYSlacks[i].y);
	  if(_db->getNodes()->getNodeHeight(elem1) >
	     _db->getNodes()->getNodeWidth(elem1))
	    break;
	}
      index = elem1;
    }

  oldOrient = _db->getNodes()->getNode(index).getOrient();
  unsigned r = rand()%4;
  
  if(oldOrient%2 == 0)
    newOrient = ORIENT(2*r+1);
  else
    newOrient = ORIENT(2*r);
    
  return 10;
}


int Annealer::makeMoveSlacks(vector<unsigned>& A, vector<unsigned>& B)
{
  unsigned size = A.size();
  _spEval->evalSlacks(A, B);

  sortSlacks(sortedXSlacks, sortedYSlacks);

  std::vector<unsigned>::iterator itb;
  unsigned temporary;
  unsigned elem1, elem2;
  unsigned movedir = rand()%100;
  unsigned choseElem1=rand()%int(ceil(size/5.0));
  unsigned choseElem2=size-rand()%int(ceil(size/5.0))-1;
  if(movedir<50)
    {
      elem1 = unsigned(sortedXSlacks[choseElem1].y);
      for(itb=A.begin();(*itb)!=elem1;++itb)
	{ }
      temporary=*itb;
      A.erase(itb);
      
      elem2=unsigned(sortedXSlacks[choseElem2].y);
      for(itb=A.begin();(*itb)!=elem2;++itb)
	{ }
      A.insert(itb,1,temporary);
      //for B seqPair
      for(itb=B.begin();(*itb)!=elem1;++itb)
	{ }
      temporary=*itb;
      B.erase(itb);
      
      for(itb=B.begin();(*itb)!=elem2;++itb)
	{ }
      B.insert(itb,1,temporary);
    }
  else
    {
      elem1 = unsigned(sortedYSlacks[choseElem1].y);
      for(itb=A.begin();(*itb)!=elem1;++itb)
	{ }
      temporary=*itb;
      A.erase(itb);
      
      elem2=unsigned(sortedYSlacks[choseElem2].y);
      for(itb=A.begin();(*itb)!=elem2;++itb)
	{ }
      A.insert(itb,1,temporary);
      
      //for B seqPair
      for(itb=B.begin();(*itb)!=elem1;++itb)
	{ }
      temporary=*itb;
      B.erase(itb);
      
      for(itb=B.begin();(*itb)!=elem2;++itb)
	{ }
      B.insert(itb,1,temporary);
    }
  return 6;
}

int Annealer::makeARMove(vector<unsigned>& A, vector<unsigned>& B, 
			 double currAR)
{
  unsigned size = A.size();
  unsigned direction, temp;
  _spEval->evalSlacks(A, B);

  sortSlacks(sortedXSlacks, sortedYSlacks);

  std::vector<unsigned>::iterator itb;
  unsigned chooseElem1=0;
  unsigned elem1=0;
  unsigned chooseElem2=0;
  unsigned elem2=0;
  unsigned temporary=0;

  bool HVDir=0;
  if(currAR > 1.05*_params->reqdAR)//width is greater than reqd,(a little bias 
                               //to increase width for better performance
    HVDir = 0;  // width needs to reduce
  else
    HVDir = 1;  // height needs to reduce

  chooseElem1=rand()%int(ceil(size/5.0));
  chooseElem2=size-rand()%int(ceil(size/5.0))-1;

  if(HVDir == 0)    //horizontal
    { 
      elem1 = unsigned(sortedXSlacks[chooseElem1].y); 
      elem2 = unsigned(sortedXSlacks[chooseElem2].y); 
    }
  else //vertical
    {
      elem1 = unsigned(sortedYSlacks[chooseElem1].y);
      elem2 = unsigned(sortedYSlacks[chooseElem2].y);
   }

  temp = rand() % 2;
  if(HVDir == 0)
    {
      if(temp == 0)
	direction = 0;  //left
      else
	direction = 3;  //right
    }
  else
    {
      if(temp == 0)
	direction = 1;  //top
      else
	direction = 2;  //bottom
    }

  for(itb=A.begin();(*itb)!=unsigned(elem1);++itb)
    { }
  temporary=*itb;
  A.erase(itb);
      
  for(itb=A.begin();(*itb)!=unsigned(elem2);++itb)
    { }
  switch(direction)
    {
    case 0:   //left
    case 1:   //top
      break;
    case 2:   //bottom
    case 3:   //right
      ++itb;
      break;
    }
  A.insert(itb,1,temporary);

  //for B seqPair
  for(itb=B.begin();(*itb)!=unsigned(elem1);++itb)
    { }
  temporary=*itb;
  B.erase(itb);
  
  for(itb=B.begin();(*itb)!=unsigned(elem2);++itb)
    { }
  switch(direction)
    {
    case 0:   //left
    case 2:   //bottom
      break;
    case 1:   //top
    case 3:   //right
      ++itb;
      break;
    }

  B.insert(itb,1,temporary);  

return 7;
}

int Annealer::makeSoftBlMove(vector<unsigned>& A, vector<unsigned>& B,
                         unsigned &index, double &newWidth, double &newHeight)
{
  unsigned size = A.size();
  _spEval->evalSlacks(A, B);

  sortSlacks(sortedXSlacks, sortedYSlacks);
  unsigned elem1=0;
  double minAR, maxAR, currAR;  
  unsigned movedir = rand()%100;
  double maxWidth, maxHeight;
  index = elem1;
  bool brokeFromLoop=0;

  if(movedir<50)
    {
      for(unsigned i=0;i<size;++i)
	{
	  elem1 = unsigned(sortedXSlacks[i].y);
	  Node& node = _db->getNodes()->getNode(elem1);
	  minAR = node.getminAR();
	  maxAR = node.getmaxAR();
          currAR = node.getWidth()/node.getHeight();
  
	  if(_spEval->ySlacks[elem1] > 0 && minAR != maxAR && currAR > minAR)
	   {
	     brokeFromLoop = 1;
	     break;
	   }
	}
      index = elem1;
    }
  
  if(movedir >= 50 || brokeFromLoop == 0)
    {
      for(unsigned i=0;i<size;++i)
	{
	  elem1 = unsigned(sortedYSlacks[i].y);
	  Node& node = _db->getNodes()->getNode(elem1);
	  minAR = node.getminAR();
	  maxAR = node.getmaxAR();
	  currAR = node.getWidth()/node.getHeight();
	  
	  if(_spEval->xSlacks[elem1] > 0 && minAR != maxAR && currAR < maxAR)
	    break;
	}
      index = elem1;
    }

  Node& node = _db->getNodes()->getNode(index);
  double origHeight = node.getHeight();
  double origWidth = node.getWidth();
  currAR = origWidth/origHeight;
  double area = node.getArea();

  if(node.getminAR() > node.getmaxAR())
   {
    minAR = node.getmaxAR();
    maxAR = node.getminAR();
   }
  else
   {
    minAR = node.getminAR();
    maxAR = node.getmaxAR();
   }

  if(node.getOrient()%2 == 0)
   {
    maxHeight = sqrt(area/minAR);
    maxWidth = sqrt(area*maxAR);
   }
  else
   {
    maxHeight = sqrt(area*maxAR);
    maxWidth = sqrt(area/minAR);
   }

  double absSlackX = _spEval->xSlacks[index]*_spEval->xSize/100;
  double absSlackY = _spEval->ySlacks[index]*_spEval->ySize/100;

  newHeight = origHeight;
  newWidth = origWidth;

  if(absSlackX > absSlackY)  //need to elongate in X direction
    {
      if(currAR < maxAR)     //need to satisfy this constraint
	{
	  newWidth = origWidth + absSlackX;
	  if(newWidth > maxWidth)
	    newWidth = maxWidth;
	  newHeight = area/newWidth;
	}
    }
  else                       //need to elongate in Y direction
    {
      if(currAR > minAR)     //need to satisfy this constraint
	{
	  newHeight = origHeight + absSlackY;
	  if(newHeight > maxHeight)
	    newHeight = maxHeight;
	  newWidth = area/newHeight;
	}
    }

  if(minAR == maxAR)
    {
      newHeight = origHeight;
      newWidth = origWidth;
    }

  return 11;
}



int Annealer::makeIndexSoftBlMove(vector<unsigned>& A, vector<unsigned>& B,
                         unsigned index, double &newWidth, double &newHeight)
{
  _spEval->evalSlacks(A, B);

  //sortSlacks(sortedXSlacks, sortedYSlacks);
  double minAR, maxAR, currAR;  
  double maxWidth, maxHeight;

  Node& node = _db->getNodes()->getNode(index);
  double origHeight = node.getHeight();
  double origWidth = node.getWidth();
  currAR = origWidth/origHeight;
  double area = node.getArea();
  
  if(node.getminAR() > node.getmaxAR())
   {
     minAR = node.getmaxAR();
     maxAR = node.getminAR();
   }
  else
   {
     minAR = node.getminAR();
     maxAR = node.getmaxAR();
   }
  
  if(node.getOrient()%2 == 0)  
   {
     maxHeight = sqrt(area/minAR);
     maxWidth = sqrt(area*maxAR);
   }
  else
   {
     maxHeight = sqrt(area*maxAR);
     maxWidth = sqrt(area/minAR);
   }

  double absSlackX = _spEval->xSlacks[index]*_spEval->xSize/100;
  double absSlackY = _spEval->ySlacks[index]*_spEval->ySize/100;

  newHeight = origHeight;
  newWidth = origWidth;

  if(absSlackX > absSlackY)  //need to elongate in X direction
    {
      newWidth = origWidth + absSlackX;
      if(newWidth > maxWidth)
	newWidth = maxWidth;
      newHeight = area/newWidth;

    }
  else                       //need to elongate in Y direction
    {
      newHeight = origHeight + absSlackY;
      if(newHeight > maxHeight)
	newHeight = maxHeight;
      newWidth = area/newHeight;
    }

  if(fabs(minAR-maxAR) < 0.0000001)
    {
      newHeight = origHeight;
      newWidth = origWidth;
    }

  return 11;
}

int Annealer::packSoftBlocks(unsigned numIter)
{
  unsigned index;
  double origHeight, origWidth;
  double newHeight, newWidth;
  double oldArea;
  double newArea;
  double change=-1;
  vector<unsigned>& XSP = _sp->getX();
  vector<unsigned>& YSP = _sp->getY();

  unsigned size = XSP.size();
  
  _spEval->evaluate(XSP, YSP);
  oldArea = _spEval->ySize * _spEval->xSize;
  unsigned iter = 0;
  while(iter < numIter)
   {
     ++iter;
     
     _spEval->evalSlacks(XSP, YSP);
     sortSlacks(sortedXSlacks, sortedYSlacks);
     
     for(unsigned i = 0; i<size; ++i)
     {
       if(numIter%2 == 0)
        index = unsigned(sortedXSlacks[i].y);
       else
        index = unsigned(sortedYSlacks[i].y);
      
       origHeight = _db->getNodes()->getNodeHeight(index);
       origWidth = _db->getNodes()->getNodeWidth(index);
	      
       makeIndexSoftBlMove(XSP, YSP, index, newWidth, newHeight);
       _spEval->changeNodeWidth(index, newWidth);
       _spEval->changeNodeHeight(index, newHeight);
       _spEval->evaluate(XSP, YSP);
       
       newArea = _spEval->ySize * _spEval->xSize;
       change = newArea-oldArea;
       if(change > 1)  //do not accept
        {
	  _spEval->changeNodeWidth(index, origWidth);
	  _spEval->changeNodeHeight(index, origHeight);
	}
       else
        {
         _db->getNodes()->putNodeWidth(index, newWidth);
         _db->getNodes()->putNodeHeight(index, newHeight);
         oldArea = newArea;
	}
     }
   }
 return -1;
}

int Annealer::makeHPWLMove(vector<unsigned>& A, vector<unsigned>& B)
{
  int i, j, temp, direction;
  int elem1,elem2;
  int size = A.size();
  elem1=rand()%size;
  int searchRadiusNum = int(ceil(size/5.0));
  double searchRadius;
  double distance;
  double xDist;
  double yDist;

  std::vector<unsigned>::iterator itb;
  int temporary;

  vector<bool> seenBlocks;
  seenBlocks.resize(size);

  double unitRadiusSize;

  vector<int> searchBlocks;

  vector<double>& xloc = _spEval->xloc;
  vector<double>& yloc = _spEval->yloc;

  
  _spEval->evaluate(A, B); //evaluate SP to determine locs, may be redundant
                           //can use db locs instead

  if(_spEval->xSize > _spEval->ySize)
    unitRadiusSize = _spEval->xSize;
  else
    unitRadiusSize = _spEval->ySize;

  unitRadiusSize /= sqrt(double(size));

  Point idealLoc = _analSolve->getOptLoc(elem1, _spEval->xloc, _spEval->yloc);
                           //get optimum location of elem1


  fill(seenBlocks.begin(), seenBlocks.end(), 0);
  searchRadius = 0;
  for(i=0; i<searchRadiusNum; ++i)
    {
      searchRadius += unitRadiusSize;
      for(j = 0; j<size; ++j)
	{
	  if(seenBlocks[j] == 0 && j != elem1)
	    {
	      xDist = xloc[j]-idealLoc.x;
	      yDist = yloc[j]-idealLoc.y;
	      distance = sqrt(xDist*xDist + yDist*yDist);
	      if(distance < searchRadius)
		{
		  seenBlocks[j] = 1;
		  searchBlocks.push_back(j);
		  if(searchBlocks.size() >= unsigned(searchRadiusNum))
		    break;
		  continue;
		}
	    }
	}
      if(searchBlocks.size() >= unsigned(searchRadiusNum))
	break;
    }
  if(searchBlocks.size() != 0)
    {
      temp = rand() % searchBlocks.size();
      elem2 = searchBlocks[temp];
    }
  else
    {
      do
	elem2 = rand() % size;
      while(elem2 == elem1);
    }

  direction = rand() % 4;

  for(itb=A.begin();(*itb)!=unsigned(elem1);++itb)
    { }
  temporary=*itb;
  A.erase(itb);
      
  for(itb=A.begin();(*itb)!=unsigned(elem2);++itb)
    { }
  switch(direction)
    {
    case 0:   //left
    case 1:   //top
      break;
    case 2:   //bottom
    case 3:   //right
      ++itb;
      break;
    }
  A.insert(itb,1,temporary);

  //for B seqPair
  for(itb=B.begin();(*itb)!=unsigned(elem1);++itb)
    { }
  temporary=*itb;
  B.erase(itb);
  
  for(itb=B.begin();(*itb)!=unsigned(elem2);++itb)
    { }
  switch(direction)
    {
    case 0:   //left
    case 2:   //bottom
      break;
    case 1:   //top
    case 3:   //right
      ++itb;
      break;
    }

  B.insert(itb,1,temporary);  
  return 12;

  return -1;
}

int Annealer::makeARWLMove(vector<unsigned>& A, vector<unsigned>& B, 
			   double currAR)
{
  unsigned size = A.size();

  std::vector<unsigned>::iterator itb;
  unsigned chooseElem1=0;
  unsigned elem1=0;
  unsigned elem2=0;
  unsigned temporary=0;
  unsigned i, j, direction, temp;
  double maxSlack;
  unsigned searchRadiusNum = unsigned(ceil(size/5.0));
  double searchRadius;
  double distance;
  double xDist;
  double yDist;

  vector<bool> seenBlocks;
  seenBlocks.resize(size);
  double unitRadiusSize;

  vector<int> searchBlocks;

  vector<double>& xloc = _spEval->xloc;
  vector<double>& yloc = _spEval->yloc;

  _spEval->evalSlacks(A, B);

  sortSlacks(sortedXSlacks, sortedYSlacks);

  _spEval->evaluate(A, B); //evaluate SP to determine locs, may be redundant
                           //can use db locs instead

  bool HVDir=0;
  if(currAR > _params->reqdAR) //width is greater than reqd,(a little bias 
                                   //to increase width for better performance
    HVDir = 0;  // width needs to reduce
  else
    HVDir = 1;  // height needs to reduce

  chooseElem1=rand()%int(ceil(size/5.0));

  if(HVDir == 0)    //horizontal
      elem1 = unsigned(sortedXSlacks[chooseElem1].y); 
  else //vertical
      elem1 = unsigned(sortedYSlacks[chooseElem1].y);

  if(_spEval->xSize > _spEval->ySize)
    unitRadiusSize = _spEval->xSize;
  else
    unitRadiusSize = _spEval->ySize;
  unitRadiusSize /= sqrt(double(size));

  Point idealLoc = _analSolve->getOptLoc(elem1, xloc, yloc);
                           //get optimum location of elem1

  fill(seenBlocks.begin(), seenBlocks.end(), 0);
  searchRadius = 0;
  for(i=0; i<searchRadiusNum; ++i)
    {
      searchRadius += unitRadiusSize;
      for(j = 0; j<size; ++j)
	{
	  if(seenBlocks[j] == 0 && j != elem1)
	    {
	      xDist = xloc[j]-idealLoc.x;
	      yDist = yloc[j]-idealLoc.y;
	      distance = sqrt(xDist*xDist + yDist*yDist);
	      if(distance < searchRadius)
		{
		  seenBlocks[j] = 1;
		  searchBlocks.push_back(j);
		  if(searchBlocks.size() >= unsigned(searchRadiusNum))
		    break;
		  continue;
		}
	    }
	}
      if(searchBlocks.size() >= unsigned(searchRadiusNum))
	break;
    }
  
  maxSlack = -1e100;
  if(HVDir == 0)  //width reduction. find max xSlack block
    {
      for(i=0; i<searchBlocks.size(); ++i)
	{
	  if(_spEval->xSlacks[searchBlocks[i]] > maxSlack)
	    {
	      maxSlack = _spEval->xSlacks[searchBlocks[i]];
	      elem2 = searchBlocks[i];
	    }
	}
    }
  else  //height reduction. find max yslack block
    {
      for(i=0; i<searchBlocks.size(); ++i)
	{
	  if(_spEval->ySlacks[searchBlocks[i]] > maxSlack)
	    {
	      maxSlack = _spEval->ySlacks[searchBlocks[i]];
	      elem2 = searchBlocks[i];
	    }
	}
    }

  if(searchBlocks.size() == 0)
    do
      elem2 = rand() % size;
    while(elem2 == elem1);

  temp = rand() % 2;
  if(HVDir == 0)
    {
      if(temp == 0)
	direction = 0;  //left
      else
	direction = 3;  //right
    }
  else
    {
      if(temp == 0)
	direction = 1;  //top
      else
	direction = 2;  //bottom
    }

  for(itb=A.begin();(*itb)!=unsigned(elem1);++itb)
    { }
  temporary=*itb;
  A.erase(itb);
      
  for(itb=A.begin();(*itb)!=unsigned(elem2);++itb)
    { }
  switch(direction)
    {
    case 0:   //left
    case 1:   //top
      break;
    case 2:   //bottom
    case 3:   //right
      ++itb;
      break;
    }
  A.insert(itb,1,temporary);

  //for B seqPair
  for(itb=B.begin();(*itb)!=unsigned(elem1);++itb)
    { }
  temporary=*itb;
  B.erase(itb);
  
  for(itb=B.begin();(*itb)!=unsigned(elem2);++itb)
    { }
  switch(direction)
    {
    case 0:   //left
    case 2:   //bottom
      break;
    case 1:   //top
    case 3:   //right
      ++itb;
      break;
    }

  B.insert(itb,1,temporary);  

return 13;
}


double Annealer::getXSize(void)
{
 return _spEval->xSize;
}


double Annealer::getYSize(void)
{
 return _spEval->ySize;
}
 

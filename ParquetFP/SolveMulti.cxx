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
#include "Annealer.h"
#include "ClusterDB.h"
#include "CommandLine.h"
#include "SolveMulti.h"
using namespace parquetfp;

SolveMulti::SolveMulti(DB * db, Command_Line* params)
{
  _db = db;
  _params = params;
  _newDB = new DB();
}

SolveMulti::~SolveMulti()
{
  if(_newDB) delete _newDB;
}

void SolveMulti::go(void)
{
  ClusterDB multiCluster(_db, _params);

  Timer T;
  T.stop();
  double totalTime=0;
  T.start(0.0);

  if(_params->clusterPhysical)
    multiCluster.clusterMultiPhysical(_newDB);
  else
    multiCluster.clusterMulti(_newDB);

  T.stop();
  totalTime += T.getUserTime();
  cout<<"Clustering took "<<totalTime<<" seconds "<<endl;

  cout<<"Num Nodes: "<<_newDB->getNumNodes()<<"  Num Nets: "
      <<_newDB->getNets()->getNumNets()<<"  Num Pins: "
      <<_newDB->getNets()->getNumPins()<<endl;
  
  /*  _newDB->plot("cluster.gpl", _newDB->evalArea(), 
	100*(_newDB->evalArea()-_newDB->getNodesArea())/_newDB->getNodesArea(),
	       _newDB->getXSize()/_newDB->getYSize(), 0, _newDB->evalHPWL(), 
	       0, 1, 0);
  */
  double reqdWidth = 1e100;
  double reqdHeight = 1e100;
  double oldDBArea = _db->getNodesArea();
  double newDBArea = _newDB->getNodesArea();
  //  double maxArea = (1+_params->maxWS/100)*_newDB->getNodesArea();
  double maxArea = (1+_params->maxWS/100)*oldDBArea;
  double newMaxWS = (maxArea/newDBArea-1)*100;
  double oldMaxWS = _params->maxWS;
  _params->maxWS = newMaxWS;
  if(_params->reqdAR != -9999)
    cout<<"newMaxWS is "<<_params->maxWS<<endl;

  if(_params->reqdAR != -9999)
    {
      reqdHeight = sqrt(maxArea/_params->reqdAR);
      reqdWidth = maxArea/reqdHeight;
    }
  double currXSize, currYSize;
  unsigned maxIter = 0;
  Annealer annealer(_params, _newDB);
  if(_params->takePl)
    {
      //convert placement to sequence pair
      annealer.takeSPfromDB();
      annealer.eval();
    }

  if(_params->initQP)
    {
      annealer.solveQP();
      annealer.takeSPfromDB();
    }

  if(_params->initCompact)
    {
      double currArea, lastArea;
      annealer.eval();
      currArea = annealer.getXSize()*annealer.getYSize();
      bool whichDir = 0;
      annealer.evalCompact(whichDir);
      do
	{
	  whichDir = !whichDir;
	  lastArea = currArea;
	  annealer.takeSPfromDB();
	  annealer.evalCompact(whichDir);
	  currArea = annealer.getXSize()*annealer.getYSize();
	}
      while(int(currArea) < int(lastArea));

      //_newDB->plot("out.gpl", currArea, 0, 0, 0, 0, 0, 0, 0);
    }

  annealer.eval();
  double startArea = annealer.getXSize()*annealer.getYSize();
  cout<<"Starting whitespace "<<100*(startArea-newDBArea)/newDBArea<<"%. Starting AR "<<annealer.getXSize()/annealer.getYSize()<<endl;

  //_newDB->save("temp");

  unsigned numNodes = _newDB->getNumNodes();
  Point dummy;
  dummy.x=0;
  dummy.y=0;
  vector<Point> bestSolnPl(numNodes, dummy);
  vector<ORIENT> bestSolnOrient(numNodes, N);
  vector<double> bestSolnWidth(numNodes, 0);
  vector<double> bestSolnHeight(numNodes, 0);
  Nodes* newNodes = _newDB->getNodes();

  double minViol = 1e100;
  bool satisfied=true;
  do
    {
      annealer.go();
      currXSize = annealer.getXSize();
      currYSize = annealer.getYSize();
      if(currXSize<=reqdWidth && currYSize<=reqdHeight)
	break;
      
      //if not satisfied. then save best solution
      double viol=0;
      if(currXSize > reqdWidth)
	viol += (currXSize - reqdWidth);
      if(currYSize > reqdHeight)
	viol += (currYSize - reqdHeight);

      if(minViol > viol)
	{
	  minViol = viol;
	  itNode node;
	  unsigned nodeId=0;
	  for(node = newNodes->nodesBegin(); node != newNodes->nodesEnd(); ++node)
	    {
	      bestSolnPl[nodeId].x = node->getX();
	      bestSolnPl[nodeId].y = node->getY();
	      bestSolnOrient[nodeId] = node->getOrient();
	      bestSolnWidth[nodeId] = node->getWidth();
	      bestSolnHeight[nodeId] = node->getHeight();
	      ++nodeId;
	    }
	}

      maxIter++;
      if(maxIter == _params->maxIterHier)
	{
	  cout<<"FAILED to satisfy fixed outline constraints for Top Level"
	      <<endl;
	  satisfied = false;
	  break;
	}
    }
  while(1);

  if(!satisfied)//failed to satisfy constraints. save best soln
    {
      itNode node;
      unsigned nodeId=0;
      for(node = newNodes->nodesBegin(); node != newNodes->nodesEnd(); ++node)
	{
	  node->putX(bestSolnPl[nodeId].x);
	  node->putY(bestSolnPl[nodeId].y);
	  node->changeOrient(bestSolnOrient[nodeId], *(_newDB->getNets()));
	  node->putWidth(bestSolnWidth[nodeId]);
	  node->putHeight(bestSolnHeight[nodeId]);
	  ++nodeId;
	}
    }

  updatePlaceUnCluster(_newDB);

  if(!_params->solveTop)
    placeSubBlocks();

  _params->maxWS = oldMaxWS;
 
  /* 
  _newDB->plot("main.gpl", _newDB->evalArea(), 
	100*(_newDB->evalArea()-_newDB->getNodesArea())/_newDB->getNodesArea(),
	       _newDB->getXSize()/_newDB->getYSize(), 0, _newDB->evalHPWL(), 
	       0, 0, 1);

  _db->plot("final.gpl", _db->evalArea(), 
	    100*(_db->evalArea()-_db->getNodesArea())/_db->getNodesArea(),
	       _db->getXSize()/_db->getYSize(), 0, _db->evalHPWL(), 
	       0, 0, 0);
  */
}

void SolveMulti::placeSubBlocks(void)
{
  Nodes* nodes = _newDB->getNodes();
  Nodes* origNodes = _db->getNodes();

  itNode node;

  Command_Line* params = new Command_Line(*_params);
  params->budgetTime = 0;

  for(node=nodes->nodesBegin(); node!=nodes->nodesEnd(); ++node)
    {
      Point dbLoc;
      dbLoc.x = node->getX();
      dbLoc.y = node->getY();
      params->reqdAR = node->getWidth()/node->getHeight();

      DB * tempDB = new DB(_db, node->getSubBlocks(), dbLoc, params->reqdAR);

      cout<<node->getName()<<"  numSubBlks : "<<node->numSubBlocks()<<"reqdAR "
	  <<params->reqdAR<<endl;

      Annealer annealer(params, tempDB);
      
      double currXSize, currYSize;
      double reqdWidth = node->getWidth();
      double reqdHeight = node->getHeight();

      unsigned maxIter = 0;
      if(node->numSubBlocks() > 1)
	{
	  unsigned numNodes = tempDB->getNumNodes();
	  Point dummy;
	  dummy.x=0;
	  dummy.y=0;
	  vector<Point> bestSolnPl(numNodes, dummy);
	  vector<ORIENT> bestSolnOrient(numNodes, N);
	  vector<double> bestSolnWidth(numNodes, 0);
	  vector<double> bestSolnHeight(numNodes, 0);
	  Nodes* tempNodes = tempDB->getNodes();
	  
	  double minViol = 1e100;
	  bool satisfied=true;
	  do
	    {
	      annealer.go();
	      currXSize = annealer.getXSize();
	      currYSize = annealer.getYSize();
	      if(currXSize<=reqdWidth && currYSize<=reqdHeight)
		break;
	      //if not satisfied. then save best solution
	      double viol=0;
	      if(currXSize > reqdWidth)
		viol += (currXSize - reqdWidth);
	      if(currYSize > reqdHeight)
		viol += (currYSize - reqdHeight);
	      
	      if(minViol > viol)
		{
		  minViol = viol;
		  unsigned nodeId=0;

		  for(itNode tempNode = tempNodes->nodesBegin(); tempNode != tempNodes->nodesEnd(); ++tempNode)
		    {
		      bestSolnPl[nodeId].x = tempNode->getX();
		      bestSolnPl[nodeId].y = tempNode->getY();
		      bestSolnOrient[nodeId] = tempNode->getOrient();
		      bestSolnWidth[nodeId] = tempNode->getWidth();
		      bestSolnHeight[nodeId] = tempNode->getHeight();
		      ++nodeId;
		    }
		}
	      
	      maxIter++;
	      if(maxIter == _params->maxIterHier)
		{
		  cout<<"FAILED to satisfy fixed outline constraints for "
		      <<node->getName()<<endl;
		  satisfied=false;
		  break;
		}
	    }
	  while(1);
	  
	  if(!satisfied)//failed to satisfy constraints. save best soln
	    {
	      unsigned nodeId=0;
	      for(itNode tempNode = tempNodes->nodesBegin(); tempNode != tempNodes->nodesEnd(); ++tempNode)
		{
		  tempNode->putX(bestSolnPl[nodeId].x);
		  tempNode->putY(bestSolnPl[nodeId].y);
		  tempNode->changeOrient(bestSolnOrient[nodeId], *(_newDB->getNets()));
		  tempNode->putWidth(bestSolnWidth[nodeId]);
		  tempNode->putHeight(bestSolnHeight[nodeId]);
		  ++nodeId;
		}
	    }
	}
      else
	{
	  Point loc;
	  loc.x = 0.0;
	  loc.y = 0.0;
	  tempDB->initPlacement(loc);
	}
      
      Point offset;
      offset.x = node->getX();
      offset.y = node->getY();
      
      tempDB->shiftDesign(offset);
 
      Nodes * tempNodes = tempDB->getNodes();

      if(node->numSubBlocks() > 1)
	{
	  for(itNode tempNode = tempNodes->nodesBegin(); 
	      tempNode != tempNodes->nodesEnd(); ++tempNode)
	    {
	      for(vector<int>::iterator tempIdx = tempNode->subBlocksBegin();
		  tempIdx != tempNode->subBlocksEnd(); ++tempIdx)
		{
		  Node& origNode = origNodes->getNode(*tempIdx);
		  origNode.putX(tempNode->getX());
		  origNode.putY(tempNode->getY());
		  origNode.changeOrient(tempNode->getOrient(), *(_db->getNets()));
		  origNode.putHeight(tempNode->getHeight());
		  origNode.putWidth(tempNode->getWidth());
		}
	    }
	}
      else if(node->numSubBlocks() == 1)
	{
	  vector<int>::iterator tempIdx = node->subBlocksBegin();
	  Node& origNode = origNodes->getNode(*tempIdx);
	  origNode.putX(node->getX());
	  origNode.putY(node->getY());
	  origNode.changeOrient(node->getOrient(), *(_db->getNets()));
	  origNode.putHeight(node->getHeight());
	  origNode.putWidth(node->getWidth());
	}
      delete tempDB;
    }
}

void SolveMulti::updatePlaceUnCluster(DB * clusterDB)
{
  Nodes* nodes = _db->getNodes();
  Nodes* newNodes = clusterDB->getNodes();

  itNode node;

  const unsigned numNew = 6;
  double layOutXSize = _db->getXSize();
  double layOutYSize = _db->getYSize();
  double xStep = layOutXSize/numNew;
  double yStep = layOutYSize/numNew;

  for(node = newNodes->nodesBegin(); node != newNodes->nodesEnd(); ++node)
    {
      if(node->numSubBlocks() > 1)
	{
	  for(vector<int>::iterator subBlockIdx = node->subBlocksBegin(); 
	      subBlockIdx != node->subBlocksEnd(); ++subBlockIdx)
	    {
	      Node& tempNode = nodes->getNode(*subBlockIdx);
	      if(!_params->usePhyLocHier || node->numSubBlocks() == 1)
		{
		  tempNode.putX(node->getX());
		  tempNode.putY(node->getY());
		  tempNode.changeOrient(node->getOrient(), *(_db->getNets()));
		}
	      else
		{
		  double xloc = tempNode.getX();
		  double yloc = tempNode.getY();
		  unsigned xIdx = unsigned(floor(xloc/xStep));
		  unsigned yIdx = unsigned(floor(yloc/yStep));
		  xloc -= xIdx*xStep;
		  yloc -= yIdx*yStep;
		  
		  tempNode.putX(xloc+node->getX());
		  tempNode.putY(yloc+node->getY());
		}
	      //tempNode.changeOrient(node->getOrient(), *(_db->getNets()));
	    }
	}
      else if(node->numSubBlocks() == 1)  //the only block
	{
	  vector<int>::iterator subBlockIdx = node->subBlocksBegin();
	  Node& tempNode = nodes->getNode(*subBlockIdx);
	  tempNode.putX(node->getX());
	  tempNode.putY(node->getY());
	  tempNode.changeOrient(node->getOrient(), *(_db->getNets()));
	  tempNode.putHeight(node->getHeight());
	  tempNode.putWidth(node->getWidth());
	}
    }
}

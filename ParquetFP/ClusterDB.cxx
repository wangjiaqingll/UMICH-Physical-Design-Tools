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





#include "CommandLine.h"
#include "FPcommon.h"
#include "DB.h"
#include "ClusterDB.h"
using namespace parquetfp;

ClusterDB::ClusterDB(DB* db, Command_Line *params)
{
  _params = params;
  _db = db;
  _oldDB = new DB();
  *(_oldDB) = *(_db);
}

void ClusterDB::clusterMulti(DB * newDB)
{
  unsigned numNewNodes;
  unsigned numOldNodes;
  unsigned layerNum = 1;

  if(_params->dontClusterMacros)
    {
      double avgNodeHeight = 5*_oldDB->getAvgHeight();
      _oldDB->markTallNodesAsMacros(avgNodeHeight);
    }
 
  cout<<"Num Nodes: "<<_oldDB->getNodes()->getNumNodes()<<"  Num Nets: "
      <<_oldDB->getNets()->getNumNets()<<"  Num Pins: "
      <<_oldDB->getNets()->getNumPins()<<endl;

  unsigned maxTopLevelNodes;
  if(_params->maxTopLevelNodes == -9999)
    {
      maxTopLevelNodes = unsigned(2*sqrt(double(_db->getNumNodes())));
      if(maxTopLevelNodes < 50)
	maxTopLevelNodes = 50;
    }
  else
    {
      maxTopLevelNodes = _params->maxTopLevelNodes;
      if(maxTopLevelNodes <= 0)
	{
	  cout<<"maxTopLevelNodes has to be > 0"<<endl;
	  exit(0);
	}
    }

  if(_db->getNumNodes() > maxTopLevelNodes)
    {
      do
	{
	  DB * tempDB = new DB();
	  _newDB = tempDB;
	  numOldNodes  = _oldDB->getNodes()->getNumNodes();
	  cluster1Layer(layerNum);
	  
	  numNewNodes = _newDB->getNodes()->getNumNodes();
	  
	  *(_oldDB) = *(_newDB);
	  
	  cout<<"Num Nodes: "<<_oldDB->getNodes()->getNumNodes()<<"  Num Nets: "
	      <<_oldDB->getNets()->getNumNets()<<"  Num Pins: "
	      <<_oldDB->getNets()->getNumPins()<<endl;
	  
	  delete tempDB;
	  ++layerNum;
	}
      while(numNewNodes > maxTopLevelNodes && numOldNodes != numNewNodes );
    }

  _newDB = _oldDB;

  addWSPerNode(); //add WS per node
  *newDB = *_newDB;

}

ClusterDB::~ClusterDB()
{
  if(_oldDB) delete _oldDB;
}

void ClusterDB::cluster1Layer(unsigned layerNum)
{
  map<unsigned, unsigned> mapping;
  
  char blockName[100];
  unsigned blkCtr;

  Nodes* nodes = _oldDB->getNodes();
  Nets* nets = _oldDB->getNets();
  Nodes* newNodes = _newDB->getNodes();
  Nets* newNets = _newDB->getNets();

  double totalArea = _oldDB->getNodesArea();
  double threshold = 0.1*totalArea;
  unsigned numNodes = nodes->getNumNodes();

  itNode node;

  blkCtr = 0;
  vector<bool> seenNodes;
  seenNodes.resize(numNodes);
  fill(seenNodes.begin(), seenNodes.end(), false);

  //set up the nodes
  for(node = nodes->nodesBegin(); node != nodes->nodesEnd(); ++node)
    {
      Node& currNode = *node;
      if(!seenNodes[currNode.getIndex()])
	{
	  unsigned currNodeIdx = currNode.getIndex();
	  seenNodes[currNodeIdx] = true;
	  double currNodeArea = currNode.getArea();
	  
	  /*
	  for(vector<int>::iterator i = currNode.subBlocksBegin();
	      i!= currNode.subBlocksEnd(); ++i)
	    mapping[*i] = blkCtr;
	  */
	  mapping[currNode.getIndex()] = blkCtr;

	  Node& nextNode = getClosestNode(currNode, nodes, nets, seenNodes);

	  if(!seenNodes[nextNode.getIndex()])
	    {
	      double nextNodeArea = nextNode.getArea();
	      double newNodeArea = currNodeArea+nextNodeArea;

	      if(newNodeArea < threshold && !currNode.isMacro() && 
		 !nextNode.isMacro()) //cluster these nodes
		{
		  unsigned nextNodeIdx = nextNode.getIndex();
		  seenNodes[nextNodeIdx] = true;

		  sprintf(blockName, "Block_%d_%d",layerNum,blkCtr);
		  Node tempNode(blockName, newNodeArea, 0.75, 1.5, blkCtr, false);
		  
		  for(vector<int>::iterator i = currNode.subBlocksBegin();
		      i!= currNode.subBlocksEnd(); ++i)
		    tempNode.addSubBlockIndex(*i);
		  
		  for(vector<int>::iterator i = nextNode.subBlocksBegin();
		      i!= nextNode.subBlocksEnd(); ++i)
		    {
		      tempNode.addSubBlockIndex(*i);
		      //mapping[*i] = blkCtr;
		    }
		  mapping[nextNode.getIndex()] = blkCtr;

		  newNodes->putNewNode(tempNode);
		}
	      else
		{
		  Node tempNode(currNode.getName(), currNode.getArea(), 
				currNode.getminAR(), currNode.getmaxAR(), 
				blkCtr, false);
		  if(currNode.isMacro())
		    tempNode.updateMacroInfo(true);
		  for(vector<int>::iterator i = currNode.subBlocksBegin();
		      i!= currNode.subBlocksEnd(); ++i)
		    tempNode.addSubBlockIndex(*i);
		  newNodes->putNewNode(tempNode);
		}
	    }
	  else
	    {
	      Node tempNode(currNode.getName(), currNode.getArea(), 
			    currNode.getminAR(), currNode.getmaxAR(), 
			    blkCtr, false);
	      if(currNode.isMacro())
		tempNode.updateMacroInfo(true);
	      for(vector<int>::iterator i = currNode.subBlocksBegin();
		  i!= currNode.subBlocksEnd(); ++i)
		tempNode.addSubBlockIndex(*i);
	      newNodes->putNewNode(tempNode);
	    }
	  ++blkCtr;
	}
    }

  for(node = nodes->terminalsBegin(); node != nodes->terminalsEnd(); ++node)
    newNodes->putNewTerm(*node);

  //set up the nets now
  addNetsToNewDB(nets, newNets, nodes, newNodes, mapping);
}

Node& ClusterDB::getClosestNode(Node& currNode, Nodes* nodes, Nets* nets,
				vector<bool>& seenNodes)
{
  unsigned numNodes = nodes->getNumNodes();
  IntPoint tempIntPoint;
  tempIntPoint.x = 0;
  tempIntPoint.y = 0;
  vector<IntPoint> numConnections(numNodes, tempIntPoint);

  for(unsigned i=0; i<numNodes; ++i)
    numConnections[i].x = i;
  
  unsigned currNodeIdx = currNode.getIndex();
  itNodePin nodePin;

  for(nodePin = currNode.pinsBegin(); nodePin != currNode.pinsEnd(); ++nodePin)
    {
      Net& net = nets->getNet(nodePin->netIndex);
      itPin netPin;
      for(netPin = net.pinsBegin(); netPin != net.pinsEnd(); ++netPin)
	{
	  if(!netPin->getType()) //not terminal
	    {
	      if(unsigned(netPin->getNodeIndex()) != currNodeIdx)
		++numConnections[netPin->getNodeIndex()].y;
	    }
	}
    }

  //sort
  std::sort(numConnections.begin(), numConnections.end(), 
	    sortNumConnections());

  unsigned maxConnectionsIdx = 0;
  for(int i=numNodes-1; i>=0; --i)
    {
      maxConnectionsIdx = numConnections[i].x;
      if(seenNodes[maxConnectionsIdx] != 1)
	{
	  numConnections.clear();
	  return nodes->getNode(maxConnectionsIdx);
	}
    }

  maxConnectionsIdx = numConnections[numNodes-1].x;
  numConnections.clear();
  return nodes->getNode(maxConnectionsIdx);
}

void ClusterDB::addWSPerNode(void)
{
  itNode node;
  Nodes* newNodes = _newDB->getNodes();

  double multFactor = 1+_params->maxWSHier/100;
  double currArea, newArea, newHeight, newWidth;

  for(node = newNodes->nodesBegin(); node != newNodes->nodesEnd(); ++node)
    {
      if(node->numSubBlocks() > 1)
	{
	  currArea = node->getArea();
	  newArea = currArea*multFactor; //add WS
	  newWidth = sqrt(newArea*node->getminAR());
	  newHeight = newWidth/node->getminAR();
	  node->putArea(newArea);
	  node->putWidth(newWidth);
	  node->putHeight(newHeight);
	}
    }
}

void ClusterDB::addNetsToNewDB(Nets* nets, Nets* newNets, Nodes* nodes, 
			    Nodes* newNodes, map<unsigned, unsigned>& mapping)
{
  int netCtr=0;
  itNet net;

  vector<bool> seenNodes(newNodes->getNumNodes());
  for(net = nets->netsBegin(); net != nets->netsEnd(); ++net)
    {
      Net tempEdge;
      fill(seenNodes.begin(), seenNodes.end(), false);
      tempEdge.putName(net->getName());
      tempEdge.putIndex(netCtr);
      for(itPin netPin = net->pinsBegin(); netPin != net->pinsEnd(); ++netPin)
	{
	  unsigned currNodeIdx = netPin->getNodeIndex();

	  if(!netPin->getType())
	    {
	      unsigned newNodeIdx = mapping[currNodeIdx];
	      Node& newNode = newNodes->getNode(newNodeIdx);
	      double poffsetX = 0, poffsetY = 0;
	      if(newNode.numSubBlocks() == 1)
		{
		  poffsetX = netPin->getXOffset();
		  poffsetY = netPin->getYOffset();
		}
	      else if(_params->clusterPhysical) //get pinoffsets of subcells
		{
		  double xMin = newNode.getX();
		  double yMin = newNode.getY();
		  double xMax = xMin + sqrt(newNode.getArea()); //assume AR 1
		  double yMax = yMin + sqrt(newNode.getArea()); //assume AR 1
		  Node& oldNode = nodes->getNode(currNodeIdx);
		  double xloc = oldNode.getX();
		  double yloc = oldNode.getY();
		  if(xloc >= xMax)
		    poffsetX = 0.5;
		  else if(xloc <= xMin)
		    poffsetX = -0.5;
		  else
		    poffsetX = ((xloc-xMin)/(xMax-xMin)) - 0.5;

		  if(yloc >= yMax)
		    poffsetY = 0.5;
		  else if(yloc <= yMin)
		    poffsetY = -0.5;
		  else
		    poffsetY = ((yloc-yMin)/(yMax-yMin)) - 0.5;
		}
	      
	      pin tempPin(newNode.getName(), false, poffsetX, poffsetY, 
			  netCtr);
	      tempPin.putNodeIndex(newNodeIdx);
	      
	      if(!seenNodes[newNodeIdx])
		{
		  tempEdge.addNode(tempPin);
		  seenNodes[newNodeIdx] = 1;
		}
	    }
	  else
	    {
	      Node& newTerm = newNodes->getTerminal(currNodeIdx);
	      double poffsetX = 0, poffsetY = 0;
	      
	      pin tempPin(newTerm.getName(), true, poffsetX, poffsetY, 
			  netCtr);
	      tempPin.putNodeIndex(currNodeIdx);
	      
	      tempEdge.addNode(tempPin);
	    }
	}

      bool needNet = false;
      int firstNodeIdx = tempEdge.pinsBegin()->getNodeIndex();
      for(itPin netPin = tempEdge.pinsBegin(); netPin != tempEdge.pinsEnd();
	  netPin++)
	{
	  if(netPin->getType())
	    {
	      needNet = true;
	      break;
	    }
	  else if(netPin->getNodeIndex() != firstNodeIdx) //atleast 1 different
	    {
	      needNet = true;
	      break;
	    }
	}
      if(needNet)
	{
	  newNets->putNewNet(tempEdge);
	  ++netCtr;
	}
    }

  //cout<<"Num Nets: "<<newNets->getNumNets()<<"  Num Pins: "
  //<<newNets->getNumPins()<<endl;
  //update the pins info in nodes
  newNodes->updatePinsInfo(*newNets);
}

void ClusterDB::clusterMultiPhysical(DB * newDB)
{
  const unsigned numNew = 6;
  unsigned numOldNodes = _oldDB->getNumNodes();

  if(_params->dontClusterMacros)
    {
      double avgNodeHeight = _oldDB->getAvgHeight();
      _oldDB->markTallNodesAsMacros(avgNodeHeight);
    }
  
  map<unsigned, unsigned> mapping;

  _newDB = newDB;

  Nodes* nodes = _oldDB->getNodes();
  Nets* nets = _oldDB->getNets();
  Nodes* newNodes = _newDB->getNodes();
  Nets* newNets = _newDB->getNets();

  itNode node;

  unsigned blkCtr = 0;
  if(numOldNodes <= 50)
    {
      *newDB = *_oldDB;
      return;
    }

  double layOutXSize = _oldDB->getXSize();
  double layOutYSize = _oldDB->getYSize();

  //put nodes outside layout region into layout region
  for(node = nodes->nodesBegin(); node != nodes->nodesEnd(); ++node)
    {
      Node& currNode = *node;
      if(currNode.getX() > layOutXSize)
	currNode.putX(layOutXSize);
      if(currNode.getX() < 0.0)
        currNode.putX(0.0);
      if(currNode.getY() > layOutYSize)
        currNode.putY(layOutYSize);
      if(currNode.getY() < 0.0)
        currNode.putY(0.0);
    }

  double xStep = layOutXSize/numNew;
  double yStep = layOutYSize/numNew;
  double xMax, yMax, xMin, yMin;

  vector<bool> seenNodes(numOldNodes, false);

  char blockName[100];

  for(unsigned i=0; i<numNew; ++i)
    {
      yMin = i*yStep;
      yMax = (i+1)*yStep;
      for(unsigned j=0; j<numNew; ++j)
	{
	  xMin = j*xStep;
	  xMax = (j+1)*xStep;

	  sprintf(blockName, "Block_%d_%d",i,j);
	  double newNodeArea = 0;
	  vector<int> newNodesIdxs;

	  for(node = nodes->nodesBegin(); node != nodes->nodesEnd(); ++node)
	    {
	      Node& currNode = *node;
	      unsigned currNodeIdx = currNode.getIndex();
	      if(!seenNodes[currNodeIdx])
		{
		  if(currNode.getX() >= xMin && currNode.getX() <= xMax &&
		     currNode.getY() >= yMin && currNode.getY() <= yMax)
		    {
		      if(!currNode.isMacro())
			{
			  newNodeArea += currNode.getArea();
			  newNodesIdxs.push_back(currNode.getIndex());
			  seenNodes[currNode.getIndex()] = 1;
			}
		      else //macro needs to stored alone
			{
			  Node tempNode(currNode.getName(), currNode.getArea(),
					currNode.getminAR(), 
					currNode.getmaxAR(), blkCtr, false);
			  mapping[currNode.getIndex()] = blkCtr;
			  tempNode.addSubBlockIndex(currNode.getIndex());
			  tempNode.putX(currNode.getX());
			  tempNode.putY(currNode.getY());
			  tempNode.putOrient(currNode.getOrient());
			  newNodes->putNewNode(tempNode);
			  ++blkCtr;
			  seenNodes[currNode.getIndex()] = 1;
			}
		    }
		}
	    }
	  if(newNodesIdxs.size() != 0)
	    {
	      Node tempNode(blockName, newNodeArea, 0.5, 2.0, blkCtr, false);
	      for(unsigned k=0; k<newNodesIdxs.size(); ++k)
		{
		  tempNode.addSubBlockIndex(newNodesIdxs[k]);
		  mapping[newNodesIdxs[k]] = blkCtr;
		}
	      tempNode.putX(xMin);
	      tempNode.putY(yMin);
	      newNodes->putNewNode(tempNode);
	      ++blkCtr;
	    }
	}
    }
  for(unsigned i=0; i<seenNodes.size(); ++i)
    if(seenNodes[i] == 0)
      {
	Node& temp = nodes->getNode(i);
	Node tempNode(temp.getName(), temp.getArea(), temp.getminAR(), 
		      temp.getmaxAR(), blkCtr, false);
	
	tempNode.addSubBlockIndex(temp.getIndex());
	double xLoc = temp.getX() > 0 ? temp.getX() : 0;
	double yLoc = temp.getY() > 0 ? temp.getY() : 0;
	tempNode.putX(xLoc);
	tempNode.putY(yLoc);
	tempNode.putOrient(temp.getOrient());
	newNodes->putNewNode(tempNode);
	++blkCtr;
	cout<<"Warning in ClusterDB.cxx "<<temp.getName()<<"("<<temp.getX()
	    <<", "<<temp.getY()<<") out of layout region "<<endl;
      }

  for(node = nodes->terminalsBegin(); node != nodes->terminalsEnd(); ++node)
    newNodes->putNewTerm(*node);

  addWSPerNode(); //add WS per node needs to be before addNetsToNewDB

  addNetsToNewDB(nets, newNets, nodes, newNodes, mapping);
}











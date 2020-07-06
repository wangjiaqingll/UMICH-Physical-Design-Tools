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





#include <math.h>
#include "FPcommon.h"
#include "DB.h"
#include "AnalSolve.h"
#include "CommandLine.h"
using namespace parquetfp;

AnalSolve::AnalSolve(Command_Line* params, DB* db)
	   :_params(params), _db(db)
{}

Point AnalSolve::getDesignOptLoc()
{
  Point finalLoc;
  Nodes *nodes = _db->getNodes();
  Nets *nets = _db->getNets();
  itNode node;
  itNodePin nodePin;
  itPin netPin;
  vector<bool> netsSeen(nets->getNumNets(), 0);
  
  double xSum = 0;
  double ySum = 0;
  double netXSum = 0;
  double netYSum = 0;
  unsigned netDegree = 0;
  unsigned nodeDegree = 0;
  bool pinType=0;
  double xOffset;
  double yOffset;

  for(node = nodes->terminalsBegin(); node != nodes->terminalsEnd(); ++node)
  {
  for(nodePin = node->pinsBegin(); nodePin != node->pinsEnd(); ++nodePin)
   {
     unsigned netIndex = nodePin->netIndex;
     if(!netsSeen[netIndex])
     {
       ++nodeDegree;
       netsSeen[netIndex] = 1;
       Net& net = nets->getNet(netIndex);
     
       netDegree = 0;
       netXSum = 0;
       netYSum = 0;
     
       for(netPin = net.pinsBegin(); netPin != net.pinsEnd(); ++netPin)
         {
           pinType = netPin->getType();	
           int currNodeIndex = netPin->getNodeIndex();
	
	   if(!pinType)  //if not terminal
	    {
	      Node& currNode = nodes->getNode(currNodeIndex);
	      double width = currNode.getWidth();
	      double height = currNode.getHeight();
	      xOffset = netPin->getXOffset()*width + width/2;
	      yOffset = netPin->getYOffset()*height + height/2;     
	      netXSum -= (currNode.getX() +xOffset);
	      netYSum -= (currNode.getY() +yOffset);
	    }
	   else
	    {
	      ++netDegree;
	      Node& terminal = nodes->getTerminal(currNodeIndex);
	      netXSum += terminal.getX();
	      netYSum += terminal.getY();
	    }
         }
       if(netDegree > 0)
        {
          netXSum = netXSum/netDegree;
	  netYSum = netYSum/netDegree;
	  xSum += netXSum;
	  ySum += netYSum;
        }
     }
   }
  }
  if(nodeDegree != 0)
   {
     xSum /= nodeDegree;
     ySum /= nodeDegree;
     finalLoc.x = xSum;
     finalLoc.y = ySum;
   }
  else
   {
     finalLoc.x = 0;
     finalLoc.y = 0;
   }
  /*
   if(finalLoc.x < 0)
    finalLoc.x = 0;
   if(finalLoc.y < 0)
    finalLoc.y = 0;
    */
 return finalLoc;
}



Point AnalSolve::getOptLoc(int index, vector<double>& xloc, 
	                   vector<double>& yloc)
{
  Point finalLoc;
  Nodes *nodes = _db->getNodes();
  Nets *nets = _db->getNets();
  Node& node = nodes->getNode(index);
  itNodePin nodePin;
  itPin netPin;
  double xSum = 0;
  double ySum = 0;
  double netXSum = 0;
  double netYSum = 0;
  unsigned netDegree = 0;
  unsigned nodeDegree = node.getDegree();
  bool pinType=0;
  double xOffset;
  double yOffset;

  double nodeXOffset;
  double nodeYOffset;
  double nodeWidth = node.getWidth();
  double nodeHeight= node.getHeight();
  
  for(nodePin = node.pinsBegin(); nodePin != node.pinsEnd(); ++nodePin)
   {
     unsigned netIndex = nodePin->netIndex;
     Net& net = nets->getNet(netIndex);
     unsigned pinIndex = nodePin->pinOffset;
     pin& thisPin = net.getPin(pinIndex);
     nodeXOffset = thisPin.getXOffset()*nodeWidth + nodeWidth/2;
     nodeYOffset = thisPin.getYOffset()*nodeHeight + nodeHeight/2;
     
     netDegree = net.getDegree();
     netXSum = 0;
     netYSum = 0;
     
     for(netPin = net.pinsBegin(); netPin != net.pinsEnd(); ++netPin)
      {
        pinType = netPin->getType();	
        int currNodeIndex = netPin->getNodeIndex();
	
        if(!pinType && currNodeIndex == index)
	 { 
	   --netDegree;
	   //cout<<"currNodeIndex "<<currNodeIndex<<" index "<<index<<endl;
	 }
	else
	 {
	   //cout<<"not currNodeIndex "<<currNodeIndex<<" index "<<index<<endl;
	   if(!pinType)  //if not terminal
	    {
	      Node& currNode = nodes->getNode(currNodeIndex);
	      double width = currNode.getWidth();
	      double height = currNode.getHeight();
	      xOffset = netPin->getXOffset()*width + width/2;
	      yOffset = netPin->getYOffset()*height + height/2;     
	      netXSum += (xloc[currNodeIndex] +xOffset-nodeXOffset);
	      netYSum += (yloc[currNodeIndex] +yOffset-nodeYOffset);
	    }
	   else
	    {
	      Node& terminal = nodes->getTerminal(currNodeIndex);
	      netXSum += terminal.getX();
	      netYSum += terminal.getY();
	    }
	 }
      }
     if(netDegree > 0)
      {
        netXSum = netXSum/netDegree;
	netYSum = netYSum/netDegree;
	xSum += netXSum;
	ySum += netYSum;
      }
   }
   
  if(nodeDegree != 0)
   {
     xSum /= nodeDegree;
     ySum /= nodeDegree;
     finalLoc.x = xSum;
     finalLoc.y = ySum;
   }
  else
   {
     finalLoc.x = 0;
     finalLoc.y = 0;
   }
 return finalLoc;

}

void AnalSolve::solveSOR()
{
  _xloc = _db->getXLocs();
  _yloc = _db->getYLocs();

  double epsilon = sqrt(_db->getNodesArea());
  Point newLoc;
  double change = 1e100;
  double indChange=0;
  unsigned numIter = 0;
  double xchange;
  double ychange;
  double overshoot;
  
  while(change > epsilon && numIter < 1000000)
  {
    numIter++;
    change = 0;
    for(unsigned i=0; i<_xloc.size(); ++i)
     {
       newLoc = getOptLoc(i, _xloc, _yloc);
       xchange = newLoc.x - _xloc[i];
       ychange = newLoc.y - _yloc[i];
       overshoot = xchange*1.7;
       _xloc[i] += overshoot;
       
       overshoot = ychange*1.7;
       _yloc[i] += overshoot;
       
       indChange = fabs(xchange) + fabs(ychange);
       
       change += indChange;
     }
    //cout<<change<<" ";
  }
}

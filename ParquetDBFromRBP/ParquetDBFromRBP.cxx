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









#include "ParquetDBFromRBP.h"

ParquetDBFromRBP::~ParquetDBFromRBP()
{
  _mapping.clear();
  _rbpNodesPlacement.clear();
  _rbpNodesOrient.clear();
}

ParquetDBFromRBP::ParquetDBFromRBP(RBPlacement& rbp, vector<unsigned>& nodes, 
			    BBox& fpRegion) : parquetfp::DB(), _rbplace(rbp), 
			    _fpRegion(fpRegion), _rbpNodes(nodes)
{
  _rbpNodesPlacement.resize(_rbpNodes.size());
  _rbpNodesOrient.resize(_rbpNodes.size());
  const HGraphWDimensions& hgWDims = _rbplace.getHGraph();

  unsigned i;
  unsigned index;
  double nodesArea = 0;
  
  vector<bool> instanceNodes(hgWDims.getNumNodes(), false);
  vector<bool> seenNodes(hgWDims.getNumNodes(), false);
  map<unsigned, unsigned> mapTerminals;

  for(i = 0; i<_rbpNodes.size(); ++i)
    {
      index = _rbpNodes[i];
      const HGFNode& origNode = hgWDims.getNodeByIdx(index);
      double nodeWidth = hgWDims.getNodeWidth(index);
      double nodeHeight = hgWDims.getNodeHeight(index);
      double nodeArea = nodeWidth*nodeHeight;
      double nodeAR = nodeWidth/nodeHeight;
      Point nodeLoc = _rbplace[index];
      char* nodeName = const_cast<char *>(origNode.getName());
      parquetfp::Node tempNode(nodeName, nodeArea, nodeAR, nodeAR, i, false);
      tempNode.addSubBlockIndex(i);
      tempNode.putX(nodeLoc.x);
      tempNode.putY(nodeLoc.y);
      _nodes->putNewNode(tempNode);
      _mapping[index] = i;
      _rbpNodesPlacement[i] = nodeLoc;
      nodesArea += nodeArea;
      instanceNodes[index] = true;
    }

  double termXLoc, termYLoc;
  double minXLoc = _fpRegion.xMin;
  double maxXLoc = _fpRegion.xMax;
  double minYLoc = _fpRegion.yMin;
  double maxYLoc = _fpRegion.xMax;

  double reqdHeight = _fpRegion.getHeight();
  double reqdWidth = _fpRegion.getWidth();
  //double reqdArea = reqdHeight*reqdWidth;
  //double reqdAR = reqdWidth/reqdHeight;
  //double maxWS = 100*(reqdArea-nodesArea)/nodesArea;

  unsigned numOrigNets = hgWDims.getNumEdges();
  vector<bool> seenNets(numOrigNets, false);

  unsigned newNetCtr=0;
  unsigned newTermCtr = 0;
  
  itHGFEdgeLocal origEdge;
  itHGFNodeLocal n;
  parquetfp::Net tempEdge;
  
  for(i=0; i<_rbpNodes.size(); ++i)
    {
      index = _rbpNodes[i];
      const HGFNode& origNode = hgWDims.getNodeByIdx(index);
      for(origEdge = origNode.edgesBegin(); origEdge != origNode.edgesEnd(); 
	  ++origEdge)
	{
	  unsigned origNetIdx = (*origEdge)->getIndex();
	  if(!seenNets[origNetIdx])
	    {
	      seenNets[origNetIdx] = true;
	      char * origNetName = const_cast<char*>
		(hgWDims._netNames[origNetIdx]);
	      
	      if(origNetName != NULL)
		tempEdge.putName(origNetName);
	      tempEdge.putIndex(newNetCtr);
	      
	      unsigned nodeOffset = 0;
	      for(n = (*origEdge)->nodesBegin(); n != (*origEdge)->nodesEnd(); 
		  n++, nodeOffset++)
		{
		  unsigned cellId = (*n)->getIndex();
		  Point cellPl = _rbplace[cellId];
		  Point pinOffset = 
		    hgWDims.nodesOnEdgePinOffset(nodeOffset,origNetIdx,
					         _rbplace.getOrient(cellId));
		  Point pinLoc = cellPl + pinOffset;

		  double poffsetX, poffsetY;
		  double cellWidth = hgWDims.getNodeWidth(cellId);
		  double cellHeight = hgWDims.getNodeHeight(cellId);
		  
		  poffsetX = (pinOffset.x-cellWidth/2)/cellWidth;
		  poffsetY = (pinOffset.y-cellHeight/2)/cellHeight;
		  
		  if(instanceNodes[cellId])
		    {
		      unsigned newNodeIdx = _mapping[cellId];
		      
		      parquetfp::Node& newNode = 
			_nodes->getNode(newNodeIdx);
		      parquetfp::pin 
			tempPin(newNode.getName(), false, poffsetX, 
				poffsetY, newNetCtr);
		      tempPin.putNodeIndex(newNodeIdx);
		      tempEdge.addNode(tempPin);
		    }
		  else //need terminal propagation
		    {
		      unsigned termId;
		      if(!seenNodes[cellId])
			{
			  parquetfp::Node 
			    tempTerm(const_cast<char *>((*n)->getName()), 0, 1,
				     1, newTermCtr, true);
			  tempTerm.addSubBlockIndex(newTermCtr);
			  
			  //terminal propagation
			  if(pinLoc.x < minXLoc)
			    termXLoc = 0;
			  else if(pinLoc.x > maxXLoc)
			    termXLoc = reqdWidth;
			  else
			    termXLoc = cellPl.x - minXLoc;
			  
			  if(pinLoc.y < minYLoc)
			    termYLoc = 0;
			  else if(pinLoc.y > maxYLoc)
			    termYLoc = reqdHeight;
			  else
			    termYLoc = cellPl.y - minYLoc;
			  
			  tempTerm.putX(termXLoc);
			  tempTerm.putY(termYLoc);
			  
			  _nodes->putNewTerm(tempTerm);
			  mapTerminals[cellId] = newTermCtr;
			  termId = newTermCtr;
			  newTermCtr++;			  
			}
		      else
			{
			  termId = mapTerminals[cellId];
			}
		      parquetfp::pin 
			tempPin(const_cast<char *>((*n)->getName()), true, 
				0, 0, termId);
		      tempPin.putNodeIndex(termId);
		      tempEdge.addNode(tempPin);
		    }
		  seenNodes[cellId] = true;
		}
	      _nets->putNewNet(tempEdge);
	      ++newNetCtr;
	      tempEdge.clean();
	    }
	}
    }
  _nodes->updatePinsInfo(*_nets);
  
  *_nodesBestCopy = *_nodes;  
}

vector<Point>& ParquetDBFromRBP::getNodeLocs()
{
  unsigned numNodes = _rbpNodesPlacement.size();
  for(unsigned i=0; i<numNodes; ++i)
    {
      parquetfp::Node& node = _nodes->getNode(i);
      _rbpNodesPlacement[i].x = node.getX();
      _rbpNodesPlacement[i].y = node.getY();
    }
  return _rbpNodesPlacement;
}

vector<Orient>& ParquetDBFromRBP::getNodeOrients()
{
  unsigned numNodes = _rbpNodesOrient.size();
  for(unsigned i=0; i<numNodes; ++i)
    {
      parquetfp::Node& node = _nodes->getNode(i);
      parquetfp::ORIENT nodeOrient = node.getOrient();
      Orient orient;
      if(nodeOrient == parquetfp::N)
	orient = Orient("N");
      else if(nodeOrient == parquetfp::S)
	orient = Orient("S");
      else if(nodeOrient == parquetfp::E)
	orient = Orient("E");
      else if(nodeOrient == parquetfp::W)
	orient = Orient("W");
      else if(nodeOrient == parquetfp::FN)
	orient = Orient("FN");
      else if(nodeOrient == parquetfp::FS)
	orient = Orient("FS");
      else if(nodeOrient == parquetfp::FE)
	orient = Orient("FE");
      else if(nodeOrient == parquetfp::FW)
	orient = Orient("FW");
      else
	abkfatal(0,"Invalid Orientation returned\n");
      _rbpNodesOrient[i] = orient;
    }
  return _rbpNodesOrient;
}


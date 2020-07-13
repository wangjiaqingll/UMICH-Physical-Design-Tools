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









#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#define NUM_NEW_VERT_NETS 1   //vertical nets added per node during shredding
#define NUM_NEW_HORIZ_NETS 1  //horizontal nets added per node during shredding

#include <string>
#include "ABKCommon/pathDelims.h"
#include "hgWDims.h"
#include <deque>
using std::deque;

void HGraphWDimensions::saveAsNodesNetsWts(const char* baseFileName, 
					   bool fixMacros) const
{
    char* fname  = new char[strlen(baseFileName) + 8];
    strcpy(fname, baseFileName);
    char* extPtr = fname + strlen(baseFileName);

    strcpy(extPtr, ".nets");
    ofstream nets(fname);

    strcpy(extPtr, ".nodes");
    ofstream nodes(fname);

    strcpy(extPtr, ".wts");
    ofstream wts (fname);

    strcpy(extPtr, ".aux");
    ofstream aux (fname);

    itHGFNodeGlobal nIt;

    //get number of macros
    unsigned numMacros = 0;
    if(fixMacros)
      {
	for(nIt = nodesBegin(); nIt != nodesEnd(); nIt++)
	  {
	    HGFNode& node   = **nIt;
	    unsigned nodeId = node.getIndex();
	    if(!isTerminal(nodeId))
	      {
		if(_isMacro[nodeId])
		  ++numMacros;
	      }
	  }
      }
	//write the nodes file
    nodes<<"UCLA nodes 1.0"<<endl;
    nodes<<TimeStamp()<<User()<<Platform()<<endl;

    nodes<<"NumNodes :     "<<setw(10)<<getNumNodes()<<endl;
    nodes<<"NumTerminals : "<<setw(10)<<getNumTerminals()+numMacros<<endl;

    for(nIt = nodesBegin(); nIt != nodesEnd(); nIt++)
    {
	HGFNode& node   = **nIt;
	unsigned nodeId = node.getIndex();
	Symmetry sym    = _nodeSymmetries[nodeId];

        if (node.getName())	nodes<<setw(10)<<node.getName()<<"  ";
        else   
        {
            if (isTerminal(nodeId)) nodes<<" p"<< nodeId+1 <<" ";
            else                   nodes<<" a"<< nodeId-getNumTerminals()<<" ";
        }

	if(getNodeWidth(nodeId) != 0 || getNodeHeight(nodeId) != 0)
	    nodes<<setw(10)<<getNodeWidth(nodeId)<<" "
                 <<setw(10)<<getNodeHeight(nodeId)<<" ";
	if(isTerminal(nodeId)) 
	  nodes<<"terminal ";
	else if(fixMacros && _isMacro[nodeId])
	  nodes<<"terminal ";

	if(sym.rot90 || sym.y || sym.x) nodes << " : "<<sym;
	nodes<<endl;
    }
    nodes.close();

    unsigned num1PinEdges=0, num0PinEdges=0;
    itHGFEdgeGlobal e;
    for (e=edgesBegin(); e!=edgesEnd(); ++e)
    {
	HGFEdge&	edge   = **e;
        if (edge.getDegree()>1)  continue;
        if (edge.getDegree()==1) num1PinEdges++;
        else                     num0PinEdges++;
    }

	//write the nets file
    nets<<"UCLA nets 1.0"<<endl;
    nets<<TimeStamp()<<User()<<Platform()<<endl;

    nets<<"NumNets : "<<getNumEdges()-num1PinEdges<<endl;
    nets<<"NumPins : "<<getNumPins() -num1PinEdges<<endl;
    
    for (e=edgesBegin(); e!=edgesEnd(); ++e)
    {
	HGFEdge&	edge   = **e;
        if (edge.getDegree()<2) continue;
	unsigned	edgeId = edge.getIndex();
        const char*     netName=getNetNameByIndex(edgeId);
	
	nets<<"NetDegree : "<<(*e)->getDegree()<<" ";
        if (netName) nets << setw(10) << netName;
        else if (abs(edge.getWeight()-1.0)>1e-10)
                     nets << " net" << edgeId;
        else
          nets << " net" << edgeId;
        nets << endl;

	unsigned nodeOffset = 0;

        itHGFNodeLocal v;
        for (v=(*e)->srcsBegin(); v!=(*e)->srcsEnd(); v++, nodeOffset++)
        {
            const HGFNode& node=(**v);
            unsigned idx=node.getIndex();
	    nets<<setw(10)<<node.getName()<<" O ";

	    Point pOffset = nodesOnEdgePinOffset(nodeOffset, edgeId);
            double dx=pOffset.x - _nodeWidths [idx]/2,
                   dy=pOffset.y - _nodeHeights[idx]/2;
//	    if(pOffset.x != 0 || pOffset.y != 0)
	    if( abs(dx)>1e-10 || abs(dy)>1e-10)
		nets<<" : "<<dx <<" "  <<dy<<endl;
	    else
	 	nets<<endl;
        }
        for (v=(*e)->srcSnksBegin(); v!=(*e)->srcSnksEnd(); ++v, nodeOffset++)
        {
            const HGFNode& node=(**v);
            unsigned idx=node.getIndex();
	    nets<<setw(10)<<node.getName()<<" B ";

	    Point pOffset = nodesOnEdgePinOffset(nodeOffset, edgeId);
            double dx=pOffset.x - _nodeWidths [idx]/2,
                   dy=pOffset.y - _nodeHeights[idx]/2;
//          if(pOffset.x != 0 || pOffset.y != 0)
	    if( abs(dx)>1e-10 || abs(dy)>1e-10)
		nets<<" : "<<dx <<" "  <<dy<<endl;
	    else nets<<endl;
        }
        for (v=(*e)->snksBegin(); v!=(*e)->snksEnd(); ++v, nodeOffset++)
        {
            const HGFNode& node=(**v);
            unsigned idx=node.getIndex();
	    nets<<setw(10)<<node.getName()<<" I ";

	    Point pOffset = nodesOnEdgePinOffset(nodeOffset, edgeId);
            double dx=pOffset.x - _nodeWidths [idx]/2,
                   dy=pOffset.y - _nodeHeights[idx]/2;

//	    if(pOffset.x != 0 || pOffset.y != 0)
	    if( abs(dx)>1e-10 || abs(dy)>1e-10)
		nets<<" : "<<dx <<" "  <<dy<<endl;
	
	    else nets<<endl;
        }
    }
    nets.close();

		//write the wts file

    wts<<"UCLA wts 1.0"<<endl;
    wts<<TimeStamp()<<User()<<Platform()<<endl;

    unsigned n;
    if(getNumWeights() >= 1)
    {
      for(n=0; n!=getNumNodes(); n++)
      {
	HGFNode& node = *_nodes[n];
	wts<<setw(10)<<node.getName()<<"  ";

        for(unsigned i=0; i!=getNumWeights(); ++i)
	    wts<<" "<<setw(6)<< node.getWeight(i);

        wts<<endl;
      }
    }

    for(n=0; n!=getNumEdges(); ++n)
    {
        const HGFEdge& net=getEdgeByIdx(n);
        if (abs(net.getWeight()-1.0)<1e-10) continue;
        wts << " ";
        const char* netName=getNetNameByIndex(n);
        if (netName) wts << setw(10) << netName ;
        else wts << "net" << n;
        wts << " " << setw(10) << net.getWeight() << endl;
    }

    wts.close();

    aux<<"HGraphWPins : ";

    const char * tmp=NULL;
    tmp=strrchr(baseFileName,pathDelim);
    if (tmp++)
        aux<<tmp<<".nets "<<tmp<<".nodes "<<tmp<<".wts"<<endl;
    else
        aux<<baseFileName<<".nets "<<baseFileName<<".nodes "
	   <<baseFileName<<".wts"<<endl;
    aux.close();

    delete []fname;
}

void HGraphWDimensions::saveAsNodesNetsWtsWCongInfo(const char* baseFileName,
					vector<double>& nodesXCongestion, 
				        vector<double>& nodesYCongestion,
					double whitespace, double rowHeight) 
{
  unsigned numExtraNodes=1;
    char* fname  = new char[strlen(baseFileName) + 8];
    strcpy(fname, baseFileName);
    char* extPtr = fname + strlen(baseFileName);

    strcpy(extPtr, ".nets");
    ofstream nets(fname);

    strcpy(extPtr, ".nodes");
    ofstream nodes(fname);

    strcpy(extPtr, ".wts");
    ofstream wts (fname);

    strcpy(extPtr, ".aux");
    ofstream aux (fname);

    itHGFNodeGlobal nIt;

    unsigned numNodes = getNumNodes();
    vector<ctrSortDoublePair> sortedNodesByCongestion(numNodes);
    for(unsigned i=0; i<numNodes; ++i)
      {
	sortedNodesByCongestion[i].index = double(i);
	sortedNodesByCongestion[i].value = nodesXCongestion[i]+
	                                   nodesYCongestion[i];
      }
    sort(sortedNodesByCongestion.begin(), sortedNodesByCongestion.end(),
	 doublePair_less_mag());
    //    double dummyNodeWidth = floor(getAvgNodeWidth()/numExtraNodes);
    double dummyNodeWidth = floor(2.0*getAvgNodeWidth());

    if(dummyNodeWidth == 0)   
      dummyNodeWidth = 1;
    double dummyNodeHeight = rowHeight;
    double dummyNodeArea = dummyNodeHeight*dummyNodeWidth;

    vector<unsigned> congestedNodes;
    int idx = numNodes-1;
    double currWS = whitespace;
    while(currWS > 0 && idx >= 0)
      {
	if(!isTerminal(unsigned(sortedNodesByCongestion[idx].index)))
	  {
	    congestedNodes.push_back(unsigned(sortedNodesByCongestion[idx].index));
	    currWS -= dummyNodeArea*numExtraNodes;
	  }
	--idx;
      }

    //TODO
	//write the nodes file
    nodes<<"UCLA nodes 1.0"<<endl;
    nodes<<TimeStamp()<<User()<<Platform()<<endl;

    nodes<<"NumNodes :     "
	 <<setw(10)<<getNumNodes()+numExtraNodes*congestedNodes.size()<<endl;
    nodes<<"NumTerminals : "<<setw(10)<<getNumTerminals()<<endl;

    for(nIt = nodesBegin(); nIt != nodesEnd(); nIt++)
    {
	HGFNode& node   = **nIt;
	unsigned nodeId = node.getIndex();
	Symmetry sym    = _nodeSymmetries[nodeId];

        if (node.getName())	nodes<<setw(10)<<node.getName()<<"  ";
        else   
        {
            if (isTerminal(nodeId)) nodes<<" p"<< nodeId+1 <<" ";
            else                   nodes<<" a"<< nodeId-getNumTerminals()<<" ";
        }

	if(getNodeWidth(nodeId) != 0 || getNodeHeight(nodeId) != 0)
	    nodes<<setw(10)<<getNodeWidth(nodeId)<<" "
                 <<setw(10)<<getNodeHeight(nodeId)<<" ";
	if(isTerminal(nodeId)) 
	  nodes<<"terminal ";

	if(sym.rot90 || sym.y || sym.x) nodes << " : "<<sym;
	nodes<<endl;
    }

    for(unsigned i = 0; i<congestedNodes.size(); ++i)
      {
	unsigned nodeId = congestedNodes[i];
	HGFNode& node = getNodeByIdx(nodeId);

	for(unsigned k=0; k<numExtraNodes; ++k)
	  {
	    if (node.getName()) 
	      nodes<<"dummy_"<<k<<"_"<<node.getName()<<"  ";
	    else
	      nodes<<"dummy_a"<<k<<"_"<<nodeId-getNumTerminals()<<" ";
	    
	    nodes<<setw(10)<<dummyNodeWidth<<" "<<setw(10)<<dummyNodeHeight
		 <<" "<<endl;
	  }
      }
    nodes.close();

    unsigned num1PinEdges=0, num0PinEdges=0;
    itHGFEdgeGlobal e;
    for (e=edgesBegin(); e!=edgesEnd(); ++e)
    {
	HGFEdge&	edge   = **e;
        if (edge.getDegree()>1)  continue;
        if (edge.getDegree()==1) num1PinEdges++;
        else                     num0PinEdges++;
    }

	//write the nets file
    nets<<"UCLA nets 1.0"<<endl;
    nets<<TimeStamp()<<User()<<Platform()<<endl;

    nets<<"NumNets : "<<getNumEdges()-num1PinEdges+
      numExtraNodes*congestedNodes.size()<<endl;
    nets<<"NumPins : "<<getNumPins() -num1PinEdges+
      2*(numExtraNodes*congestedNodes.size())<<endl;
    
    for (e=edgesBegin(); e!=edgesEnd(); ++e)
    {
	HGFEdge&	edge   = **e;
        if (edge.getDegree()<2) continue;
	unsigned	edgeId = edge.getIndex();
        const char*     netName=getNetNameByIndex(edgeId);
	
	nets<<"NetDegree : "<<(*e)->getDegree()<<" ";
        if (netName) nets << setw(10) << netName;
        else if (abs(edge.getWeight()-1.0)>1e-10)
                     nets << " net" << edgeId;
        else
          nets << " net" << edgeId;
        nets << endl;

	unsigned nodeOffset = 0;

        itHGFNodeLocal v;
        for (v=(*e)->srcsBegin(); v!=(*e)->srcsEnd(); v++, nodeOffset++)
        {
            const HGFNode& node=(**v);
            unsigned idx=node.getIndex();
	    nets<<setw(10)<<node.getName()<<" O ";

	    Point pOffset = nodesOnEdgePinOffset(nodeOffset, edgeId);
            double dx=pOffset.x - _nodeWidths [idx]/2,
                   dy=pOffset.y - _nodeHeights[idx]/2;
//	    if(pOffset.x != 0 || pOffset.y != 0)
	    if( abs(dx)>1e-10 || abs(dy)>1e-10)
		nets<<" : "<<dx <<" "  <<dy<<endl;
	    else
	 	nets<<endl;
        }
        for (v=(*e)->srcSnksBegin(); v!=(*e)->srcSnksEnd(); ++v, nodeOffset++)
        {
            const HGFNode& node=(**v);
            unsigned idx=node.getIndex();
	    nets<<setw(10)<<node.getName()<<" B ";

	    Point pOffset = nodesOnEdgePinOffset(nodeOffset, edgeId);
            double dx=pOffset.x - _nodeWidths [idx]/2,
                   dy=pOffset.y - _nodeHeights[idx]/2;
//          if(pOffset.x != 0 || pOffset.y != 0)
	    if( abs(dx)>1e-10 || abs(dy)>1e-10)
		nets<<" : "<<dx <<" "  <<dy<<endl;
	    else nets<<endl;
        }
        for (v=(*e)->snksBegin(); v!=(*e)->snksEnd(); ++v, nodeOffset++)
        {
            const HGFNode& node=(**v);
            unsigned idx=node.getIndex();
	    nets<<setw(10)<<node.getName()<<" I ";

	    Point pOffset = nodesOnEdgePinOffset(nodeOffset, edgeId);
            double dx=pOffset.x - _nodeWidths [idx]/2,
                   dy=pOffset.y - _nodeHeights[idx]/2;

//	    if(pOffset.x != 0 || pOffset.y != 0)
	    if( abs(dx)>1e-10 || abs(dy)>1e-10)
		nets<<" : "<<dx <<" "  <<dy<<endl;
	
	    else nets<<endl;
        }
    }

    for (unsigned i=0; i<congestedNodes.size(); ++i)
    {
	unsigned nodeId = congestedNodes[i];
	HGFNode& node = getNodeByIdx(nodeId);
	unsigned dummyNetDegree = 2;
	for(unsigned k=0; k<numExtraNodes; ++k)
	  {
	    nets<<"NetDegree : "<<dummyNetDegree<<" ";
	    if (node.getName()) 
	      nets<<"dummy_net_"<<k<<"_"<<node.getName()<<"  ";
	    else
	      nets<<"dummy_net_a"<<k<<"_"<<nodeId-getNumTerminals()<<" ";
	    nets<<endl;
	    
	    if (node.getName())	
	      nets<<setw(10)<<node.getName()<<"  ";
	    else   
	      nets<<setw(10)<<" a"<< nodeId-getNumTerminals()<<" ";
	    nets<<"B "<<endl;
	    
	    if (node.getName()) 
	      nets<<setw(10)<<"dummy_"<<k<<"_"<<node.getName()<<"  ";
	    else
	      nets<<setw(10)<<"dummy_"<<k<<"_"<<nodeId-getNumTerminals()<<" ";
	    nets<<"B "<<endl;
	  }
    }
    nets.close();

		//write the wts file

    wts<<"UCLA wts 1.0"<<endl;
    wts<<TimeStamp()<<User()<<Platform()<<endl;

    unsigned n;
    if(getNumWeights() >= 1)
    {
      for(n=0; n!=getNumNodes(); n++)
      {
	HGFNode& node = *_nodes[n];
	wts<<setw(10)<<node.getName()<<"  ";

        for(unsigned i=0; i!=getNumWeights(); ++i)
	    wts<<" "<<setw(6)<< node.getWeight(i);

        wts<<endl;
      }
    }

    for(n=0; n!=getNumEdges(); ++n)
    {
        const HGFEdge& net=getEdgeByIdx(n);
        if (abs(net.getWeight()-1.0)<1e-10) continue;
        wts << " ";
        const char* netName=getNetNameByIndex(n);
        if (netName) wts << setw(10) << netName ;
        else wts << "net" << n;
        wts << " " << setw(10) << net.getWeight() << endl;
    }

    wts.close();

    aux<<"HGraphWPins : ";

    const char * tmp=NULL;
    tmp=strrchr(baseFileName,pathDelim);
    if (tmp++)
        aux<<tmp<<".nets "<<tmp<<".nodes "<<tmp<<".wts"<<endl;
    else
        aux<<baseFileName<<".nets "<<baseFileName<<".nodes "
	   <<baseFileName<<".wts"<<endl;
    aux.close();

    delete []fname;
}


void HGraphWDimensions::saveAsNodesNetsWtsShred(const char* baseFileName,
						double maxCoreRowHeight) const
{
    char* fname  = new char[strlen(baseFileName) + 8];
    strcpy(fname, baseFileName);
    char* extPtr = fname + strlen(baseFileName);

    strcpy(extPtr, ".nets");
    ofstream nets(fname);

    strcpy(extPtr, ".nodes");
    ofstream nodes(fname);

    strcpy(extPtr, ".wts");
    ofstream wts (fname);

    strcpy(extPtr, ".aux");
    ofstream aux (fname);

    unsigned numExtraNodes=0;
    unsigned numExtraNets=0;
    itHGFNodeGlobal nIt;

    for(nIt = nodesBegin(); nIt != nodesEnd(); nIt++)
      {
	HGFNode& node   = **nIt;
	unsigned nodeId = node.getIndex();
	double nodeHeight = getNodeHeight(nodeId);
	if(nodeHeight > maxCoreRowHeight && !isTerminal(nodeId)) 
	  {
	    unsigned numNewNodes = unsigned(ceil(nodeHeight/maxCoreRowHeight));
	    numExtraNodes += numNewNodes - 1;
	    numExtraNets += (numNewNodes - 1)*NUM_NEW_VERT_NETS;
	  }
      }

	//write the nodes file
    nodes<<"UCLA nodes 1.0"<<endl;
    nodes<<TimeStamp()<<User()<<Platform()<<endl;

    nodes<<"NumNodes :     "<<setw(10)<<getNumNodes()+numExtraNodes<<endl;
    nodes<<"NumTerminals : "<<setw(10)<<getNumTerminals()<<endl;

    for(nIt = nodesBegin(); nIt != nodesEnd(); nIt++)
    {
	HGFNode& node   = **nIt;
	unsigned nodeId = node.getIndex();
	Symmetry sym    = _nodeSymmetries[nodeId];

	if(getNodeHeight(nodeId) > maxCoreRowHeight && !isTerminal(nodeId)) 
	                       //shred these nodes
	  {
	    double nodeHeight = getNodeHeight(nodeId);
	    double remainingHeight = nodeHeight;
	    double thisNodeHeight = maxCoreRowHeight;
	    char* nodeName  = new char[strlen(node.getName()) + 8];
	    unsigned numNewNodes = unsigned(ceil(nodeHeight/maxCoreRowHeight));
	    for(unsigned i=0; i<numNewNodes; ++i)
	      {
		sprintf(nodeName, "%s@%d",node.getName(),i);
		nodes<<setw(10)<<nodeName<<"  ";
		nodes<<setw(10)<<getNodeWidth(nodeId)<<" ";
		if(remainingHeight < maxCoreRowHeight)
		  thisNodeHeight = remainingHeight;
		else
		  thisNodeHeight = maxCoreRowHeight;
		
		nodes<<setw(10)<<thisNodeHeight<<" ";
		remainingHeight -= thisNodeHeight;

		if(sym.rot90 || sym.y || sym.x) nodes << " : "<<sym;
		nodes<<endl;
	      }
	  }
	else
	  {
	    if (node.getName())	nodes<<setw(10)<<node.getName()<<"  ";
	    else   
	      {
		if (isTerminal(nodeId)) nodes<<" p"<< nodeId+1 <<" ";
		else                   nodes<<" a"<< nodeId-getNumTerminals()<<" ";
	      }
	    
	    if(getNodeWidth(nodeId) != 0 || getNodeHeight(nodeId) != 0)
	      nodes<<setw(10)<<getNodeWidth(nodeId)<<" "
		   <<setw(10)<<getNodeHeight(nodeId)<<" ";
	    if(isTerminal(nodeId)) nodes<<"terminal ";
	    if(sym.rot90 || sym.y || sym.x) nodes << " : "<<sym;
	    nodes<<endl;
	  }
    }
    nodes.close();

    unsigned num1PinEdges=0, num0PinEdges=0;
    itHGFEdgeGlobal e;
    for (e=edgesBegin(); e!=edgesEnd(); ++e)
    {
	HGFEdge&	edge   = **e;
        if (edge.getDegree()>1)  continue;
        if (edge.getDegree()==1) num1PinEdges++;
        else                     num0PinEdges++;
    }

	//write the nets file
    nets<<"UCLA nets 1.0"<<endl;
    nets<<TimeStamp()<<User()<<Platform()<<endl;

    nets<<"NumNets : "<<getNumEdges()-num1PinEdges+numExtraNets<<endl;
    nets<<"NumPins : "<<getNumPins() -num1PinEdges+numExtraNets*2<<endl;
    
    for (e=edgesBegin(); e!=edgesEnd(); ++e)
    {
	HGFEdge&	edge   = **e;
        if (edge.getDegree()<2) continue;
	unsigned	edgeId = edge.getIndex();
        const char*     netName=getNetNameByIndex(edgeId);
	
	nets<<"NetDegree : "<<(*e)->getDegree()<<" ";
        if (netName) nets << setw(10) << netName;
        else if (abs(edge.getWeight()-1.0)>1e-10)
                     nets << " net" << edgeId;
        nets << endl;

	unsigned nodeOffset = 0;

        itHGFNodeLocal v;
	double nodeHeight;
	unsigned subIdx;
	unsigned maxSubIdx;
	double thisNodeHeight;
        for (v=(*e)->srcsBegin(); v!=(*e)->srcsEnd(); v++, nodeOffset++)
        {
            const HGFNode& node=(**v);
            unsigned idx=node.getIndex();

	    char* nodeName  = new char[strlen(node.getName()) + 8];
	    nodeHeight = getNodeHeight(idx);
	    if(nodeHeight > maxCoreRowHeight)
	      {
		maxSubIdx = unsigned(ceil(nodeHeight/maxCoreRowHeight)-1);
		Point pOffset = nodesOnEdgePinOffset(nodeOffset, edgeId);
		double dx=pOffset.x - _nodeWidths [idx]/2,
		  dy=pOffset.y;
		if(dy > maxCoreRowHeight)
		    subIdx = unsigned(ceil(dy/maxCoreRowHeight)-1);
		else
		  subIdx = 0;
		
		if(subIdx > maxSubIdx)
		  subIdx = maxSubIdx;

		sprintf(nodeName, "%s@%d",node.getName(),subIdx);
		nets<<setw(10)<<nodeName<<" O ";

		if(subIdx == maxSubIdx)
		  thisNodeHeight = nodeHeight-subIdx*maxCoreRowHeight;
		else
		  thisNodeHeight = maxCoreRowHeight;

		dy -= subIdx*maxCoreRowHeight;
		dy -= thisNodeHeight/2;

		if( abs(dx)>1e-10 || abs(dy)>1e-10)
		  nets<<" : "<<dx <<" "  <<dy<<endl;
		else
		  nets<<endl;
	      }
	    else
	      {
		nets<<setw(10)<<node.getName()<<" O ";
		Point pOffset = nodesOnEdgePinOffset(nodeOffset, edgeId);
		double dx=pOffset.x - _nodeWidths [idx]/2,
		  dy=pOffset.y - _nodeHeights[idx]/2;
		//	    if(pOffset.x != 0 || pOffset.y != 0)
		if( abs(dx)>1e-10 || abs(dy)>1e-10)
		  nets<<" : "<<dx <<" "  <<dy<<endl;
		else
		  nets<<endl;
	      }
        }
        for (v=(*e)->srcSnksBegin(); v!=(*e)->srcSnksEnd(); ++v, nodeOffset++)
        {
            const HGFNode& node=(**v);
            unsigned idx=node.getIndex();

	    char* nodeName  = new char[strlen(node.getName()) + 8];
	    nodeHeight = getNodeHeight(idx);
	    if(nodeHeight > maxCoreRowHeight)
	      {
		maxSubIdx = unsigned(ceil(nodeHeight/maxCoreRowHeight)-1);
		Point pOffset = nodesOnEdgePinOffset(nodeOffset, edgeId);
		double dx=pOffset.x - _nodeWidths [idx]/2,
		  dy=pOffset.y;
		if(dy > maxCoreRowHeight)
		    subIdx = unsigned(ceil(dy/maxCoreRowHeight)-1);
		else
		  subIdx = 0;

		if(subIdx > maxSubIdx)
		  subIdx = maxSubIdx;

		sprintf(nodeName, "%s@%d",node.getName(),subIdx);
		nets<<setw(10)<<nodeName<<" B ";

		if(subIdx == maxSubIdx)
		  thisNodeHeight = nodeHeight-subIdx*maxCoreRowHeight;
		else
		  thisNodeHeight = maxCoreRowHeight;

		dy -= subIdx*maxCoreRowHeight;
		dy -= thisNodeHeight/2;

		if( abs(dx)>1e-10 || abs(dy)>1e-10)
		  nets<<" : "<<dx <<" "  <<dy<<endl;
		else
		  nets<<endl;
	      }
	    else
	      {
		nets<<setw(10)<<node.getName()<<" B ";
		
		Point pOffset = nodesOnEdgePinOffset(nodeOffset, edgeId);
		double dx=pOffset.x - _nodeWidths [idx]/2,
		  dy=pOffset.y - _nodeHeights[idx]/2;
		//          if(pOffset.x != 0 || pOffset.y != 0)
		if( abs(dx)>1e-10 || abs(dy)>1e-10)
		  nets<<" : "<<dx <<" "  <<dy<<endl;
		else nets<<endl;
	      }
        }
        for (v=(*e)->snksBegin(); v!=(*e)->snksEnd(); ++v, nodeOffset++)
        {
            const HGFNode& node=(**v);
            unsigned idx=node.getIndex();

	    char* nodeName  = new char[strlen(node.getName()) + 8];
	    nodeHeight = getNodeHeight(idx);
	    if(nodeHeight > maxCoreRowHeight)
	      {
		maxSubIdx = unsigned(ceil(nodeHeight/maxCoreRowHeight)-1);
		Point pOffset = nodesOnEdgePinOffset(nodeOffset, edgeId);
		double dx=pOffset.x - _nodeWidths [idx]/2,
		  dy=pOffset.y;
		if(dy > maxCoreRowHeight)
		    subIdx = unsigned(ceil(dy/maxCoreRowHeight)-1);
		else
		  subIdx = 0;

		if(subIdx > maxSubIdx)
		  subIdx = maxSubIdx;

		sprintf(nodeName, "%s@%d",node.getName(),subIdx);
		nets<<setw(10)<<nodeName<<" I ";

		if(subIdx == maxSubIdx)
		  thisNodeHeight = nodeHeight-subIdx*maxCoreRowHeight;
		else
		  thisNodeHeight = maxCoreRowHeight;

		dy -= subIdx*maxCoreRowHeight;
		dy -= thisNodeHeight/2;

		if( abs(dx)>1e-10 || abs(dy)>1e-10)
		  nets<<" : "<<dx <<" "  <<dy<<endl;
		else
		  nets<<endl;
	      }
	    else
	      {
		nets<<setw(10)<<node.getName()<<" I ";
		
		Point pOffset = nodesOnEdgePinOffset(nodeOffset, edgeId);
		double dx=pOffset.x - _nodeWidths [idx]/2,
		  dy=pOffset.y - _nodeHeights[idx]/2;
		
		//	    if(pOffset.x != 0 || pOffset.y != 0)
		if( abs(dx)>1e-10 || abs(dy)>1e-10)
		  nets<<" : "<<dx <<" "  <<dy<<endl;
		else nets<<endl;
	      }
        }
    }

		//write the wts file

    wts<<"UCLA wts 1.0"<<endl;
    wts<<TimeStamp()<<User()<<Platform()<<endl;

    unsigned n;
    if(getNumWeights() >= 1)
    {
      for(n=0; n!=getNumNodes(); n++)
      {
	HGFNode& node = *_nodes[n];
	wts<<setw(10)<<node.getName()<<"  ";

        for(unsigned i=0; i!=getNumWeights(); ++i)
	  wts<<" "<<setw(6)<<node.getWeight(i);
        wts<<endl;
      }
    }
    double maxWeight = 0;
    double weight;

    for(n=0; n!=getNumEdges(); ++n)
    {
        const HGFEdge& net=getEdgeByIdx(n);
	weight = net.getWeight();

        if (abs(weight-1.0)<1e-10) continue;
        wts << " ";
        const char* netName=getNetNameByIndex(n);
        if (netName) wts << setw(10) << netName ;
        else wts << "net" << n;

        wts << " " << setw(10) << weight << endl;
	if(maxWeight > weight)
	  maxWeight = weight;
    }

    if(maxWeight == 0)
      maxWeight = 1;
    else
      maxWeight *= 1;


    //add edges to shredded cells
    unsigned netId = 0;
    for(nIt = nodesBegin(); nIt != nodesEnd(); nIt++)
      {
	HGFNode& node   = **nIt;
	unsigned nodeId = node.getIndex();
	if(getNodeHeight(nodeId) > maxCoreRowHeight && !isTerminal(nodeId)) 
	  //shred these nodes
	  {
	    double nodeHeight = getNodeHeight(nodeId);
//          double nodeWidth = getNodeWidth(nodeId);
//          double dx;
//          double dyAbove = -maxCoreRowHeight/2;
//          double dyBelow = maxCoreRowHeight/2;

	    char* nodeName  = new char[strlen(node.getName()) + 8];
	    char* nodeNameNext  = new char[strlen(node.getName()) + 8];
	    char* netName  = new char[15];

	    unsigned numNewNodes = unsigned(ceil(nodeHeight/maxCoreRowHeight));
	    for(unsigned i=0; i<numNewNodes-1; ++i)
	      {
		sprintf(nodeName, "%s@%d",node.getName(),i);
		sprintf(nodeNameNext, "%s@%d",node.getName(),i+1);
		
		for(unsigned j = 0; j<NUM_NEW_VERT_NETS; ++j)
		  {
		    sprintf(netName, "temp@%d",netId);
		    netId++;

		    nets<<"NetDegree : 2 ";
		    nets << setw(10) << netName <<endl;
		    /*
		    dx = j*(nodeWidth/(NUM_NEW_VERT_NETS-1));
		    dx -= nodeWidth/2;

		    nets<<setw(10)<<nodeName<<" B ";
		    nets<<" : "<<dx <<" "  <<dyBelow<<endl;
		    nets<<setw(10)<<nodeNameNext<<" B ";
		    nets<<" : "<<dx <<" "  <<dyAbove<<endl;
		    */
		    nets<<setw(10)<<nodeName<<" B "<<endl;;
		    nets<<setw(10)<<nodeNameNext<<" B "<<endl;

		    wts << setw(10) << netName ;
		    wts << " " << setw(10) << maxWeight << endl;
		  }
	      }
	  }
      }

    nets.close();
    wts.close();

    aux<<"HGraphWPins : ";

    const char * tmp=NULL;
    tmp=strrchr(baseFileName,pathDelim);
    if (tmp++)
        aux<<tmp<<".nets "<<tmp<<".nodes "<<tmp<<".wts"<<endl;
    else
        aux<<baseFileName<<".nets "<<baseFileName<<".nodes "
	   <<baseFileName<<".wts"<<endl;
    aux.close();

    delete []fname;
}

void HGraphWDimensions::saveAsNodesNetsWtsUnShred(const char* baseFileName,
						double maxCoreRowHeight) const
{
    char* fname  = new char[strlen(baseFileName) + 8];
    strcpy(fname, baseFileName);
    char* extPtr = fname + strlen(baseFileName);
    std::map<std::string, double> widths;
    std::map<std::string, double> heights;

    strcpy(extPtr, ".nets");
    ofstream nets(fname);

    strcpy(extPtr, ".nodes");
    ofstream nodes(fname);

    strcpy(extPtr, ".wts");
    ofstream wts (fname);

    strcpy(extPtr, ".aux");
    ofstream aux (fname);

    unsigned numExtraNodes=0;
    unsigned numExtraNets=0;
    itHGFNodeGlobal nIt;

    for(nIt = nodesBegin(); nIt != nodesEnd(); nIt++)
      {
	HGFNode& node   = **nIt;
	const char* nodeName  = node.getName();
	char* actualName = new char[strlen(nodeName)];
	strcpy(actualName, nodeName);
	char* pos = strchr(actualName, '@');

	if(pos != NULL)  //found
	  {
	    *pos = '\0';
	    char *subNodeName = new char[strlen(nodeName)+10];
	    while(nIt != nodesEnd())
	      {
		HGFNode& subNode   = **nIt;
		nodeName = subNode.getName();
		strcpy(subNodeName, nodeName);
		pos = strchr(subNodeName, '@');
		if(pos == NULL)
		  break;
		*pos = '\0';
		if(strcmp(actualName, subNodeName) != 0)
		  break;

		++numExtraNodes;
		numExtraNets += NUM_NEW_VERT_NETS;
		++nIt;
	      }
	    --nIt;
	    --numExtraNodes;
	    numExtraNets -= NUM_NEW_VERT_NETS;
	  }
      }

	//write the nodes file
    nodes<<"UCLA nodes 1.0"<<endl;
    nodes<<TimeStamp()<<User()<<Platform()<<endl;

    nodes<<"NumNodes :     "<<setw(10)<<getNumNodes()-numExtraNodes<<endl;
    nodes<<"NumTerminals : "<<setw(10)<<getNumTerminals()<<endl;

    for(nIt = nodesBegin(); nIt != nodesEnd(); nIt++)
    {
	HGFNode& node   = **nIt;
	unsigned nodeId = node.getIndex();
	Symmetry sym    = _nodeSymmetries[nodeId];

	const char* nodeName  = node.getName();
	char* actualName = new char[strlen(nodeName)];
	strcpy(actualName, nodeName);
	char* pos = strchr(actualName, '@');

	std::string tempName;

	if(pos != NULL)  //found
	  {
	    *pos = '\0';
	    char *subNodeName = new char[strlen(nodeName)+10];
	    double nodeHeight = 0;
	    double nodeWidth = getNodeWidth(nodeId);

	    while(nIt != nodesEnd())
	      {
		HGFNode& subNode   = **nIt;
		unsigned subNodeId = subNode.getIndex();
		nodeName = subNode.getName();
		strcpy(subNodeName, nodeName);
		pos = strchr(subNodeName, '@');
		if(pos == NULL)
		  break;
		*pos = '\0';
		if(strcmp(actualName, subNodeName) != 0)
		  break;

		nodeHeight += getNodeHeight(subNodeId);
		nIt++;
	      }
	    --nIt;
	    nodes<<setw(10)<<actualName<<"  ";
	    nodes<<setw(10)<<nodeWidth<<" ";
	    nodes<<setw(10)<<nodeHeight<<" ";
	    if(sym.rot90 || sym.y || sym.x) nodes << " : "<<sym;
	    nodes<<endl;

	    tempName = actualName;
	    widths[tempName] = nodeWidth;
	    heights[tempName] = nodeHeight;
	  }
	else
	  {
	    if (node.getName())	nodes<<setw(10)<<node.getName()<<"  ";
	    else   
	      {
		if (isTerminal(nodeId)) nodes<<" p"<< nodeId+1 <<" ";
		else                   nodes<<" a"<< nodeId-getNumTerminals()<<" ";
	      }
	    
	    if(getNodeWidth(nodeId) != 0 || getNodeHeight(nodeId) != 0)
	      nodes<<setw(10)<<getNodeWidth(nodeId)<<" "
		   <<setw(10)<<getNodeHeight(nodeId)<<" ";
	    if(isTerminal(nodeId)) nodes<<"terminal ";
	    if(sym.rot90 || sym.y || sym.x) nodes << " : "<<sym;
	    nodes<<endl;
	  }
    }
    nodes.close();

    unsigned num1PinEdges=0, num0PinEdges=0;
    itHGFEdgeGlobal e;
    for (e=edgesBegin(); e!=edgesEnd(); ++e)
    {
	HGFEdge&	edge   = **e;
        if (edge.getDegree()>1)  continue;
        if (edge.getDegree()==1) num1PinEdges++;
        else                     num0PinEdges++;
    }

	//write the nets file
    nets<<"UCLA nets 1.0"<<endl;
    nets<<TimeStamp()<<User()<<Platform()<<endl;

    nets<<"NumNets : "<<getNumEdges()-num1PinEdges-numExtraNets<<endl;
    nets<<"NumPins : "<<getNumPins() -num1PinEdges-numExtraNets*2<<endl;
    
    for (e=edgesBegin(); e!=edgesEnd(); ++e)
    {
	HGFEdge&	edge   = **e;
        if (edge.getDegree()<2) continue;
	unsigned	edgeId = edge.getIndex();
        const char*     netName=getNetNameByIndex(edgeId);
	const char* pos = strchr(netName, '@');
	if(pos != NULL)
	  continue;

	nets<<"NetDegree : "<<(*e)->getDegree()<<" ";
        if (netName) nets << setw(10) << netName;
        else if (abs(edge.getWeight()-1.0)>1e-10)
                     nets << " net" << edgeId;
        nets << endl;

	unsigned nodeOffset = 0;

        itHGFNodeLocal v;
	double nodeHeight;
	double nodeWidth;
	unsigned subIdx;
        for (v=(*e)->srcsBegin(); v!=(*e)->srcsEnd(); v++, nodeOffset++)
        {
            const HGFNode& node=(**v);
            unsigned idx=node.getIndex();
	    const char* nodeName  = node.getName();
	    char* actualName = new char[strlen(nodeName)];
	    strcpy(actualName, nodeName);
	    char* subIndex = new char[strlen(nodeName)];
	    char* pos = strchr(actualName, '@');
	    std::string tempName;
	    if(pos != NULL)  //found
	      {
		*pos = '\0';
		strcpy(subIndex, pos+1);
		subIdx = atoi(subIndex);
		tempName = actualName;
		nodeWidth = widths[tempName];
		nodeHeight = heights[tempName];

		Point pOffset = nodesOnEdgePinOffset(nodeOffset, edgeId);
		double dx = pOffset.x - nodeWidth/2,
		       dy = pOffset.y;
		dy = dy + subIdx*maxCoreRowHeight - nodeHeight/2;

		nets<<setw(10)<<actualName<<" O ";
		if( abs(dx)>1e-10 || abs(dy)>1e-10)
		  nets<<" : "<<dx <<" "  <<dy<<endl;
		else
		  nets<<endl;
	      }
	    else
	      {
		nets<<setw(10)<<node.getName()<<" O ";
		Point pOffset = nodesOnEdgePinOffset(nodeOffset, edgeId);
		double dx=pOffset.x - _nodeWidths [idx]/2,
		  dy=pOffset.y - _nodeHeights[idx]/2;
		//	    if(pOffset.x != 0 || pOffset.y != 0)
		if( abs(dx)>1e-10 || abs(dy)>1e-10)
		  nets<<" : "<<dx <<" "  <<dy<<endl;
		else
		  nets<<endl;
	      }
        }
        for (v=(*e)->srcSnksBegin(); v!=(*e)->srcSnksEnd(); ++v, nodeOffset++)
        {
            const HGFNode& node=(**v);
            unsigned idx=node.getIndex();
	    const char* nodeName  = node.getName();
	    char* actualName = new char[strlen(nodeName)];
	    strcpy(actualName, nodeName);
	    char* subIndex = new char[strlen(nodeName)];
	    char* pos = strchr(actualName, '@');

	    if(pos != NULL)  //found
	      {
		*pos = '\0';
		strcpy(subIndex, pos+1);
		subIdx = atoi(subIndex);
		nodeWidth = widths[actualName];
		nodeHeight = heights[actualName];

		//		nodeWidth = widths[actualName];
		//nodeHeight = heights[actualName];
		Point pOffset = nodesOnEdgePinOffset(nodeOffset, edgeId);
		double dx = pOffset.x - nodeWidth/2,
		       dy = pOffset.y;
		dy = dy + subIdx*maxCoreRowHeight - nodeHeight/2;

		//cout<<actualName<<nodeWidth<<"\t"<<nodeHeight<<endl;

		nets<<setw(10)<<actualName<<" B ";
		if( abs(dx)>1e-10 || abs(dy)>1e-10)
		  nets<<" : "<<dx <<" "  <<dy<<endl;
		else
		  nets<<endl;
	      }
	    else
	      {
		nets<<setw(10)<<node.getName()<<" B ";
		Point pOffset = nodesOnEdgePinOffset(nodeOffset, edgeId);
		double dx=pOffset.x - _nodeWidths [idx]/2,
		  dy=pOffset.y - _nodeHeights[idx]/2;
		//	    if(pOffset.x != 0 || pOffset.y != 0)
		if( abs(dx)>1e-10 || abs(dy)>1e-10)
		  nets<<" : "<<dx <<" "  <<dy<<endl;
		else
		  nets<<endl;
	      }
        }
        for (v=(*e)->snksBegin(); v!=(*e)->snksEnd(); ++v, nodeOffset++)
        {
            const HGFNode& node=(**v);
            unsigned idx=node.getIndex();
	    const char* nodeName  = node.getName();
	    char* actualName = new char[strlen(nodeName)];
	    strcpy(actualName, nodeName);
	    char* subIndex = new char[strlen(nodeName)];
	    char* pos = strchr(actualName, '@');

	    if(pos != NULL)  //found
	      {
		*pos = '\0';
		strcpy(subIndex, pos+1);
		subIdx = atoi(subIndex);
		nodeWidth = widths[actualName];
		nodeHeight = heights[actualName];
		Point pOffset = nodesOnEdgePinOffset(nodeOffset, edgeId);
		double dx = pOffset.x - nodeWidth/2,
		       dy = pOffset.y;
		dy = dy + subIdx*maxCoreRowHeight - nodeHeight/2;

		nets<<setw(10)<<actualName<<" I ";
		if( abs(dx)>1e-10 || abs(dy)>1e-10)
		  nets<<" : "<<dx <<" "  <<dy<<endl;
		else
		  nets<<endl;
	      }
	    else
	      {
		nets<<setw(10)<<node.getName()<<" I ";
		Point pOffset = nodesOnEdgePinOffset(nodeOffset, edgeId);
		double dx=pOffset.x - _nodeWidths [idx]/2,
		  dy=pOffset.y - _nodeHeights[idx]/2;
		//	    if(pOffset.x != 0 || pOffset.y != 0)
		if( abs(dx)>1e-10 || abs(dy)>1e-10)
		  nets<<" : "<<dx <<" "  <<dy<<endl;
		else
		  nets<<endl;
	      }
        }
    }
    nets.close();



		//write the wts file

    wts<<"UCLA wts 1.0"<<endl;
    wts<<TimeStamp()<<User()<<Platform()<<endl;

    unsigned n;
    if(getNumWeights() >= 1)
    {
      for(n=0; n!=getNumNodes(); n++)
      {
	HGFNode& node = *_nodes[n];
	wts<<setw(10)<<node.getName()<<"  ";

        for(unsigned i=0; i!=getNumWeights(); ++i)
	  wts<<" "<<setw(6)<<node.getWeight(i);
        wts<<endl;
      }
    }

    double weight;

    for(n=0; n!=getNumEdges(); ++n)
    {
        const HGFEdge& net=getEdgeByIdx(n);
        const char*     netName=getNetNameByIndex(n);
	char* pos = strchr(netName, '@');
	if(pos != NULL)
	  continue;

	weight = net.getWeight();

        if (abs(weight-1.0)<1e-10) continue;
        wts << " ";
        if (netName) wts << setw(10) << netName ;
        else wts << "net" << n;

        wts << " " << setw(10) << weight << endl;
    }

    wts.close();

    aux<<"HGraphWPins : ";

    const char * tmp=NULL;
    tmp=strrchr(baseFileName,pathDelim);
    if (tmp++)
        aux<<tmp<<".nets "<<tmp<<".nodes "<<tmp<<".wts"<<endl;
    else
        aux<<baseFileName<<".nets "<<baseFileName<<".nodes "
	   <<baseFileName<<".wts"<<endl;
    aux.close();

    delete []fname;
}



//split in both dimensions
void HGraphWDimensions::saveAsNodesNetsWtsShredHW(const char* baseFileName,
			    double minWidthNode, double maxCoreRowHeight,
			    unsigned singleNetWt) const
{
    unsigned numNewSingleNets=20;
    bool useSingleNet=false;
    if(singleNetWt > 0 )
      {
	useSingleNet = true;
	numNewSingleNets = singleNetWt;
      }
    char* fname  = new char[strlen(baseFileName) + 8];
    strcpy(fname, baseFileName);
    char* extPtr = fname + strlen(baseFileName);

    strcpy(extPtr, ".nets");
    ofstream nets(fname);

    strcpy(extPtr, ".nodes");
    ofstream nodes(fname);

    strcpy(extPtr, ".wts");
    ofstream wts (fname);

    strcpy(extPtr, ".aux");
    ofstream aux (fname);

    unsigned numExtraNodes=0;
    unsigned numExtraNets=0;
    unsigned numExtraPins=0;

    itHGFNodeGlobal nIt;

    for(nIt = nodesBegin(); nIt != nodesEnd(); nIt++)
      {
	HGFNode& node   = **nIt;
	unsigned nodeId = node.getIndex();
	double nodeHeight = getNodeHeight(nodeId);
	double nodeWidth = getNodeWidth(nodeId);
	if(_isMacro[nodeId] == true && !isTerminal(nodeId)) 
	  {
	    unsigned numNewVertNodes = unsigned(ceil(nodeHeight/maxCoreRowHeight));
	    unsigned numNewHorizNodes = unsigned(ceil(nodeWidth/minWidthNode));
	    numExtraNodes += (numNewVertNodes*numNewHorizNodes) - 1;
	    if(!useSingleNet)
	      {
		numExtraNets += 
		  (numNewVertNodes*(numNewHorizNodes-1)*NUM_NEW_HORIZ_NETS + 
		   numNewHorizNodes*(numNewVertNodes-1)*NUM_NEW_VERT_NETS);
	      }
	    else
	      {
		numExtraNets += numNewSingleNets;
		numExtraPins += 
		  numNewSingleNets*numNewVertNodes*numNewHorizNodes;
	      }	      
	  }
      }
    if(!useSingleNet)
      numExtraPins = numExtraNets*2;

    //write the nodes file and part of wts file
    
    wts<<"UCLA wts 1.0"<<endl;
    wts<<TimeStamp()<<User()<<Platform()<<endl;

    nodes<<"UCLA nodes 1.0"<<endl;
    nodes<<TimeStamp()<<User()<<Platform()<<endl;

    nodes<<"NumNodes :     "<<setw(10)<<getNumNodes()+numExtraNodes<<endl;
    nodes<<"NumTerminals : "<<setw(10)<<getNumTerminals()<<endl;

    for(nIt = nodesBegin(); nIt != nodesEnd(); nIt++)
    {
	HGFNode& node   = **nIt;
	unsigned nodeId = node.getIndex();
	Symmetry sym    = _nodeSymmetries[nodeId];

	if(_isMacro[nodeId] == true && !isTerminal(nodeId)) 
	                       //shred these nodes
	  {
	    double nodeHeight = getNodeHeight(nodeId);
	    double nodeWidth = getNodeWidth(nodeId);
	    double remainingHeight = nodeHeight;
	    double remainingWidth = nodeWidth;

	    double thisNodeHeight = maxCoreRowHeight;
	    double thisNodeWidth = minWidthNode;

	    char* nodeName  = new char[strlen(node.getName()) + 10];
	    unsigned numNewVertNodes = 
	      unsigned(ceil(nodeHeight/maxCoreRowHeight));
	    unsigned numNewHorizNodes = 
	      unsigned(ceil(nodeWidth/minWidthNode));

	    for(unsigned i=0; i<numNewVertNodes; ++i)
	      {
		if(remainingHeight < maxCoreRowHeight)
		  thisNodeHeight = remainingHeight;
		else
		  thisNodeHeight = maxCoreRowHeight;

		remainingWidth = nodeWidth;
		for(unsigned j=0; j<numNewHorizNodes; ++j)
		  {
		    sprintf(nodeName, "%s@%d#%d",node.getName(),i,j);
		    nodes<<setw(10)<<nodeName<<"  ";

		    if(remainingWidth < minWidthNode)
		      thisNodeWidth = remainingWidth;
		    else
		      thisNodeWidth = minWidthNode;

		    if(thisNodeWidth < 1e-5) //double roundoff error
		      thisNodeWidth = minWidthNode/100;

		    nodes<<setw(10)<<thisNodeWidth<<" ";
		    nodes<<setw(10)<<thisNodeHeight<<" ";
		    if(sym.rot90 || sym.y || sym.x) nodes << " : "<<sym;
		    nodes<<endl;

		    remainingWidth -= thisNodeWidth;

		    wts<<setw(10)<<nodeName<<"  ";
		    wts<<setw(10)<<thisNodeWidth*thisNodeHeight<<" "<<endl;
		  }
		remainingHeight -= thisNodeHeight;
	      }
	  }
	else
	  {
	    if (node.getName())
	      {	
		nodes<<setw(10)<<node.getName()<<"  ";
		wts<<setw(10)<<node.getName()<<"  ";
	      }
	    else   
	      {
		if (isTerminal(nodeId)) 
		  {
		    nodes<<" p"<< nodeId+1 <<" ";
		    wts<<setw(10)<<" p"<<nodeId+1;
		  }
		else
		  {
		    nodes<<" a"<< nodeId-getNumTerminals()<<" ";
		    wts<<setw(10)<<" a"<<nodeId-getNumTerminals();
		  }
	      }
	    
	    if(getNodeWidth(nodeId) != 0 || getNodeHeight(nodeId) != 0)
	      nodes<<setw(10)<<getNodeWidth(nodeId)<<" "
		   <<setw(10)<<getNodeHeight(nodeId)<<" ";
	    if(isTerminal(nodeId)) nodes<<"terminal ";
	    if(sym.rot90 || sym.y || sym.x) nodes << " : "<<sym;
	    nodes<<endl;

	    for(unsigned i=0; i!=getNumWeights(); ++i)
	      wts<<" "<<setw(6)<<node.getWeight(i);
	    wts<<endl;
	  }
    }
    nodes.close();

    unsigned num1PinEdges=0, num0PinEdges=0;
    itHGFEdgeGlobal e;
    for (e=edgesBegin(); e!=edgesEnd(); ++e)
    {
	HGFEdge&	edge   = **e;
        if (edge.getDegree()>1)  continue;
        if (edge.getDegree()==1) num1PinEdges++;
        else                     num0PinEdges++;
    }

	//write the nets file
    nets<<"UCLA nets 1.0"<<endl;
    nets<<TimeStamp()<<User()<<Platform()<<endl;

    nets<<"NumNets : "<<getNumEdges()-num1PinEdges+numExtraNets<<endl;
    nets<<"NumPins : "<<getNumPins() -num1PinEdges+numExtraPins<<endl;
    
    for (e=edgesBegin(); e!=edgesEnd(); ++e)
    {
	HGFEdge&	edge   = **e;
        if (edge.getDegree()<2) continue;
	unsigned	edgeId = edge.getIndex();
        const char*     netName=getNetNameByIndex(edgeId);
	
	nets<<"NetDegree : "<<(*e)->getDegree()<<" ";
        if (netName) nets << setw(10) << netName;
        else if (abs(edge.getWeight()-1.0)>1e-10)
                     nets << " net" << edgeId;
        nets << endl;

	unsigned nodeOffset = 0;

        itHGFNodeLocal v;
	double nodeHeight;
	double nodeWidth;
	unsigned subVertIdx;
	unsigned subHorizIdx;
	unsigned maxSubVertIdx;
	unsigned maxSubHorizIdx;

	double thisNodeHeight;
	double thisNodeWidth;
        for (v=(*e)->srcsBegin(); v!=(*e)->srcsEnd(); v++, nodeOffset++)
        {
            const HGFNode& node=(**v);
            unsigned idx=node.getIndex();

	    char* nodeName  = new char[strlen(node.getName()) + 10];
	    nodeHeight = getNodeHeight(idx);
	    nodeWidth = getNodeWidth(idx);
	    if(_isMacro[idx] == true && !isTerminal(idx))
	      {
		maxSubVertIdx = unsigned(ceil(nodeHeight/maxCoreRowHeight)-1);
		maxSubHorizIdx = unsigned(ceil(nodeWidth/minWidthNode)-1);

		Point pOffset = nodesOnEdgePinOffset(nodeOffset, edgeId);
		double dx=pOffset.x, dy=pOffset.y;

		if(dx < 0)
		  dx = 0;
		if(dy < 0)
		  dy = 0;
		
		if(fabs(dy)<0.0001)
		  subVertIdx = 0;
		else
		  subVertIdx = unsigned(ceil(dy/maxCoreRowHeight)-1);

		if(fabs(dx)<0.0001)
		  subHorizIdx = 0;
		else
		  subHorizIdx = unsigned(ceil(dx/minWidthNode)-1);

		if(subHorizIdx > maxSubHorizIdx)
		  subHorizIdx = maxSubHorizIdx;

		if(subVertIdx > maxSubVertIdx)
		  subVertIdx = maxSubVertIdx;

		sprintf(nodeName, "%s@%d#%d",node.getName(),subVertIdx,
			subHorizIdx);
		nets<<setw(10)<<nodeName<<" O ";

		if(subVertIdx == maxSubVertIdx)
		  thisNodeHeight = nodeHeight-subVertIdx*maxCoreRowHeight;
		else
		  thisNodeHeight = maxCoreRowHeight;

		if(subHorizIdx == maxSubHorizIdx)
		  thisNodeWidth = nodeWidth-subHorizIdx*minWidthNode;
		else
		  thisNodeWidth = minWidthNode;

		dy -= subVertIdx*maxCoreRowHeight;
		dy -= thisNodeHeight/2;
		dx -= subHorizIdx*minWidthNode;
		dx -= thisNodeWidth/2;

		if( abs(dx)>1e-10 || abs(dy)>1e-10)
		  nets<<" : "<<dx <<" "  <<dy<<endl;
		else
		  nets<<endl;
	      }
	    else
	      {
		nets<<setw(10)<<node.getName()<<" O ";
		Point pOffset = nodesOnEdgePinOffset(nodeOffset, edgeId);
		double dx=pOffset.x - _nodeWidths [idx]/2,
		  dy=pOffset.y - _nodeHeights[idx]/2;
		//	    if(pOffset.x != 0 || pOffset.y != 0)
		if( abs(dx)>1e-10 || abs(dy)>1e-10)
		  nets<<" : "<<dx <<" "  <<dy<<endl;
		else
		  nets<<endl;
	      }
        }
        for (v=(*e)->srcSnksBegin(); v!=(*e)->srcSnksEnd(); ++v, nodeOffset++)
        {
            const HGFNode& node=(**v);
            unsigned idx=node.getIndex();

	    char* nodeName  = new char[strlen(node.getName()) + 10];
	    nodeHeight = getNodeHeight(idx);
	    nodeWidth = getNodeWidth(idx);
	    if(_isMacro[idx] == true && !isTerminal(idx))
	      {
		maxSubVertIdx = unsigned(ceil(nodeHeight/maxCoreRowHeight)-1);
		maxSubHorizIdx = unsigned(ceil(nodeWidth/minWidthNode)-1);

		Point pOffset = nodesOnEdgePinOffset(nodeOffset, edgeId);
		double dx=pOffset.x, dy=pOffset.y;

		if(dx < 0)
		  dx = 0;
		if(dy < 0)
		  dy = 0;

		if(fabs(dy)<0.0001)
		  subVertIdx = 0;
		else
		  subVertIdx = unsigned(ceil(dy/maxCoreRowHeight)-1);

		if(fabs(dx)<0.0001)
		  subHorizIdx = 0;
		else
		  subHorizIdx = unsigned(ceil(dx/minWidthNode)-1);

		if(subHorizIdx > maxSubHorizIdx)
		  subHorizIdx = maxSubHorizIdx;

		if(subVertIdx > maxSubVertIdx)
		  subVertIdx = maxSubVertIdx;

		sprintf(nodeName, "%s@%d#%d",node.getName(),subVertIdx,
			subHorizIdx);
		nets<<setw(10)<<nodeName<<" B ";

		if(subVertIdx == maxSubVertIdx)
		  thisNodeHeight = nodeHeight-subVertIdx*maxCoreRowHeight;
		else
		  thisNodeHeight = maxCoreRowHeight;

		if(subHorizIdx == maxSubHorizIdx)
		  thisNodeWidth = nodeWidth-subHorizIdx*minWidthNode;
		else
		  thisNodeWidth = minWidthNode;

		dy -= subVertIdx*maxCoreRowHeight;
		dy -= thisNodeHeight/2;
		dx -= subHorizIdx*minWidthNode;
		dx -= thisNodeWidth/2;

		if( abs(dx)>1e-10 || abs(dy)>1e-10)
		  nets<<" : "<<dx <<" "  <<dy<<endl;
		else
		  nets<<endl;
	      }
	    else
	      {
		nets<<setw(10)<<node.getName()<<" B ";
		
		Point pOffset = nodesOnEdgePinOffset(nodeOffset, edgeId);
		double dx=pOffset.x - _nodeWidths [idx]/2,
		  dy=pOffset.y - _nodeHeights[idx]/2;
		//          if(pOffset.x != 0 || pOffset.y != 0)
		if( abs(dx)>1e-10 || abs(dy)>1e-10)
		  nets<<" : "<<dx <<" "  <<dy<<endl;
		else nets<<endl;
	      }
        }
        for (v=(*e)->snksBegin(); v!=(*e)->snksEnd(); ++v, nodeOffset++)
        {
            const HGFNode& node=(**v);
            unsigned idx=node.getIndex();

	    char* nodeName  = new char[strlen(node.getName()) + 10];
	    nodeHeight = getNodeHeight(idx);
	    nodeWidth = getNodeWidth(idx);
	    if(_isMacro[idx] == true && !isTerminal(idx))
	      {
		maxSubVertIdx = unsigned(ceil(nodeHeight/maxCoreRowHeight)-1);
		maxSubHorizIdx = unsigned(ceil(nodeWidth/minWidthNode)-1);

		Point pOffset = nodesOnEdgePinOffset(nodeOffset, edgeId);
		double dx=pOffset.x, dy=pOffset.y;

		if(dx < 0)
		  dx = 0;
		if(dy < 0)
		  dy = 0;

		if(fabs(dy)<0.0001)
		  subVertIdx = 0;
		else
		  subVertIdx = unsigned(ceil(dy/maxCoreRowHeight)-1);

		if(fabs(dx)<0.0001)
		  subHorizIdx = 0;
		else
		  subHorizIdx = unsigned(ceil(dx/minWidthNode)-1);

		if(subHorizIdx > maxSubHorizIdx)
		  subHorizIdx = maxSubHorizIdx;

		if(subVertIdx > maxSubVertIdx)
		  subVertIdx = maxSubVertIdx;

		sprintf(nodeName, "%s@%d#%d",node.getName(),subVertIdx,
			subHorizIdx);
		nets<<setw(10)<<nodeName<<" I ";

		if(subVertIdx == maxSubVertIdx)
		  thisNodeHeight = nodeHeight-subVertIdx*maxCoreRowHeight;
		else
		  thisNodeHeight = maxCoreRowHeight;

		if(subHorizIdx == maxSubHorizIdx)
		  thisNodeWidth = nodeWidth-subHorizIdx*minWidthNode;
		else
		  thisNodeWidth = minWidthNode;

		dy -= subVertIdx*maxCoreRowHeight;
		dy -= thisNodeHeight/2;
		dx -= subHorizIdx*minWidthNode;
		dx -= thisNodeWidth/2;

		if( abs(dx)>1e-10 || abs(dy)>1e-10)
		  nets<<" : "<<dx <<" "  <<dy<<endl;
		else
		  nets<<endl;
	      }
	    else
	      {
		nets<<setw(10)<<node.getName()<<" I ";
		
		Point pOffset = nodesOnEdgePinOffset(nodeOffset, edgeId);
		double dx=pOffset.x - _nodeWidths [idx]/2,
		  dy=pOffset.y - _nodeHeights[idx]/2;
		
		//	    if(pOffset.x != 0 || pOffset.y != 0)
		if( abs(dx)>1e-10 || abs(dy)>1e-10)
		  nets<<" : "<<dx <<" "  <<dy<<endl;
		else nets<<endl;
	      }
        }
    }


    //write the wts for nets
    double maxWeight = 0;
    double weight;
    int n;
    //non fake nets should have 10 times weight of fake nets
    for(n=0; n!=int(getNumEdges()); ++n)
    {
        const HGFEdge& net=getEdgeByIdx(n);
	weight = net.getWeight();

        if (abs(weight-1.0)<1e-10) continue;
        wts << " ";
        const char* netName=getNetNameByIndex(n);
        if (netName) wts << setw(10) << netName ;
        else wts << "net" << n;

        //wts << " " << setw(10) << 10*weight << endl;
	wts << " " << setw(10) << weight << endl;	
	if(maxWeight > weight)
	  maxWeight = weight;
    }

    if(maxWeight == 0)
      maxWeight = 1;
    else
      maxWeight *= 1;


    //add edges to shredded cells
    unsigned netId = 0;
    for(nIt = nodesBegin(); nIt != nodesEnd(); nIt++)
      {
	HGFNode& node   = **nIt;
	unsigned nodeId = node.getIndex();
	if(_isMacro[nodeId] == true && !isTerminal(nodeId)) 
	  //shred these nodes
	  {

	    if(!useSingleNet)
	      {
		double nodeHeight = getNodeHeight(nodeId);
		double nodeWidth = getNodeWidth(nodeId);
/*
		double dx;
		double dyAbove = -maxCoreRowHeight/2;
		double dyBelow = maxCoreRowHeight/2;
		double dy;
		double dxLeft = minWidthNode/2;
		double dxRight = -minWidthNode/2;
*/
		
		char* nodeName  = new char[strlen(node.getName()) + 8];
		char* nodeNameNext  = new char[strlen(node.getName()) + 8];
		char* netName  = new char[15];
		
		unsigned numNewVertNodes = unsigned(ceil(nodeHeight/maxCoreRowHeight));
		unsigned numNewHorizNodes = unsigned(ceil(nodeWidth/minWidthNode));
		
		for(unsigned i=0; i<numNewVertNodes; ++i)
		  {
		    for(unsigned j=0; j<numNewHorizNodes; ++j)
		      {
			if(j != numNewHorizNodes - 1)
			  {
			    sprintf(nodeName, "%s@%d#%d",node.getName(),i,j);
			    sprintf(nodeNameNext, "%s@%d#%d",node.getName(),i,j+1);
			    
			    for(unsigned k = 0; k<NUM_NEW_HORIZ_NETS; ++k)
			      {
				sprintf(netName, "temp@%d",netId);
				netId++;
				
				nets<<"NetDegree : 2 ";
				nets << setw(10) << netName <<endl;
				
				/*
				  dy = k*(maxCoreRowHeight/(NUM_NEW_HORIZ_NETS-1));
				  dy -= maxCoreRowHeight/2;
				  
				  nets<<setw(10)<<nodeName<<" B ";
				  nets<<" : "<<dxLeft <<" "  <<dy<<endl;
				  nets<<setw(10)<<nodeNameNext<<" B ";
				  nets<<" : "<<dxRight <<" " <<dy<<endl;
				*/
				nets<<setw(10)<<nodeName<<" B "<<endl;;
				nets<<setw(10)<<nodeNameNext<<" B "<<endl;
				
				wts << setw(10) << netName ;
				wts << " " << setw(10) << maxWeight << endl;
			      }
			  }
			
			if(i != numNewVertNodes - 1)
			  {
			    sprintf(nodeName, "%s@%d#%d",node.getName(),i,j);
			    sprintf(nodeNameNext, "%s@%d#%d",node.getName(),i+1,j);
			    
			    for(unsigned k = 0; k<NUM_NEW_VERT_NETS; ++k)
			      {
				sprintf(netName, "temp@%d",netId);
				netId++;
				
				nets<<"NetDegree : 2 ";
				nets << setw(10) << netName <<endl;
				/*
				  dx = k*(minWidthNode/(NUM_NEW_VERT_NETS-1));
				  dx -= minWidthNode/2;
				  
				  nets<<setw(10)<<nodeName<<" B ";
				  nets<<" : "<<dx <<" "  <<dyBelow<<endl;
				  nets<<setw(10)<<nodeNameNext<<" B ";
				  nets<<" : "<<dx <<" "  <<dyAbove<<endl;
				*/
				nets<<setw(10)<<nodeName<<" B "<<endl;;
				nets<<setw(10)<<nodeNameNext<<" B "<<endl;
				
				wts << setw(10) << netName ;
				wts << " " << setw(10) << maxWeight << endl;
			      }
			  }
		      }
		  }
	      }
	    else //add single nets instead of distributed nets
	      {
		double nodeHeight = getNodeHeight(nodeId);
		double nodeWidth = getNodeWidth(nodeId);
		char* nodeName  = new char[strlen(node.getName()) + 8];
		char* netName  = new char[15];
		
		unsigned numNewVertNodes = unsigned(ceil(nodeHeight/maxCoreRowHeight));
		unsigned numNewHorizNodes = unsigned(ceil(nodeWidth/minWidthNode));
		for(unsigned l=0; l<numNewSingleNets; ++l)
		  {
		    sprintf(netName, "temp@%d",netId);
		    netId++;
		    nets<<"NetDegree :  "<<(numNewVertNodes*numNewHorizNodes);
		    nets <<" "<< setw(10) << netName <<endl;
		    wts << setw(10) << netName ;
		    wts << " " << setw(10) << maxWeight << endl;
		    for(unsigned i=0; i<numNewVertNodes; ++i)
		      {
			for(unsigned j=0; j<numNewHorizNodes; ++j)
			  {
			    sprintf(nodeName, "%s@%d#%d",node.getName(),i,j);
			    nets<<setw(10)<<nodeName<<" B "<<endl;;
			  }
		      }
		  }
	      }
	  }
      }
    
    nets.close();
    wts.close();

    aux<<"HGraphWPins : ";

    const char * tmp=NULL;
    tmp=strrchr(baseFileName,pathDelim);
    if (tmp++)
        aux<<tmp<<".nets "<<tmp<<".nodes "<<tmp<<".wts"<<endl;
    else
        aux<<baseFileName<<".nets "<<baseFileName<<".nodes "
	   <<baseFileName<<".wts"<<endl;
    aux.close();

    delete []fname;
}


void HGraphWDimensions::saveAsNodesNetsFloorplan(const char* baseFileName) const
{
    char* fname  = new char[strlen(baseFileName) + 8];
    strcpy(fname, baseFileName);
    char* extPtr = fname + strlen(baseFileName);

    strcpy(extPtr, ".nets");
    ofstream nets(fname);

    strcpy(extPtr, ".blocks");
    ofstream nodes(fname);

	//write the nodes file
    nodes<<"UCLA blocks 1.0"<<endl;
    nodes<<"#"<<TimeStamp()<<User()<<Platform()<<endl;

    nodes<<"NumSoftRectangularBlocks :     "<<setw(10)
	 <<getNumNodes()-getNumTerminals()<<endl;
    nodes<<"NumHardRectilinearBlocks :     "<<setw(10)<<0<<endl;
    nodes<<"NumTerminals : "<<setw(10)<<getNumTerminals()<<endl;

    itHGFNodeGlobal nIt;
    for(nIt = nodesBegin(); nIt != nodesEnd(); nIt++)
    {
	HGFNode& node   = **nIt;
	unsigned nodeId = node.getIndex();
	Symmetry sym    = _nodeSymmetries[nodeId];

        if (node.getName())
	  {	
	    nodes<<setw(10)<<node.getName()<<" ";
	  }
        else   
	  {
	    if (isTerminal(nodeId)) nodes<<" p"<< nodeId+1 <<" ";
            else                   nodes<<" a"<< nodeId-getNumTerminals()<<" ";
	  }
	
	if(isTerminal(nodeId))
	  nodes<<"terminal ";
	else
	  {
	    double area = getNodeWidth(nodeId)*getNodeHeight(nodeId);
	    double ar = getNodeWidth(nodeId)/getNodeHeight(nodeId);
	    nodes<<"softrectangular ";
	    nodes<<area<<" ";
	    nodes<<ar<<" "<<ar;
	    if(sym.rot90 || sym.y || sym.x) nodes << " : "<<sym;
	  }
	nodes<<endl;
    }
    nodes.close();

    unsigned num1PinEdges=0, num0PinEdges=0;
    itHGFEdgeGlobal e;
    for (e=edgesBegin(); e!=edgesEnd(); ++e)
    {
	HGFEdge&	edge   = **e;
        if (edge.getDegree()>1)  continue;
        if (edge.getDegree()==1) num1PinEdges++;
        else                     num0PinEdges++;
    }
    
	//write the nets file
    nets<<"UCLA nets 1.0"<<endl;
    nets<<"#"<<TimeStamp()<<User()<<Platform()<<endl;

    nets<<"NumNets : "<<getNumEdges()-num1PinEdges<<endl;
    nets<<"NumPins : "<<getNumPins() -num1PinEdges<<endl;
    
    for (e=edgesBegin(); e!=edgesEnd(); ++e)
    {
	HGFEdge&	edge   = **e;
        if (edge.getDegree()<2) continue;
	unsigned	edgeId = edge.getIndex();
        //const char*     netName=getNetNameByIndex(edgeId);
	
	nets<<"NetDegree : "<<(*e)->getDegree()<<" ";
        nets << endl;

	unsigned nodeOffset = 0;

        itHGFNodeLocal v;
        for (v=(*e)->srcsBegin(); v!=(*e)->srcsEnd(); v++, nodeOffset++)
        {
            const HGFNode& node=(**v);
            unsigned idx=node.getIndex();
	    nets<<setw(10)<<node.getName()<<" O ";

	    Point pOffset = nodesOnEdgePinOffset(nodeOffset, edgeId);
            double dx=pOffset.x - _nodeWidths [idx]/2,
                   dy=pOffset.y - _nodeHeights[idx]/2;
	    dx = (dx/_nodeWidths [idx])*100;
	    dy = (dy/_nodeHeights [idx])*100;

	    if( abs(dx)>1e-10 || abs(dy)>1e-10)
		nets<<" : %"<<dx <<" %"  <<dy<<endl;
	    else
	 	nets<<endl;
        }
        for (v=(*e)->srcSnksBegin(); v!=(*e)->srcSnksEnd(); ++v, nodeOffset++)
        {
            const HGFNode& node=(**v);
            unsigned idx=node.getIndex();
	    nets<<setw(10)<<node.getName()<<" B ";

	    Point pOffset = nodesOnEdgePinOffset(nodeOffset, edgeId);
            double dx=pOffset.x - _nodeWidths [idx]/2,
                   dy=pOffset.y - _nodeHeights[idx]/2;
	    dx = (dx/_nodeWidths [idx])*100;
	    dy = (dy/_nodeHeights [idx])*100;

	    if( abs(dx)>1e-10 || abs(dy)>1e-10)
		nets<<" : %"<<dx <<" %"  <<dy<<endl;
	    else
	 	nets<<endl;
        }
        for (v=(*e)->snksBegin(); v!=(*e)->snksEnd(); ++v, nodeOffset++)
        {
            const HGFNode& node=(**v);
            unsigned idx=node.getIndex();
	    nets<<setw(10)<<node.getName()<<" I ";

	    Point pOffset = nodesOnEdgePinOffset(nodeOffset, edgeId);
            double dx=pOffset.x - _nodeWidths [idx]/2,
                   dy=pOffset.y - _nodeHeights[idx]/2;
	    dx = (dx/_nodeWidths [idx])*100;
	    dy = (dy/_nodeHeights [idx])*100;

	    if( abs(dx)>1e-10 || abs(dy)>1e-10)
		nets<<" : %"<<dx <<" %"  <<dy<<endl;
	    else
	 	nets<<endl;
        }
    }
    nets.close();
}


void HGraphWDimensions::saveMacrosAsNodesNetsFloorplan 
                                               (const char* baseFileName) const
{
    char* fname  = new char[strlen(baseFileName) + 8];
    strcpy(fname, baseFileName);
    char* extPtr = fname + strlen(baseFileName);

    strcpy(extPtr, ".nets");
    ofstream nets(fname);

    strcpy(extPtr, ".blocks");
    ofstream nodes(fname);

    unsigned numNodes = 0;
    itHGFNodeGlobal nIt;
    for(nIt = nodesBegin(); nIt != nodesEnd(); nIt++)
      {
	HGFNode& node   = **nIt;
	unsigned nodeId = node.getIndex();
	if(!isTerminal(nodeId))
	  {
	    if(_isMacro[nodeId])
	      ++numNodes;
	  }
      }

	//write the nodes file
    nodes<<"UCLA blocks 1.0"<<endl;
    nodes<<"#"<<TimeStamp()<<User()<<Platform()<<endl;

    nodes<<"NumSoftRectangularBlocks :     "<<setw(10)<<numNodes<<endl;
    nodes<<"NumHardRectilinearBlocks :     "<<setw(10)<<0<<endl;
    nodes<<"NumTerminals : "<<setw(10)<<getNumTerminals()<<endl;

    for(nIt = nodesBegin(); nIt != nodesEnd(); nIt++)
    {
	HGFNode& node   = **nIt;
	unsigned nodeId = node.getIndex();
	Symmetry sym    = _nodeSymmetries[nodeId];

	if(isTerminal(nodeId))
	  {
	    if (node.getName())
	      nodes<<setw(10)<<node.getName()<<" ";
	    else   
	      nodes<<" p"<< nodeId+1 <<" ";
	    nodes<<"terminal ";
	    nodes<<endl;
	  }
	else
	  {
	    if(_isMacro[nodeId])
	      {
		if (node.getName())
		  nodes<<setw(10)<<node.getName()<<" ";
		else   
		  nodes<<" a"<< nodeId-getNumTerminals()<<" ";

		nodes<<"softrectangular ";
		nodes<<getNodeWidth(nodeId)*getNodeHeight(nodeId)<<" ";
		nodes<<"1.0 1.0";
		if(sym.rot90 || sym.y || sym.x) nodes << " : "<<sym;
		nodes<<endl;
	      }
	  }
    }
    nodes.close();

	//write the nets file
    nets<<"UCLA nets 1.0"<<endl;
    nets<<"#"<<TimeStamp()<<User()<<Platform()<<endl;

    nets<<"NumNets : "<<0<<endl;
    nets<<"NumPins : "<<0<<endl;
    nets.close();
}


void HGraphWDimensions::saveAsNodesNetsWtsWDummy(const char* baseFileName,
					  double siteWidth, double siteHeight, 
				        double areaDummy, bool fixMacros
				     ) const
{
    char* fname  = new char[strlen(baseFileName) + 8];
    strcpy(fname, baseFileName);
    char* extPtr = fname + strlen(baseFileName);

    strcpy(extPtr, ".nets");
    ofstream nets(fname);

    strcpy(extPtr, ".nodes");
    ofstream nodes(fname);

    strcpy(extPtr, ".wts");
    ofstream wts (fname);

    strcpy(extPtr, ".aux");
    ofstream aux (fname);

    itHGFNodeGlobal nIt;

    //get number of macros
    unsigned numMacros = 0;
    if(fixMacros)
      {
	for(nIt = nodesBegin(); nIt != nodesEnd(); nIt++)
	  {
	    HGFNode& node   = **nIt;
	    unsigned nodeId = node.getIndex();
	    if(!isTerminal(nodeId))
	      {
		if(_isMacro[nodeId])
		  ++numMacros;
	      }
	  }
      }
	//write the nodes file
    double avgWidth=0;
    double numCore=0;

    for(nIt = nodesBegin(); nIt != nodesEnd(); nIt++)
      {
	HGFNode& node   = **nIt;
	unsigned nodeId = node.getIndex();
	if(isTerminal(nodeId) || _isMacro[nodeId])
	  continue;
	numCore++;
	avgWidth += getNodeWidth(nodeId);
      }
    avgWidth /= numCore;
    double dummyWidth = 4*floor(avgWidth/siteWidth)*siteWidth;
    double dummyHeight = siteHeight;
    double dummyArea = dummyWidth*dummyHeight;
    unsigned numDummy = unsigned(floor(areaDummy/dummyArea));

    nodes<<"UCLA nodes 1.0"<<endl;
    nodes<<TimeStamp()<<User()<<Platform()<<endl;

    nodes<<"NumNodes :     "<<setw(10)<<getNumNodes()+numDummy<<endl;
    nodes<<"NumTerminals : "<<setw(10)<<getNumTerminals()+numMacros<<endl;

    for(nIt = nodesBegin(); nIt != nodesEnd(); nIt++)
    {
	HGFNode& node   = **nIt;
	unsigned nodeId = node.getIndex();
	Symmetry sym    = _nodeSymmetries[nodeId];

        if (node.getName())	nodes<<setw(10)<<node.getName()<<"  ";
        else   
        {
            if (isTerminal(nodeId)) nodes<<" p"<< nodeId+1 <<" ";
            else                   nodes<<" a"<< nodeId-getNumTerminals()<<" ";
        }

	if(getNodeWidth(nodeId) != 0 || getNodeHeight(nodeId) != 0)
	    nodes<<setw(10)<<getNodeWidth(nodeId)<<" "
                 <<setw(10)<<getNodeHeight(nodeId)<<" ";
	if(isTerminal(nodeId)) 
	  nodes<<"terminal ";
	else if(fixMacros && _isMacro[nodeId])
	  nodes<<"terminal ";

	if(sym.rot90 || sym.y || sym.x) nodes << " : "<<sym;
	nodes<<endl;
    }
    for(unsigned i=0; i<numDummy; ++i)
      nodes<<setw(10)<<"dummy"<<i<<" "<<dummyWidth<<"  "<<dummyHeight<<endl;

    nodes.close();

    unsigned num1PinEdges=0, num0PinEdges=0;
    itHGFEdgeGlobal e;
    for (e=edgesBegin(); e!=edgesEnd(); ++e)
    {
	HGFEdge&	edge   = **e;
        if (edge.getDegree()>1)  continue;
        if (edge.getDegree()==1) num1PinEdges++;
        else                     num0PinEdges++;
    }

	//write the nets file
    nets<<"UCLA nets 1.0"<<endl;
    nets<<TimeStamp()<<User()<<Platform()<<endl;

    nets<<"NumNets : "<<getNumEdges()-num1PinEdges<<endl;
    nets<<"NumPins : "<<getNumPins() -num1PinEdges<<endl;
    
    for (e=edgesBegin(); e!=edgesEnd(); ++e)
    {
	HGFEdge&	edge   = **e;
        if (edge.getDegree()<2) continue;
	unsigned	edgeId = edge.getIndex();
        const char*     netName=getNetNameByIndex(edgeId);
	
	nets<<"NetDegree : "<<(*e)->getDegree()<<" ";
        if (netName) nets << setw(10) << netName;
        else if (abs(edge.getWeight()-1.0)>1e-10)
                     nets << " net" << edgeId;
        else
          nets << " net" << edgeId;
        nets << endl;

	unsigned nodeOffset = 0;

        itHGFNodeLocal v;
        for (v=(*e)->srcsBegin(); v!=(*e)->srcsEnd(); v++, nodeOffset++)
        {
            const HGFNode& node=(**v);
            unsigned idx=node.getIndex();
	    nets<<setw(10)<<node.getName()<<" O ";

	    Point pOffset = nodesOnEdgePinOffset(nodeOffset, edgeId);
            double dx=pOffset.x - _nodeWidths [idx]/2,
                   dy=pOffset.y - _nodeHeights[idx]/2;
//	    if(pOffset.x != 0 || pOffset.y != 0)
	    if( abs(dx)>1e-10 || abs(dy)>1e-10)
		nets<<" : "<<dx <<" "  <<dy<<endl;
	    else
	 	nets<<endl;
        }
        for (v=(*e)->srcSnksBegin(); v!=(*e)->srcSnksEnd(); ++v, nodeOffset++)
        {
            const HGFNode& node=(**v);
            unsigned idx=node.getIndex();
	    nets<<setw(10)<<node.getName()<<" B ";

	    Point pOffset = nodesOnEdgePinOffset(nodeOffset, edgeId);
            double dx=pOffset.x - _nodeWidths [idx]/2,
                   dy=pOffset.y - _nodeHeights[idx]/2;
//          if(pOffset.x != 0 || pOffset.y != 0)
	    if( abs(dx)>1e-10 || abs(dy)>1e-10)
		nets<<" : "<<dx <<" "  <<dy<<endl;
	    else nets<<endl;
        }
        for (v=(*e)->snksBegin(); v!=(*e)->snksEnd(); ++v, nodeOffset++)
        {
            const HGFNode& node=(**v);
            unsigned idx=node.getIndex();
	    nets<<setw(10)<<node.getName()<<" I ";

	    Point pOffset = nodesOnEdgePinOffset(nodeOffset, edgeId);
            double dx=pOffset.x - _nodeWidths [idx]/2,
                   dy=pOffset.y - _nodeHeights[idx]/2;

//	    if(pOffset.x != 0 || pOffset.y != 0)
	    if( abs(dx)>1e-10 || abs(dy)>1e-10)
		nets<<" : "<<dx <<" "  <<dy<<endl;
	
	    else nets<<endl;
        }
    }
    nets.close();

		//write the wts file

    wts<<"UCLA wts 1.0"<<endl;
    wts<<TimeStamp()<<User()<<Platform()<<endl;

    unsigned n;
    if(getNumWeights() >= 1)
    {
      for(n=0; n!=getNumNodes(); n++)
      {
	HGFNode& node = *_nodes[n];
	wts<<setw(10)<<node.getName()<<"  ";

        for(unsigned i=0; i!=getNumWeights(); ++i)
	    wts<<" "<<setw(6)<< node.getWeight(i);

        wts<<endl;
      }
    }

    for(n=0; n!=getNumEdges(); ++n)
    {
        const HGFEdge& net=getEdgeByIdx(n);
        if (abs(net.getWeight()-1.0)<1e-10) continue;
        wts << " ";
        const char* netName=getNetNameByIndex(n);
        if (netName) wts << setw(10) << netName ;
        else wts << "net" << n;
        wts << " " << setw(10) << net.getWeight() << endl;
    }

    wts.close();

    aux<<"HGraphWPins : ";

    const char * tmp=NULL;
    tmp=strrchr(baseFileName,pathDelim);
    if (tmp++)
        aux<<tmp<<".nets "<<tmp<<".nodes "<<tmp<<".wts"<<endl;
    else
        aux<<baseFileName<<".nets "<<baseFileName<<".nodes "
	   <<baseFileName<<".wts"<<endl;
    aux.close();

    delete []fname;
}

void HGraphWDimensions::getCellsToTetherBFS(vector<unsigned>& tetheredCells,
					    unsigned numTetherCells,
					    bool takeConstrFrmFile) const
{
  unsigned numActTerms = getNumTerminals();
  unsigned numNodes = getNumNodes();
  vector<bool> iamTethered(numNodes, false);

  if(0)
    {
      for(unsigned i=0; i<numTetherCells;)
	{
	  unsigned randIdx = RandomUnsigned(numActTerms, numNodes);
	  if(isTerminal(randIdx) || iamTethered[randIdx])
	    continue;
	  tetheredCells.push_back(randIdx);
	  iamTethered[randIdx]=true;
	  ++i;
	}
    }
  else
    {
      vector<bool> seenCells(numNodes, false);
      while(1)
	{
	  deque<unsigned> cells;
	  for(unsigned i=numActTerms; i<numNodes; ++i)
	    {
	      if(!seenCells[i])
		{
		  cells.push_back(i);
		  break;
		}
	    }
	  if(cells.size()==0)
	    break;
	  //the BFS loop
	  seenCells[cells[0]] = true;
	  vector<unsigned> groupCells;
	  groupCells.push_back(cells[0]);
	  while(1)
	    {
	      if(cells.size()==0)
		break;
	      unsigned rootIdx = cells[0];
	      const HGFNode& origNode = getNodeByIdx(rootIdx);
	      itHGFEdgeLocal e;
	      itHGFNodeLocal v;
	      for(e = origNode.edgesBegin(); e != origNode.edgesEnd(); ++e)
		{
		  for (v=(*e)->nodesBegin(); v!=(*e)->nodesEnd(); v++)
		    {
		      HGFNode& node = (**v);
		      unsigned nodeIndex = node.getIndex();
		      if(!seenCells[nodeIndex])
			{
			  seenCells[nodeIndex] = true;
			  cells.push_back(nodeIndex);
			  groupCells.push_back(nodeIndex);
			}
		    }
		}
	      //std::vector<unsigned>::iterator cellsBegin = cells.begin();
	      //cout<<"cellsBegin "<<*cellsBegin<<endl;
	      //cells.erase(cellsBegin);
	      cells.pop_front();
	    }
	  for(unsigned i=0; i<groupCells.size(); ++i)
	    {
	      unsigned randIdx =  RandomUnsigned(0, groupCells.size());
	      if(!iamTethered[groupCells[randIdx]] && 
		 !isTerminal(groupCells[randIdx]))
		{
		  tetheredCells.push_back(groupCells[randIdx]);
		  iamTethered[groupCells[randIdx]]=true;
		  break;
		}
	    }
	  cout<<"Group Size "<<groupCells.size()<<endl;
	  groupCells.clear();
	  cells.clear();
	}
      unsigned numBFSTethered = tetheredCells.size();

      //now check if a constraint file specified.
      if(takeConstrFrmFile)
	{
	  ifstream constrFile("constraints.nodes");
	  abkfatal(constrFile, " Could not open constraints.nodes");
	  cout << " Reading constraints.nodes ... " << endl;
	  char     nodeName[1024];
	  int lineno = 0;

	  constrFile>>eathash(lineno);
	  while(!constrFile.eof())
	    {
	      constrFile>>eathash(lineno)>>nodeName>>eatblank;
	      unsigned nodeId = getNodeByName(nodeName).getIndex();
	      if(!isTerminal(nodeId) && !iamTethered[nodeId])
		{
		  tetheredCells.push_back(nodeId);
		  iamTethered[nodeId]=true;
		}
	      constrFile>>skiptoeol>>eathash(lineno);
	    }
	  constrFile.close();
	}
      unsigned numFileTethered = tetheredCells.size() - numBFSTethered;
      //the remaining cells are random
      for(unsigned i=tetheredCells.size(); i<numTetherCells;)
        {
          unsigned randIdx = RandomUnsigned(numActTerms, numNodes);
          if(isTerminal(randIdx) || iamTethered[randIdx])
            continue;
          tetheredCells.push_back(randIdx);
	  iamTethered[randIdx]=true;
          ++i;
        }
      unsigned numRandomTethered = 
	tetheredCells.size()-numBFSTethered-numFileTethered;
      cout<<"numBFSTethered "<<numBFSTethered<<" numFileTethered "<<numFileTethered<<" numRandomTethered "<<numRandomTethered<<endl;
    }
}

void HGraphWDimensions::saveAsNodesNetsWtsWTether(const char* baseFileName,
			   double fract, vector<unsigned>& tetheredCells,
			   bool takeConstrFrmFile) const
{
    char* fname  = new char[strlen(baseFileName) + 8];
    strcpy(fname, baseFileName);
    char* extPtr = fname + strlen(baseFileName);

    strcpy(extPtr, ".nets");
    ofstream nets(fname);

    strcpy(extPtr, ".nodes");
    ofstream nodes(fname);

    strcpy(extPtr, ".wts");
    ofstream wts (fname);

    strcpy(extPtr, ".aux");
    ofstream aux (fname);

    itHGFNodeGlobal nIt;

    //get number of macros
    unsigned numMacros=0;
    bool fixMacros=0;
    if(fixMacros)
      {
	for(nIt = nodesBegin(); nIt != nodesEnd(); nIt++)
	  {
	    HGFNode& node   = **nIt;
	    unsigned nodeId = node.getIndex();
	    if(!isTerminal(nodeId))
	      {
		if(_isMacro[nodeId])
		  ++numMacros;
	      }
	  }
      }
	//write the nodes file
//  double avgWidth=0;
    double numCore=getNumNodes()-getNumTerminals();

    unsigned numTetherCells = unsigned(floor(fract*numCore/100));
    unsigned numTetherTerms = 4*numTetherCells;
//  unsigned numActTerms = getNumTerminals();
//  unsigned numNodes = getNumNodes();

    getCellsToTetherBFS(tetheredCells, numTetherCells, takeConstrFrmFile);
    if(numTetherCells != tetheredCells.size())
      {
        cout<<"numTetherCells "<<numTetherCells<<" vs. tetheredCells.size() "
            <<tetheredCells.size()<<endl;
        abkwarn(0,"numTetherCells != tetheredCells.size() \n");
      }
    numTetherCells = tetheredCells.size();
    numTetherTerms = 4*numTetherCells;

    nodes<<"UCLA nodes 1.0"<<endl;
    nodes<<TimeStamp()<<User()<<Platform()<<endl;

    nodes<<"NumNodes :     "<<setw(10)<<getNumNodes()+numTetherTerms<<endl;
    nodes<<"NumTerminals : "<<setw(10)<<getNumTerminals()+numMacros+numTetherTerms<<endl;

    unsigned numTermsAdded=0;
    for(unsigned i=0; i<numTetherCells; ++i)
      {
	for(unsigned j=0; j<4; ++j)
	  {
	   nodes<<setw(10)<<"dummyTerm"<<numTermsAdded<<" 1 1 terminal "<<endl;
	   ++numTermsAdded;
	  }
      }

    if(numTermsAdded != numTetherTerms)
      {
	cout<<"numTermsAdded "<<numTermsAdded<<" numTermsExpected "<<numTetherTerms<<endl;
	abkfatal(0, "numTermsAdded != numTetherTerms\n");
      }

    for(nIt = nodesBegin(); nIt != nodesEnd(); nIt++)
    {
	HGFNode& node   = **nIt;
	unsigned nodeId = node.getIndex();
	Symmetry sym    = _nodeSymmetries[nodeId];

        if (node.getName())	nodes<<setw(10)<<node.getName()<<"  ";
        else   
        {
            if (isTerminal(nodeId)) nodes<<" p"<< nodeId+1 <<" ";
            else                   nodes<<" a"<< nodeId-getNumTerminals()<<" ";
        }

	if(getNodeWidth(nodeId) != 0 || getNodeHeight(nodeId) != 0)
	    nodes<<setw(10)<<getNodeWidth(nodeId)<<" "
                 <<setw(10)<<getNodeHeight(nodeId)<<" ";
	if(isTerminal(nodeId)) 
	  nodes<<"terminal ";
	else if(fixMacros && _isMacro[nodeId])
	  nodes<<"terminal ";

	if(sym.rot90 || sym.y || sym.x) nodes << " : "<<sym;
	nodes<<endl;
    }

    nodes.close();

    unsigned num1PinEdges=0, num0PinEdges=0;
    itHGFEdgeGlobal e;
    for (e=edgesBegin(); e!=edgesEnd(); ++e)
    {
	HGFEdge&	edge   = **e;
        if (edge.getDegree()>1)  continue;
        if (edge.getDegree()==1) num1PinEdges++;
        else                     num0PinEdges++;
    }

    unsigned numTetherNets = numTetherTerms;

	//write the nets file
    nets<<"UCLA nets 1.0"<<endl;
    nets<<TimeStamp()<<User()<<Platform()<<endl;

    nets<<"NumNets : "<<getNumEdges()-num1PinEdges+numTetherNets<<endl;
    nets<<"NumPins : "<<getNumPins() -num1PinEdges+2*numTetherNets<<endl;
    
    unsigned tetherTermIdx=0;
    for(unsigned i=0; i<numTetherCells; ++i)
      {
	const HGFNode& node = getNodeByIdx(tetheredCells[i]);
	for(unsigned j=0; j<4; ++j)
	  {
	    nets<<"NetDegree : 2 "<<endl;
	    nets<<setw(10)<<node.getName()<<" B "<<endl;
	    nets<<setw(10)<<"dummyTerm"<<tetherTermIdx<<" B "<<endl;
	    ++tetherTermIdx;
	  }
      }

    for (e=edgesBegin(); e!=edgesEnd(); ++e)
    {
	HGFEdge&	edge   = **e;
        if (edge.getDegree()<2) continue;
	unsigned	edgeId = edge.getIndex();
        const char*     netName=getNetNameByIndex(edgeId);
	
	nets<<"NetDegree : "<<(*e)->getDegree()<<" ";
        if (netName) nets << setw(10) << netName;
        else if (abs(edge.getWeight()-1.0)>1e-10)
                     nets << " net" << edgeId;
        else
          nets << " net" << edgeId;
        nets << endl;

	unsigned nodeOffset = 0;

        itHGFNodeLocal v;
        for (v=(*e)->srcsBegin(); v!=(*e)->srcsEnd(); v++, nodeOffset++)
        {
            const HGFNode& node=(**v);
            unsigned idx=node.getIndex();
	    nets<<setw(10)<<node.getName()<<" O ";

	    Point pOffset = nodesOnEdgePinOffset(nodeOffset, edgeId);
            double dx=pOffset.x - _nodeWidths [idx]/2,
                   dy=pOffset.y - _nodeHeights[idx]/2;
//	    if(pOffset.x != 0 || pOffset.y != 0)
	    if( abs(dx)>1e-10 || abs(dy)>1e-10)
		nets<<" : "<<dx <<" "  <<dy<<endl;
	    else
	 	nets<<endl;
        }
        for (v=(*e)->srcSnksBegin(); v!=(*e)->srcSnksEnd(); ++v, nodeOffset++)
        {
            const HGFNode& node=(**v);
            unsigned idx=node.getIndex();
	    nets<<setw(10)<<node.getName()<<" B ";

	    Point pOffset = nodesOnEdgePinOffset(nodeOffset, edgeId);
            double dx=pOffset.x - _nodeWidths [idx]/2,
                   dy=pOffset.y - _nodeHeights[idx]/2;
//          if(pOffset.x != 0 || pOffset.y != 0)
	    if( abs(dx)>1e-10 || abs(dy)>1e-10)
		nets<<" : "<<dx <<" "  <<dy<<endl;
	    else nets<<endl;
        }
        for (v=(*e)->snksBegin(); v!=(*e)->snksEnd(); ++v, nodeOffset++)
        {
            const HGFNode& node=(**v);
            unsigned idx=node.getIndex();
	    nets<<setw(10)<<node.getName()<<" I ";

	    Point pOffset = nodesOnEdgePinOffset(nodeOffset, edgeId);
            double dx=pOffset.x - _nodeWidths [idx]/2,
                   dy=pOffset.y - _nodeHeights[idx]/2;

//	    if(pOffset.x != 0 || pOffset.y != 0)
	    if( abs(dx)>1e-10 || abs(dy)>1e-10)
		nets<<" : "<<dx <<" "  <<dy<<endl;
	
	    else nets<<endl;
        }
    }
    nets.close();

		//write the wts file

    wts<<"UCLA wts 1.0"<<endl;
    wts<<TimeStamp()<<User()<<Platform()<<endl;

    unsigned n;
    if(getNumWeights() >= 1)
    {
      for(n=0; n!=getNumNodes(); n++)
      {
	HGFNode& node = *_nodes[n];
	wts<<setw(10)<<node.getName()<<"  ";

        for(unsigned i=0; i!=getNumWeights(); ++i)
	    wts<<" "<<setw(6)<< node.getWeight(i);

        wts<<endl;
      }
    }

    for(n=0; n!=getNumEdges(); ++n)
    {
        const HGFEdge& net=getEdgeByIdx(n);
        if (abs(net.getWeight()-1.0)<1e-10) continue;
        wts << " ";
        const char* netName=getNetNameByIndex(n);
        if (netName) wts << setw(10) << netName ;
        else wts << "net" << n;
        wts << " " << setw(10) << net.getWeight() << endl;
    }

    wts.close();

    aux<<"HGraphWPins : ";

    const char * tmp=NULL;
    tmp=strrchr(baseFileName,pathDelim);
    if (tmp++)
        aux<<tmp<<".nets "<<tmp<<".nodes "<<tmp<<".wts"<<endl;
    else
        aux<<baseFileName<<".nets "<<baseFileName<<".nodes "
	   <<baseFileName<<".wts"<<endl;
    aux.close();

    delete []fname;
}

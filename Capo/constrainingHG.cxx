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

#include "constrainingHG.h"

ConstrainingHGraph::ConstrainingHGraph(
   	const vector<CapoBlock*>&       allBlocks,
        const HGraphFixed&              hgraphOfNetlist,
        const Verbosity 		verb)
	: HGraphFixed(hgraphOfNetlist.getNumNodes() + allBlocks.size()*4, 1),
  	  _allBlocks(allBlocks), _netlistHG(hgraphOfNetlist)
{
    static_cast<void>(verb);

    _numTerminals = _netlistHG.getNumTerminals();
//  unsigned numNonTerms  = getNumNodes() - _numTerminals;
    
    _nodeNames = vector<const char*>(getNumNodes());
    _edges.reserve(_netlistHG.getNumEdges() + getNumNodes()*4);

    //all existing nodes are the same...add them in first.

    itHGFNodeGlobal n;
    for(n = _netlistHG.nodesBegin(); n != _netlistHG.nodesEnd(); n++)
    {
	unsigned nId = (*n)->getIndex();

		//note: names are in fact not very essential in this context..
		//though they will be helpful in debugging. This process
		//can be speedup by not having node names in the new HG
	char* newName = new char[strlen((*n)->getName())];
	strcpy(newName, (*n)->getName());
	_nodeNamesMap[newName] = nId;
	_nodeNames[nId] = newName;

		//in the interest of speed..this could prob. be done w/
		//a block memory copy..but this would be much more fragile
	_nodes[nId]->setWeight((*n)->getWeight());
    }

    //setup (name and give 0 weight) to the 4*numBlocks new nodes

    unsigned nId;
    for(nId = _netlistHG.getNumNodes(); nId < _nodes.size(); nId++)
    {
	char* newName = new char[20];

	unsigned blockNum = (nId - _netlistHG.getNumNodes())/4;
	unsigned subNum   = (nId - _netlistHG.getNumNodes()) % 4;

	sprintf(newName, "BlockNode%d-%d", blockNum, subNum); 	

	_nodeNamesMap[newName] = nId;
	_nodes[nId]->setWeight(0);	
	_nodeNames[nId] = newName;
    }

    //copy all existing edges

    itHGFEdgeGlobal e;
    for(e = _netlistHG.edgesBegin(); e != _netlistHG.edgesEnd(); e++)
    {
	const HGFEdge& origE = **e;
	HGFEdge &newEdge = *(addEdge(origE.getWeight()));

     	itHGFNodeLocal nIt;
	for(nIt = origE.nodesBegin(); nIt != origE.nodesEnd(); nIt++)
	    addSrcSnk(**nIt, newEdge);	
    }

   //add in the new edges

    unsigned numOrigNodes = _netlistHG.getNumNodes();
    for(unsigned blk = 0; blk < _allBlocks.size(); blk++)
    {
	const CapoBlock& block = *_allBlocks[blk];

	const HGFNode& subNode0 = *_nodes[numOrigNodes+blk*4];
	const HGFNode& subNode1 = *_nodes[numOrigNodes+blk*4+1];
	const HGFNode& subNode2 = *_nodes[numOrigNodes+blk*4+2];
	const HGFNode& subNode3 = *_nodes[numOrigNodes+blk*4+3];

        std::vector<unsigned>::const_iterator cellIds;
	for(cellIds = block.cellIdsBegin(); 
	    cellIds != block.cellIdsEnd(); cellIds++)
	{
	    const HGFNode& node = _netlistHG.getNodeByIdx(*cellIds);
	    unsigned origDegree = node.getDegree();
	    unsigned numToAdd   = 
		static_cast<unsigned>(ceil((float)(origDegree)*0.25));

	    abkfatal3(origDegree == 0 || numToAdd >= 1, origDegree,
		" numToAdd is 0 for a node with degree > 0 ",numToAdd);

            unsigned k;
	    for(k = 0; k < numToAdd; k++)
	    {
		HGFEdge *newE = addEdge(1);
		addSrcSnk(subNode0, *newE);
		addSrcSnk(node    , *newE);	
	    }
	    for(k = 0; k < numToAdd; k++)
	    {
		HGFEdge *newE = addEdge(1);
		addSrcSnk(subNode1, *newE);
		addSrcSnk(node    , *newE);	
	    }
	    for(k = 0; k < numToAdd; k++)
	    {
		HGFEdge *newE = addEdge(1);
		addSrcSnk(subNode2, *newE);
		addSrcSnk(node    , *newE);	
	    }
	    for(k = 0; k < numToAdd; k++)
	    {
		HGFEdge *newE = addEdge(1);
		addSrcSnk(subNode3, *newE);
		addSrcSnk(node    , *newE);	
	    }
	}
    }

    finalize();
}


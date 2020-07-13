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

#include "baseClustHGraph.h"
#include "Combi/combi.h"
#include "HGraph/hgFixed.h"
#include "Combi/partitionIds.h"
 
BaseClusteredHGraph::BaseClusteredHGraph(
        const HGraphFixed &leafLevel,
        const Parameters& params,
	const Partitioning* fixedConst,
        const Partitioning* origPart)
        : 
 _numTerminals(leafLevel.getNumTerminals()),_origPart(origPart),_params(params)
  {
    _clusteredHGraphs.push_back(const_cast<HGraphFixed*>(&leafLevel));
   _ownsHGraph.push_back(false);

   if(fixedConst == NULL)
   {
   	_fixedConst.push_back(Partitioning(leafLevel.getNumNodes()));
	Partitioning leafFix = _fixedConst.back();
	PartitionIds movable; movable.setToAll(31);
	std::fill(leafFix.begin(), leafFix.end(), movable);
   }
   else
	_fixedConst.push_back(*fixedConst);

}

BaseClusteredHGraph::~BaseClusteredHGraph()
{
    for(unsigned i = 0; i < _clusteredHGraphs.size(); i++)
    {
	if(_ownsHGraph[i])
	    delete _clusteredHGraphs[i];
    }
}

void BaseClusteredHGraph::mapPartitionings(const Partitioning &srcPart,
		Partitioning& destPart, unsigned level)
{
    abkfatal(level < _clusteredHGraphs.size()-1, "level is out of range");

    const HGraphFixed& destGraph = *_clusteredHGraphs[level];
    vector<unsigned>&  mapping   =  _clusterMap[level];

    if(mapping.size() != destGraph.getNumNodes())
    {
	cout<<"Mapping size:   "<<mapping.size()<<endl;
	cout<<"DestGraph size: "<<destGraph.getNumNodes()<<endl;
	cout<<"level:          "<<level<<endl;

      abkfatal(mapping.size() == destGraph.getNumNodes(),
	"level and mapping have dif. size");
    }

    if (destPart.size() < destGraph.getNumNodes())
        cout << " Dest size " << destPart.size() 
             << " dest graph size " << destGraph.getNumNodes() << endl;
    abkfatal(destPart.size() >= destGraph.getNumNodes(),
	"partitions are undersized");

    for(unsigned n = 0; n < destGraph.getNumNodes(); n++)
    {
      #ifdef ABKDEBUG
      	unsigned mappingNum = mapping[n];
	abkfatal3(mappingNum < srcPart.size(), mappingNum,
	    " (the mapping) excedes length of srcPartition,\n which has size ",
	    srcPart.size());
      #endif

	destPart[n] = 	srcPart[mapping[n]];
    }
}
unsigned BaseClusteredHGraph::computeCut
	(const HGraphFixed& graph, const Partitioning& part)
{
    unsigned totalCut = 0;
    itHGFEdgeGlobal e;
    for(e = graph.edgesBegin(); e != graph.edgesEnd(); e++)
    {
        const HGFEdge& edge = **e;
        itHGFNodeLocal n;
        bool p0 = false; bool p1 = false;
        for(n = edge.nodesBegin(); n != edge.nodesEnd(); n++)
        {
            if(part[(*n)->getIndex()].isInPart(0))
                p0 = true;
            else
                p1 = true;
        }
        if(p0 && p1) totalCut += unsigned(ceil(edge.getWeight()));
    }
 
    return totalCut;
}

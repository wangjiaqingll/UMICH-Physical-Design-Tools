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






#include "Stats/trivStats.h"
#include "partitioning.h"

int main(int argc, const char *argv[]) 
{
    StringParam  baseFileName("f",argc,argv);  
    abkfatal(baseFileName.found()," -f baseFileName not found");
    PartitioningProblem problem(baseFileName);

    //cout<<"Stats for Problem"<<endl;
    const HGraphFixed &hgraph = problem.getHGraph();

    vector<unsigned> nodeDegs;
    unsigned	     numPins = 0;
    vector<unsigned> totNodeDeg(hgraph.getNumEdges(),0);
    double           totalArea = 0;
    double   	     maxArea = 0;
    vector<double>   nodeAreas;

    for(itHGFNodeGlobal n = hgraph.nodesBegin(); n != hgraph.nodesEnd(); n++)
    {
	nodeDegs.push_back((*n)->getDegree());
	numPins += (*n)->getDegree();
	totNodeDeg[(*n)->getDegree()]++;
	totalArea += (*n)->getWeight();
	nodeAreas.push_back((*n)->getWeight());	
	maxArea = max(maxArea, (*n)->getWeight());
    }

    vector<unsigned> netDegs;
    vector<unsigned> totNetDeg(hgraph.getNumNodes(),0); 
    unsigned externalNets = 0;
    for(itHGFEdgeGlobal e = hgraph.edgesBegin(); e != hgraph.edgesEnd(); e++)
    {
	netDegs.push_back((*e)->getDegree());
	totNetDeg[(*e)->getDegree()]++;
	for(itHGFNodeLocal c = (*e)->nodesBegin(); c != (*e)->nodesEnd(); c++)
	    if(hgraph.isTerminal((*c)->getIndex()))
	    {    externalNets++; break; }
    }

    cout<<"___Node Stats______"<<endl<<endl;
    cout<<"Total Nodes: "<<hgraph.getNumNodes()<<endl;
    cout<<"  Terminals: "<<hgraph.getNumTerminals()<<endl;
    cout<<"  Non-Terms: "<<hgraph.getNumNodes()-hgraph.getNumTerminals()<<endl;
    cout<<"TotalNodeArea: "<<totalArea<<endl;
    cout<<" Node Areas: "<<TrivialStats(nodeAreas)<<endl;
    cout<<" Largest Node is "<<(maxArea / totalArea)*100<<
			"% of total area"<<endl;

    cout<<"Node Degrees: "<<TrivialStats(nodeDegs)<<endl;

    unsigned i;
    for(i = 0; i < totNodeDeg.size(); i++)
	if(totNodeDeg[i] > 0)
	cout<<"  "<<i<<")  "<<totNodeDeg[i]<<endl;
    cout<<" Total Pins: "<<numPins<<endl;

    cout<<endl<<"___Net Stats______"<<endl;

    cout<<"Total Nets:  "<<hgraph.getNumEdges()<<endl;
    cout<<" Net Degrees: "<<TrivialStats(netDegs)<<endl;
    for(i = 0; i < totNetDeg.size(); i++)
	if(totNetDeg[i] > 0)
	cout<<"  "<<i<<")  "<<totNetDeg[i]<<endl;

    cout<<"External Nets "<<externalNets<<endl;

    return 0;
}
	

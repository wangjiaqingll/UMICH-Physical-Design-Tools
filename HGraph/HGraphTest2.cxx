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
#include "hgFixed.h"
#include "cluHGraph.h"

int main(int argc, const char *argv[])
{

    StringParam  baseFileName("f",argc,argv);

    abkfatal(baseFileName.found()," -f baseFileName not found");

    HGraphFixed hgraph(baseFileName);

    vector<unsigned> cluMap(hgraph.getNumNodes());
    unsigned k, numTerminalClusters=(hgraph.getNumTerminals()+1)/2,
                numClusters        =(hgraph.getNumNodes    ()+1)/2;
    for(k=0; k!=cluMap.size(); k++) cluMap[k]=k/2;
    CluHGraph chg(hgraph,cluMap,numClusters,numTerminalClusters);

    chg.printEdgeSizeStats(cout);
    cout<<endl;
    chg.printEdgeWtStats(cout);
    cout<<endl;
    chg.printNodeDegreeStats(cout);
    cout<<endl;
    chg.printNodeWtStats(cout);
    cout<<endl;

  cout<<" Total Edges: "<<chg.getNumEdges()<<endl;
  cout<<" Total Nodes: "<<chg.getNumNodes()<<endl;
  cout<<"   Terminals: "<<chg.getNumTerminals()<<endl;
  cout<<"   Non-Terms: "<<chg.getNumNodes() - chg.getNumTerminals()<<endl;

    return 0;
}



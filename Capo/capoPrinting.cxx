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












// Created on 99/02/04 by Andrew Caldwell
//  output functions for CapoPlacer

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include "ABKCommon/abkcommon.h"
#include "Stats/trivStats.h"
#include "capoPlacer.h"
#include "partProbForCapo.h"
   
void CapoPlacer::printSmallProblemStats()
{ 
    cout<<"Small Partitioning Problem Stats"<<endl<<endl;
    cout<<"#NonTerms       All Edges           Only Essential Edges "<<endl;
    cout<<"       NumEdges/EdgeDeg/NodeDeg  NumEdges/EdgeDeg/NodeDeg"<<endl;
      
    for(unsigned m = 0; m < _numProblemsOfSize.size(); m++)
    {
	unsigned numProbs = _numProblemsOfSize[m];
	cout<<" "<<setw(3)<<m<<"("<<setw(4)<<numProbs<<")  ";
	cout.precision(4);
	if(_numProblemsOfSize[m] == 0)
	    	cout<<"  ---- "<<endl;
	else
	{
	    cout<<setw(7)<<(double)_numOrigNets[m]/(double)numProbs<<"/";
	    cout<<setw(7)<<(double)_origNetPins[m]/
				(double)_numOrigNets[m]<<"/";
	    cout<<setw(7)<<(double)_origNetPins[m]/
				(double)(m*numProbs)<<"   ";

	    cout<<setw(7)<<(double)_numEssentialNets[m]/
				(double)numProbs<<"/";
	    cout<<setw(7)<<(double)_essentialNetPins[m]/
				(double)_numEssentialNets[m]<<"/";
	    cout<<setw(7)<<(double)_essentialNetPins[m]/
				(double)(m*numProbs)<<endl;
	}
    }
}

const pair<unsigned,double> junkvPairUDForCompiler;

void CapoPlacer::printCutDensityStats()
{
   abkfatal(_vCutStats.size() == _hCutStats.size() &&
	    _vCutRatios.size() == _hCutRatios.size() &&
	    _vCutStats.size() == _vCutRatios.size(),
		"size mis-match in cut density stats");

   for(unsigned l = 0; l < _vCutStats.size(); l++)
   {
	vector<double> vCut(_vCutStats[l].size());
	vector<double> hCut(_hCutStats[l].size());
	vector<double> vRat(_vCutRatios[l].size());
	vector<double> hRat(_hCutRatios[l].size());
	unsigned totHCut = 0;
	unsigned totVCut = 0;

   	unsigned k;
        for(k = 0; k < _vCutStats[l].size(); k++)
	{
	    vCut[k] = (double)_vCutStats[l][k].first/ _vCutStats[l][k].second;
	    totVCut += _vCutStats[l][k].first;
	}
        for(k = 0; k < _hCutStats[l].size(); k++)
	{
	    hCut[k] = (double)_hCutStats[l][k].first/ _hCutStats[l][k].second;
	    totHCut += _hCutStats[l][k].first;
	}

        for(k = 0; k < _vCutRatios[l].size(); k++)
	    vRat[k] = (double)_vCutRatios[l][k].first/ _vCutRatios[l][k].second;
        for(k = 0; k < _hCutRatios[l].size(); k++)
	    hRat[k] = (double)_hCutRatios[l][k].first/ _hCutRatios[l][k].second;



	cout<<endl<<"Layer "<<setw(2)<<l<<":"<<endl;

	cout<<"  Horizontal: "<<endl;
	cout<<"    TotalCut:    "<<totHCut<<endl;
	cout<<"    Cut/Length: ("<<hCut.size()<<")";
	  if(hCut.size() > 0) cout<<TrivialStats(hCut);
   	  else cout<<endl;
        
	cout<<"    Prev Cut/Cut:("<<hRat.size()<<")";
	  if(hRat.size() > 0) cout<<TrivialStats(hRat);
	  else cout<<endl;

	cout<<"  Vertical: "<<endl;
	cout<<"    TotalCut:    "<<totVCut<<endl;
	cout<<"    Cut/Length: ("<<vCut.size()<<")";
	  if(vCut.size() > 0) cout<<TrivialStats(vCut);
	  else cout<<endl;
	cout<<"    Prev Cut/Cut:("<<vRat.size()<<")";
	  if(vRat.size() > 0) cout<<TrivialStats(vRat);
	  else cout<<endl;
   }
}

void CapoPlacer::printNetlistStats()
{
        cout<<"--Netlist HGraph Stats--"<<endl;
        cout<<" NumNodes :"<<_hgraphOfNetlist.getNumNodes()<<endl;
        cout<<"  Terminals: "<<_hgraphOfNetlist.getNumTerminals()<<endl;
        cout<<"  NonTerms:  "<<_hgraphOfNetlist.getNumNodes() -
                                _hgraphOfNetlist.getNumTerminals()<<endl;
        cout<<" MaxDegree: "<<_hgraphOfNetlist.getMaxNodeDegree()<<endl;

        if(_params.verb.forMajStats > 9)
                _hgraphOfNetlist.printNodeWtStats(cout);
        cout<<endl;

        cout<<" Edges:    "<<_hgraphOfNetlist.getNumEdges()<<endl;
        cout<<"  MaxDegree: "<<_hgraphOfNetlist.getMaxEdgeDegree()<<endl;

        if(_params.verb.forMajStats > 9)
                _hgraphOfNetlist.printEdgeSizeStats(cout);
        cout<<endl;
}

void CapoPlacer::saveSmallProblem(const PartitioningProblemForCapo& problem,
				  const CapoBlock& block)
{
    const HGraphFixed& hgraph = problem.getHGraph();

    unsigned numSPPNodes = block.getNumCells();
    unsigned numSPPEdges = hgraph.getNumEdges();
    unsigned numSPPRows  = block.getNumRows();
 
    char probName[128];
 
    sprintf(probName, "spp_N%d_E%d_R%d_%d", numSPPNodes, numSPPEdges,
                        numSPPRows, block.getIndex());

    const_cast<PartitioningProblemForCapo&>(problem).saveAsNodesNets(probName);
}

ostream& operator<<(ostream& os, const vector<CapoBlock*>& layout)
{
    os<<"CapoPlacer Layout"<<endl;
    os<<" Total Blocks "<<layout.size()<<endl<<endl;
 
    for(unsigned i = 0; i < layout.size(); i++)
	os<<layout[i]<<endl;

    return os;
}


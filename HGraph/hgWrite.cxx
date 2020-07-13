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

#include "hgFixed.h"
#include "ABKCommon/pathDelims.h"
#include <ctype.h>
#include <stdio.h>

void HGraphFixed::saveAsNetDAre(const char* baseFileName) const
{
    char* netDFileName = new char[strlen(baseFileName) + 8];
    strcpy(netDFileName, baseFileName);
    strcat(netDFileName, ".netD"); 

    ofstream netD(netDFileName);

    netD<<"0\n"
    <<_numPins<<"\n"
    <<getNumEdges()<<"\n"
    <<getNumNodes()<<"\n"
    <<getNumNodes()-getNumTerminals()-1<<"\n";

    itHGFEdgeGlobal e;
    for (e=edgesBegin(); e!=edgesEnd(); ++e)
    {
        bool newNet=true;
        itHGFNodeLocal v;
        for (v=(*e)->srcsBegin(); v!=(*e)->srcsEnd(); ++v)
        {
	    unsigned nId = (*v)->getIndex();
	    if(isTerminal(nId))
	  	netD<<"p"<<nId+1;
	    else
		netD<<"a"<<nId-_numTerminals;
            if (newNet)
            {
                netD<<" s ";
                newNet=false;
            } else
            netD<<" l ";
            netD<<"O\n";
        }
        for (v=(*e)->snksBegin(); v!=(*e)->snksEnd(); ++v)
        {
	    unsigned nId = (*v)->getIndex();
	    if(isTerminal(nId))
	  	netD<<"p"<<nId+1;
	    else
		netD<<"a"<<nId-_numTerminals;
            if (newNet)
            {
                netD<<" s ";
                newNet=false;
            } else
            netD<<" l ";
            netD<<"I\n";
        }
        for (v=(*e)->srcSnksBegin(); v!=(*e)->srcSnksEnd(); ++v)
        {
	    unsigned nId = (*v)->getIndex();
	    if(isTerminal(nId))
	  	netD<<"p"<<nId+1;
	    else
		netD<<"a"<<nId-_numTerminals;
            if (newNet)
            {
                netD<<" s ";
                newNet=false;
            } else
            netD<<" l ";
            netD<<"B\n";
        }
    }
    netD<<endl;
    netD.close();
  
    char* areMFileName = 0;

    if(getNumWeights() > 0)
    {
      areMFileName = new char[strlen(baseFileName) + 7];
      strcpy(areMFileName, baseFileName);
      strcat(areMFileName, ".areM"); 
      ofstream areM(areMFileName);

      unsigned n;
      for(n=getNumTerminals(); n!=getNumNodes(); n++)
      {
	areM<<"a"<<n-_numTerminals;
        for(unsigned i=0; i!=getNumMultiWeights(); ++i)
                    areM<<" "<<setw(6)<<getNodeByIdx(n).getWeight(i);
        areM<<"\n";
      }
    
      for(n=0; n!=getNumTerminals(); ++n)
      {
	areM<<"p"<<n+1;
        for (unsigned i=0; i!=getNumMultiWeights(); ++i)
                    areM<<" "<<setw(6)<< getNodeByIdx(n).getWeight(i);
        areM<<"\n";
      }
      areM<<endl;
      areM.close();
    }

    char* auxFileName = new char[strlen(baseFileName) + 5];
    strcpy(auxFileName, baseFileName);
    strcat(auxFileName, ".aux");
  
    ofstream aux(auxFileName);
    aux<<"HGraph : ";
   
    const char * tmp=NULL;
    tmp=strrchr(netDFileName,pathDelim); 
    if (tmp) aux << ++tmp;
    else     aux << netDFileName;
    aux<<"  ";

    tmp=strrchr(areMFileName,pathDelim);
    if (tmp)  aux << ++tmp;
    else      aux << areMFileName;
    aux<<endl;  

    aux.close();

    delete[] netDFileName;
    if(areMFileName) delete[] areMFileName;
    delete[] auxFileName;
}


void HGraphFixed::saveAsNodesNetsWts(const char* baseFileName) const
{
    char* nodesFileName = new char[strlen(baseFileName) + 8];
    strcpy(nodesFileName, baseFileName);
    strcat(nodesFileName, ".nodes"); 

    ofstream nodes(nodesFileName);

    nodes<<"UCLA nodes 1.0 "<<endl;
    nodes<<TimeStamp()<<User()<<Platform()<<endl; 

    nodes<<"NumNodes :       "<<setw(10)<<getNumNodes()<<endl;
    nodes<<"NumTerminals :   "<<setw(10)<<getNumTerminals()<<endl;

    unsigned nId;
    for(nId = 0; nId < _nodes.size(); nId++)
    {
        if (_nodeNames[nId])     nodes<<_nodeNames[nId];
        else
        {
            if (nId < _numTerminals) nodes << " p" << nId+1 <<" ";
            else                     nodes << " a" << nId - _numTerminals <<" ";
        }
        if (nId < _numTerminals) nodes<<" terminal";
        nodes << endl;
    }
    nodes.close();

    char* netsFileName = new char[strlen(baseFileName) + 8];
    strcpy(netsFileName, baseFileName);
    strcat(netsFileName, ".nets"); 

    ofstream nets(netsFileName);

    nets<<"UCLA nets 1.0"<<endl;
    nets<<TimeStamp()<<User()<<Platform()<<endl; 

    nets<<"NumNets : "<<getNumEdges()<<endl;
    nets<<"NumPins : "<<_numPins<<endl;

    itHGFEdgeGlobal e;
    for (e=edgesBegin(); e!=edgesEnd(); ++e)
    {
        const HGFEdge& net=(**e);
        const char* netName=getNetNameByIndex(net.getIndex());
	nets<<"NetDegree : "<< net.getDegree() << " ";
        if (netName) nets << setw(10) << netName;
        else if (abs(net.getWeight()-1.0)>1e-10)
                     nets << " net" << net.getIndex();
        nets << endl;

        itHGFNodeLocal v;
        for (v=(*e)->srcsBegin(); v!=(*e)->srcsEnd(); ++v)
        {
            nets<<_nodeNames[(*v)->getIndex()];
            nets<<" O\n";
        }
        for (v=(*e)->snksBegin(); v!=(*e)->snksEnd(); ++v)
        {
            nets<<_nodeNames[(*v)->getIndex()];
            nets<<" I\n";
        }
        for (v=(*e)->srcSnksBegin(); v!=(*e)->srcSnksEnd(); ++v)
        {
            nets<<_nodeNames[(*v)->getIndex()];
            nets<<" B\n";
        }
    }
    nets<<endl;
    nets.close();

    bool haveWts=false;
    unsigned n; 
    for(n=getNumTerminals(); n!=getNumNodes(); n++)
    {
        for (unsigned i=0; i!=getNumWeights(); ++i)
            if (getNodeByIdx(n).getWeight(i)!=1.0)
                 { haveWts=true; break; }
    }
    if (!haveWts)
    for(n=0; n!=getNumEdges(); ++n)
    {
        const HGFEdge& net=getEdgeByIdx(n);
        if (abs(net.getWeight()-1.0)<1e-10) continue;
        haveWts=true; 
        break;
    }

  char* wtsFileName = new char[strlen(baseFileName) + 8];
  if (haveWts)
  {
    strcpy(wtsFileName, baseFileName);
    strcat(wtsFileName, ".wts"); 

    ofstream wts(wtsFileName);

    wts<<"UCLA wts 1.0"<<endl;
    nets<<TimeStamp()<<User()<<Platform()<<endl; 
  
    for(n=getNumTerminals(); n!=getNumNodes(); n++)
    {
        wts<<setw(6) << getNodeNameByIndex(n);
        for (unsigned i=0; i!=getNumWeights(); ++i)
                    wts<<" "<<setw(6)<< getNodeByIdx(n).getWeight(i);
        wts<<"\n";
    }
    
    for(n=0; n!=getNumTerminals(); ++n)
    {
        wts<<setw(6) << getNodeNameByIndex(n);
        for (unsigned i=0; i!=getNumMultiWeights(); ++i)
                    wts<<" "<<setw(6)<< getNodeByIdx(n).getWeight(i);
        wts<<"\n";
    }
    wts<<endl;

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
  }

    char* auxFileName = new char[strlen(baseFileName) + 5];
    strcpy(auxFileName, baseFileName);
    strcat(auxFileName, ".aux");
  
    ofstream aux(auxFileName);
    aux<<"HGraph : ";
   
    const char * tmp=NULL;
    tmp=strrchr(nodesFileName,pathDelim); 
    if (tmp) aux << ++tmp;
    else     aux << nodesFileName;
    aux<<"  ";

    tmp=strrchr(netsFileName,pathDelim);
    if (tmp)  aux << ++tmp;
    else      aux << netsFileName;
    aux<<"  ";

  if (haveWts)
  {
    tmp=strrchr(wtsFileName,pathDelim);
    if (tmp)  aux << ++tmp;
    else      aux << wtsFileName;
    aux<<endl;  
  }

    aux.close();

    delete[] netsFileName;
    delete[] nodesFileName;
    delete[] wtsFileName;
    delete[] auxFileName;
}


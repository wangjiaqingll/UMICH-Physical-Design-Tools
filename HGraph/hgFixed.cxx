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





// 020811  ilm   ported to g++ 3.0


#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <algorithm>
#include "hgFixed.h"

void HGraphFixed::init(unsigned numNodes, unsigned numWeights)
{
    _dummyEdge._index=UINT_MAX;
    _numMultiWeights = numWeights;
    _numTotalWeights = numWeights;
    _multiWeights    = vector<HGWeight>(numNodes*numWeights);
    _maxNodeDegree   = 0;
    _maxEdgeDegree   = 0;
    _ignoredEdges    = 0;
    _finalized=false;

    _nodes.reserve(numNodes);

    unsigned i;
    for(i=0; i!=numNodes; ++i) _nodes.push_back(new HGFNode(i, this));
}


HGraphFixed::HGraphFixed(const HGraphFixed& orig)
{
    _param = orig._param;
    init(orig.getNumNodes(), orig.getNumMultiWeights());

    _numPins         = orig._numPins;
    _numTerminals    = orig._numTerminals;
    _binWeights      = vector<HGWeightPair>(orig._binWeights.size());
    _maxNodeDegree   = orig._maxNodeDegree;
    _maxEdgeDegree   = orig._maxEdgeDegree;
    _weightSort      = orig._weightSort;
    _degreeSort      = orig._degreeSort;
    _ignoredEdges    = orig._ignoredEdges;
    _nodeNames       = vector<const char*>(orig.getNumNodes());
    _netNames        = vector<const char*>(orig.getNumEdges());

    int c;
    for(c = orig._multiWeights.size()-1; c >= 0; --c)
    {   _multiWeights[c] = orig._multiWeights[c];}

    for(c = orig._binWeights.size()-1; c >= 0; --c)
    {	_binWeights[c] = orig._binWeights[c];}
	
    itHGFEdgeGlobal edgeIt;
    for (edgeIt=orig.edgesBegin(); edgeIt!=orig.edgesEnd(); ++edgeIt)
    {
	HGFEdge& oEdge = **edgeIt;

        HGFEdge* nEdge=addEdge(oEdge.getWeight());
        itHGFNodeLocal adjIt;

        for (adjIt=oEdge.srcsBegin(); adjIt!=oEdge.srcsEnd(); ++adjIt)
            addSrc(getNodeByIdx((*adjIt)->getIndex()), *nEdge);
        for (adjIt=oEdge.snksBegin(); adjIt!=oEdge.snksEnd(); ++adjIt)
            addSnk(getNodeByIdx((*adjIt)->getIndex()), *nEdge);
        for (adjIt=oEdge.srcSnksBegin(); adjIt!=oEdge.srcSnksEnd(); ++adjIt)
            addSrcSnk(getNodeByIdx((*adjIt)->getIndex()), *nEdge);
   
        const char* oldName=orig._netNames[oEdge.getIndex()];
        if (!oldName) continue;
	char* newName = new char[strlen(oldName)+1];
	strcpy(newName, oldName);
	_netNames[nEdge->getIndex()] = newName;
	_netNamesMap[newName] = nEdge->getIndex();
    }

    unsigned nodeId;
    for(nodeId = 0; nodeId < _nodes.size(); nodeId++)
    {
	_nodes[nodeId]->_binWtBegin = orig._nodes[nodeId]->_binWtBegin;
	_nodes[nodeId]->_binWtEnd   = orig._nodes[nodeId]->_binWtEnd;
	char* tmpName = new char[strlen(orig._nodeNames[nodeId])+1];
	strcpy(tmpName, orig._nodeNames[nodeId]);
	_nodeNames[nodeId] = tmpName;
	_nodeNamesMap[tmpName] = nodeId;
    }

    finalize();
}

void HGraphFixed::finalize()
{
    abkfatal(_finalized==false, "cannot run finalize twice");

    if(_addEdgeStyle==FAST_ADDEDGE_USED) 
    {
        _finalized=true;
        return;
    }

    vector<unsigned> nodeDegrees(getNumNodes());
    vector<unsigned> edgeDegrees(getNumEdges());

    std::list<pair<unsigned, unsigned> >::iterator it;
    _numPins = 0;

    for(it=_srcs.begin(); it!=_srcs.end(); ++it)
    {
	_numPins++;
        ++nodeDegrees[(*it).first];
        ++edgeDegrees[(*it).second];
    }

    for(it=_srcSnks.begin(); it!=_srcSnks.end(); ++it)
    {
	_numPins++;
        ++nodeDegrees[(*it).first];
        ++edgeDegrees[(*it).second];
    }

    for (it=_snks.begin(); it!=_snks.end(); ++it)
    {
	_numPins++;
        ++nodeDegrees[(*it).first];
        ++edgeDegrees[(*it).second];
    }

    if (_param.removeBigNets)
    {
        vector<unsigned> old2new(getNumEdges());
        unsigned last=0;
        unsigned old;
        unsigned rmCt=0;
        for(old=0; old!=getNumEdges(); ++old)
	{
            if (edgeDegrees[old]<=_param.netThreshold)
                old2new[old]=last++;
            else
            {
                old2new[old]=UINT_MAX;
                rmCt++;
            }
	}

        for (it=_srcs.begin(); it!=_srcs.end(); ++it)
            (*it).second=old2new[(*it).second];

        for (it=_srcSnks.begin(); it!=_srcSnks.end(); ++it)
            (*it).second=old2new[(*it).second];

        for (it=_snks.begin(); it!=_snks.end(); ++it)
            (*it).second=old2new[(*it).second];

        last=0;
        for (unsigned i=0; i!=old2new.size(); ++i)
        {
            if (old2new[i]==UINT_MAX)
            {
                delete _edges[last];
                _edges.erase(_edges.begin()+last);
                _netNames.erase(_netNames.begin()+last);
            }
            else
            {
                edgeDegrees[old2new[i]]=edgeDegrees[i];
                _edges[old2new[i]]->_index=old2new[i];
                last++;
            }
        }
        cout<<"Thresholding "<<rmCt<<" Net(s) away."<<endl;
    }
    
    // reserve() is needed both for
    // (1) efficiency (to avoid extra mem allocs and vector moves)
    // (2) correcness (to ensure _srcSnksBegin, etc. iterators stay unchanged
    // while _srcSnks, etc. containers are pushed-back below)

    unsigned v,e;
    for (v=0; v!=getNumNodes(); ++v)
        getNodeByIdx(v)._edges.reserve(nodeDegrees[v]);

    for (e=0; e!=getNumEdges(); ++e)
        getEdgeByIdx(e)._nodes.reserve(edgeDegrees[e]);


    for (it=_srcs.begin(); it!=_srcs.end(); ++it)
    {
        if ((*it).second==UINT_MAX)
            continue;
        if (edgeDegrees[(*it).second] > _param.netThreshold)
            continue;
        getNodeByIdx((*it).first)._edges.push_back(&getEdgeByIdx((*it).second));
        getEdgeByIdx((*it).second)._nodes.push_back(&getNodeByIdx((*it).first));
    }
    for (v=0; v!=getNumNodes(); ++v)
        getNodeByIdx(v)._srcSnksBegin=getNodeByIdx(v)._edges.end();
    for (e=0; e!=getNumEdges(); ++e)
        getEdgeByIdx(e)._srcSnksBegin=getEdgeByIdx(e)._nodes.end();

    for (it=_srcSnks.begin(); it!=_srcSnks.end(); ++it)
    {
        if ((*it).second==UINT_MAX)
            continue;
        if (edgeDegrees[(*it).second] > _param.netThreshold)
            continue;
        getNodeByIdx((*it).first)._edges.push_back(&getEdgeByIdx((*it).second));
        getEdgeByIdx((*it).second)._nodes.push_back(&getNodeByIdx((*it).first));
    }
    for (v=0; v!=getNumNodes(); ++v)
        getNodeByIdx(v)._snksBegin=getNodeByIdx(v)._edges.end();
    for (e=0; e!=getNumEdges(); ++e)
        getEdgeByIdx(e)._snksBegin=getEdgeByIdx(e)._nodes.end();

    for (it=_snks.begin(); it!=_snks.end(); ++it)
    {
        if ((*it).second==UINT_MAX)
            continue;
        if (edgeDegrees[(*it).second] > _param.netThreshold)
            continue;
        getNodeByIdx((*it).first)._edges.push_back(&getEdgeByIdx((*it).second));
        getEdgeByIdx((*it).second)._nodes.push_back(&getNodeByIdx((*it).first));
    }

    _srcs.clear();
    _srcSnks.clear();
    _snks.clear();

    sortNodes();
    sortEdges();
    if (_netNames.size()==0)
    _netNames  = vector<const char*>(_edges.size(), (const char*)(NULL));

    _finalized=true;
}

void HGraphFixed::adviseNodeDegrees(const vector<unsigned>& nodeDegrees) 
{
   abkfatal(nodeDegrees.size()==getNumNodes(), 
		"adviseNodeDegrees parameter must be of size #nodes");

   abkwarn(_addEdgeStyle!=SLOW_ADDEDGE_USED, 
	"adviseNodeDegrees is useless when combined with slow addEdge");

   for (unsigned i=0; i<=getNumNodes(); ++i)
       _nodes[i]->_edges.reserve(nodeDegrees[i]);
}


void HGraphFixed::computeMaxNodeDegree() const
{
    HGraphFixed* fakeThis=const_cast<HGraphFixed*>(this);
    fakeThis->_maxNodeDegree=0;
    for (itHGFNodeGlobal v=nodesBegin(); v!=nodesEnd(); ++v)
        fakeThis->_maxNodeDegree=std::max(_maxNodeDegree, (*v)->getDegree());
}

void HGraphFixed::computeMaxEdgeDegree() const
{
    HGraphFixed* fakeThis=const_cast<HGraphFixed*>(this);
    fakeThis->_maxEdgeDegree=0;
    for (itHGFEdgeGlobal e=edgesBegin(); e!=edgesEnd(); ++e)
        fakeThis->_maxEdgeDegree=std::max(_maxEdgeDegree, (*e)->getDegree());
}

void HGraphFixed::computeNumPins() const
{
    HGraphFixed* fakeThis=const_cast<HGraphFixed*>(this);
    fakeThis->_numPins=0;
    for (itHGFNodeGlobal v=nodesBegin(); v!=nodesEnd(); ++v)
        fakeThis->_numPins+=(*v)->getDegree();
}

HGFEdge* HGraphFixed::addEdge(HGWeight weight)
{
    abkfatal (_addEdgeStyle!=FAST_ADDEDGE_USED,
        "cannot mix slow and fast addEdge in one HGraph object")
    _addEdgeStyle=SLOW_ADDEDGE_USED;
    HGFEdge* edge=new HGFEdge(weight);
    _edges.push_back(edge);
    edge->_index=_edges.size()-1;
    return edge;
}

HGraphFixed::~HGraphFixed()
{
    if (_nodes.size() != _nodeNames.size())
    {
        cerr << "In ~HGraphFixed(): _nodes.size()==" << _nodes.size()
             << " but _nodeNames.size()==" << _nodeNames.size() << endl;
        abkfatal(0," Size mismatch ");
    }
    if (_edges.size() != _netNames.size())
    {
        cerr << "In ~HGraphFixed(): _edges.size()==" << _edges.size()
             << " but _netNames.size()==" << _netNames.size() << endl;
        abkfatal(0," Size mismatch ");
    }
 
    unsigned n;
    for(n = 0; n < _edges.size(); n++) 
    {
        if (_netNames[n]) delete [] const_cast<char*>(_netNames[n]);
        if (_edges[n])    delete _edges[n];
    }
    for(n = 0; n < _nodes.size(); n++)
    {
        if(_nodeNames[n]) delete [] const_cast<char*>(_nodeNames[n]);
        if(_nodes[n])     delete _nodes[n];
    }
}

bool HGFNode::isEdgeSrc(const HGFEdge* edge) const
{
  for(itHGFEdgeLocal e = srcEdgesBegin(); e != srcEdgesEnd(); e++)
  {	if((*e) == edge) return true;}
  
  return false;
}

bool HGFNode::isEdgeSnk(const HGFEdge* edge) const
{ 
  for(itHGFEdgeLocal e = snkEdgesBegin(); e != snkEdgesEnd(); e++)
  {	if((*e) == edge) return true;}
  
  return false;
}

bool HGFNode::isEdgeSrcSnk(const HGFEdge* edge) const
{
  for(itHGFEdgeLocal e = srcSnkEdgesBegin(); e != srcSnkEdgesEnd(); e++)
  {	if((*e) == edge) return true;}
  
  return false;
}

bool HGFEdge::isNodeSrc(const HGFNode* node) const
{
  for(itHGFNodeLocal n = srcsBegin(); n != srcsEnd(); n++)
  {	if((*n) == node) return true;}
  
  return false;
}

bool HGFEdge::isNodeSnk(const HGFNode* node) const
{
  for(itHGFNodeLocal n = snksBegin(); n != snksEnd(); n++)
  {	if((*n) == node) return true;}
  
  return false;
}

bool HGFEdge::isNodeSrcSnk(const HGFNode* node) const
{
  for(itHGFNodeLocal n = srcSnksBegin(); n != srcSnksEnd(); n++)
  {	if((*n) == node) return true;}
  
  return false;
}

static std::bit_vector visited;
static const HGraphFixed* graph;

static void DFS(unsigned nodeId)
{
    visited[nodeId]=true;
    itHGFEdgeLocal edgeIter;
    const HGFNode& node=graph->getNodeByIdx(nodeId);
    for (edgeIter=node.edgesBegin(); edgeIter!=node.edgesEnd(); ++edgeIter)
    {
        const HGFEdge& edge=graph->getEdgeByIdx((*edgeIter)->getIndex());
        itHGFNodeLocal n1Iter;
        for(n1Iter=edge.nodesBegin(); n1Iter!=edge.nodesEnd(); ++n1Iter)
            if (!visited[(*n1Iter)->getIndex()])
                    DFS((*n1Iter)->getIndex());
    }
}

unsigned HGAlgo::connectedComponents(const HGraphFixed& g)
{
    graph=&g;
    visited=std::bit_vector(graph->getNumNodes(), false);
    unsigned numComponents=0;
    for (unsigned v=0; v!=graph->getNumNodes(); ++v)
    {
        if (visited[v])
            continue;
        ++numComponents;
        DFS(v);
    }
    return numComponents;
}



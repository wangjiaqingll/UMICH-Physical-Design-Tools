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







//NOTE:  in the interest of having easily managable files, HGFixed
//has been split across a few headers.
//	hgFNode.h	defines class HGFNode
//	hgFEdge.h	defines class HGFEdge
//	hgFNodeInline.h	defines inline functions of HGFNode
//	hgFEdgeInline.h	defines inline functions of HGFEdge
// the later two header files are included at the end of this document
// because they require that HGraphFixed be defined first

#ifndef _HGRAPHFIXED_H_
#define _HGRAPHFIXED_H_

#include "ABKCommon/abkcommon.h"
#include "ABKCommon/sgi_hash_map.h"
#include "Ctainers/listO1size.h"
#include "hgBase.h"
#include "hgFNode.h"
#include "hgFEdge.h"

#include <utility>
#include <map>
#include <list>

 using std::pair;
 using std::list;

typedef vector<HGFNode*> ctainerHGFNodesGlobal;
typedef vector<HGFEdge*> ctainerHGFEdgesGlobal;

typedef ctainerHGFNodesGlobal::const_iterator itHGFNodeGlobal;
typedef ctainerHGFEdgesGlobal::const_iterator itHGFEdgeGlobal;

typedef ctainerHGFNodesGlobal::iterator itHGFNodeGlobalMutable;
typedef ctainerHGFEdgesGlobal::iterator itHGFEdgeGlobalMutable;


struct HGeqstr
{ bool operator()(const char* s1, const char* s2) const
//{ return strcasecmp(s1, s2) < 0; }
  { return strcasecmp(s1, s2) == 0; }
};

typedef  hash_map<const char*, unsigned, hash<const char*>, HGeqstr>
//typedef map<const char*, unsigned, HGeqstr> 
       HGNodeNamesMap;
typedef  HGNodeNamesMap HGNetNamesMap;


class HGraphFixed : public HGraphBase
{
    friend class HGFNode;
    friend class HGFEdge;

  protected:
    bool 	_finalized;

    ctainerHGFNodesGlobal _nodes;
    ctainerHGFEdgesGlobal _edges;

    HGFEdge _dummyEdge;

    list<pair<unsigned, unsigned> > _srcs;
    list<pair<unsigned, unsigned> > _snks;
    list<pair<unsigned, unsigned> > _srcSnks;

    void init(unsigned nNodes, unsigned nWeights);

	//these can only be used before finalize is called.
    void addSrc   (const HGFNode& node, const HGFEdge& edge);
    void addSnk   (const HGFNode& node, const HGFEdge& edge);
    void addSrcSnk(const HGFNode& node, const HGFEdge& edge);

    virtual void fastAddSrcSnk  (HGFNode* node, HGFEdge* edge);

    void computeMaxNodeDegree() const;
    void computeMaxEdgeDegree() const;
    void computeNumPins() const;
    void computeNodesSortedByWeights() const;
    void computeNodesSortedByDegrees() const;

    virtual void sortNodes();
    virtual void sortEdges();

    void removeBigNets();

	//functions useful in populateing the HGraph

    virtual void parseAux  (const char* filename);
    virtual void readNetD  (const char* filename);
    virtual void readAreM  (const char* filename);
    virtual void readWts   (const char* filename);
    virtual void readNodes (const char* filename);
    virtual void readNets  (const char* filename);
    unsigned getNodeIndexByName(const char* str) const;
    const char* getNodeNameByIndex(unsigned nId) const
    {    return _nodeNames[nId]; }
    unsigned getNetIndexByName(const char* str) const;
    /*
    const char* getNetNameByIndex(unsigned nId) const
    {    return _netNames[nId]; }
    */
  public:

    vector<const char*>	_nodeNames;	//vector of all node names,
					//indexed by nodeId
    vector<const char*>	_netNames;	//vector of all net  names,
					//indexed by netId
    HGNodeNamesMap	_nodeNamesMap;	//map from name to nodeId.
					//the names pointed to by this map
					//are owned by the _nodeNames vector
    HGNetNamesMap	_netNamesMap;	//map from name to netId.
					//the names pointed to by this map
					//are owned by the _netNames vector
  protected:

    HGraphFixed(HGraphParameters param) { _param = param;}

  public:

    HGraphFixed(unsigned numNodes  =0, 
		unsigned numWeights=1, 
		HGraphParameters param=HGraphParameters())
	{ _param = param; init(numNodes, numWeights); }

	//this ctor takes:
	// an aux file only
	// a netD and (optionally) and areM
	// a nodes, a nets and (optionally) an wts
	//the can be in any order.  The ctor determins the file
	//types from the filename extensions 

    HGraphFixed(const char* Filename1, 
		const char* Filename2=0, 
		const char* Filename3=0, 
		HGraphParameters param=HGraphParameters());

    HGraphFixed(const HGraphFixed&);

    virtual ~HGraphFixed();

    unsigned getNumNodes() const { return _nodes.size(); }
    unsigned getNumEdges() const { return _edges.size(); }

    double getAvgNodeDegree() const { return 1.0*getNumPins()/getNumNodes(); }
    double getAvgEdgeDegree() const { return 1.0*getNumPins()/getNumEdges(); }

    itHGFNodeGlobal nodesBegin() const { return _nodes.begin(); }
    itHGFNodeGlobal nodesEnd()   const { return _nodes.end();   }

    itHGFNodeGlobal terminalsBegin() const { return _nodes.begin(); }
    itHGFNodeGlobal terminalsEnd()   const { return _nodes.begin()+_numTerminals; }

    itHGFEdgeGlobal edgesBegin() const { return _edges.begin(); }
    itHGFEdgeGlobal edgesEnd()   const { return _edges.end();   }

    itHGFNodeGlobalMutable nodesBegin(){ return _nodes.begin(); }
    itHGFNodeGlobalMutable nodesEnd()  { return _nodes.end();   }
    itHGFEdgeGlobalMutable edgesBegin(){ return _edges.begin(); }
    itHGFEdgeGlobalMutable edgesEnd()  { return _edges.end();   }

    const HGFNode& getNodeByIdx(unsigned nodeIndex) const
    {
        abkassert3(nodeIndex<getNumNodes(), nodeIndex,
           " is an out-of-range index, max=", getNumNodes());
        return *_nodes[nodeIndex];
    }

    bool  haveSuchNode(const char* name) const
    {
        HGNodeNamesMap::const_iterator nameItr = _nodeNamesMap.find(name);
        return nameItr != _nodeNamesMap.end(); 
    }

    bool  haveSuchNet(const char* name) const
    {
        HGNetNamesMap::const_iterator nameItr = _netNamesMap.find(name);
        return nameItr != _netNamesMap.end(); 
    }

    const HGFNode& getNodeByName(const char* name) const
    {
	HGNodeNamesMap::const_iterator nameItr = _nodeNamesMap.find(name);
	abkassert2(nameItr != _nodeNamesMap.end(), 
		"name not found in getNodeByName: ", name);
        return *_nodes[(*nameItr).second];
    }

    const HGFEdge& getNetByName(const char* name) const
    {
	HGNetNamesMap::const_iterator nameItr = _netNamesMap.find(name);
	abkassert2(nameItr != _netNamesMap.end(), 
		"name not found in getNetByName: ", name);
        return *_edges[(*nameItr).second];
    }


    HGFNode& getNodeByIdx(unsigned nodeIndex)
    {
        abkassert3(nodeIndex<getNumNodes(), nodeIndex,
           " is an out-of-range index, max=", getNumNodes());
        return *_nodes[nodeIndex];
    }

    HGFNode& getNodeByName(const char* name)
    {
	HGNodeNamesMap::const_iterator nameItr = _nodeNamesMap.find(name);
	abkassert2(nameItr != _nodeNamesMap.end(), 
		"name not found in getNodeByName: ", name);
        return *_nodes[(*nameItr).second];
    }

    const HGFEdge& getEdgeByIdx(unsigned edgeIndex) const
    {
        abkassert(edgeIndex<getNumEdges(), "edge index too big");
        return *_edges[edgeIndex];
    }

    HGFEdge& getEdgeByIdx(unsigned edgeIndex)
    {
        abkassert(edgeIndex<getNumEdges(), "edge index too big");
        return *_edges[edgeIndex];
    }

    HGFEdge& getNetByName(const char* name)
    {
	HGNetNamesMap::const_iterator nameItr = _netNamesMap.find(name);
	abkassert2(nameItr != _netNamesMap.end(), 
		"name not found in getNetByName: ", name);
        return *_edges[(*nameItr).second];
    }

    const Permutation& getNodesSortedByWeights() const
    {
        if (_weightSort.getSize()==0)
            computeNodesSortedByWeights();
        return _weightSort;
    }

    const Permutation& getNodesSortedByDegrees() const
    {
        if (_degreeSort.getSize()==0)
            computeNodesSortedByDegrees();
        return _degreeSort;
    }

    unsigned maxNodeIndex() const { return _nodes.size()-1; }
    unsigned maxEdgeIndex() const { return _edges.size()-1; }

    virtual HGFEdge* addEdge(HGWeight weight=HGWeight(1.0));
    virtual HGFEdge* fastAddEdge(unsigned numPins, HGWeight weight=HGWeight());
    void adviseNodeDegrees(const vector<unsigned>& nodeDegrees);

    virtual void finalize();
    // after changes are made to the graph, this method must be called
    // before any methods can be called that rely on src/snk/srcSnk distinction

    void saveAsNetDAre(const char* baseName) const;
    virtual void saveAsNodesNetsWts(const char* baseName) const;

    virtual bool isConsistent() const;
    virtual void sortAsDB();

    void printEdgeSizeStats(ostream& str=cout)   const;
    void printEdgeWtStats(ostream& str=cout)     const;
    void printNodeWtStats(ostream& str=cout)     const;
    void printNodeDegreeStats(ostream& str=cout) const;

    friend ostream& operator<<(ostream& out, const HGraphFixed& graph);

    const char* getNetNameByIndex(unsigned nId) const
    {    return _netNames[nId]; }
};

inline void HGraphFixed::addSrc(const HGFNode& node, const HGFEdge& edge)
{ _srcs.push_back(pair<unsigned,unsigned>(node.getIndex(), edge.getIndex())); }

inline void HGraphFixed::addSnk(const HGFNode& node, const HGFEdge& edge)
{ _snks.push_back(pair<unsigned,unsigned>(node.getIndex(), edge.getIndex())); }

inline void HGraphFixed::addSrcSnk(const HGFNode& node, const HGFEdge& edge)
{ _srcSnks.push_back(pair<unsigned,unsigned>(node.getIndex(), edge.getIndex())); }

inline void HGraphFixed::fastAddSrcSnk(HGFNode* node, HGFEdge* edge)
{
   edge->_nodes.push_back(node);
   node->_edges.push_back(edge);
}

inline HGFEdge* HGraphFixed::fastAddEdge(unsigned numPins, HGWeight weight) 
{
    abkassert(_addEdgeStyle != SLOW_ADDEDGE_USED, 
	"cannot mix fast and slow addEdge in one HGraph object");

    abkassert2(_param.makeAllSrcSnk, 
	"cannot use fast addEdge if graph has directionality",
	" (i.e., allSrcSnks is false)");

    #ifdef DABKDEBUG
      _addEdgeStyle=FAST_ADDEDGE_USED;

      if (_param.netThreshold<numPins) 
      {
        abkfatal(_param.removeBigNets, 
		"fast addEdge requires that removeBigNets is set to true");
        return &mDummyEdge;
      }
    #endif

    HGFEdge* edge=new HGFEdge(weight);
    _edges.push_back(edge);
    edge->_index=_edges.size()-1;
    edge->_nodes.reserve(numPins);
    edge->_snksBegin = edge->_srcSnksBegin = edge->_nodes.begin();
    return edge;
}

#include "hgFNodeInline.h"
#include "hgFEdgeInline.h"

struct HGNodeSortByIndex
{
    bool operator()(const HGFNode* ptr1, const HGFNode* ptr2) const
    { return ptr1->getIndex()<ptr2->getIndex(); }
};

struct HGNodeSortByWeight
{
    bool operator()(const HGFNode* ptr1, const HGFNode* ptr2) const
    { return ptr1->getWeight()<ptr2->getWeight(); }
};

struct HGNodeIdSortByWeights
{
    const HGraphFixed& _hgraph;

    HGNodeIdSortByWeights(const HGraphFixed& hgraph) : _hgraph(hgraph) {}

    bool operator()(unsigned n1, unsigned n2)
    { return _hgraph.getNodeByIdx(n1).getWeight() < 
             _hgraph.getNodeByIdx(n2).getWeight(); }
};

struct HGNodeSortByAscendingDegree
{
    bool operator()(const HGFNode* ptr1, const HGFNode* ptr2) const
    { return ptr1->getDegree()<ptr2->getDegree(); }
};

struct HGNodeSortByDescendingDegree
{
    bool operator()(const HGFNode* ptr1, const HGFNode* ptr2) const
    { return ptr1->getDegree()>ptr2->getDegree(); }
};

struct HGNodeIdSortByDegrees
{
    const HGraphFixed& _graph;
    HGNodeIdSortByDegrees(const HGraphFixed& graph): _graph(graph) {}

    bool operator()(unsigned lhs, unsigned rhs) const
    {
        return _graph.getNodeByIdx(lhs).getDegree()<
               _graph.getNodeByIdx(rhs).getDegree();
    }
};

struct HGEdgeSortByIndex
{
    bool operator()(const HGFEdge* ptr1, const HGFEdge* ptr2) const
    { return ptr1->getIndex()<ptr2->getIndex(); }
};

struct HGEdgeSortByAscendingDegree
{
    bool operator()(const HGFEdge* ptr1, const HGFEdge* ptr2) const
    { return ptr1->getDegree()<ptr2->getDegree(); }
};

struct HGEdgeSortByDescendingDegree
{
    bool operator()(const HGFEdge* ptr1, const HGFEdge* ptr2) const
    { return ptr1->getDegree()>ptr2->getDegree(); }
};

class HGAlgo
{
  public:
    unsigned connectedComponents(const HGraphFixed& graph);
        // Compute the number of connected components in given graphFixed
};


#endif


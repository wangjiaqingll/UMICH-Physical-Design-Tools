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














//created by Andrew Caldwell on 02/02/99
#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include "hgWDims.h"
#include "ABKCommon/pathDelims.h"

#ifdef _MSC_VER
  #pragma warning(disable:4355)
#endif

HGraphWDimensions::HGraphWDimensions(
			unsigned  numNodes, unsigned numWeights,
			HGraphParameters param,
			const vector<double>& heights,
			const vector<double>& widths)

	: HGraphFixed(numNodes, numWeights, param),
	 _nodeHeights(heights), _nodeWidths(widths),
	 _nonConstThis(this)
{
  setupUnitDimensions();
  setupCenterPinOffsets();
  setupEmptySymmetries();
  _isMacro  = vector<bool>(numNodes, false);
}

HGraphWDimensions::HGraphWDimensions(
			const char* fileName1,
			const char* fileName2,
			const char* fileName3,
			HGraphParameters  param)
	:HGraphFixed(param), _nonConstThis(this)
{
    abkfatal(fileName1, "null char* for first filename in HGraphWDims ctor");

    const char* auxFileName   = 0;
    const char* netDFileName  = 0;
    const char* areMFileName  = 0;
    const char* wtsFileName   = 0;
    const char* netsFileName  = 0;
    const char* nodesFileName = 0;

    const char* files[3] = {fileName1, fileName2, fileName3};

    unsigned f;
    for(f = 0; f < 3; f++)
    {
        if(!files[f]) continue;

        if(strstr(files[f],".aux"))     auxFileName   = files[f];
        else if(strstr(files[f],".wts"))        wtsFileName   = files[f];
        else if(strstr(files[f],".are"))        areMFileName  = files[f];
        else if(strstr(files[f],".nodes"))      nodesFileName = files[f];
        else if(strstr(files[f],".netD"))       netDFileName  = files[f];
        else if(strstr(files[f],".nets"))       netsFileName  = files[f];
        else if(strstr(files[f],".net"))        netDFileName  = files[f];
    }

    abkfatal(!((netDFileName || areMFileName) &&
               (netsFileName || nodesFileName || wtsFileName)),
                "can't mix netD/areM and nets/nodes/wts");

    if(auxFileName) parseAux(auxFileName);
    else if(netDFileName)
    {
        readNetD(netDFileName);
        if(areMFileName) readAreM(areMFileName);
        else
        {       //set unit weights
            abkfatal(_numMultiWeights == 0, "there should be no weights here");
            _numMultiWeights = 1;
            _multiWeights = vector<HGWeight>(_nodes.size(), 1.0);
        }
    }
    else if(nodesFileName)
    {
        abkfatal(netsFileName, "must have both a nodes and nets file");

        readNodes(nodesFileName);
        readNets(netsFileName);
        if(wtsFileName) readWts(wtsFileName);
        else
        {       //set unit weights
            abkfatal(_numMultiWeights == 0, "there should be no weights here");
            _numMultiWeights = 1;
            _multiWeights = vector<HGWeight>(_nodes.size(), 1.0);
        }
    }
    else
    {
        cerr<<"aux, netD or nets file not found.  ";
        cerr<<"Filename arguments were:"<<endl;
        if(fileName1)   cout<<fileName1<<endl;
        if(fileName2)   cout<<fileName2<<endl;
        if(fileName3)   cout<<fileName3<<endl;
    }

    finalize();

    setupUnitDimensions();
    setupCenterPinOffsets();
    setupEmptySymmetries();
    _isMacro  = vector<bool>(_nodes.size(), false);
}

HGraphWDimensions::HGraphWDimensions(
			const HGraphFixed& hg,
			const vector<double>& heights,
			const vector<double>& widths)

	:HGraphFixed(hg), _nodeHeights(heights), _nodeWidths(widths),
	 _nonConstThis(this)
{
    setupUnitDimensions();
    setupCenterPinOffsets();
    setupEmptySymmetries();
    _isMacro  = vector<bool>(_nodes.size(), false);
}

void HGraphWDimensions::setupUnitDimensions()
{
    if(_nodeWidths.size() == 0)
    {
	_nodeWidths  = vector<double>(_nodes.size(), 1.0);
	_nodeHeights = vector<double>(_nodes.size(), 1.0);
    }
}

void HGraphWDimensions::setupEmptySymmetries()
{
    if(_nodeSymmetries.size()  != _nodes.size())
	_nodeSymmetries  = vector<Symmetry>(_nodes.size());
}

void HGraphWDimensions::setupCenterPinOffsets()
{
    if(_pinOffsets.size() == 0)
    {
      _nodesOnEdgePinIdx = vector<unsigned>(_numPins);
      _edgesOnNodePinIdx = vector<unsigned>(_numPins);
      _noeBeginEnd       = vector<unsigned>(_edges.size()+1);
      _eonBeginEnd       = vector<unsigned>(_nodes.size()+1);
      _pinOffsets        = vector<Point>   (_nodes.size());

      unsigned offset = 0;
      unsigned n;
      for(n = 0; n < _nodes.size(); n++)
      {
	_eonBeginEnd[n] = offset;
	_pinOffsets[n].x = _nodeWidths[n]/2.0;
	_pinOffsets[n].y = _nodeHeights[n]/2.0;

	for(unsigned e = 0; e < _nodes[n]->getDegree(); e++, offset++)
	    _edgesOnNodePinIdx[offset] = n;	//all pins are in the middle    
      }
      _eonBeginEnd[n] = offset;

      itHGFNodeLocal nIt;
      offset = 0;
      unsigned e;
      for(e = 0; e < _edges.size(); e++)
      {
	_noeBeginEnd[e] = offset;
	for(nIt = _edges[e]->nodesBegin(); 
	    nIt != _edges[e]->nodesEnd(); nIt++, offset++)
	    _nodesOnEdgePinIdx[offset] = (*nIt)->getIndex();
      }
      _noeBeginEnd[e] = offset;
    }
}

void HGraphWDimensions::makeCenterPinOffsets()
{
  unsigned n;
  for(n = 0; n < _nodes.size(); n++)
    {
     for(unsigned e = 0; e < _nodes[n]->getDegree(); e++)
	{
	  unsigned pOffsetIdx = _edgesOnNodePinIdx[_eonBeginEnd[n]+e];
	  _pinOffsets[pOffsetIdx].x = _nodeWidths[n]/2.0;
	  _pinOffsets[pOffsetIdx].y = _nodeHeights[n]/2.0;
	}
    }
}

void HGraphWDimensions::updateNOEPinOffsets(unsigned nodeOffset, 
					   unsigned edgeId, Point newPinOffset)
{
  unsigned pOffsetIdx = _nodesOnEdgePinIdx[_noeBeginEnd[edgeId]+nodeOffset];
  _pinOffsets[pOffsetIdx] = newPinOffset;
}

void HGraphWDimensions::updateEONPinOffsets(unsigned edgeOffset, 
					   unsigned nodeId, Point newPinOffset)
{
  unsigned pOffsetIdx = _edgesOnNodePinIdx[_eonBeginEnd[nodeId]+edgeOffset];
  _pinOffsets[pOffsetIdx] = newPinOffset;
}

void HGraphWDimensions::calcRealOffset(unsigned cellId,
				       Point&   pinOffset, 
				       const    Orient& ori)
{
    double h = getNodeHeight(cellId);
    double w = getNodeWidth (cellId);

    if(!ori.isFaceUp()) pinOffset.x = w - pinOffset.x;

    double tmp;
    switch(ori.getAngle())
    {
        case   0:   //( x,  y)
            break;
        case  90:   //( y, -x)
            tmp = pinOffset.x;
            pinOffset.x = pinOffset.y;
            pinOffset.y = w - tmp;
            break;
        case 180:   //(-x, -y)
            pinOffset.x = w - pinOffset.x;
            pinOffset.y = h - pinOffset.y;
            break;
        case 270:   //(-y,  x)
            tmp = pinOffset.y;
            pinOffset.y = pinOffset.x;
            pinOffset.x = h - tmp;
            break;
        default: abkfatal(0," Orientation corrupted ");
    }

}

bool HGraphWDimensions::isConsistent() const 
{ 
    abkfatal(_finalized, "HGraphWDims was not finalized");
    return HGraphFixed::isConsistent(); 
}

void HGraphWDimensions::finalize()
{
   if (_param.verb.forActions>5)
      cout<< "Finalizing HGWDims"<<endl;
   abkfatal(_finalized==false, "cannot run finalize twice");

   if( (!_srcs.empty() || !_snks.empty() || !_srcSnks.empty()) &&
       (_srcsWPins.size() || _snksWPins.size() || _srcSnksWPins.size()))
	abkfatal(0, "Error: both HGraph and HGWDims node-net pairs are populated");

   if(!_srcs.empty() || !_snks.empty() || !_srcSnks.empty()) 
   {
	HGraphFixed::finalize(); 
	return; 
   }

   if(_addEdgeStyle==FAST_ADDEDGE_USED)
   { _finalized=true; return; }

/*    abkwarn(!_param.removeBigNets,
		"HGraphWDims does not support removal of large edges");*/
 

    vector<unsigned> nodeDegrees(getNumNodes());
    vector<unsigned> edgeDegrees(getNumEdges());

    std::vector<HGWDimsPin>::iterator it;
    _numPins = 0;

    for(it=_srcsWPins.begin(); it!=_srcsWPins.end(); ++it)
    {
        _numPins++;
        ++nodeDegrees[it->nodeId];
        ++edgeDegrees[it->edgeId];
    }

    for(it=_srcSnksWPins.begin(); it!=_srcSnksWPins.end(); ++it)
    {
        _numPins++;
        ++nodeDegrees[it->nodeId];
        ++edgeDegrees[it->edgeId];
    }

    for (it=_snksWPins.begin(); it!=_snksWPins.end(); ++it)
    {
        _numPins++;
        ++nodeDegrees[it->nodeId];
        ++edgeDegrees[it->edgeId];
    }

    if (_param.removeBigNets)
    {
        cout<<"Removing nets..."<<endl;
        //std::list<pair<unsigned, unsigned> >::iterator it;
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

        for (it=_srcsWPins.begin(); it!=_srcsWPins.end(); ++it)
            (*it).edgeId=old2new[(*it).edgeId];

        for (it=_srcSnksWPins.begin(); it!=_srcSnksWPins.end(); ++it)
            (*it).edgeId=old2new[(*it).edgeId];

        for (it=_snksWPins.begin(); it!=_snksWPins.end(); ++it)
            (*it).edgeId=old2new[(*it).edgeId];

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



	//setup all pin offsets info, except the actual offsets.
    _pinOffsets.reserve(_numPins);
    _noeBeginEnd  = vector<unsigned>(_edges.size()+1);
    _eonBeginEnd  = vector<unsigned>(_nodes.size()+1);

    unsigned n,v,e;
    unsigned pinsBegin = 0;
    for(n = 0; n < _nodes.size(); n++)
    {
        _eonBeginEnd[n] = pinsBegin;
        pinsBegin += nodeDegrees[n];
        getNodeByIdx(n)._edges.reserve(nodeDegrees[n]);
    }
    _eonBeginEnd[n] = pinsBegin;

    pinsBegin = 0;
    for(e = 0; e < _edges.size(); e++)
    {
        _noeBeginEnd[e] = pinsBegin;
        pinsBegin += edgeDegrees[e];
        getEdgeByIdx(e)._nodes.reserve(edgeDegrees[e]);
    }
    _noeBeginEnd[e] = pinsBegin;

    _nodesOnEdgePinIdx = vector<unsigned>(_numPins);
    _edgesOnNodePinIdx = vector<unsigned>(_numPins);

	//these are used to put the pin offsets in the right loc
    vector<unsigned> nodesNextPin (_nodes.size(), 0);
    vector<unsigned> edgesNextPin(_edges.size(), 0);


    for (it=_srcsWPins.begin(); it!=_srcsWPins.end(); ++it)
    {
        if((*it).edgeId==UINT_MAX)
           continue;
        if(edgeDegrees[(*it).edgeId] > _param.netThreshold)
           continue;
	unsigned nId = it->nodeId;
	unsigned eId = it->edgeId;

        getNodeByIdx(nId)._edges.push_back(&getEdgeByIdx(eId));
        getEdgeByIdx(eId)._nodes.push_back(&getNodeByIdx(nId));
	_nodesOnEdgePinIdx[_noeBeginEnd[eId] + edgesNextPin[eId]] = 
	  _edgesOnNodePinIdx[_eonBeginEnd[nId] + nodesNextPin[nId]] = 
						_pinOffsets.size();
	edgesNextPin[eId]++;
	nodesNextPin[nId]++;
	_pinOffsets.push_back(it->pOffset);
    }
    for (v=0; v!=getNumNodes(); ++v)
        getNodeByIdx(v)._srcSnksBegin=getNodeByIdx(v)._edges.end();
    for (e=0; e!=getNumEdges(); ++e)
        getEdgeByIdx(e)._srcSnksBegin=getEdgeByIdx(e)._nodes.end();

    for (it=_srcSnksWPins.begin(); it!=_srcSnksWPins.end(); ++it)
    {
        if((*it).edgeId==UINT_MAX)
           continue;
        if(edgeDegrees[(*it).edgeId] > _param.netThreshold)
           continue;
	unsigned nId = it->nodeId;
	unsigned eId = it->edgeId;
        getNodeByIdx(nId)._edges.push_back(&getEdgeByIdx(eId));
        getEdgeByIdx(eId)._nodes.push_back(&getNodeByIdx(nId));
	_nodesOnEdgePinIdx[_noeBeginEnd[eId] + edgesNextPin[eId]] = 
	  _edgesOnNodePinIdx[_eonBeginEnd[nId] + nodesNextPin[nId]] = 
						_pinOffsets.size();
	edgesNextPin[eId]++;
	nodesNextPin[nId]++;
	_pinOffsets.push_back(it->pOffset);
    }
    for (v=0; v!=getNumNodes(); ++v)
        getNodeByIdx(v)._snksBegin=getNodeByIdx(v)._edges.end();
    for (e=0; e!=getNumEdges(); ++e)
        getEdgeByIdx(e)._snksBegin=getEdgeByIdx(e)._nodes.end();

    for (it=_snksWPins.begin(); it!=_snksWPins.end(); ++it)
    {
        if((*it).edgeId==UINT_MAX)
           continue;
        if(edgeDegrees[(*it).edgeId] > _param.netThreshold)
           continue;
	unsigned nId = it->nodeId;
	unsigned eId = it->edgeId;
        getNodeByIdx(nId)._edges.push_back(&getEdgeByIdx(eId));
        getEdgeByIdx(eId)._nodes.push_back(&getNodeByIdx(nId));
	_nodesOnEdgePinIdx[_noeBeginEnd[eId] + edgesNextPin[eId]] = 
	  _edgesOnNodePinIdx[_eonBeginEnd[nId] + nodesNextPin[nId]] = 
						_pinOffsets.size();
	edgesNextPin[eId]++;
	nodesNextPin[nId]++;
	_pinOffsets.push_back(it->pOffset);
    }
    _srcsWPins.clear();
    _srcSnksWPins.clear();
    _snksWPins.clear();

    sortNodes();
    sortEdges();

    _finalized=true;
    if (_netNames.size()==0)
    _netNames  = vector<const char*>(_edges.size(), (const char*)(NULL));
}

void HGraphWDimensions::setNodeDims(double newHeight)
{
    for(unsigned n = 0; n < _nodes.size(); n++)
    {
	if(_nodes[n]->getWeight() > 0)
	{
	  _nodeHeights[n] = newHeight;
	  _nodeWidths[n]  = _nodes[n]->getWeight()/newHeight;
        }
    }
}

void HGraphWDimensions::setNodeDimsMacros(double newHeight)
{
  double totArea = 0;
  unsigned n;
  for(n = 0; n < _nodes.size(); n++)
    totArea += _nodes[n]->getWeight();

  
  //double threshold = 0.001*totArea;
  for(n = 0; n < _nodes.size(); n++)
    {
      if(_nodes[n]->getWeight() > 0)
	{
	  double potentialWidth = _nodes[n]->getWeight()/newHeight;

	  //if(_nodes[n]->getWeight() < threshold)
	  if(potentialWidth < 15*newHeight)
	    {
	      _nodeHeights[n] = rint(newHeight);
	      _nodeWidths[n]  = rint(_nodes[n]->getWeight()/newHeight);
	      _isMacro[n] = true;
	    }
	  else
	    {
	      double potentialHeight = rint(sqrt(_nodes[n]->getWeight()));
	      double remainder = int(potentialHeight)%int(newHeight);
	      if(remainder < newHeight/2)
		potentialHeight -= remainder;
	      else
		potentialHeight += (newHeight-remainder);
	      
	      _nodeHeights[n] = rint(potentialHeight);
	      _nodeWidths[n]  = rint(_nodes[n]->getWeight()/_nodeHeights[n]);
	    }
        }
    }
}

void HGraphWDimensions:: markNodesAsMacro(double maxWidth, double maxHeight)
{
  for(unsigned i=0; i<_nodes.size(); ++i)
    {
      if(!isTerminal(i))
	{
	  if(_nodeWidths[i] > maxWidth || _nodeHeights[i] > maxHeight)
	    _isMacro[i] = true;
	}
    }
}

double HGraphWDimensions::getNodesArea()
{
  double nodesArea = 0;
  for(unsigned n = 0; n < _nodes.size(); n++)
    {
      if(!isTerminal(n))
	nodesArea += _nodeWidths[n]*_nodeHeights[n];
    }
  return(nodesArea);
}

double HGraphWDimensions::getAvgNodeWidth(void)
{
  double avgWidth = 0;
  int numNodes = 0;
  for(unsigned n = 0; n < _nodes.size(); n++)
    {
      if(!isTerminal(n))
	{
	  avgWidth += _nodeWidths[n];
	  numNodes++;
	}
    }
  avgWidth /= numNodes;
  return(avgWidth);
}

double HGraphWDimensions::getAvgNodeHeight(void)
{
  double avgHeight = 0;
  int numNodes = 0;
  for(unsigned n = 0; n < _nodes.size(); n++)
    {
      if(!isTerminal(n))
	{
	  avgHeight += _nodeHeights[n];
	  ++numNodes;
	}
    }
  avgHeight /= numNodes;
  return(avgHeight);
}


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

#include <algorithm>
#include "partProbForCapo.h"
#include "capoBlock.h"
#include "subHGForCapo.h"
#include "Partitioning/termiProp.h"
#include "capoPlacer.h"

bit_vector PartitioningProblemForCapo::_edgesVisited;

void PartitioningProblemForCapo::setPartDims()
{
    abkassert(_blocks.size() == 2, 
	"error: called setPartDims without 2 blocks");

    _nDim = _mDim = 0; 

    if(_blocks[0]->getBBox().yMin >= _blocks[1]->getBBox().yMax)	//hcut
    {	_nDim = 2; _mDim = 1;}

    if(_blocks[1]->getBBox().yMin >= _blocks[0]->getBBox().yMax)  	
    {   //hcut, blocks reversed
	cout<<"Blocks are reversed: HCut"<<endl;
	cout<<"P0: "<<*_blocks[0]<<endl;
	cout<<"P1: "<<*_blocks[1]<<endl;
	abkfatal(0,"Blocks are reversed: p0 should be above p1");
    }

    if(_blocks[0]->getBBox().xMax <= _blocks[1]->getBBox().xMin)	//vcut
    {	_nDim = 1; _mDim = 2;}

    if(_blocks[1]->getBBox().xMax <= _blocks[0]->getBBox().xMin)	
    {   //vcut, blocks reversed
	cout<<"Blocks are reversed: VCut"<<endl;
	cout<<"P0: "<<*_blocks[0]<<endl;
	cout<<"P1: "<<*_blocks[1]<<endl;
	abkfatal(0,"Blocks are reversed: p0 should be left of p1");
    }

    if(_nDim == 0 )//no clear seperating line....
    {
	Point p0Center = _blocks[0]->getCenter();
	Point p1Center = _blocks[1]->getCenter();

	if(p1Center.y > _blocks[0]->getBBox().yMax &&
	   p0Center.y < _blocks[1]->getBBox().yMin)
	       abkfatal(0, "Blocks are reversed: p0 should be above p1");

	if(p0Center.y > _blocks[1]->getBBox().yMax &&
	   p1Center.y < _blocks[0]->getBBox().yMin)
	   { _nDim = 2; _mDim = 1;}
	else
	{
	   cout<<"Blocks with Horizontal Overlap"<<endl;
	   cout<<"P0: "<<*_blocks[0]<<endl;
	   cout<<"P1: "<<*_blocks[1]<<endl;
	   abkfatal(0, "blocks w/ horizontal overlap");
	}
    }

    const CapoBlock& block0 = *_blocks[0];
    const CapoBlock& block1 = *_blocks[1];

    double C0 = block0.getTotalCellArea();
    double C1 = block1.getTotalCellArea();
    _totalWeight = new vector<double>(1, C0+C1);

    _partitions  = new vector<BBox>(2,BBox());
    (*_partitions)[0] = block0.getBBox();
    (*_partitions)[1] = block1.getBBox();


    _capacities     = new vector<vector<double> >(2,vector<double>(1,0));
    _maxCapacities  = new vector<vector<double> >(2,vector<double>(1,0));
    _minCapacities  = new vector<vector<double> >(2,vector<double>(1,0));

    double S0 = block0.getCapacity();
    double S1 = block1.getCapacity();
    double w0 = max(block0.getRelativeWhitespace(), 0.0);
    double w1 = max(block1.getRelativeWhitespace(), 0.0);

    double P0Max = max(S0-(S0*w0), S0-(S0*w1));
    double P1Max = max(S1-(S1*w0), S1-(S1*w1));
    double P0Min = min(S0-(S0*w0), S0-(S0*w1));
    double P1Min = min(S1-(S1*w0), S1-(S1*w1));

    double Target0 = (P0Max+P0Min)/2.0;
    double Target1 = (P1Max+P1Min)/2.0;

    (*_maxCapacities)[0][0] = P0Max;
    (*_maxCapacities)[1][0] = P1Max;
    (*_minCapacities)[0][0] = P0Min;
    (*_minCapacities)[1][0] = P1Min;
    (*_capacities)[0][0]    = Target0;
    (*_capacities)[1][0]    = Target1;


    if(_verb.forActions > 7)
    {	
	cout<<"Repartitioning "<<endl;
	cout<<"  P0 Capacity(S0)      "<<S0<<endl;
	cout<<"  P1 Capacity(S1)      "<<S1<<endl;
	cout<<"  Total Cell Area(C)   "<<C0+C1<<endl;
        cout<<"  Original %WS         "<<w0*100 <<"/"<<w1*100<<endl;
	cout<<"  Targets              "<<Target0<<"/"<<Target1<<endl;
	cout<<"  Max                  "<<P0Max  <<"/"<<P1Max<<endl;
	cout<<"  Min %WhiteSpace      "<<((S0-P0Max)/S0)*100<<"/"
				       <<((S1-P1Max)/S1)*100<<endl;
	cout<<"  Min                  "<<P0Min  <<"/"<<P1Min<<endl;
	cout<<"  Max %WhiteSpace      "<<((S0-P0Min)/S0)*100<<"/"
				       <<((S1-P1Min)/S1)*100<<endl;
	cout<<"  Tolerance %          "<<100.0*(P0Max-Target0)/Target0<<"/"<<
				         100.0*(P1Max-Target1)/Target1<<endl;
    }

    _setPartitionCenters();
}

void PartitioningProblemForCapo::setPartDims(const vector<double>& partDims)
{
    abkassert(_blocks.size() == 2, 
	"error: called setPartDims without 2 blocks");

    _nDim = _mDim = 0; 

    if(_blocks[0]->getBBox().yMin >= _blocks[1]->getBBox().yMax)	//hcut
    {	_nDim = 2; _mDim = 1;}

    if(_blocks[1]->getBBox().yMin >= _blocks[0]->getBBox().yMax)  	
    {   //hcut, blocks reversed
	cout<<"Blocks are reversed: HCut"<<endl;
	cout<<"P0: "<<*_blocks[0]<<endl;
	cout<<"P1: "<<*_blocks[1]<<endl;
	abkfatal(0,"Blocks are reversed: p0 should be above p1");
    }

    if(_blocks[0]->getBBox().xMax <= _blocks[1]->getBBox().xMin)	//vcut
    {	_nDim = 1; _mDim = 2;}

    if(_blocks[1]->getBBox().xMax <= _blocks[0]->getBBox().xMin)	
    {   //vcut, blocks reversed
	cout<<"Blocks are reversed: VCut"<<endl;
	cout<<"P0: "<<*_blocks[0]<<endl;
	cout<<"P1: "<<*_blocks[1]<<endl;
	abkfatal(0,"Blocks are reversed: p0 should be left of p1");
    }

    if(_nDim == 0 )//no clear seperating line....
    {
	Point p0Center = _blocks[0]->getCenter();
	Point p1Center = _blocks[1]->getCenter();

	if(p1Center.y > _blocks[0]->getBBox().yMax &&
	   p0Center.y < _blocks[1]->getBBox().yMin)
	       abkfatal(0, "Blocks are reversed: p0 should be above p1");

	if(p0Center.y > _blocks[1]->getBBox().yMax &&
	   p1Center.y < _blocks[0]->getBBox().yMin)
	   { _nDim = 2; _mDim = 1;}
	else
	{
	   cout<<"Blocks with Horizontal Overlap"<<endl;
	   cout<<"P0: "<<*_blocks[0]<<endl;
	   cout<<"P1: "<<*_blocks[1]<<endl;
	   abkfatal(0, "blocks w/ horizontal overlap");
	}
    }

    const CapoBlock& block0 = *_blocks[0];
    const CapoBlock& block1 = *_blocks[1];

    double C0 = block0.getTotalCellArea();
    double C1 = block1.getTotalCellArea();
    _totalWeight = new vector<double>(1, C0+C1);

    _partitions  = new vector<BBox>(2,BBox());
    (*_partitions)[0] = block0.getBBox();
    (*_partitions)[1] = block1.getBBox();


    _capacities     = new vector<vector<double> >(2,vector<double>(1,0));
    _maxCapacities  = new vector<vector<double> >(2,vector<double>(1,0));
    _minCapacities  = new vector<vector<double> >(2,vector<double>(1,0));

    double S0 = block0.getCapacity();
    double S1 = block1.getCapacity();
    double w0 = max(block0.getRelativeWhitespace(), 0.0);
    double w1 = max(block1.getRelativeWhitespace(), 0.0);
    double totArea = C0+C1;
    double totWS = S0+S1-C0-C1;
    double ws0 = totWS*partDims[1];
    double ws1 = totWS*partDims[0];
    double Target0, Target1;
    if(1)
      {
        Target0 = totArea*(partDims[0]+0.5)/2;
        Target1 = totArea*(partDims[1]+0.5)/2;
      }
    else if(totWS < 0)
      {
        Target0 = 0.5*totArea;
        Target1 = 0.5*totArea;
      } 
    else
      {
        Target0 = S0-ws0;
        Target1 = S1-ws1;
      }

    if(Target0 >= S0 || Target1 >= S1)
      {
	if(totWS < 0)
	  {
	    Target0 = 0.5*totArea;
	    Target1 = 0.5*totArea;
	  } 
	else
	  {
	    Target0 = S0-ws0;
	    Target1 = S1-ws1;
	  }
      }
    
    if(Target0 > S0)
     {
      Target0 = S0;
      Target1 = totArea - Target0;
     }
    if(Target1 > S1)
     {
      Target1 = S1;
      Target0 = totArea - Target1;
     }
    
    if(Target0 == 0)
      {
	Target0 = 0.2*totArea;
	Target1 = 0.8*totArea;
      }
    else if(Target1 == 0)
      {
	Target0 = 0.8*totArea;
	Target1 = 0.2*totArea;
      }
    
    double tolPct = 0;
    double P0Max = min(Target0*(1.0 + tolPct/100.0), S0);
    double P0Min = Target0*(1.0 - tolPct/100.0);
    double P1Max = min(Target1*(1.0 + tolPct/100.0), S1);
    double P1Min = Target1*(1.0 - tolPct/100.0);

    (*_maxCapacities)[0][0] = P0Max;
    (*_maxCapacities)[1][0] = P1Max;
    (*_minCapacities)[0][0] = P0Min;
    (*_minCapacities)[1][0] = P1Min;
    (*_capacities)[0][0]    = Target0;
    (*_capacities)[1][0]    = Target1;

    if(1/*_verb.forActions > 7*/)
    {	
	cout<<"Repartitioning "<<endl;
	cout<<"  P0 Capacity(S0)      "<<S0<<endl;
	cout<<"  P1 Capacity(S1)      "<<S1<<endl;
	cout<<"  Total Cell Area(C)   "<<C0+C1<<endl;
	cout<<"  PartDims             "<<partDims[0]<<"/"<<partDims[1]<<endl;
        cout<<"  Original %WS         "<<w0*100 <<"/"<<w1*100<<endl;
	cout<<"  Targets              "<<Target0<<"/"<<Target1<<endl;
	cout<<"  Max                  "<<P0Max  <<"/"<<P1Max<<endl;
	cout<<"  Min %WhiteSpace      "<<((S0-P0Max)/S0)*100<<"/"
				       <<((S1-P1Max)/S1)*100<<endl;
	cout<<"  Min                  "<<P0Min  <<"/"<<P1Min<<endl;
	cout<<"  Max %WhiteSpace      "<<((S0-P0Min)/S0)*100<<"/"
				       <<((S1-P1Min)/S1)*100<<endl;
	cout<<"  Tolerance %          "<<100.0*(P0Max-Target0)/Target0<<"/"<<
				         100.0*(P1Max-Target1)/Target1<<endl;
    }

    _setPartitionCenters();
}


void PartitioningProblemForCapo::setBuffer()
{
  unsigned numStarts = 1;

  if(_blocks.size() == 1)
  {
    _solnBuffers = new PartitioningBuffer(_hgraph->getNumNodes(), 1);
    _solnBuffers->setBeginUsedSoln(0);
    _solnBuffers->setEndUsedSoln(1);

    PartitionIds clearId;
    clearId.setToAll(2);
    std::fill((*_solnBuffers)[0].begin(), (*_solnBuffers)[0].end(), clearId);
    //set the terminal locations
    (*_solnBuffers)[0][0].setToPart(0);
    (*_solnBuffers)[0][1].setToPart(1);
  }
  else
  {
    numStarts = 1; 
    
    _solnBuffers =
                new PartitioningBuffer(_hgraph->getNumNodes(), numStarts);
    _solnBuffers->setBeginUsedSoln(0);
    _solnBuffers->setEndUsedSoln(numStarts);

    for(unsigned s = 0; s < numStarts; s++)
    {
	//set the terminal locations
	(*_solnBuffers)[s][0].setToPart(0);
	(*_solnBuffers)[s][1].setToPart(1);


	//put the movable nodes in their current blocks
	itHGFNodeGlobal n = _subHG->nodesBegin();
	n += _subHG->getNumTerminals();
	for( ; n != _subHG->nodesEnd(); n++)
	{
	    unsigned origId = _subHG->newNode2OrigIdx((*n)->getIndex());

	    if(_cellToBlockMap[origId] == _blocks[0])
	  	(*_solnBuffers)[s][(*n)->getIndex()].setToPart(0);
	    else if(_cellToBlockMap[origId] == _blocks[1])
	  	(*_solnBuffers)[s][(*n)->getIndex()].setToPart(1);
 	    else
	        abkfatal(_subHG->isTerminal(s), 
				"non-terminal is not in P0 or P1");
	}
    }

    _bestSolnNum = 0;
  }

  _costs      = vector<double>(numStarts);
  _violations = vector<double>(numStarts);
  _imbalances = vector<double>(numStarts);
}

void PartitioningProblemForCapo::buildHGraph(bool honourGrpConstr)
{
    _numOrigEdges   = _numEssentialEdges   = 0;
    _origEdgeDegree = _essentialEdgeDegree = 0;

    if(_edgesVisited.size() < _hgraphOfNetlist.getNumEdges())
    {
	unsigned numToAdd = _hgraphOfNetlist.getNumEdges() -
		            _edgesVisited.size();
        _edgesVisited.insert(_edgesVisited.end(), 
				numToAdd, false);
    }

    std::fill(_edgesVisited.begin(), _edgesVisited.end(), false); 

    vector<unsigned> unionOfCells = _blocks[0]->getCellIds();

    for(unsigned b = 1; b < _blocks.size(); b++)
	unionOfCells.insert(unionOfCells.end(), 
		_blocks[b]->cellIdsBegin(), _blocks[b]->cellIdsEnd());

    SubHGraphForCapo* newHG = new SubHGraphForCapo(2, unionOfCells.size(),
					_hgraphOfNetlist.getNumEdges());

    _hgraph = static_cast<HGraphFixed*>(newHG);
    _subHG  = static_cast<SubHGraph*>(newHG);

    newHG->_param.makeAllSrcSnk = true;

    std::vector<unsigned>::const_iterator cItr;
    itHGFEdgeLocal eItr;

    newHG->_param.makeAllSrcSnk = true;
    unsigned newId = 2;
    for(cItr= unionOfCells.begin();cItr != unionOfCells.end();cItr++,newId++)
	newHG->addNode(_hgraphOfNetlist.getNodeByIdx(*cItr), newId);

    vector<unsigned> nonTermsOnNet; //ids of all nonTerminals on this edge
    unsigned         compoundTerm;  // the id of the (1) terminal on this edge
    bool	     essential;     //is the net being examined essential?

    double p0Boundry, p1Boundry;
    const BBox& p0Box = (*_partitions)[0];
    const BBox& p1Box = (*_partitions)[1];

    const double fuzzyFactor = 0.3;
    if(_nDim == 2) //horizontal bisection
    {
		//vert. space between the blocks
	double ySpace = max(p0Box.yMin - p1Box.yMax, 0.0);

	double p0Bottom = p0Box.yMin - ySpace/2.0;
	double p1Top    = p1Box.yMax + ySpace/2.0;

		//lowest y-loc that propagates into p0
	p0Boundry = p0Bottom + (p0Box.yMax - p0Bottom  )*fuzzyFactor;
		//highest y-loc that propagates into p1
	p1Boundry = p1Top    - (p1Top      - p1Box.yMin)*fuzzyFactor;
    }
    else
    {
	double xSpace  = max(p1Box.xMin - p0Box.xMax, 0.0);
	double p0Right = p0Box.xMax + xSpace/2.0;
	double p1Left  = p1Box.xMin - xSpace/2.0;
	p0Boundry = p0Right - (p0Right - p0Box.xMin)*fuzzyFactor;
	p1Boundry = p1Left  + (p1Box.xMax - p1Left )*fuzzyFactor;
    }

    unsigned numBlocks = _blocks.size();
   
    for(cItr = unionOfCells.begin(); cItr != unionOfCells.end(); cItr++)
    {
        const HGFNode& node = _hgraphOfNetlist.getNodeByIdx(*cItr);

        for(eItr = node.edgesBegin(); eItr != node.edgesEnd(); eItr++)
        {
            if(_edgesVisited[(*eItr)->getIndex()]) 
		continue;
            _edgesVisited[(*eItr)->getIndex()] = true;

	    nonTermsOnNet.clear();
	    compoundTerm = UINT_MAX;
	    essential = true;

	    PartitionIds termIds;
            itHGFNodeLocal adjNd;

 	    unsigned eId = (*eItr)->getIndex();
            for(adjNd=(*eItr)->nodesBegin();adjNd!=(*eItr)->nodesEnd();adjNd++)
            {
		unsigned adjIdx = (*adjNd)->getIndex();

                if(_cellToBlockMap[adjIdx] == _blocks[0])
		    nonTermsOnNet.push_back(adjIdx);
	 	else if(numBlocks == 2 &&
			_cellToBlockMap[adjIdx] == _blocks[1])
		{
		  nonTermsOnNet.push_back(adjIdx);
		}
	 	else
		{   //propagate this terminal.
		    const Point& termLoc = _capoPl.getPinLocation(adjIdx,eId);
		    termIds.loadBitsFrom(0);
		    if(_nDim > 1)
		    {
			if(termLoc.y > p1Boundry) termIds.addToPart(0);
			if(termLoc.y < p0Boundry) termIds.addToPart(1);
		    }
		    else
		    {
			if(termLoc.x < p1Boundry) termIds.addToPart(0);
			if(termLoc.x > p0Boundry) termIds.addToPart(1);
		    }

		    if(termIds[0] && termIds[1]) 
			continue;

		    if( (termIds[0] && compoundTerm == 1)  ||
			(termIds[1] && compoundTerm == 0))
		    { essential = false; break; }

		    if(termIds[0])
			compoundTerm = 0;
		    else if(termIds[1])
			compoundTerm = 1;
		    else
		    {
			cout<<"Node "<<adjIdx<<endl;
			cout<<"Located at "<<
				_capoPl.getPinLocation(adjIdx,eId)<<endl;
		 	cout<<"Propagated to "<<termIds<<endl;
			cout<<"p0Boundry: "<<p0Boundry<<endl;
			cout<<"p1Boundry: "<<p1Boundry<<endl;
			if(_nDim > 1)
			   cout<<" Both in terms if y-loc"<<endl;
			else
			   cout<<" Both in terms if x-loc"<<endl;

		       	abkfatal(0, "terminal propagated nowhere");
	     	    }
		}
            }

	    unsigned edgeDegree = nonTermsOnNet.size();
	    if(compoundTerm != UINT_MAX) edgeDegree++;

	    if(edgeDegree <= 1)
		 continue;

	    _numOrigEdges++;
	    _origEdgeDegree += edgeDegree;

	    if(!essential) continue; //skip this edge entierly for this problem

	    _numEssentialEdges++;
	    _essentialEdgeDegree += edgeDegree;

	    HGFEdge& newEdge = newHG->addNewEdge(**eItr);
	    
	    if(compoundTerm != UINT_MAX)
		newHG->addSrcSnk(newHG->getNodeByIdx(compoundTerm), newEdge);
	
            std::vector<unsigned>::iterator c;
	    for(c = nonTermsOnNet.begin(); c != nonTermsOnNet.end(); c++)
	    {
		const HGFNode& newNd = newHG->getNewNodeByOrigIdx(*c);
		newHG->addSrcSnk(newNd, newEdge);
	    }
        }
    }

    newHG->finalize();

    _terminalToBlock = new vector<unsigned>(2, UINT_MAX);
    _padBlocks = new vector<BBox>(2);

    PartitionIds movable;
    movable.setToAll(2);
    _fixedConstr = new Partitioning(_hgraph->getNumNodes(),movable);
    (*_fixedConstr)[0].setToPart(0);
    (*_fixedConstr)[1].setToPart(1);

    (*_terminalToBlock)[0] = 0;
    (*_terminalToBlock)[1] = 1;

    (*_padBlocks)[0] += Point((*_partitions)[0].xMin, (*_partitions)[0].yMax);
    (*_padBlocks)[1] += Point((*_partitions)[1].xMax, (*_partitions)[1].yMin);

    if(honourGrpConstr)
      {
	vector<unsigned> fixToP0;
	vector<unsigned> fixToP1;
	
	const BBox& p0Box = (*_partitions)[0];
	const BBox& p1Box = (*_partitions)[1];
	BBox intersectionP0;
	BBox intersectionP1;
	for(unsigned i=0; i<unionOfCells.size(); ++i)
	  {
	    unsigned cellId = unionOfCells[i];
	    unsigned groupId = _capoPl.cellToGrpMapping[cellId];
	    if(groupId != UINT_MAX)
	      {
		const BBox& rgnBBox = 
		  _capoPl.groupRegionConstr[groupId].first;
		if(_nDim > 1) //horizontal split
		  {
		    if(rgnBBox.yMin >= p0Box.yMin)
		      fixToP0.push_back(i+2);
		    else if(rgnBBox.yMax <= p1Box.yMax)
		      fixToP1.push_back(i+2);
		    else //bbox intersects
		      {
			intersectionP0 = p0Box.intersect(rgnBBox);
			intersectionP1 = p1Box.intersect(rgnBBox);
			double area0 = intersectionP0.getArea();
			double area1 = intersectionP1.getArea();
			double area = area0+area1;
			if((area0/area) > 0.7)
			  fixToP0.push_back(i+2);
			else if((area1/area) > 0.7)
			  fixToP1.push_back(i+2);
			else
			  abkwarn(0,"Cutting a group with regionConstraints \n");			
			/*
			if(intersectionP0.getArea() > intersectionP1.getArea())
			  fixToP0.push_back(i+2);
			else
			  fixToP1.push_back(i+2);
			*/
		      }
		  }
		else //vertical split
		  {
		    if(rgnBBox.xMax <= p0Box.xMax)
		      fixToP0.push_back(i+2);
		    else if(rgnBBox.xMin >= p1Box.xMin)
		      fixToP1.push_back(i+2);
		    else //bbox intersects
		      {
			intersectionP0 = p0Box.intersect(rgnBBox);
			intersectionP1 = p1Box.intersect(rgnBBox);
			double area0 = intersectionP0.getArea();
			double area1 = intersectionP1.getArea();
			double area = area0+area1;
			if((area0/area) > 0.7)
			  fixToP0.push_back(i+2);
			else if((area1/area) > 0.7)
			  fixToP1.push_back(i+2);
			else
			  abkwarn(0,"Cutting a group with regionConstraints \n");			
			/*
			if(intersectionP0.getArea() > intersectionP1.getArea())
			  fixToP0.push_back(i+2);
			else
			  fixToP1.push_back(i+2);
			*/
		      }
		  }
	      }
	  }
	for(unsigned i=0; i<fixToP0.size(); ++i)
	  (*_fixedConstr)[fixToP0[i]].setToPart(0);
	for(unsigned i=0; i<fixToP1.size(); ++i)
	  (*_fixedConstr)[fixToP1[i]].setToPart(1);
      }
}

void   PartitioningProblemForCapo::setTolerance(double tolPct)
{
    for(unsigned p = 0; p < (*_capacities).size(); p++)
    {
	double target = (*_capacities)[p][0];
	(*_maxCapacities)[p][0] = target*(1.0 + tolPct/100.0);
	(*_minCapacities)[p][0] = target*(1.0 - tolPct/100.0);
    }
//  cout << " Capacities are now " << (*_maxCapacities)[0][0]
//       << " and " << (*_maxCapacities)[1][0] << endl;
}

double PartitioningProblemForCapo::getTolerance(unsigned wtNum) const
{
    double totalCap = (*_capacities)[0][wtNum] + (*_capacities)[1][wtNum];
    double P0Tol = ( (*_maxCapacities)[0][wtNum]- (*_minCapacities)[0][wtNum] )
                   / totalCap;
    double P1Tol = ( (*_maxCapacities)[1][wtNum]- (*_minCapacities)[1][wtNum] )
                   / totalCap;
    return max(P0Tol, P1Tol);
}


void PartitioningProblemForCapo::setPaperMethodTolerance
				(unsigned splitRow)
{
    const CapoBlock& block = *_blocks[0];

    double p0SiteArea = 0;		//S0
    double p1SiteArea = 0;		//S1
    double totalCellArea = block.getTotalCellArea(); //C
    double totalCap      = block.getCapacity();//S

/*
    cout << " The block has " << block.getNumRows() << " rows and is "
            "partitioned "    
         << ( splitRow == UINT_MAX ? "vertically " : "horizontally" ) << endl;
*/

    if (splitRow!=UINT_MAX && block.getNumRows()>1)
    {
       unsigned c;
       for(c = 0; c < splitRow; c++)
	    p1SiteArea += block.getContainedAreaInCoreRow(c);
       for(c = splitRow; c < block.getNumRows(); c++)
	    p0SiteArea += block.getContainedAreaInCoreRow(c);
    }
    else 
    {
       p0SiteArea = ceil (totalCap / 2.0);
       p1SiteArea = floor(totalCap / 2.0);
    }
/*
    cout << " Total Cell Area " << totalCellArea 
         << "    Capacity " << totalCap << endl;
    cout << " p0siteArea "  << p0SiteArea
         << " p1siteArea "  << p1SiteArea << endl;
*/

    double MaxC0, MaxC1, MinC0, MinC1;

    if(totalCellArea < totalCap) //not an overfilled block
    {
	double relWS = block.getRelativeWhitespace();
//      cout << " Relative whitespace " << relWS << endl;
   	double n = ceil(log(block.getNumRows()) / log(2));	
//      cout << " Expected depth of the cut tree " << n << endl;
	double oneOverNP1 = 1/n;
	double alpha = (pow(1-relWS, oneOverNP1)-(1-relWS)) / 
				relWS * pow(1-relWS, oneOverNP1);

	double CoverS = totalCellArea/totalCap;

	MaxC0 = min(totalCellArea, 
		(1-alpha)*p0SiteArea + alpha*CoverS*p0SiteArea);
	MaxC1 = min(totalCellArea, 
		(1-alpha)*p1SiteArea + alpha*CoverS*p1SiteArea);
	MinC0 = max(0.0, totalCellArea - MaxC1);
	MinC1 = max(0.0, totalCellArea - MaxC0);

	/* there's a problem of this tolerance calculation.
	   When white space is high, say > 60%, MinC0 could be 0, allowing
	   end-case in partitioning (0-100 partitioning).
	   We fix the bug here by controlling the utilization of the
	   child as (1+/-15%) of its parents. 
	   -Xiaojian Yang 2003/01/23 */
	
	/*
	double	tolerance = 0.15;
	if (MinC0 < p0SiteArea*(1-relWS)*(1-tolerance)) 
	  {
	    //cout << " Tolerance change (C0) from " << 100*(MaxC0-MinC0)/(MaxC0+MinC0) << "%";
	    MinC0 = p0SiteArea*(1-relWS)*(1-tolerance);
	    MaxC0 = totalCellArea - MinC0;
	    //cout << " to " << 100*(MaxC0-MinC0)/(MaxC0+MinC0) << "%" << endl;
	  }
	if (MinC1 < p1SiteArea*(1-relWS)*(1-tolerance)) 
	  {
	    //cout << " Tolerance change (C1) from " << 100*(MaxC1-MinC1)/(MaxC1+MinC1) << "%";
	    MinC1 = p1SiteArea*(1-relWS)*(1-tolerance);
	    MaxC1 = totalCellArea - MinC1;
	    //cout << " to " << 100*(MaxC1-MinC1)/(MaxC1+MinC1) << "%" << endl;
	  }
	*/



	(*_capacities)[0][0] = (MaxC0 + MinC0)/2;
       	(*_capacities)[1][0] = (MaxC1 + MinC1)/2;
    }
    else	//set tight tolerance
    {
	(*_capacities)[0][0] = (p0SiteArea/totalCap)*totalCellArea;
	(*_capacities)[1][0] = (p1SiteArea/totalCap)*totalCellArea;
        double medianCellSize =  block.getMedianCellArea();
        medianCellSize /= 2.0;
        MaxC0 = (*_capacities)[0][0] + medianCellSize;
        MinC0 = (*_capacities)[0][0] - medianCellSize;
        MaxC1 = (*_capacities)[1][0] + medianCellSize;
        MinC1 = (*_capacities)[1][0] - medianCellSize;
//      cout << " Partitioning tolerance was 0.0%" << endl;
    }

    (*_maxCapacities)[0][0] = MaxC0;
    (*_maxCapacities)[1][0] = MaxC1;
    (*_minCapacities)[0][0] = MinC0;
    (*_minCapacities)[1][0] = MinC1;
}


void  PartitioningProblemForCapo::setVBlockTolerance(double xSplit, 
						     double minTol)
{
    const CapoBlock& block = *_blocks[0];

    double p0SiteArea = 0;		//S0
    double p1SiteArea = 0;		//S1
    double totalCellArea = block.getTotalCellArea(); //C
    double totalCap      = block.getCapacity();//S
    vector<double>            siteAreas(2,0.0);

    if (block.getNumRows()>1)
    {
      block.computePartAreas(xSplit, siteAreas);
    }
    else 
    {
      siteAreas[0] = ceil (totalCap / 2.0);
      siteAreas[1] = floor(totalCap / 2.0);
    }
    p0SiteArea = siteAreas[0];
    p1SiteArea = siteAreas[1];

    //initialize the capacities and tolerance
    double p0Target = (p0SiteArea/totalCap)*totalCellArea;
    setCapacity(0, p0Target);
    setCapacity(1, totalCellArea - p0Target);
    setTolerance(minTol);
/*
    cout << " Total Cell Area " << totalCellArea 
         << "    Capacity " << totalCap << endl;
    cout << " p0siteArea "  << p0SiteArea
         << " p1siteArea "  << p1SiteArea << endl;
*/

    double MaxC0, MaxC1, MinC0, MinC1;

    if(totalCellArea < totalCap) //not an overfilled block
    {
      //do nothing
      double stretchFactor=1;
        //double stretchFactor=
        //( block.getAvgRowSpacing() > 1.5 * block.getAvgCellHeight() ? 2 : 1);
	//double blockAR = block.getWidth()/block.getHeight();
	//stretchFactor = blockAR * stretchFactor;

	double relWS = block.getRelativeWhitespace();
   	double n = ceil(log(stretchFactor*block.getNumRows()) / log(2));
	double oneOverNP1 = 1/n;
	double alpha = (pow(1-relWS, oneOverNP1)-(1-relWS)) / 
				relWS * pow(1-relWS, oneOverNP1);

	double CoverS = totalCellArea/totalCap;

	MaxC0 = min(totalCellArea, 
		(1-alpha)*p0SiteArea + alpha*CoverS*p0SiteArea);
	MaxC1 = min(totalCellArea, 
		(1-alpha)*p1SiteArea + alpha*CoverS*p1SiteArea);
	MinC0 = max(0.0, totalCellArea - MaxC1);
	MinC1 = max(0.0, totalCellArea - MaxC0);

	MaxC0 = max(MaxC0, (*_maxCapacities)[0][0]);
	MaxC1 = max(MaxC1, (*_maxCapacities)[1][0]);
	MinC0 = min(MinC0, (*_minCapacities)[0][0]);
	MinC1 = min(MinC1, (*_minCapacities)[1][0]);
	  
	(*_capacities)[0][0] = (MaxC0 + MinC0)/2;
       	(*_capacities)[1][0] = (MaxC1 + MinC1)/2;
    }
    else	//set tight tolerance
    {
	(*_capacities)[0][0] = (p0SiteArea/totalCap)*totalCellArea;
	(*_capacities)[1][0] = (p1SiteArea/totalCap)*totalCellArea;
        double medianCellSize =  block.getMedianCellArea();
        medianCellSize /= 2.0;
        MaxC0 = (*_capacities)[0][0] + medianCellSize;
        MinC0 = (*_capacities)[0][0] - medianCellSize;
        MaxC1 = (*_capacities)[1][0] + medianCellSize;
        MinC1 = (*_capacities)[1][0] - medianCellSize;
//      cout << " Partitioning tolerance was 0.0%" << endl;
    }
    (*_maxCapacities)[0][0] = MaxC0;
    (*_maxCapacities)[1][0] = MaxC1;
    (*_minCapacities)[0][0] = MinC0;
    (*_minCapacities)[1][0] = MinC1;
}


void PartitioningProblemForCapo::buildHGraphNE(bool honourGrpConstr)
{
//const PlacementWOrient& placement = _capoPl.getPlacement();
  double epsilon;
    _numOrigEdges   = _numEssentialEdges   = 0;
    _origEdgeDegree = _essentialEdgeDegree = 0;

    unsigned numNEEdges=0;
    unsigned numNonNEEdges=0;

    if(_edgesVisited.size() < _hgraphOfNetlist.getNumEdges())
    {
	unsigned numToAdd = _hgraphOfNetlist.getNumEdges() -
		            _edgesVisited.size();
        _edgesVisited.insert(_edgesVisited.end(), 
				numToAdd, false);
    }

    std::fill(_edgesVisited.begin(), _edgesVisited.end(), false); 

    vector<unsigned> unionOfCells = _blocks[0]->getCellIds();

    for(unsigned b = 1; b < _blocks.size(); b++)
	unionOfCells.insert(unionOfCells.end(), 
		_blocks[b]->cellIdsBegin(), _blocks[b]->cellIdsEnd());

    SubHGraphForCapo* newHG = new SubHGraphForCapo(2, unionOfCells.size(),
					_hgraphOfNetlist.getNumEdges()*4);

    _hgraph = static_cast<HGraphFixed*>(newHG);
    _subHG  = static_cast<SubHGraph*>(newHG);

    newHG->_param.makeAllSrcSnk = true;

    std::vector<unsigned>::const_iterator cItr;
    itHGFEdgeLocal eItr;

    newHG->_param.makeAllSrcSnk = true;
    unsigned newId = 2;
    for(cItr= unionOfCells.begin();cItr != unionOfCells.end();cItr++,newId++)
	newHG->addNode(_hgraphOfNetlist.getNodeByIdx(*cItr), newId);

    vector<unsigned> nonTermsOnNet; //ids of all nonTerminals on this edge
    unsigned         compoundTerm;  // the id of the (1) terminal on this edge
    bool	     essential;     //is the net being examined essential?

    double p0Boundry, p1Boundry;
    const BBox& p0Box = (*_partitions)[0];
    const BBox& p1Box = (*_partitions)[1];
    if(_nDim == 2) //horizontal bisection
    {
		//vert. space between the blocks
	double ySpace = max(p0Box.yMin - p1Box.yMax, 0.0);

	double p0Bottom = p0Box.yMin - ySpace/2.0;
	double p1Top    = p1Box.yMax + ySpace/2.0;

		//lowest y-loc that propagates into p0
	p0Boundry = p0Bottom + (p0Box.yMax - p0Bottom  )*0.1;
		//highest y-loc that propagates into p1
	p1Boundry = p1Top    - (p1Top      - p1Box.yMin)*0.1;
    }
    else
    {
	double xSpace  = max(p1Box.xMin - p0Box.xMax, 0.0);
	double p0Right = p0Box.xMax + xSpace/2.0;
	double p1Left  = p1Box.xMin - xSpace/2.0;
	p0Boundry = p0Right - (p0Right - p0Box.xMin)*0.1;
	p1Boundry = p1Left  + (p1Box.xMax - p1Left )*0.1;
    }

    unsigned numBlocks = _blocks.size();
   
    for(cItr = unionOfCells.begin(); cItr != unionOfCells.end(); cItr++)
    {
        const HGFNode& node = _hgraphOfNetlist.getNodeByIdx(*cItr);

        for(eItr = node.edgesBegin(); eItr != node.edgesEnd(); eItr++)
        {
            if(_edgesVisited[(*eItr)->getIndex()]) 
		continue;
            _edgesVisited[(*eItr)->getIndex()] = true;

	    nonTermsOnNet.clear();
	    compoundTerm = UINT_MAX;
	    essential = true;

	    PartitionIds termIds;
            itHGFNodeLocal adjNd;

 	    unsigned eId = (*eItr)->getIndex();
            for(adjNd=(*eItr)->nodesBegin();adjNd!=(*eItr)->nodesEnd();adjNd++)
            {
		unsigned adjIdx = (*adjNd)->getIndex();

                if(_cellToBlockMap[adjIdx] == _blocks[0])
		    nonTermsOnNet.push_back(adjIdx);
	 	else if(numBlocks == 2 &&
			_cellToBlockMap[adjIdx] == _blocks[1])
		{
		  nonTermsOnNet.push_back(adjIdx);
		}
	 	else
		{   //propagate this terminal.
		    const Point& termLoc = _capoPl.getPinLocation(adjIdx,eId);
		    termIds.loadBitsFrom(0);
		    if(_nDim > 1)
		    {
			if(termLoc.y > p1Boundry) termIds.addToPart(0);
			if(termLoc.y < p0Boundry) termIds.addToPart(1);
		    }
		    else
		    {
			if(termLoc.x < p1Boundry) termIds.addToPart(0);
			if(termLoc.x > p0Boundry) termIds.addToPart(1);
		    }

		    if(termIds[0] && termIds[1]) 
			continue;

		    if( (termIds[0] && compoundTerm == 1)  ||
			(termIds[1] && compoundTerm == 0))
		    { essential = false; break; }

		    if(termIds[0])
			compoundTerm = 0;
		    else if(termIds[1])
			compoundTerm = 1;
		    else
		    {
			cout<<"Node "<<adjIdx<<endl;
			cout<<"Located at "<<
				_capoPl.getPinLocation(adjIdx,eId)<<endl;
		 	cout<<"Propagated to "<<termIds<<endl;
			cout<<"p0Boundry: "<<p0Boundry<<endl;
			cout<<"p1Boundry: "<<p1Boundry<<endl;
			if(_nDim > 1)
			   cout<<" Both in terms if y-loc"<<endl;
			else
			   cout<<" Both in terms if x-loc"<<endl;

		       	abkfatal(0, "terminal propagated nowhere");
	     	    }
		}
            }

	    unsigned edgeDegree = nonTermsOnNet.size();
	    if(compoundTerm != UINT_MAX) edgeDegree++;

	    if(edgeDegree <= 1)
		 continue;

	    _numOrigEdges++;
	    _origEdgeDegree += edgeDegree;

	    if(!essential) continue; //skip this edge entierly for this problem

	    _numEssentialEdges++;
	    _essentialEdgeDegree += edgeDegree;
	    numNonNEEdges++;


	    //add the Net Effect nets now (before the actual edge)
            std::vector<unsigned>::iterator c;
	    std::vector<unsigned>::iterator d;
	    if(nonTermsOnNet.size() >= 2 && nonTermsOnNet.size() < 4)
	    {
	      BBox blkBBox = _blocks[0]->getBBox();
	      if(numBlocks == 2)
		blkBBox = blkBBox.expandToInclude(_blocks[1]->getBBox());
	      if(_nDim ==  2)
		epsilon = blkBBox.getHeight();
	      else 
		epsilon = blkBBox.getWidth();
	      epsilon /= 10;
	      unsigned nonTermCellId=0;
	      for(c = nonTermsOnNet.begin(); c != nonTermsOnNet.end(); c++, nonTermCellId++)
		{
		  for(d = c+1; d != nonTermsOnNet.end(); d++)
		    {
		      if(*c != *d)
			{
			  //cout<<_hgraphOfNetlist.getNodeByIdx(*c).getName()<<" "<<_hgraphOfNetlist.getNodeByIdx(*d).getName()<<endl;
			  //cout<<(*c)<<"  "<<(*d)<<endl;

			  Point cLoc = _capoPl.getPinLocation(*c,eId);
			  Point dLoc = _capoPl.getPinLocation(*d,eId);
			  //Point cLoc = placement[*c];
			  //Point dLoc = placement[*d];
			  //which is the preferred partition direction
			  double nodeDistance;
			  bool cInPart; 
			  if(_nDim ==  2) //horizontal partitioning
			    {
			      nodeDistance = dLoc.y - cLoc.y;
			      if(nodeDistance < 0)
				{
				  nodeDistance = -nodeDistance;
				  cInPart = 0;
				}
			      else
				{
				  cInPart = 1;
				}
			    }
			  else //vertical partitioning
			    {
			      nodeDistance = dLoc.x - cLoc.x;
			      if(nodeDistance < 0)
				{
				  nodeDistance = -nodeDistance;
				  cInPart = 1;
				}
			      else
				{
				  cInPart = 0;
				}
			    }
			  bool needConstraint = false;
			  if(nodeDistance > epsilon)
			    needConstraint = true;

			  //if too close don't add a constraint
			  if(needConstraint) 
			    {
			      // 2 edges to be added
			      HGFEdge& neEdge1 = newHG->addNewEdge(**eItr);
			      HGFEdge& neEdge2 = newHG->addNewEdge(**eItr);
			      HGFEdge& neEdge3 = newHG->addNewEdge(**eItr);
			      
			      newHG->addSrcSnk(newHG->getNodeByIdx(0),neEdge1);
			      newHG->addSrcSnk(newHG->getNodeByIdx(1),neEdge3);
			      if(cInPart == 0) //constraint 0->c->d->1
				{
				  const HGFNode& newNdC = 
				    newHG->getNewNodeByOrigIdx(*c);
				  newHG->addSrcSnk(newNdC, neEdge1);
				  const HGFNode& newNdD = 
				    newHG->getNewNodeByOrigIdx(*d);
				  newHG->addSrcSnk(newNdD, neEdge3);
				  newHG->addSrcSnk(newNdC, neEdge2);
				  newHG->addSrcSnk(newNdD, neEdge2);
				}
			      else //constraint 0->d->c->1
				{
				  const HGFNode& newNdC = 
				    newHG->getNewNodeByOrigIdx(*c);
				  newHG->addSrcSnk(newNdC, neEdge3);
				  const HGFNode& newNdD = 
				    newHG->getNewNodeByOrigIdx(*d);
				  newHG->addSrcSnk(newNdD, neEdge1);
				  newHG->addSrcSnk(newNdC, neEdge2);
				  newHG->addSrcSnk(newNdD, neEdge2);
				}
			      _numOrigEdges += 3;
			      _origEdgeDegree += 6;
			      
			      _numEssentialEdges += 3;
			      _essentialEdgeDegree += 6;
			      
			      numNEEdges += 3;
			    }
			}
		    }
		}
	    }

	    HGFEdge& newEdge = newHG->addNewEdge(**eItr);
	    
	    if(compoundTerm != UINT_MAX)
		newHG->addSrcSnk(newHG->getNodeByIdx(compoundTerm), newEdge);
	
	    for(c = nonTermsOnNet.begin(); c != nonTermsOnNet.end(); c++)
	    {
		const HGFNode& newNd = newHG->getNewNodeByOrigIdx(*c);
		newHG->addSrcSnk(newNd, newEdge);
	    }
	}
    }

    cout<<"Total Orig Edges "<<_numEssentialEdges-numNEEdges<<" neEdges "<<numNEEdges/3.0<<endl;

    newHG->finalize();
    
    _terminalToBlock = new vector<unsigned>(2, UINT_MAX);
    _padBlocks = new vector<BBox>(2);

    PartitionIds movable;
    movable.setToAll(2);
    _fixedConstr = new Partitioning(_hgraph->getNumNodes(),movable);
    (*_fixedConstr)[0].setToPart(0);
    (*_fixedConstr)[1].setToPart(1);

    (*_terminalToBlock)[0] = 0;
    (*_terminalToBlock)[1] = 1;

    (*_padBlocks)[0] += Point((*_partitions)[0].xMin, (*_partitions)[0].yMax);
    (*_padBlocks)[1] += Point((*_partitions)[1].xMax, (*_partitions)[1].yMin);

    if(honourGrpConstr)
      {
	vector<unsigned> fixToP0;
	vector<unsigned> fixToP1;

	const BBox& p0Box = (*_partitions)[0];
	const BBox& p1Box = (*_partitions)[1];
	BBox intersectionP0;
	BBox intersectionP1;
	for(unsigned i=0; i<unionOfCells.size(); ++i)
	  {
	    unsigned cellId = unionOfCells[i];
	    unsigned groupId = _capoPl.cellToGrpMapping[cellId];
	    if(groupId != UINT_MAX)
	      {
		const BBox& rgnBBox = 
		  _capoPl.groupRegionConstr[groupId].first;
		if(_nDim > 1) //horizontal split
		  {
		    if(rgnBBox.yMin >= p0Box.yMin)
		      fixToP0.push_back(i+2);
		    else if(rgnBBox.yMax <= p1Box.yMax)
		      fixToP1.push_back(i+2);
		    else //bbox intersects
		      {
			intersectionP0 = p0Box.intersect(rgnBBox);
			intersectionP1 = p1Box.intersect(rgnBBox);
			double area0 = intersectionP0.getArea();
			double area1 = intersectionP1.getArea();
			double area = area0+area1;
			if((area0/area) > 0.7)
			  fixToP0.push_back(i+2);
			else if((area1/area) > 0.7)
			  fixToP1.push_back(i+2);
			else
			  abkwarn(0,"Cutting a group with regionConstraints \n");			
			/*
			if(intersectionP0.getArea() > intersectionP1.getArea())
			  fixToP0.push_back(i+2);
			else
			  fixToP1.push_back(i+2);
			*/
		      }
		  }
		else //vertical split
		  {
		    if(rgnBBox.xMax <= p0Box.xMax)
		      fixToP0.push_back(i+2);
		    else if(rgnBBox.xMin >= p1Box.xMin)
		      fixToP1.push_back(i+2);
		    else //bbox intersects
		      {
			intersectionP0 = p0Box.intersect(rgnBBox);
			intersectionP1 = p1Box.intersect(rgnBBox);
			double area0 = intersectionP0.getArea();
			double area1 = intersectionP1.getArea();
			double area = area0+area1;
			if((area0/area) > 0.7)
			  fixToP0.push_back(i+2);
			else if((area1/area) > 0.7)
			  fixToP1.push_back(i+2);
			else
			  abkwarn(0,"Cutting a group with regionConstraints \n");			
			/*
			if(intersectionP0.getArea() > intersectionP1.getArea())
			  fixToP0.push_back(i+2);
			else
			  fixToP1.push_back(i+2);
			*/
		      }
		  }
	      }
	  }
	for(unsigned i=0; i<fixToP0.size(); ++i)
	  (*_fixedConstr)[fixToP0[i]].setToPart(0);
	for(unsigned i=0; i<fixToP1.size(); ++i)
	  (*_fixedConstr)[fixToP1[i]].setToPart(1);
      }
}

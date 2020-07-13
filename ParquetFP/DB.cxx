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





#include <map>
#include "FPcommon.h"
#include "DB.h"
using namespace parquetfp;

//ctor
DB::DB(char* baseName)
{
  cout<<"Reading Nodes"<<endl;
  _nodes = new Nodes(baseName);

  cout<<"Reading Nets"<<endl;
  _nets =  new Nets(baseName);
  _nets->updateNodeInfo(*_nodes);
  _nodes->updatePinsInfo(*_nets);

  cout<<"Done creating DB "<<endl;

  _nodesBestCopy = new Nodes();
  //_nodesBestCopy = new Nodes(baseName);
  //_nodesBestCopy->updatePinsInfo(*_nets);

  _initArea = 0;
  successAR = 0;
}

DB::DB()
{
  _nodes = new Nodes();
  _nets =  new Nets();
  _nodesBestCopy = new Nodes();

  _initArea = 0;
  successAR = 0;
}

DB::DB(DB * db, vector<int>& subBlocksIndices, Point& dbLoc, double reqdAR)
{
  _nodes = new Nodes();
  _nets =  new Nets();
  _nodesBestCopy = new Nodes();
  _initArea = 0;
  successAR = 0;

  Nodes* origNodes = db->getNodes();
  Nets* origNets = db->getNets();

  map<unsigned, unsigned> mapping;
  unsigned numOrigNodes = origNodes->getNumNodes();
  vector<bool> seenNodes(numOrigNodes, false);

  double nodesArea = 0;
  for(unsigned i=0; i<subBlocksIndices.size(); ++i)
    {
      Node& origNode = origNodes->getNode(subBlocksIndices[i]);
      nodesArea += origNode.getArea();
      Node tempNode(origNode.getName(), origNode.getArea() , 
		    origNode.getminAR(), origNode.getmaxAR(), i, false);
      tempNode.addSubBlockIndex(subBlocksIndices[i]);
      tempNode.putX(origNode.getX());
      tempNode.putY(origNode.getY());
      _nodes->putNewNode(tempNode);
      seenNodes[subBlocksIndices[i]] = true;
      mapping[subBlocksIndices[i]] = i;
    }

  double reqdHeight = sqrt(nodesArea/reqdAR);
  double reqdWidth = reqdHeight*reqdAR;
  double termXLoc, termYLoc;
  double minXLoc = dbLoc.x;
  double maxXLoc = minXLoc + reqdWidth;
  double minYLoc = dbLoc.y;
  double maxYLoc = minYLoc + reqdHeight;

  unsigned numOrigNets = origNets->getNumNets();
  vector<bool> seenNets(numOrigNets, false);

  unsigned newNetCtr=0;
  unsigned newTermCtr = 0;

  itNodePin origNodePin;
  Net tempEdge;
  
  for(unsigned i=0; i<subBlocksIndices.size(); ++i)
    {
      Node& origNode = origNodes->getNode(subBlocksIndices[i]);
      for(origNodePin = origNode.pinsBegin(); 
	  origNodePin != origNode.pinsEnd(); ++origNodePin)
	{
	  Net& origNet = origNets->getNet(origNodePin->netIndex);
	  if(!seenNets[origNet.getIndex()])
	    {
	      seenNets[origNet.getIndex()] = true;
	      tempEdge.putName(origNet.getName());
	      tempEdge.putIndex(newNetCtr);
	      for(itPin netPin = origNet.pinsBegin(); 
		  netPin != origNet.pinsEnd(); ++netPin)
		{
		  double poffsetX, poffsetY;
		  unsigned origNodeIdx = netPin->getNodeIndex();
		  if(!netPin->getType()) //not a terminal
		    {
		      if(seenNodes[origNodeIdx])
			{
			  unsigned newNodeIdx = mapping[origNodeIdx];
			  poffsetX = netPin->getXOffset();
			  poffsetY = netPin->getYOffset();

			  Node& newNode = _nodes->getNode(newNodeIdx);
			  pin tempPin(newNode.getName(), false, poffsetX, 
				      poffsetY, newNetCtr);
			  tempPin.putNodeIndex(newNodeIdx);
			  tempEdge.addNode(tempPin);
			}
		      else //need terminal propagation
			{
			  Node& origTerm = 
			    origNodes->getNode(netPin->getNodeIndex());
			  
			  Node tempTerm(origTerm.getName(), 0, 1, 1, 
					newTermCtr, true);
			  tempTerm.addSubBlockIndex(origTerm.getIndex());

			  //terminal propagation
			  if(origTerm.getX() < minXLoc)
			    termXLoc = 0;
			  else if(origTerm.getX() > maxXLoc)
			    termXLoc = reqdWidth;
			  else
			    termXLoc = origTerm.getX() - minXLoc;

			  if(origTerm.getY() < minYLoc)
			    termYLoc = 0;
			  else if(origTerm.getY() > maxYLoc)
			    termYLoc = reqdHeight;
			  else
			    termYLoc = origTerm.getY() - minYLoc;

			  tempTerm.putX(termXLoc);
			  tempTerm.putY(termYLoc);

			  _nodes->putNewTerm(tempTerm);
			  
			  pin tempPin(origTerm.getName(), true, 0, 0, newNetCtr);
			  tempPin.putNodeIndex(newTermCtr);
			  tempEdge.addNode(tempPin);
			  newTermCtr++;			  
			}
		    }
		  else //actual terminal
		    {
		      Node& origTerm = 
			origNodes->getTerminal(netPin->getNodeIndex());

		      Node tempTerm(origTerm.getName(), 0, 1, 1, newTermCtr,
				    true);
		      tempTerm.addSubBlockIndex(origTerm.getIndex());


		      //terminal propagation
		      if(origTerm.getX() < minXLoc)
			termXLoc = 0;
		      else if(origTerm.getX() > maxXLoc)
			termXLoc = reqdWidth;
		      else
			termXLoc = origTerm.getX() - minXLoc;
		      
		      if(origTerm.getY() < minYLoc)
			termYLoc = 0;
		      else if(origTerm.getY() > maxYLoc)
			termYLoc = reqdHeight;
		      else
			termYLoc = origTerm.getY() - minYLoc;
		      
		      tempTerm.putX(termXLoc);
		      tempTerm.putY(termYLoc);
		      
		      _nodes->putNewTerm(tempTerm);

		      pin tempPin(origTerm.getName(), true, 0, 0, newNetCtr);
		      tempPin.putNodeIndex(newTermCtr);
		      tempEdge.addNode(tempPin);
		      newTermCtr++;
		    }
		}
	      _nets->putNewNet(tempEdge);
	      ++newNetCtr;
	      tempEdge.clean();
	    }
	}
    }
  _nodes->updatePinsInfo(*_nets);

  *_nodesBestCopy = *_nodes;
}

DB::~DB()
{
  if(_nodes) delete _nodes;
  if(_nets) delete _nets;
  if(_nodesBestCopy) delete _nodesBestCopy;
}

DB& DB::operator=(DB& db2)
{
  if(this != &db2)
    {
      if(_nodes) delete _nodes;
      if(_nets) delete _nets;
      if(_nodesBestCopy) delete _nodesBestCopy;
      
      _nodes = new Nodes();
      _nets = new Nets();
      _nodesBestCopy = new Nodes();
      
      *(_nodes) = *(db2.getNodes());
      *(_nets) = *(db2.getNets());
      *(_nodesBestCopy) = *(db2._nodesBestCopy);
  
      _area = db2._area;
      _initArea = db2._initArea;
    }
  return *this;
}

void DB::clean(void)
{
  if(_nodes) _nodes->clean();
  if(_nets) _nets->clean();
  if(_nodesBestCopy) _nodesBestCopy->clean();
}

Nodes* DB::getNodes(void)
{ return _nodes; }

Nets* DB::getNets(void)
{ return _nets; }

unsigned DB::getNumNodes(void)
{
  return _nodes->getNumNodes();
}

vector<double> DB::getNodeWidths(void)
{
  return _nodes->getNodeWidths();
}

vector<double> DB::getNodeHeights(void)
{
  return _nodes->getNodeHeights();
}

vector<double> DB::getXLocs(void)
{
  return _nodes->getXLocs();
}

vector<double> DB::getYLocs(void)
{
  return _nodes->getYLocs();
}

double DB::getNodesArea(void)
{
  if(_initArea)
    return _area;
  else
    {
      _area=_nodes->getNodesArea();
      _initArea = 1;
      return _area;
    }
}

double DB::getAvgHeight(void)
{
  itNode node;
  double avgHeight = 0;
  for(node = _nodes->nodesBegin(); node != _nodes->nodesEnd(); ++node)
    avgHeight += node->getHeight();
  avgHeight /= getNumNodes();
  return(avgHeight);
}

void DB::updatePlacement(vector<double>& xloc, vector<double>& yloc)
{
  for(unsigned i=0; i<xloc.size(); ++i)
    {
      _nodes->getNode(i).putX(xloc[i]);
      _nodes->getNode(i).putY(yloc[i]);
    }
}

void DB::initPlacement(Point& loc)
{
  itNode node;
  for(node = _nodes->nodesBegin(); node != _nodes->nodesEnd(); ++node)
    {
      node->putX(loc.x);
      node->putY(loc.y);
    }
}

void DB::updateSlacks(vector<double>& xSlack, vector<double>& ySlack)
{
  for(unsigned i=0; i<xSlack.size(); ++i)
    {
      _nodes->getNode(i).putslackX(xSlack[i]);
      _nodes->getNode(i).putslackY(ySlack[i]);
    }  
}

double DB::evalHPWL(void)
{
  itNet net;
  itPin pin;
  unsigned nodeIndex;
  BBox netBBox;
  Point tempPoint;
  double HPWL=0;
  double width;
  double height;
  double absOffsetX;
  double absOffsetY;
  double xloc;
  double yloc;
  
  for(net = _nets->netsBegin(); net != _nets->netsEnd(); ++net)
    {
      for(pin = net->pinsBegin(); pin != net->pinsEnd(); ++pin)
        {
	  nodeIndex = pin->getNodeIndex();
	  if(!pin->getType())        //if not terminal
	   {
	    Node& node = _nodes->getNode(nodeIndex);
	    width = node.getWidth();
	    height = node.getHeight();
	    xloc = node.getX();
	    yloc = node.getY();
	   }			
	  else
	    {
	     Node& node = _nodes->getTerminal(nodeIndex);
	     width = node.getWidth();
	     height = node.getHeight();
	     xloc = node.getX();
	     yloc = node.getY();
	    }
	  absOffsetX = width/2 + (pin->getXOffset()*width);
	  absOffsetY = height/2 + (pin->getYOffset()*height);
	  
          tempPoint.x = xloc + absOffsetX;
	  tempPoint.y = yloc + absOffsetY;
	  netBBox.put(tempPoint);
	}
      if(netBBox.isValid())
      {
	//cout<<net->getName()<<" "<<netBBox.getHPWL()<<" "<<HPWL<<endl;
        HPWL += netBBox.getHPWL();
      }
      netBBox.clear();	
    }
  return HPWL;    
}

double DB::evalHPWL(double xSize, double ySize)
{
  itNet net;
  itPin pin;
  unsigned nodeIndex;
  BBox netBBox;
  Point tempPoint;
  double HPWL=0;
  double width;
  double height;
  double absOffsetX;
  double absOffsetY;
  double xloc;
  double yloc;
  
  for(net = _nets->netsBegin(); net != _nets->netsEnd(); ++net)
    {
      for(pin = net->pinsBegin(); pin != net->pinsEnd(); ++pin)
        {
	  nodeIndex = pin->getNodeIndex();
	  if(!pin->getType())        //if not terminal
	   {
	    Node& node = _nodes->getNode(nodeIndex);
	    width = node.getWidth();
	    height = node.getHeight();
	    xloc = node.getX();
	    yloc = node.getY();
	   }			
	  else
	    {
	     Node& node = _nodes->getTerminal(nodeIndex);
	     width = node.getWidth();
	     height = node.getHeight();
	     xloc = node.getX();
	     yloc = node.getY();
	     if(xloc > xSize)
	       xloc = xSize;
	     if(yloc > ySize)
	       yloc = ySize;
	    }
	  absOffsetX = width/2 + (pin->getXOffset()*width);
	  absOffsetY = height/2 + (pin->getYOffset()*height);
	  
          tempPoint.x = xloc + absOffsetX;
	  tempPoint.y = yloc + absOffsetY;
	  netBBox.put(tempPoint);
	}
      if(netBBox.isValid())
      {
	//cout<<net->getName()<<" "<<netBBox.getHPWL()<<" "<<HPWL<<endl;
        HPWL += netBBox.getHPWL();
      }
      netBBox.clear();	
    }
  return HPWL;    
}

double DB::evalArea(void)
{
  BBox area;
  itNode node;
  Point P;
  for(node = getNodes()->nodesBegin(); node != getNodes()->nodesEnd(); ++node)
    {
      P.x = node->getX();
      P.y = node->getY();
      area.put(P);
      P.x = node->getX()+node->getWidth();
      P.y = node->getY()+node->getHeight();
      area.put(P);
    }
  return(area.getXSize()*area.getYSize());
}

double DB::getXSize(void)
{
  BBox xSize;
  itNode node;
  Point P;
  for(node = getNodes()->nodesBegin(); node != getNodes()->nodesEnd(); ++node)
    {
      P.x = node->getX();
      P.y = 0;
      xSize.put(P);
      P.x = node->getX()+node->getWidth();
      P.y = 0;
      xSize.put(P);
    }
  return(xSize.getXSize());
}

double DB::getYSize(void)
{
  BBox ySize;
  itNode node;
  Point P;
  for(node = getNodes()->nodesBegin(); node != getNodes()->nodesEnd(); ++node)
    {
      P.y = node->getY();
      P.x = 0;
      ySize.put(P);
      P.y = node->getY()+node->getHeight();
      P.x = 0;
      ySize.put(P);
    }
  return(ySize.getYSize());
}

void DB::plot(char* fileName, double area, double whitespace, 
	      double aspectRatio, double time, double HPWL, bool plotSlacks,
	      bool plotNets, bool plotNames)
{
  double x=0;
  double y=0;
  double w=0;
  double h=0;
  double nodesArea = getNodesArea();
  double starDelta = sqrt(nodesArea) / 200;
  itNode it;

  cout<<"OutPut Plot file is "<<fileName<<endl;
  ofstream gpOut(fileName);


  gpOut<<"#Use this file as a script for gnuplot"<<endl;
  gpOut<<"#(See http://www.cs.dartmouth.edu/gnuplot_info.html for details)"<<endl;
  gpOut << "set nokey"<<endl;
									  
  gpOut << "set title ' "<<fileName<<" area= "<<area<<" WS= "<<whitespace<<"%"<<" AR= "<<aspectRatio<<" time= "<<time<<"s"<<" HPWL= "<<HPWL<<endl<<endl;

  gpOut<<"#   Uncomment these two lines starting with \"set\""<<endl ;
  gpOut<<"#   to save an EPS file for inclusion into a latex document"<<endl;
  gpOut << "# set terminal postscript eps color 10"<<endl;
  gpOut << "# set output \"out.eps\""<<endl<<endl<<endl;

  gpOut<<"#   Uncomment these two lines starting with \"set\""<<endl ;
  gpOut<<"#   to save a PS file for printing"<<endl;
  gpOut<<"# set terminal postscript portrait color 8"<<endl;
  gpOut << "# set output \"out.ps\""<<endl<<endl<<endl;

  if(plotNames)
  {
   for(it=_nodes->nodesBegin();it!=_nodes->nodesEnd();++it)
    {
     gpOut<<"set label \""<<it->getName()<<"\" at "<<it->getX()+it->getWidth()/4<<" , "<<it->getY()+it->getHeight()/4<<endl;
    }
   
   for(it=_nodes->terminalsBegin();it!=_nodes->terminalsEnd();++it)
    {
     gpOut<<"set label \""<<it->getName()<<"\" at "<<it->getX()+it->getWidth()/4<<" , "<<it->getY()+it->getHeight()/4<<endl;
    }
  }
	    
  
 if(plotSlacks)
   {
     for(it=_nodes->nodesBegin();it!=_nodes->nodesEnd();++it)
      {
	double xSlack = it->getslackX();
	double ySlack = it->getslackY();
	if(xSlack < 1e-5)
	  xSlack = 0;
	if(ySlack < 1e-5)
	  ySlack = 0;
 
        gpOut.precision(4);
        gpOut<<"set label \"x "<<xSlack<<"\" at "<<it->getX()+it->getWidth()/6<<" , "<<it->getY()+it->getHeight()/2<<endl;
        gpOut<<"set label \"y "<<ySlack<<"\" at "<<it->getX()+it->getWidth()/6<<" , "<<it->getY()+it->getHeight()*3/4<<endl;
      }
   }

 gpOut.precision(6);
 gpOut << "plot '-' w l, '-' w l 3";
 gpOut<<endl;

 for(it=_nodes->nodesBegin();it!=_nodes->nodesEnd();++it)
   {
     x=it->getX();
     y=it->getY();
     w=it->getWidth();
     h=it->getHeight();

     gpOut<<x<<" "<<y<<endl;
     gpOut<<x+w<<" "<<y<<endl;
     gpOut<<x+w<<" "<<y+h<<endl;
     gpOut<<x<<" "<<y+h<<endl;
     gpOut<<x<<" "<<y<<endl<<endl;
   }
 gpOut << "EOF"<<endl;

 if(plotNets)
 {
  double width;
  double height;
  double absOffsetX;
  double absOffsetY;
 
  itNet net;
  itPin netPin;
  for(net = _nets->netsBegin(); net != _nets->netsEnd(); ++net)
    {
      Point starPoint;
      starPoint.x = 0;
      starPoint.y = 0;
      unsigned netDegree = 0;
      for(netPin = net->pinsBegin(); netPin != net->pinsEnd(); netPin++)
        {
          Node node;
	  if(!netPin->getType())  //if not terminal
	    node = _nodes->getNode(netPin->getNodeIndex());
	  else
	    node = _nodes->getTerminal(netPin->getNodeIndex());

  	  width = node.getWidth();
          height = node.getHeight();
          absOffsetX = width/2 + (netPin->getXOffset()*width);
          absOffsetY = height/2 + (netPin->getYOffset()*height);
	  starPoint.x += node.getX() + absOffsetX;
	  starPoint.y += node.getY() + absOffsetY;
	  ++netDegree;
        }
      if(netDegree != 0)
        {
          starPoint.x /= netDegree;
  	  starPoint.y /= netDegree;
          for(netPin = net->pinsBegin(); netPin != net->pinsEnd(); netPin++)
            {
              Node node;
	      if(!netPin->getType())
	        node = _nodes->getNode(netPin->getNodeIndex());
	      else
	        node = _nodes->getTerminal(netPin->getNodeIndex());
              width = node.getWidth();
              height = node.getHeight();
              absOffsetX = width/2 + (netPin->getXOffset()*width);
              absOffsetY = height/2 + (netPin->getYOffset()*height);
              gpOut<<starPoint.x<<"  "<<starPoint.y<<endl;
	      gpOut<<node.getX()+absOffsetX<<"  "<<node.getY()+absOffsetY<<endl;
	      gpOut<<starPoint.x<<"  "<<starPoint.y<<endl<<endl;

	      gpOut<<starPoint.x-starDelta<<"  "<<starPoint.y<<endl;
	      gpOut<<starPoint.x<<"  "<<starPoint.y+starDelta<<endl;
	      gpOut<<starPoint.x+starDelta<<"  "<<starPoint.y<<endl;
	      gpOut<<starPoint.x<<"  "<<starPoint.y-starDelta<<endl;
	      gpOut<<starPoint.x-starDelta<<"  "<<starPoint.y<<endl<<endl;

	    }
        }
    }
 }
 gpOut << "EOF"<<endl<<endl; 
 gpOut << "pause -1 'Press any key' "<<endl;
 gpOut.close();  
}


void DB::saveCapo(char* baseFileName, double reqdAR)
{
  cout<<"Saving in Capo Format "<<baseFileName<<endl;
  _nodes->saveCapoNodes(baseFileName);
  _nodes->saveCapoPl(baseFileName);
  saveCapoNets(baseFileName);
  _nodes->saveCapoScl(baseFileName, reqdAR);

  //save the aux file now
  char fileName[100];
  strcpy(fileName, baseFileName);
  strcat(fileName, ".aux");
  ofstream aux(fileName);
  aux<<"RowBasedPlacement : "<<baseFileName<<".nodes "<<baseFileName<<".nets "<<
       baseFileName<<".scl "<<baseFileName<<".pl "<<endl;
  aux.close();     
}

void DB::save(char* baseFileName)
{
  cout<<"Saving in Floorplan Format "<<baseFileName<<endl;
  _nodes->saveNodes(baseFileName);
  _nodes->savePl(baseFileName);
  saveNets(baseFileName);
}

void DB::saveCapoNets(char* baseFileName)
{
  Nets* nets;
  nets = getNets();
  double absOffsetX;
  double absOffsetY;
  double width;
  double height;
  double temp;
  int nodeIndex;
  
  char fileName[100];
  strcpy(fileName, baseFileName);
  strcat(fileName, ".nets");
  ofstream file(fileName);

  file<<"UCLA nets   1.0"<<endl<<endl<<endl;
  file<<"NumPins : "<<nets->getNumPins()<<endl<<endl;
  
  itNet net;
  itPin pin;
  for(net = nets->netsBegin(); net != nets->netsEnd(); ++net)
   {
    file<<"NetDegree : "<<net->_pins.size()<<"\t"<<net->getName()<<endl;
    for(pin = net->pinsBegin(); pin != net->pinsEnd(); ++pin)
     {
	  nodeIndex = pin->getNodeIndex();
	  if(!pin->getType())        //if not terminal
	   {
	    Node& node = _nodes->getNode(nodeIndex);
	    width = node.getWidth();
	    height = node.getHeight();
	   }			
	  else
	    {
	     Node& node = _nodes->getTerminal(nodeIndex);
	     width = node.getWidth();
	     height = node.getHeight();
	    }
	  if(int(pin->getOrient())%2 == 1)
	    {
	      temp = width;
	      width = height;
	      height = temp;
	    }
	  absOffsetX = (pin->getOrigXOffset()*width);
	  absOffsetY = (pin->getOrigYOffset()*height);
	  
          file<<"\t"<<pin->getName()<<" B : \t"<<absOffsetX<<"\t "<<absOffsetY<<endl;
     }
   }
 file.close();
}

void DB::saveNets(char* baseFileName)
{
  Nets* nets;
  nets = getNets();

  char fileName[100];
  strcpy(fileName, baseFileName);
  strcat(fileName, ".nets");
  ofstream file(fileName);

  file<<"UCLA nets   1.0"<<endl<<endl<<endl;
  file<<"NumNets : "<<nets->getNumNets()<<endl;
  file<<"NumPins : "<<nets->getNumPins()<<endl<<endl;
  
  itNet net;
  itPin pin;
  for(net = nets->netsBegin(); net != nets->netsEnd(); ++net)
   {
    file<<"NetDegree : "<<net->_pins.size()<<endl;
    for(pin = net->pinsBegin(); pin != net->pinsEnd(); ++pin)
     {
       file<<pin->getName()<<" B : \t%"<<pin->getOrigXOffset()*100<<
	 "\t %"<<pin->getOrigYOffset()*100<<endl;
     }
   }
  file.close();
}

void DB::shiftDesign(Point& offset)
{
 itNode node;
 for(node = _nodes->nodesBegin(); node != _nodes->nodesEnd(); ++node)
  {
    node->putX(node->getX() + offset.x);
    node->putY(node->getY() + offset.y);
  }
}

void DB::saveInBestCopy(void)
{
  successAR = 1;
  itNode node;
  itNode nodeBest;
  nodeBest = _nodesBestCopy->nodesBegin();
  for(node = _nodes->nodesBegin(); node != _nodes->nodesEnd(); node++)
    {
      nodeBest->putX(node->getX());
      nodeBest->putY(node->getY());
      nodeBest->putWidth(node->getWidth());
      nodeBest->putHeight(node->getHeight());
      nodeBest->putOrient(node->getOrient());
      nodeBest++;
    }
}

void DB::saveBestCopyPl(char* fileName)
{
  _nodesBestCopy->savePl(fileName);
}

void DB::markTallNodesAsMacros(double maxHeight)
{
  itNode node;
  for(node = _nodes->nodesBegin(); node != _nodes->nodesEnd(); ++node)
    {
      if(node->getHeight() > maxHeight)
	node->updateMacroInfo(true);
    }
}

void DB::reduceCoreCellsArea(double layoutArea, double maxWS)
{
  double currNodesArea = getNodesArea();
  double currWS = (layoutArea - currNodesArea)/currNodesArea;

  if(currWS > maxWS)
    return;

  double macroArea = 0;
  itNode node;
  for(node = _nodes->nodesBegin(); node != _nodes->nodesEnd(); ++node)
    {
      if(node->isMacro())
	macroArea += node->getHeight()*node->getWidth();
    }

  double newCtoOldCRatio = ((layoutArea - (1+maxWS)*macroArea)*(1+currWS))/
                           ((layoutArea - (1+currWS)*macroArea)*(1+maxWS));

  for(node = _nodes->nodesBegin(); node != _nodes->nodesEnd(); ++node)
    {
      if(!node->isMacro())
	{
	  double nodeWidth = node->getWidth();
	  double newNodeWidth = nodeWidth*newCtoOldCRatio;
	  node->putWidth(newNodeWidth);
	}
    }
  
  _initArea = false;
  double newNodesArea = getNodesArea();

  currWS = (layoutArea - newNodesArea)/newNodesArea;
}

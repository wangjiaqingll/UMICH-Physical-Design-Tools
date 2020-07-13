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
#include "Nets.h"
#include "Nodes.h"
using namespace parquetfp;

//ctor

Nodes::Nodes(char* baseName)
{
  char fname[100];
  strcpy(fname,baseName);
  strcat(fname,".blocks");
  parseNodes(fname);
  strcpy(fname,baseName);
  strcat(fname,".pl");
  parsePl(fname);
}

void Nodes::parseNodes(char* fnameBl)
{
    char block_name[100];
    char block_type[100];
    char tempWord1[100];
    
    vector<Point> vertices;
    int numVertices;
    bool success; 
    double width, height;
    
    double area,minAr,maxAr;
    int numSoftBl=0;
    int numHardBl=0;
    int numTerm=0;
    
    int indexBlock=0;
    int indexTerm=0;
    
    ifstream input(fnameBl);
    if(!input)
     {
      cout<<"ERROR: .blocks file could not be opened successfully"<<endl;
      exit(0);
     }
    skiptoeol(input);
    while(!input.eof())
      {
        input>>tempWord1;
	if(!(strcmp(tempWord1,"NumSoftRectangularBlocks")))
	  break;
      }
    input>>tempWord1;
    input>>numSoftBl;
    while(!input.eof())
      {
        input>>tempWord1;
	if(!(strcmp(tempWord1,"NumHardRectilinearBlocks")))
	  break;
      }
    input>>tempWord1;
    input>>numHardBl;
    
    while(!input.eof())
      {
	input>>tempWord1;
	if(!(strcmp(tempWord1,"NumTerminals")))
	  break;
      }
    input>>tempWord1;
    input>>numTerm;
    
    while(!input.eof())
      {
	block_type[0] = '\0';
        eatblank(input);
	if(input.eof())
	  break;
        if(input.peek()=='#')
          eathash(input);
	else
	{
	  eatblank(input);
          if(input.peek() == '\n' || input.peek() == '\r')
	    {
 	      input.get();
	      continue;
	    }

	  input >> block_name;
	  input >>block_type ;

	  if(!strcmp(block_type,"softrectangular")) 
	   {
	    input >> area;
	    input >> minAr;
	    input >> maxAr;
	    Node temp(block_name,area,minAr,maxAr,indexBlock,false);
	    temp.addSubBlockIndex(indexBlock);
	    _nodes.push_back(temp);
	    
	    ++indexBlock;
	    //cout<<block_name<<" "<<area<<endl;
	   }
	  else if(!strcmp(block_type,"hardrectilinear"))
	  {
	    input >> numVertices;
	    Point tempPoint;
	    success = 1;
	    if(numVertices > 4)
	      cout<<"ERROR in parsing .blocks file. rectilinear blocks can be only rectangles for now\n";
	    for(int i=0; i<numVertices; ++i)
	     {
	       success &= needCaseChar(input, '(');  input.get();
	       input >> tempPoint.x;
	       success &= needCaseChar(input, ',');  input.get();
	       input >> tempPoint.y;
	       success &= needCaseChar(input, ')');  input.get();
	       vertices.push_back(tempPoint);
	     }
	    if(!success)
	      cout<<"ERROR in parsing .blocks file while processing hardrectilinear blocks"<<endl;
	    
	    width = vertices[2].x - vertices[0].x;
	    height = vertices[2].y - vertices[0].y;
	    area = width*height;
	    minAr = width/height;
	    maxAr = minAr;
	    Node temp(block_name,area,minAr,maxAr,indexBlock,false);
	    temp.addSubBlockIndex(indexBlock);
	    _nodes.push_back(temp);
	    ++indexBlock;
	    vertices.clear();
	    //cout<<block_name<<" "<<area<<endl;
	  }
	  else if(!strcmp(block_type,"terminal"))
	   {
	    Node temp(block_name,0,1,1,indexTerm,true);
	    temp.addSubBlockIndex(indexTerm);
	    _terminals.push_back(temp);
	    ++indexTerm;
	    //cout<<indexTerm<<"  "<<block_name<<endl;
	   }
	  /*
	  else
	   cout<<"ERROR in parsing .blocks file"<<endl;
	  */
	 }  
      }
    input.close();

    if(numSoftBl+numHardBl+numTerm != indexBlock+indexTerm)
      cout<<"ERROR in parsing .blocks file. No: of blocks do not tally "<<(indexBlock+indexTerm)<<" vs "<<(numSoftBl+numHardBl+numTerm)<<endl;
     
}

void Nodes::parsePl(char* fnamePl)
{
   char block_name[100];
   char tempWord[100];
   char tempWord1[100];
   double xloc;
   double yloc;
   double width;
   double height;
   bool success;
   ORIENT newOrient;
   itNode node;

   map<const char*, int, ltstr> index;
   map<const char*, bool, ltstr> type;
   for(node = nodesBegin(); node != nodesEnd(); node++)
     {
       index[node->getName()] = node->getIndex();
       type[node->getName()] = 0;
     }
   for(node = terminalsBegin(); node != terminalsEnd(); node++)
     {
       index[node->getName()] = node->getIndex();
       type[node->getName()] = 1;
     }
   
   ifstream input(fnamePl);
   if(!input)
     {
       cout<<"ERROR: .pl file could not be opened successfully"<<endl;
       return;
     }
     
   skiptoeol(input);
   while(!input.eof())
     {
       eatblank(input);
       if(input.eof())
         break;
       if(input.peek()=='#')
         eathash(input);
       else
       {
         eatblank(input);
	 if(input.peek() == '\n' || input.peek() == '\r')
	   {
	     input.get();
	     continue;
	   }
	 input>>block_name;
	 input>>xloc;
	 input>>yloc;
	 int thisIndex = index[block_name];
	 int thisType = type[block_name];

	 updatePlacement(thisIndex, thisType, xloc, yloc);
	 
	 eatblank(input);
	 success = 1;
	 if(input.peek() == 'D')   //block with width and height
	  {
	   input>>tempWord;  
	   success &= needCaseChar(input,'('); input.get();
	   input>>width;
	   success &= needCaseChar(input,','); input.get();
	   input>>height;
	   success &= needCaseChar(input,')'); input.get();
	   
	   if(!success || strcmp(tempWord,"DIMS"))
	     cout<<"ERROR in parsing .pl file while reading in DIMS\n";
	   updateHW(thisIndex, thisType, width, height);
	   getNode(thisIndex).needSyncOrient = false;
	  }
	  
	 success = 1;
	 eatblank(input);
	 if(input.peek() == ':') //initial orientation given
	  {
	    input.get();
	    input>>tempWord1;    //the orientation in words;
	    newOrient = toOrient(tempWord1);
	    updateOrient(thisIndex, thisType, newOrient);
	  }
	 //cout<<block_name<<" "<<xloc<<" "<<yloc<<endl;  
       }
     }
   input.close();
}

  
void Nodes::updatePlacement(int index, bool type, double xloc, double yloc)
{
  if(!type)
    {
      Node& node = getNode(index);
      node.putX(xloc);
      node.putY(yloc);
    }
  else
    {
      Node& node = getTerm(index);
      node.putX(xloc);
      node.putY(yloc);
    }
  // cout<<"ERROR: could not find node to update placement for. Name: "<<
  //block_name<<endl;
  return;
}

void Nodes::updateOrient(int index, bool type, ORIENT newOrient)
{
  if(!type)
    {
      Node& node = getNode(index);
      node.putOrient(newOrient);
    }
  else
    {
      Node& node = getTerm(index);
      node.putOrient(newOrient);
    }
  //cout<<"ERROR: could not find node to update Orient for. Name: "<<block_name
  //    <<endl;
}

void Nodes::updateHW(int index, bool type, double width, double height)
{
  if(!type)
    {
      Node& node = getNode(index);
      node.putWidth(width);
      node.putHeight(height);
    }
  else
    {
      Node& node = getTerm(index);
      node.putWidth(width);
      node.putHeight(height);
    }
  //cout<<"ERROR: could not find node to update Height/Width for. Name: "<<
  //  block_name<<endl;
}

unsigned Nodes::getNumNodes(void)
{ return _nodes.size(); }

double Nodes::getNodeWidth(unsigned index)
{
  return _nodes[index].getWidth();
}

double Nodes::getNodeHeight(unsigned index)
{
  return _nodes[index].getHeight();
}

void Nodes::putNodeWidth(unsigned index, double width)
{
  _nodes[index].putWidth(width);
}

void Nodes::putNodeHeight(unsigned index, double height)
{
  _nodes[index].putHeight(height);
}

vector<double> Nodes::getNodeWidths(void)
{
  vector<double> widths;
  for(itNode iNodes=nodesBegin(); iNodes!=nodesEnd(); iNodes++)
    widths.push_back(iNodes->getWidth());
  return widths;
}

vector<double> Nodes::getNodeHeights(void)
{
  vector<double> heights;
  for(itNode iNodes=nodesBegin(); iNodes!=nodesEnd(); iNodes++)
    heights.push_back(iNodes->getHeight());
  return heights;
}


vector<double> Nodes::getXLocs(void)
{
  vector<double> xloc;
  for(itNode iNodes=nodesBegin(); iNodes!=nodesEnd(); iNodes++)
    xloc.push_back(iNodes->getX());
  return xloc;
}

vector<double> Nodes::getYLocs(void)
{
  vector<double> yloc;
  for(itNode iNodes=nodesBegin(); iNodes!=nodesEnd(); iNodes++)
    yloc.push_back(iNodes->getY());
  return yloc;
}

double Nodes::getNodesArea()
{
  itNode iNode;
  double area = 0;
  for(iNode = nodesBegin(); iNode != nodesEnd(); ++iNode)
    {
      area += iNode->getHeight()*iNode->getWidth();
    }
  return area;
}

void Nodes::changeOrient(unsigned index, ORIENT newOrient, Nets& nets)
{
  Node& node = _nodes[index];
  node.changeOrient(newOrient, nets);
}

/*following function assumes that pin info about node index is correct.*/
void Nodes::updatePinsInfo(Nets& nets)
{
  itNet net;
  itPin pin;
  itNode node;
  NodePin tempNodePin;
  unsigned pinOffset;
  
  for(net = nets.netsBegin(); net != nets.netsEnd(); net++)
    {
      pinOffset = 0;
      for(pin = net->pinsBegin(); pin != net->pinsEnd(); pin++)
        {
	  tempNodePin.netIndex = net->getIndex();
	  tempNodePin.pinOffset = pinOffset;
	  unsigned nodeIndex = pin->getNodeIndex();
	  if(!pin->getType())     //if not pad
	  {
           Node& node = getNode(nodeIndex);
	   node.addPin(tempNodePin);
	  }
	  else
	  {
	   Node& node = getTerminal(nodeIndex);
	   node.addPin(tempNodePin);
	  }
	  pinOffset++;
	}
    }

  //now sync the orientations of the pins with that of the nodes
  for(node = nodesBegin(); node != nodesEnd(); ++node)
    { 
      if(node->getOrient() != N)   //synchronize orientation only if not N
        node->syncOrient(nets);
    }
}


void Nodes::savePl(char* baseFileName)
{
  char fileName[100];
  strcpy(fileName, baseFileName);
  strcat(fileName, ".pl");
  ofstream outPl(fileName);
  if(!outPl)
    {
      cout<<"ERROR in saving .pl file"<<endl;
      return;
    }
  cout<<"Saving "<<fileName<<" as output .pl file"<<endl;
  outPl<<"UMICH blocks 1.0"<<endl<<endl<<endl<<endl;
  
  itNode node;
  
  for(node = nodesBegin(); node != nodesEnd(); node++)
    {
      outPl<<node->getName()<<"\t"<<node->getX()<<"\t"<<node->getY();
      outPl<<"\tDIMS = ("<<node->getWidth()<<", "<<node->getHeight()<<")";
      outPl<<"\t: "<<toChar(node->getOrient())<<endl;
    }
  outPl<<endl;

  for(node = terminalsBegin(); node != terminalsEnd(); node++)
    {
      outPl<<node->getName()<<"\t"<<node->getX()<<"\t"<<node->getY()
	   <<"\t: "<<toChar(node->getOrient())<<endl;
    }
  outPl<<endl;
  outPl.close();
}

void Nodes::saveCapoNodes(char* baseFileName)
{
  
  char fileName[100];
  strcpy(fileName, baseFileName);
  strcat(fileName, ".nodes");
  ofstream file(fileName);
  file<<"UCLA nodes  1.0"<<endl<<endl<<endl;
  
  file<<"NumNodes : \t"<<_nodes.size()+_terminals.size()<<endl;
  file<<"NumTerminals : \t"<<_terminals.size()<<endl;

  itNode node;

  double width, height, temp;
  for(node = nodesBegin(); node != nodesEnd(); ++node)
    {
      width = node->getWidth();
      height = node->getHeight();
      if(int(node->getOrient())%2 == 1)
	{
	  temp = width;
	  width = height;
	  height = temp;
	}
      file<<"\t"<<node->getName()<<"\t"<<width<<"\t"<<height<<endl;      
    }

  for(node = terminalsBegin(); node != terminalsEnd(); ++node)
    {
      width = node->getWidth();
      height = node->getHeight();
      if(int(node->getOrient())%2 == 1)
	{
	  temp = width;
	  width = height;
	  height = temp;
	}
      file<<"\t"<<node->getName()<<"\t"<<width<<"\t"<<height<<"\tterminal "
	  <<endl;      
    }
  file.close();
}

void Nodes::saveNodes(char* baseFileName)
{
  char fileName[100];
  strcpy(fileName, baseFileName);
  strcat(fileName, ".blocks");
  ofstream file(fileName);
  file<<"UCSC blocks  1.0"<<endl<<endl<<endl;
  
  file<<"NumSoftRectangularBlocks : \t"<<_nodes.size()<<endl;
  file<<"NumHardRectilinearBlocks : \t0"<<endl;
  file<<"NumTerminals : \t"<<_terminals.size()<<endl<<endl;

  itNode node;
     
  for(node = nodesBegin(); node != nodesEnd(); ++node)
    {
      file<<node->getName()<<" softrectangular "<<node->getArea()<<"\t"
          <<node->getminAR()<<"\t"<<node->getmaxAR()<<endl;      
    }
  file<<endl;
  for(node = terminalsBegin(); node != terminalsEnd(); ++node)
    file<<node->getName()<<" terminal"<<endl;

  file.close();
}

void Nodes::saveCapoPl(char* baseFileName)
{
  char fileName[100];
  strcpy(fileName, baseFileName);
  strcat(fileName, ".pl");
  ofstream file(fileName);

  file<<"UCLA pl   1.0"<<endl<<endl<<endl;
  
  itNode node;
     
  for(node = nodesBegin(); node != nodesEnd(); ++node)
    {
      file<<"\t"<<node->getName()<<"\t"<<node->getX()<<"\t"
	  <<node->getY()<<" : \t"<<toChar(node->getOrient())<<endl;      
    }

    
  for(node = terminalsBegin(); node != terminalsEnd(); ++node)
    {
      file<<"\t"<<node->getName()<<"\t"<<node->getX()<<"\t"
          <<node->getY()<<" : \t"<<toChar(node->getOrient())<<endl;
    }
    
  file.close();
}

void Nodes::saveCapoScl(char* baseFileName, double reqdAR)
{
  //default required aspect ratio of 1
  double AR;
  if(reqdAR == -9999)
    AR = 1;
  else
    AR = reqdAR;
    
  double area = 1.3*getNodesArea();  //add 50% WS
  double height = sqrt(area/AR);
  double width = AR*height;
  int siteWidth = 1;
  int numSites = int(width/siteWidth) + 1;
  int rowHeight = int(getMinHeight());
  int numRows = int(height/rowHeight) + 1;
  
  int rowCoord = 1;
  
  char fileName[100];
  strcpy(fileName, baseFileName);
  strcat(fileName, ".scl");
  ofstream file(fileName);

  file<<"UCLA scl   1.0"<<endl<<endl<<endl;
  file<<"Numrows : "<<numRows<<endl;
  
  for(int i=0; i<numRows; ++i)
   {
     file<<"CoreRow Horizontal"<<endl;
     file<<" Coordinate\t: "<<rowCoord<<endl;
     file<<" Height\t: "<<rowHeight<<endl;
     file<<" SiteWidth\t: "<<siteWidth<<endl;
     file<<" Sitespacing\t: "<<siteWidth<<endl;
     file<<" Siteorient\t: N"<<endl;
     file<<" Sitesymmetry\t: Y"<<endl;
     file<<" SubrowOrigin\t: 0 Numsites\t: "<<numSites<<endl;
     file<<"End"<<endl;

     rowCoord += rowHeight;
   }

  file.close();
}

double Nodes::getMinHeight()
{
  itNode node;
  double minHeight = 1e100;
  for(node = nodesBegin(); node != nodesEnd(); ++node)
   {
     if(minHeight > node->getHeight())
       minHeight = node->getHeight();
   }
  return minHeight;
}

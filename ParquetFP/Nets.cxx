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

Nets::Nets(char* baseName)
{
  char basefile[100];
  strcpy(basefile,baseName);
  char fnameNets[100];
  strcpy(fnameNets,basefile);
  strcat(fnameNets,".nets");
  parseNets(fnameNets);
}

void Nets::parseNets(char* fnameNets)
{
  char block_name[100];
  ifstream nets(fnameNets);
  char tempWord1[1000];
  char netName[100];
  double poffsetX;
  double poffsetY;
  int netIndex = 0;
  int numNets = 0;
  int numPins;
  unsigned netDegree=0;
  unsigned netCtr=0;

  Net tempEdge;

  if(!nets)
   {
    cout<<"ERROR: .nets file could not be opened successfully"<<endl;
    return;
   }
  skiptoeol(nets);
  while(!nets.eof())
    {
      nets>>tempWord1;
      if(!(strcmp(tempWord1,"NumNets")))
        break;
    }
  nets>>tempWord1;
  nets>>numNets;
  while(!nets.eof())
    {
      nets>>tempWord1;
      if(!(strcmp(tempWord1,"NumPins")))
        break;
    }
  nets>>tempWord1;
  nets>>numPins;
  
  while(!nets.eof())
    {
      nets>>tempWord1;
      if(!(strcmp(tempWord1,"NetDegree")))
        break;
    }
  nets>>tempWord1;
  nets>>netDegree;

  eatblank(nets);
  if(nets.peek() == '\n' || nets.peek() == '\r')
    {
      sprintf(netName,"N%d",netIndex);
    }
  else  //netName present
    {
      nets>>netName;
    }
  skiptoeol(nets);
  tempEdge.putName(netName);
  
  while(!nets.eof())
    { 
      eatblank(nets);
      if(nets.eof())
        break;
      if(nets.peek()=='#')
	eathash(nets);
      else
	{
	  eatblank(nets);
	  if(nets.peek() == '\n' || nets.peek() == '\r')
	    {
	      nets.get();
	      continue;
	    }
	    
	  nets>>block_name;
	  nets>>tempWord1;
	  
	  if(!strcmp(tempWord1,"B") || !strcmp(tempWord1,"O") || 
	     !strcmp(tempWord1,"I"))
	    {
	      eatblank(nets);
		
	      if(nets.peek()=='\n' || nets.peek()=='\r' || nets.eof())
		{
		  nets.get();
		  //put terminal info in vector
		  pin tempPin(block_name,true,0,0,netIndex);
	          tempEdge.addNode(tempPin);
//cout<<block_name<<"\t"<<tempWord1<<"\t"<<endl;
		  ++netCtr;
		  continue;
		}
	      else
		{
		  nets>>tempWord1;
		  if(!strcmp(tempWord1,":"))
		    {
		      eatblank(nets);
		    }
		  else
		    cout << "error in parsing"<<endl;
		  
		  if(nets.peek()!='%')
		    {cout<<"expecting %"<<endl;}
		  else
		    {
		      nets.get();
		      nets>>poffsetX;
		      eatblank(nets);
		      nets.get();
		      nets>>poffsetY;
		      nets.get();
		      //convert from %
		      poffsetX /= 100;
		      poffsetY /= 100;
		      //put block info here
		      pin tempPin(block_name,false,poffsetX,poffsetY,netIndex);
		      tempEdge.addNode(tempPin);
//cout<<block_name<<"\t"<<tempWord1<<"\t"<<poffsetX<<"\t"<<poffsetY<<endl;
		      ++netCtr;
		    }
		}
	    }
	  
	  else if(!strcmp(block_name,"NetDegree"))//new net starts
	    {
	      tempEdge.putIndex(netIndex);
	      _nets.push_back(tempEdge);

	      if(netCtr != netDegree)
	        { 
		  cout<<"ERROR in parsing .nets file. For net "<<tempEdge.getName()<<" netDegree do not match with no: of pins. "<<netCtr<<" vs "<<netDegree<<"\n"<<endl;
                }
	      netCtr = 0;
              tempEdge.clean();
	      netIndex++;
              
	      nets>>netDegree;
              eatblank(nets);
              if(nets.peek() == '\n' || nets.peek() == '\r')
                {
                  sprintf(netName,"N%d",netIndex);
                }
              else  //netName present
                { 
                  nets>>netName;
                }
              skiptoeol(nets);
              tempEdge.putName(netName);
	    }
	}
    }
  nets.close();
  
  //put the last net info inside
  tempEdge.putIndex(netIndex);
  _nets.push_back(tempEdge);
  ++netIndex;

  if(netIndex != numNets)
    cout<<"Error in parsing .nets file. Number of nets do not tally "<<netIndex<<" vs "<<numNets<<endl;

/*
  int actNumPins = getNumPins();
  if(numPins != actNumPins)
    cout<<"Error in parsing .nets file. Number of pins do not tally "<<actNumPins<<" vs "<<numPins<<endl;
*/ 
}

void Nets::updateNodeInfo(Nodes& nodes)
{
  itNet net;
  itPin pin;
  itNode node;

  map<const char*, int, ltstr> index;
  map<const char*, bool, ltstr> type;

  for(node = nodes.nodesBegin(); node != nodes.nodesEnd(); node++)
    {
      index[node->getName()] = node->getIndex();
      type[node->getName()] = 0;
    }
  for(node = nodes.terminalsBegin(); node != nodes.terminalsEnd(); node++)
    {
      index[node->getName()] = node->getIndex();
      type[node->getName()] = 1;
    }

  for(net = _nets.begin(); net != _nets.end(); net++)
    {
      for(pin = net->pinsBegin(); pin != net->pinsEnd(); pin++)
        {
	  int thisIndex = index[pin->getName()];
	  bool thisType = type[pin->getName()];
	  pin->putNodeIndex(thisIndex);
	  pin->putType(thisType);
	    
	  //  cout<<"ERROR: Node "<<pin->getName()<<" not found in updateNodeInfo"<<endl;
	}
    }
}

int Nets::getNumPins(void)
{
  itNet net;
  int numPins = 0;
  for(net = netsBegin(); net != netsEnd(); ++net)
   {
     numPins += net->_pins.size();
   }
  return numPins;
}

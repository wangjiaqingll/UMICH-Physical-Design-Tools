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





#include "FPcommon.h"
#include "Nets.h"
#include "Node.h"
using namespace parquetfp;

Node:: Node(char* block_name,double block_area,double minAr,double maxAr,
	    int index,bool type)
   : _area(block_area),_minAr(minAr),_maxAr(maxAr),_orient(N),_slackX(0),
     _slackY(0),_index(index),_type(type),_isMacro(false),needSyncOrient(true)
   {
     strcpy(_name,block_name);
     _placement.x=0;
     _placement.y=0;
     _width = sqrt(_area*_minAr);   //by default orientation is N
     _height = _width/_minAr;
     _origWidth = _width;
     _origHeight = _height;
   }

//default ctor 
Node::Node()
  : _area(0),_minAr(0),_maxAr(0),_orient(N),_slackX(0),_slackY(0),_index(0),
    _type(true),_isMacro(false)
{
  strcpy(_name,"");
  _placement.x=0;
  _placement.y=0;
  _height = 0;
  _width = 0;
}

void Node::changeOrient(ORIENT newOrient, Nets& nets)
{
  if(_orient == newOrient)
    return;
  
  if(_orient%2 != newOrient%2)
    {
      double tempHeight = _height;
      _height = _width;
      _width = tempHeight;
    }

  //update the pinoffsets of the netlist now
  itNodePin itP;
  for(itP = _pins.begin(); itP != _pins.end(); ++itP)
    {
      pin& netPin = nets.getNet(itP->netIndex).getPin(itP->pinOffset);
      netPin.changeOrient(newOrient);
    }
  _orient = newOrient;

}

void Node::syncOrient(Nets& nets)
{
  //update the heights and widths only if not updated earlier in parsePl
  if(needSyncOrient)
    {
     if(int(_orient)%2 == 1)  //needs swap of height and width
      {
        double tempHeight = _height;
        _height = _width;
        _width = tempHeight;
      }
    }
    
  //update the pinoffsets of the netlist now
  itNodePin itP;
  for(itP = _pins.begin(); itP != _pins.end(); ++itP)
    {
      pin& netPin = nets.getNet(itP->netIndex).getPin(itP->pinOffset);
      netPin.changeOrient(_orient);
    }
}

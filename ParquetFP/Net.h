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





#include <vector>
#include <algorithm>
#include <math.h>
#include <stdlib.h>

namespace parquetfp
{

class pin;
typedef std::vector<pin>::iterator itPin;


//this class holds the pin offsets and pinindex on a particular node
class pin
{
 private:
  Point _origOffset;    //original offset wrt center. In relative terms
  Point _offset;  //offsets during iteration. to account for orientation changes
  ORIENT _orient;       //keeps track of orientation of the node
  char _name[100];
  bool _type;
  int _nodeIndex;       //index of the node in which the pin is located
  int _netIndex;        //index of net to which pin is attached
  
 public:
  
  pin(char* name, bool type, double xoffset, double yoffset, int netIndex)
    :_type(type)
    {
      strcpy(_name,name);
      _origOffset.x=xoffset;
      _origOffset.y=yoffset;
      _offset.x=xoffset;
      _offset.y=yoffset;
      _orient=N;
      _netIndex=netIndex;
    }
  pin()
    {
    }

  bool getType()
    { return _type;}
  char* getName(void)
    {return _name;}
  int getNodeIndex()
    {return _nodeIndex;}
  int getNetIndex()
    { return _netIndex; }
  void putNodeIndex(int nodeIndex)
    { _nodeIndex = nodeIndex; }
  void putNetIndex(int netIndex)
    { _netIndex = netIndex; }
  void putType(bool type)
    { _type = type; }
  
  double getXOffset(void)
    { return _offset.x; }
  double getYOffset(void)
    { return _offset.y; }
  double getOrigXOffset(void)
    { return _origOffset.x; }
  double getOrigYOffset(void)
    { return _origOffset.y; }

  ORIENT getOrient(void)
    { return _orient; }
  
  void changeOrient(ORIENT newOrient);
	    
};



class Net
{

 public:
  vector<pin> _pins;
 
 private: 
  int _index;
  char _name[100];
  
 public:
  Net()
    {
    }

  void putName(char* name)
    { strcpy(_name, name); }

  void addNode(pin node)
    {
      _pins.push_back(node);
    }
  void clean(void)
    {
      _pins.resize(0);
    }
  itPin pinsBegin()
    {
      return _pins.begin();
    }
  itPin pinsEnd()
    {
      return _pins.end();
    }

  pin& getPin(unsigned pinOffset)
    { return _pins[pinOffset]; }

  int getIndex()
    {
      return _index;
    }
  void putIndex(int netIndex)
    {
      _index = netIndex;
    }
  char* getName(void)
    { return _name; }

  unsigned getDegree(void)
    { return _pins.size(); }
  
};

}

//using namespace parquetfp;

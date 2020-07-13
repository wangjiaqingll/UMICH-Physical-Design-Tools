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

using namespace std;

namespace parquetfp
{
class Nets;

struct NodePin
{
  unsigned netIndex;
  unsigned pinOffset;
};

typedef std::vector<NodePin>::iterator itNodePin;

class Node
{
 private:
  char _name[100];
  double _area;
  double _minAr;
  double _maxAr;
  double _origWidth;
  double _origHeight;
  double _width;
  double _height;
  ORIENT _orient;          //N,E,S,W,FN.FE,FS or FW. N on initialization
  double _slackX;
  double _slackY;
  int _index;
  bool _type;              //0 normal node,1 pad
  bool _isMacro;           //is Node Macro(used only during clustering)
  Point _placement;
  vector<NodePin> _pins;       //all the pins of this node

  vector<int> _subBlockIndices; //indices of subBlocks

 public:
  //ctors
   Node(char* block_name,double block_area,double minAr,double maxAr,
	   int index,bool type);
   Node();

   bool getType()
     { return _type;}
   int getIndex(void)
     {return _index;} 
   ORIENT getOrient(void)
     {return _orient;}
   double getHeight(void)
     {return _height;}
   double getWidth(void)
     {return _width;}
   double getOrigHeight(void)
     {return _origHeight;}
   double getOrigWidth(void)
     {return _origWidth;}
   char* getName(void)
     {return _name;}
   double getX(void)
     {return _placement.x;}
   double getY(void)
     {return _placement.y;}
   double getslackX(void)
     {return _slackX;}
   double getslackY(void)
     {return _slackY;}
   double getminAR(void)
     {return _minAr;}
   double getmaxAR(void)
     {return _maxAr;}
   double getArea(void)
     {return _area;}
   bool isMacro(void)
     {return _isMacro;}
   void updateMacroInfo(bool isMacro)
     {_isMacro = isMacro;}

   itNodePin pinsBegin()
     {return _pins.begin(); }
   itNodePin pinsEnd()
     {return _pins.end(); }
   unsigned getDegree()
     { return _pins.size(); }
   
   vector<int>::iterator subBlocksBegin()
     {return _subBlockIndices.begin();}
   vector<int>::iterator subBlocksEnd()
     {return _subBlockIndices.end();}
   unsigned numSubBlocks()
     {return _subBlockIndices.size();}
   vector<int>& getSubBlocks()
     { return _subBlockIndices; }


   void putArea(double area)
     {_area=area;}
   void putWidth(double w)
     {_width=w;}
   void putHeight(double h)
     {_height=h;}
   void putX(double x)
     {_placement.x=x;}
   void putY(double y)
     {_placement.y=y;}
   void putslackX(double x)
     {_slackX=x;}
   void putslackY(double y)
     {_slackY=y;}
   void addPin(NodePin& pinTemp)
     {_pins.push_back(pinTemp);}
   void putOrient(ORIENT newOrient)    //to be used only during initialization
     { _orient = newOrient; }          //else use changeOrient

   void addSubBlockIndex(int index)
     { _subBlockIndices.push_back(index); }

   void changeOrient(ORIENT newOrient, Nets& nets);
   void syncOrient(Nets& nets);
   bool needSyncOrient;  //during parsing if DIMS found then no need to change
                         //H & W. If orient found and no DIMS then need change
                         //in H & W

};

}
//using namespace parquetfp;



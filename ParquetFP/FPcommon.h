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







#ifndef FPCOMMON
#define FPCOMMON

#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <algorithm>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "ABKCommon/abktimer.h"

using namespace std;

namespace parquetfp
{
enum ORIENT {N, E, S, W, FN, FE, FS, FW};

ostream& operator<<(ostream&, const ORIENT&);

struct ltstr
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) < 0;
  }
};

struct Point
{
double x;
double y;
};

struct IntPoint
{
int x;
int y;
};

class BBox
{
 private:
  double _minX;
  double _maxX;
  double _minY;
  double _maxY;

  bool _valid;
  
 public:
  //ctor
  BBox();

  void put(Point& point);
  void clear(void);  
  double getHPWL(void);
  double getXSize(void);
  double getYSize(void);
  bool isValid(void);
};

//global parsing functions
void eatblank(ifstream& i);

void skiptoeol(ifstream& i);

void eathash(ifstream& i);

bool needCaseChar(ifstream& i, char character);

//functions to manage the orientations
ORIENT toOrient(char* orient);
char* toChar(ORIENT orient);
}

//using namespace parquetfp;


	
#endif




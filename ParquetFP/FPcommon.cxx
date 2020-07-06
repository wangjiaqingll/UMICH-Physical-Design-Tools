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
using namespace parquetfp;

ostream& operator<<(ostream& out, const ORIENT& orient)
{
  if(orient == N)
    out<<"N";
  else if(orient == E)
    out<<"E";
  else if(orient == S)
    out<<"S";
  else if(orient == W)
    out<<"W";
  else if(orient == FN)
    out<<"FN";
  else if(orient == FE)
    out<<"FE";
  else if(orient == FS)
    out<<"FS";
  else if(orient == FW)
    out<<"FW";
  else
    cout<<"ERROR in outputting orientations"<<endl;
  return out;
}

BBox::BBox()
{
  _minX=(1e306);
  _maxX=(-1e306);
  _minY=(1e306);
  _maxY=(-1e306);
  _valid=0;
}

void BBox::clear(void)
{
  _minX=(1e306);
  _maxX=(-1e306);
  _minY=(1e306);
  _maxY=(-1e306);
  _valid=0;
}
void BBox::put(Point& point)
{
  if(point.x < _minX)
    _minX = point.x;
  if(point.x > _maxX)
    _maxX = point.x;
  if(point.y < _minY)
    _minY = point.y;
  if(point.y > _maxY)
    _maxY = point.y;
  _valid = 1;
}

double BBox::getHPWL(void)
{
  return((_maxX-_minX)+(_maxY-_minY));
}

double BBox::getXSize(void)
{ return(_maxX-_minX); }

double BBox::getYSize(void)
{ return(_maxY-_minY); }

bool BBox::isValid(void)
{
  return _valid;
}
void parquetfp::eatblank(ifstream& i)
{
  while (i.peek()==' ' || i.peek()=='\t') i.get();
}

void parquetfp::skiptoeol(ifstream& i)
{
  while (!i.eof() && i.peek()!='\n' && i.peek()!='\r') i.get();
  i.get();
}

bool parquetfp::needCaseChar(ifstream& i, char character)
{
  while(!i.eof() && i.peek() != character) i.get();
  if(i.eof())
    return 0;
  else
    return 1;
}
void parquetfp::eathash(ifstream& i)
{
  skiptoeol(i);
}

ORIENT parquetfp::toOrient(char* orient)
{
  if(!strcmp(orient, "N"))
    return N;
  if(!strcmp(orient, "E"))
    return E;
  if(!strcmp(orient, "S"))
    return S;
  if(!strcmp(orient, "W"))
    return W;
  if(!strcmp(orient, "FN"))
    return FN;
  if(!strcmp(orient, "FE"))
    return FE;
  if(!strcmp(orient, "FS"))
    return FS;
  if(!strcmp(orient, "FW"))
    return FW;

  cout<<"ERROR: in converting char* to ORIENT"<<endl;
  return N;
}

char* parquetfp::toChar(ORIENT orient)
{
  if(orient == N)
   { return("N"); }
  if(orient == E)
   { return("E"); }
  if(orient == S)
   { return("S"); }
  if(orient == W)
   { return("W"); }
  if(orient == FN)
   { return("FN"); }
  if(orient == FE)
   { return("FE"); }
  if(orient == FS)
   { return("FS"); }
  if(orient == FW)
   { return("FW"); }
  cout<<"ERROR: in converting ORIENT to char* "<<endl;
  return "N";
}

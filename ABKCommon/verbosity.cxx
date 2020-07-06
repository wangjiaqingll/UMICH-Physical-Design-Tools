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






//Created by ilm, Nov 16, 1997

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif


#include <limits.h>
#include <stdarg.h>
#include <string>
#include <vector>
#include <iostream>
#include "abkcommon.h"
#include "verbosity.h"
Verbosity& Verbosity::operator=(const Verbosity& v)
      {
         abkwarn(v.getNumTypes()<=_levels.size(),
                 "  Losing debug levels while assigning Verbosities\n  Use copy-construction");
         for(unsigned k=0;k<v.getNumTypes();k++)
             _levels[k]=v._levels[k];
         _levels.erase(_levels.begin()+v.getNumTypes(),
                       _levels.begin()+getNumTypes());
         return *this;
      }

ostream& operator<<(ostream& os, const Verbosity& verbsty) // 0,1,2三个等级
{
        unsigned numLevels=verbsty._levels.size(); 

        os << " Verbosity levels : " << numLevels << endl;
        os << " For actions : " << verbsty.forActions << ",  "
           << " For sys resources : " << verbsty.forSysRes << ",  "
           << " For major stats : " << verbsty.forMajStats << endl;
      
        if (numLevels > 3)
        {
          os << " Other levels : " ;
          for (unsigned i=3; i!=numLevels; ++i)
          {
                if ( i % 10 == 0 )
                {
                    if (i) os << endl;
                    os <<"      ";
                }
                os<<setw(3)<<verbsty._levels[i]<<" ";
          }
          os << endl;
        }

    return os;
}

void Verbosity::_ctructFromString(const char* levels)
{
   if (levels==NULL || 
       !strcmp(levels,"0") || !strcasecmp(levels,"silent"))
   {
      _levels.erase(_levels.begin()+3,_levels.end());
      return;
   }

   unsigned len=strlen(levels);
   char * levs=new char[len+1];
   for(unsigned c=0;c<len;c++) 
   if (levels[c]=='_') levs[c]=' '; else levs[c]=levels[c];
   levs[len]='\0';

   unsigned  numLev=0;
   char *start =levs;
   char *finish=levs;
   while (start<levs+len)
   {
      unsigned tmp=strtoul(start,&finish,10);
      if (start==finish) start++;
      else 
      { 
         abkfatal(tmp<=INT_MAX,"Verbosity levels can't be >INT_MAX");
         abkfatal(numLev<100,"This ctor can't work with 100+ verbosity levels");
         _levels[numLev]=tmp;
         start=finish;
         numLev++;
      }
   }
   abkfatal(numLev>2," Can't ctruct Verbosity: need at least 3 levels\n ");
   _levels.erase(_levels.begin()+numLev,_levels.end());
   delete[] levs;
}

Verbosity::Verbosity(const char* levels)
// space or underscore-separated unsigneds
:_levels(100,0),
forActions(_levels[0]),forSysRes(_levels[1]),forMajStats(_levels[2])
{
  _ctructFromString(levels);
}

Verbosity::Verbosity(int argc, const char *argv[]) // catches -verb
:_levels(100,0),
forActions(_levels[0]),forSysRes(_levels[1]),forMajStats(_levels[2])
{
    StringParam verb("verb",argc,argv);
    BoolParam helpRequest1("h",argc,argv);
    BoolParam helpRequest2("help",argc,argv);

    if (helpRequest1.found() || helpRequest2.found())
    {
      cout << " -verb 1_1_1 | silent " << endl; 
      _levels.erase(_levels.begin()+3,_levels.end());
      return;
    }
  
    if (verb.found())  _ctructFromString(verb);
    else 
    { 
       _levels.erase(_levels.begin()+3,_levels.end());
       _levels[0]=_levels[1]=_levels[2]=1;
    }
}

Verbosity::Verbosity():
_levels(3,1),
forActions(_levels[0]),forSysRes(_levels[1]),forMajStats(_levels[2])
{
}

Verbosity::Verbosity(const std::vector<unsigned>& levels):
_levels(levels),
forActions(_levels[0]),forSysRes(_levels[1]),forMajStats(_levels[2])
{
   abkfatal(levels.size()>=3,
            "Can not initialize verbosity --- too few levels\n ");
}


Verbosity::Verbosity(unsigned numLevels, unsigned ForActions,
                     unsigned ForSysRes, unsigned ForMajStats, ...):
_levels(numLevels,0),
forActions(_levels[0]),forSysRes(_levels[1]),forMajStats(_levels[2])
{
   abkfatal(numLevels>=3,"Can not initialize verbosity --- too few levels\n ");
   _levels[0]=ForActions;
   _levels[1]=ForSysRes;
   _levels[2]=ForMajStats;

   va_list ap;
   va_start(ap,ForMajStats);

   for(unsigned k=3;k<numLevels;k++)
   {
     _levels[k]=va_arg(ap,unsigned);
     abkfatal(_levels[k]<INT_MAX/2," Verbosity levels can't be > INT_MAX /2");
   }
}

unsigned & Verbosity::operator[](unsigned diagType)
{ 
   abkfatal(diagType<_levels.size(),"Inexistant diagnostic type");
   return _levels[diagType];
}


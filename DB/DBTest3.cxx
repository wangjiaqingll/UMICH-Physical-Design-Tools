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
#pragma warning (disable:4786)
#endif

#include "ABKCommon/abkcommon.h"
#include "DB.h"

// This is how you can plot a cell placement
// to view the resulting "xy" file in gnuplot plot type
//              plot "filename.xy" with lines
//            to produce a PS file (e.g. if you don't have an X terminal)
//            type
//              set terminal postscript
//              set output   "myfile.ps"
//            before issuing the plot command

int main(int argc, char *argv[])
{
  StringParam auxFileName("f",argc,argv);
  BoolParam check("checkDB",argc,argv);
  DB::Parameters params;
  params.alwaysCheckConsistency=check;
  abkfatal(auxFileName.found(),"Usage: prog -f filename.aux");
  DB db(auxFileName,params);

  db.saveXYLineDrawing("dbpic.xy");

  return 0;
}

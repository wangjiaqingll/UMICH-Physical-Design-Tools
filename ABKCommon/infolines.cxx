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






// June 15, 1997   Igor Markov  VLSI CAD UCLA ABKGROUP

// This file to be included into all projects in the group

#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <iostream>
#include <iomanip>

#include "infolines.h"


/* ======================== IMPLEMENTATIONS ======================== */
TimeStamp::TimeStamp()
{ 
   char * date;
   time_t tp;   

   char expl[]="# Created      : ";
   if (time(&tp)==-1)
   {
     _infoLine = new char[80]; 
     strcpy(_infoLine," Error in time() "); 
      return;
   }
   date=asctime(localtime(&tp)); 
   _infoLine = new char[strlen(expl)+strlen(date)+1];
   strcpy(_infoLine,expl);
   strcat(_infoLine,date);
}

CmdLine::CmdLine(int argc, const char *argv[])
{
  char expl[]="# Command line :";
  int len=strlen(expl), n=argc;
  while (n--) len+=(1+strlen(argv[n])); 
  if (len<255) len=255;

  _infoLine=new char[len+3]; 

  char * infoPtr=_infoLine;
  strcpy(infoPtr,expl);
  infoPtr += strlen(expl);
  infoPtr[0]=' ';
  infoPtr++;

  n=-1;
  while (++n<argc)
  {
      strcpy(infoPtr,argv[n]);
      infoPtr += strlen(argv[n]);
      infoPtr[0]=' '; 
      infoPtr++;
  }
  infoPtr[0]='\n';
  infoPtr[1]='\0';
}

ostream& operator<<(ostream& out, const MemUsage& memu)
{
  out << "# Memory usage : " << setw(7) << memu.getPeakMem() << "Mb (peak)  " 
      << setw(4) << memu.getEstimate() << "Mb (estimate)"   << endl;
  return out;
}


ostream& operator<<(ostream& out, const SysInfo& si)
{
  out << si.tm
      << si.pl
      << si.us
      << si.mu
      << si.cpunorm << endl;
  return out;
}


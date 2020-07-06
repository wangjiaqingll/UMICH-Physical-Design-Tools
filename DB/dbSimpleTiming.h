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









// Created 010721 by Igor Markov, Univ. of Michigan
// 020811   ilm    ported to g++ 3.0

#ifndef _SIMPLE_TIMING_H_
#define _SIMPLE_TIMING_H_

#include "ABKCommon/abkcommon.h"
#include "ABKCommon/sgi_hash_map.h"

struct dbSimpleTiming
{
    struct PinRecord 
    { double drivRes; double sinkCap;
      PinRecord(): drivRes(DBL_MAX), sinkCap(DBL_MAX)       {}
      PinRecord(double r, double c): drivRes(r), sinkCap(c) {}
    };

    struct eqstr
    {
      bool operator()(const char* s1, const char* s2) const
      { return (strcmp(s1, s2) == 0); }
    };

    bool   populated;
//  The Cint_per_unit and Rint_per_unit are functions of netlength:
//  Cint_per_unit = exp(Cint_K * log(length) + Cint_C)
//  Rint_per_unit = exp(Rint_K * log(length) + Rint_C)
    double rInt_c, rInt_k;
    double cInt_c, cInt_k;

    hash_map<const char*, PinRecord, hash<const char*>, eqstr> pinInfo;

    dbSimpleTiming(): populated(false), 
                      rInt_c(DBL_MAX), rInt_k(DBL_MAX),
                      cInt_c(DBL_MAX), cInt_k(DBL_MAX) {}

    const PinRecord* getPinRecordByName(const char* combinedName) const
    {
       hash_map<const char*, PinRecord, hash<const char*>, eqstr>
              ::const_iterator prIt;
       prIt=pinInfo.find(combinedName);
       if (prIt==pinInfo.end()) return NULL;
       else   return &(*prIt).second; 
    }
};

#endif

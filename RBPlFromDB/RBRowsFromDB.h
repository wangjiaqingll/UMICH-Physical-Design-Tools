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






// Created 970804 by Igor Markov and Paul Tucker, VLSI CAD ABKGROUP, UCLA/CS

#ifndef _RBROWSFROMDB_H_
#define _RBROWSFROMDB_H_

#include "DB/dbContext.h"
#include "RBPlace/RBRows.h"

class DB;
class dbEasyAccess;
class RBPCoreRow;
class RBPSubRow;
 
class dbRowYlt
{
public:
  dbRowYlt(){}
  bool operator() (const dbRow * r1, const dbRow *r2) const
  {
    return r1->getOrigin().y < r2->getOrigin().y;
  }
};

class dbRowXlt
{
public:
  dbRowXlt(){}
  bool operator() (const dbRow *r1, const dbRow *r2) const
  { return r1->getOrigin().x < r2->getOrigin().x; }
};

#endif

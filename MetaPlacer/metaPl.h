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








// Created: Dec 1, 1999 by Igor Markov <imarkov@cs.ucla.edu>


#ifndef _METAPL_H_
#define _METAPL_H_

#include <iostream>
using std::ostream;
#include "Placement/placement.h"

class RBPlacement;
class DB;

class MetaPlacerParams
{
   public:
     Verbosity verb;
     bool save;
     bool flip;
     bool skipRowIroning;
     bool saveXorder;
     unsigned num;

  MetaPlacerParams():save(false),flip(false),skipRowIroning(false),
                     saveXorder(false), num(1) {};
  MetaPlacerParams(int argc, const char *argv[]);
};

ostream& operator<< (ostream&, const MetaPlacerParams&);

class MetaPlacer 
{

  vector<double> wls, runTs;

   protected:

    MetaPlacerParams _params;
    DB *_db;

    enum InputType {DB_, RBPlace_, HGraph_};
    InputType inputTyp;

    int  _greetingEtc(int argc, const char *argv[]);
    void _workWithRBPlace(RBPlacement& rbplace, 
                          int argc, const char *argv[]);
    void _workWithRBPlaceNtimes(RBPlacement& rbplace, 
                          int argc, const char *argv[]);

   public:

    MetaPlacer(int argc, const char *argv[]);
    MetaPlacer(RBPlacement& rbplace, int argc, const char *argv[]);
    MetaPlacer(RBPlacement& rbplace, MetaPlacerParams par=MetaPlacerParams());
   ~MetaPlacer();
};

#endif

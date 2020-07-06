/**************************************************************************
***
***    Copyright (c) 1995-1997 by ABKGroup, UCLA VLSI CAD Laboratory,
***    UCLA Computer Science Department, Los Angeles, CA 90095-1596 USA,
***    and by the Regents of the University of California.
***    All rights reserved.
***
***    No portion of this file may be used, copied, or transmitted for
***    any purpose, including any research or commercial purpose,
***    without the express written consent of Professor Andrew B. Kahng,
***    UCLA Computer Science Department, 3713 Boelter Hall, Los Angeles,
***    CA 90095-1596 USA.   Any copy made of this file must include this
***    notice.    ABKGroup and the Regents of the University of California
***    retain title to all copies of this file and/or its compiled
***    derivatives.
***
***************************************************************************/

//Created 970718 by Igor Markov, ABKGROUP VLSI CAD UCLA

#ifndef _PARSERXXX_H_
#define _PARSERXXX_H_

// XXX is a database [multi]-file format (e.g. LEFDEF)
// The class below may be declared as friend by classes in the DB package
// (see the MakeFriendsWithParsers in dbDefines.h)
// If should probably be instantiated inside DB::parseXXX() 
//  (see Parser/DBreadXXX.cc).
//   Among other uses, this class can accomodate the first pass for a 
// multipass XXX reader, setting up a parsing tree, computing 
// numbers of components (e.g. for preallocation), supporting
// global buffering etc.

#include "DB.h"

class ParserXXX
{
   DB& _db;        // ParserXXX knows which DB object instantiated it
                   // and populates its members
   public:         
                   // a typical instatiation in DB::parseXXX() will be
                   // ParserXXX(*this  /*, other params */);
     ParserXXX(DB db/*, other params */);
};

#endif 

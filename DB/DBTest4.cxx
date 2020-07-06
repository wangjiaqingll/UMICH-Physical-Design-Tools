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









// CHANGES
#ifdef _MSC_VER
#pragma warning (disable:4786)
#endif

#include "DB.h"

int main(int argc, const char *argv[])
{
  NoParams         noParams       (argc,argv);
  BoolParam        helpRequest    ("help",argc,argv);
  BoolParam        helpRequest1   ("h",   argc,argv);
  BoolParam check("checkDB",argc,argv);
  DB::Parameters params;
  params.alwaysCheckConsistency=check;

  UnsignedParam    kValues     ("kValue", argc, argv);

  StringParam 	 auxFileName 	("f", 	argc,argv);
  abkfatal(auxFileName.found(),"Usage: prog -f filename.aux <more params>");

  DB db(auxFileName,params);
  abkfatal(!db.routedNets.isRouted(), "DB can't be routed yet");
  db.routedNets.readLEFDEF(db.getNetlist(), db.getLayout(), auxFileName);
  abkfatal(db.routedNets.isRouted(), "DB has to be routed now");

  cout << db.routedNets << endl;
  cout << "\ntotal WL: " << db.routedNets.getTotalWL() << " ( "
       << db.routedNets.getTotalHorizWL() << " H " 
       << db.routedNets.getTotalVertWL() << " V )\n";

  db.saveDEF("saveDEF.out");

  cout << MemUsage() << endl;

  return 0;
}

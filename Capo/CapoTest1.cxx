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
#pragma warning(disable:4786)
#endif


#include "ABKCommon/abkcommon.h"
#include "ABKCommon/abkversion.h"
#include "capoPlacer.h"
#include "RBPlace/RBPlacement.h"
//#include "greedyOrientOpt.h"
//#include "rowdp.h"
//#include "rownet.h"
//#include "netopt.h"

int main(int argc, const char *argv[])
{
  NoParams         noParams       (argc,argv);
  BoolParam        helpRequest    ("help",argc,argv);
  BoolParam        helpRequest1   ("h",   argc,argv);
  StringParam      savePl         ("savePl",argc,argv);
// BoolParam        doCellFlipping ("flip",argc,argv);
//  BoolParam        doCellSpacing  ("cellSpace",argc,argv);

  Verbosity	   verb(argc, argv);

  cout<<"\n(c) copyright, 1998-1999 Regents of the University of California\n";
  cout<<" Authors: Andrew E. Caldwell, Andrew B. Kahng, Igor L. Markov\n";
  cout<<" Email:   {caldwell,abk,imarkov}@cs.ucla.edu"<<endl<<endl;
  PRINT_VERSION_INFO
  cout << CmdLine(argc,argv);

// ------------- Process parameters before getting bogged down in computations

  CapoParameters capoParams(argc,argv);

  if (noParams.found() || helpRequest.found() || helpRequest1.found())
  {
     cout << "  Use '-help' or '-f filename.aux' " << endl;
     cout << "  To save output, use -savePl filename " << endl;
     return 0;
  }

  cout << capoParams << endl;

  StringParam 	 auxFileName 	("f", 	argc,argv);
  abkfatal(auxFileName.found(),"Usage: prog -f filename.aux <more params>");

  RBPlaceParams rbParam(argc,argv);
  cout << rbParam <<endl;
  RBPlacement rbplace(auxFileName, rbParam);   

  Timer capoTimer;
  CapoPlacer capo(rbplace, capoParams);
  capoTimer.stop();

  cout << "After Capo  "<<endl;
  cout << "  Capo Runtime: "<<capoTimer.getUserTime()<<endl;

  cout << "  RBPl Est HPWL: "<<rbplace.evalHPWL(false)<<endl;;
  cout << "  RBPl Est Pin-to-Pin HPWL: "<<rbplace.evalHPWL(true)<<endl;;
  cout << "  RBPl Est  WWL: "<<rbplace.evalWeightedWL(false)<<endl;;
  cout << "  RBPl Est Pin-to-Pin  WWL: "<<rbplace.evalWeightedWL(true)<<endl;;

  if(savePl.found())
  	rbplace.savePlacement(savePl);

  return 0;
}

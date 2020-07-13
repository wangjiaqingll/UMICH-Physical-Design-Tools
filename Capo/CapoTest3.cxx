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

  cout<<"\n(c) copyright, 1998-1999 Regents of the University of California\n";
  cout<<" Authors: Andrew E. Caldwell, Andrew B. Kahng, Igor L. Markov\n";
  cout<<" Email:   {caldwell,abk,imarkov}@cs.ucla.edu"<<endl<<endl;
  cout<<endl;
  cout<<"Permission is hereby granted, without written agreement and \n"
      <<"without license or royalty fee, to use, copy, modify, and \n"
      <<"distribute and sell this software and its documentation for \n"
      <<"any purpose, provided that the above copyright notice, this \n"
      <<"permission notice, and the following two paragraphs appear \n"
      <<"in all copies of this software as well as in all copies of \n"
      <<"supporting documentation.\n\n"; 
  cout<<"THIS SOFTWARE AND SUPPORTING DOCUMENTATION IS PROVIDED \"AS IS\".\n"
      <<"The Microelectronics Advanced Research Corporation (MARCO), the\n"
      <<"Gigascale Silicon Research Center (GSRC),  and \n"
      <<"(\"PROVIDERS\") MAKE NO WARRANTIES, whether express \n"
      <<"or implied, including warranties of merchantability or fitness\n"
      <<"for a particular purpose or noninfringement, with respect to \n"
      <<"this software and supporting documentation.\n"
      <<"Providers have NO obligation to provide ANY support, assistance,\n"
      <<"installation, training or other services, updates, enhancements\n"
      <<"or modifications related to this software and supporting\n"
      <<"documentation.\n\n";
  cout<<"Providers shall NOT be liable for ANY costs of procurement of \n"
      <<"substitutes, loss of profits, interruption of business, or any \n"
      <<"other direct, indirect, special, consequential or incidental \n"
      <<"damages arising from the use of this software and its \n"
      <<"documentation, whether or not Providers have been advised of \n"
      <<"the possibility of such damages."<<endl<<endl;
    
  PRINT_VERSION_INFO

// ------------- Process parameters before getting bogged down in computations


  if (noParams.found() || helpRequest.found() || helpRequest1.found())
  {
     cout<<" -h		help"<<endl;
     cout<<" -help	help"<<endl;
     cout<<" -f      <auxFilename> auxfile for design to place"<<endl;
     cout<<" -block  <blockPlFileName> "<<endl;
     cout<<" -hcl    <hclFileName> "<<endl;
     cout<<" -savePl <fileName>	save resulting placement"<<endl;

     return 0;
  }

  StringParam 	 auxFileName 	("f", 		argc,argv);
  StringParam 	 blockFileName 	("block", 	argc,argv);
  StringParam 	 hclFileName 	("hcl", 	argc,argv);
  abkfatal(auxFileName.found(),   "Usage: prog -f filename.aux <more params>");
  abkfatal(blockFileName.found(), "Usage: prog -block blockfilename ");
  abkfatal(hclFileName.found(),   "Usage: prog -hcl hclfilename ");

  RBPlaceParams rbParam(0,0);
  RBPlacement rbplace(auxFileName, rbParam);   

  CapoParameters capoParams(argc,argv);

  Timer capoTimer;
  CapoPlacer capo(rbplace, hclFileName, blockFileName, capoParams);
  capoTimer.stop();

  cout<<" Capo Runtime: "<<capoTimer.getUserTime()<<endl;

  cout<<" Origin-to-Origin HPWL: "<<rbplace.evalHPWL(false)<<endl;;
  cout<<" Pin-to-Pin       WPWL: "<<rbplace.evalHPWL(true)<<endl;;
  cout<<" Origin-to-Origin  WWL: "<<rbplace.evalWeightedWL(false)<<endl;;
  cout<<" Pin-to-Pin        WWL: "<<rbplace.evalWeightedWL(true)<<endl;;

  if(savePl.found())
  	rbplace.savePlacement(savePl);

  return 0;
}

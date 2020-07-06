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





/*Authors Matt Guthaus and Saurabh Adya*/



// CHANGES

#include "ABKCommon/abkcommon.h"
#include "ABKCommon/abkversion.h"
#include "RBPlace/RBPlacement.h"
#include "simAnneal.h"

int main(int argc, const char *argv[])
{
  NoParams         noParams       (argc,argv);
  BoolParam        helpRequest    ("help",argc,argv);
  BoolParam        helpRequest1   ("h",   argc,argv);
  StringParam      auxFileName    ("f",   argc,argv);
  BoolParam        save           ("save",argc,argv);
  BoolParam        keepOverlaps   ("skipLegal",argc,argv);  
  BoolParam        greedy         ("greedy", argc, argv);

  if (noParams.found() || helpRequest.found() || helpRequest1.found())
    {
      cout << "  Use '-help' or '-f filename.aux' " << endl;
      cout <<"Options\n"
	   <<"-save \n"
           <<"-greedy (no annealing, just greedy moves)\n\n ";
      exit(0);
    } 

  RBPlaceParams rbParam(argc,argv);
  RBPlacement rbplace(auxFileName, rbParam);   

  if(!keepOverlaps.found())
    rbplace.remOverlaps();

  cout << " ====== Launching Detailed Placement ... " << endl;
  
  double initHPWL = rbplace.evalHPWL(true);
  cout <<"Initial Origin-to-Origin WL: "<<rbplace.evalHPWL(false)<<endl;
  cout <<"Initial Pin-to-Pin WL:       "<<initHPWL<<endl;
  
  Timer SATimer;  
  SimAnneal SA(rbplace, greedy);
  rbplace.remOverlaps();
  SATimer.stop();

  double HPWLafter = rbplace.evalHPWL(true);

  cout << " Final Origin-to-Origin WL: "<<rbplace.evalHPWL(false)<<endl;
  cout << " Final Pin-to-Pin WL:       "<<HPWLafter<<endl;

  cout<<"  % improvement in HPWL is "<<(initHPWL - HPWLafter)*100/HPWLafter
      <<endl;
  cout<<"Time taken = "<<SATimer.getUserTime()<<" seconds\n";

  if(save.found())
    {
      cout << "Saving out.pl" << endl;
      rbplace.savePlacement("out.pl");
    }

  //rbplace.saveAsPlot("plot",-DBL_MAX,DBL_MAX,-DBL_MAX,DBL_MAX,"out");
  return 0;
}

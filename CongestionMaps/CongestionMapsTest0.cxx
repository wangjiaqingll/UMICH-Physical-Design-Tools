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






#include "ABKCommon/abkcommon.h"
#include "RBPlace/RBPlacement.h"
#include "CongestionMaps.h"

int main(int argc, const char *argv[])
{
  NoParams         noParams       (argc,argv);
  BoolParam        helpRequest    ("help",argc,argv);
  BoolParam        helpRequest1   ("h",   argc,argv);
  StringParam      plotCongMap("plotCongMap", argc, argv);
  StringParam      plotCongMapGP("plotCongMapGP", argc, argv);
  StringParam      plotCongMapXPM("plotCongMapXPM", argc, argv);
  BoolParam        noProbability   ("noProb",   argc,argv);
  DoubleParam      gridSize ("gridSize", argc, argv);

  RBPlacement::Parameters rbParams(argc,argv);

  cout<<"/**************************************************************************\n"
      <<"***\n"
      <<"*** Copyright (c) 1995-2000 Regents of the University of California,\n"
      <<"***               Andrew E. Caldwell, Andrew B. Kahng and Igor L. Markov\n"
      <<"*** Copyright (c) 2000-2003 Regents of the University of Michigan,\n"
      <<"***               Saurabh N. Adya and Igor L. Markov\n"
      <<"***\n"
      <<"***  Contact author(s): abk@cs.ucsd.edu, imarkov@umich.edu\n"
      <<"***  Original Affiliation:   UCLA, Computer Science Department,\n"
      <<"***                          Los Angeles, CA 90095-1596 USA\n"
      <<"***\n"
      <<"***  Permission is hereby granted, free of charge, to any person obtaining \n"
      <<"***  a copy of this software and associated documentation files (the\n"
      <<"***  Software, to deal in the Software without restriction, including\n"
      <<"***  without limitation \n"
      <<"***  the rights to use, copy, modify, merge, publish, distribute, sublicense, \n"
      <<"***  and/or sell copies of the Software, and to permit persons to whom the \n"
      <<"***  Software is furnished to do so, subject to the following conditions:\n"
      <<"***\n"
      <<"***  The above copyright notice and this permission notice shall be included\n"
      <<"***  in all copies or substantial portions of the Software.\n"
      <<"***\n"
      <<"*** THE SOFTWARE IS PROVIDED AS IS, WITHOUT WARRANTY OF ANY KIND, \n"
      <<"*** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES\n"
      <<"*** OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. \n"
      <<"*** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY\n"
      <<"*** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT\n"
      <<"*** OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR\n"
      <<"*** THE USE OR OTHER DEALINGS IN THE SOFTWARE.\n"
      <<"***\n"
      <<"***\n"
      <<"***************************************************************************\n"<<endl;


  if (noParams.found() || helpRequest.found() || helpRequest1.found())
  {
    cout << "  Congestion Map plotting utility"<<endl;
     cout << "  Use '-help' or '-f filename.aux' " << endl;
     cout << "  Other options: \n"
          << "  -noProb (do not use Probabilistic congestion map)\n"
          << "  -gridSize <double>  (the grid size to be used for congestion grid) \n"
          << "  -plotCongMap base_filename  (plot congestion map in matlab format) \n"
          << "  -plotCongMapGP base_filename (plot congestion map in gnuplot format)\n"
          << "  -plotCongMapXPM base_filename (plot congestion map in xpm format)\n"
          << endl;
     return 0;
  }
  StringParam auxFile("f", argc, argv);
  abkfatal(auxFile.found(), "must have -f <auxfilename>");
  RBPlacement rbplace(auxFile, rbParams);

  double congGridSize = -9999;
  if(gridSize.found())
    congGridSize = gridSize;

  CongestionMaps congMap(&rbplace, congGridSize);

  if(noProbability.found())
    {
      congMap.createCongestionMap();
    }
  else
    {
      congMap.initializeFMatrix();
      congMap.createCongestionMapProb();
    }

  congMap.printCongestionStats();

  if(plotCongMap.found())
    congMap.plotMatCongMap(plotCongMap);

  if(plotCongMapGP.found())
    congMap.plotCongMap(plotCongMapGP);

  if(plotCongMapXPM.found())
    congMap.plotXPMCongMap(plotCongMapXPM);

  return 0;
}



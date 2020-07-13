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
#include "DB/DB.h"
#include "capoPlacer.h"
#include "RBPlFromDB/RBPlaceFromDB.h"
#include "NetTopology/hgWDimsFromDB.h"
//#include "greedyOrientOpt.h"
//#include "rowdp.h"
//#include "rownet.h"
//#include "netopt.h"
#include "PlaceEvals/edgePlEval.h"

//#include "TDplace/tdPlace.h"

int main(int argc, const char *argv[])
{
  NoParams         noParams       (argc,argv);
  BoolParam        helpRequest    ("help",argc,argv);
  BoolParam        helpRequest1   ("h",   argc,argv);
  BoolParam        saveDef        ("save",argc,argv);
  BoolParam        saveOrigDef    ("saveOrig",argc,argv);
  BoolParam        doCellFlipping ("flip",argc,argv);
  BoolParam        doCellSpacing  ("cellSpace",argc,argv);

  Verbosity	   verb(argc, argv);

  PRINT_VERSION_INFO
  cout << CmdLine(argc,argv);

// ------------- Process parameters before getting bogged down in computations

  CapoParameters capoParams(argc,argv);

  if (noParams.found() || helpRequest.found() || helpRequest1.found())
  {
     cerr << "  use '-help' or '-f filename.aux' " << endl;
     return 0;
  }

  cout << capoParams << endl;

  StringParam 	 auxFileName 	("f", 	argc,argv);
  abkfatal(auxFileName.found(),"Usage: prog -f filename.aux <more params>");

  BoolParam check("checkDB",argc,argv);
  DB::Parameters dbparams;
  dbparams.alwaysCheckConsistency=check;

  DB db(auxFileName,dbparams);

  if(saveOrigDef.found())
	db.saveDEF("origDef");

  cerr<<"Design Name: "<<db.getDesignName()<<endl;

  RBPlaceParams rbParam(argc,argv);
  RBPlaceFromDB rbplace(db, rbParam);   

  Timer capoTimer;
  CapoPlacer capo(rbplace, capoParams);
  capoTimer.stop();

  db.setPlaceAndOrient(rbplace);
       
  cout << "After Capo  "<<endl;
  cout << "  Capo Runtime:             "<<capoTimer.getUserTime()<<endl;

  cout << "  DB   Est Pin-to-Pin HPWL: "<<
	db.getNetlist().evalHalfPerimCost(db.spatial.locations,
				  db.spatial.orientations)<<endl<<endl;

  cout << "  RBPl Est            HPWL: "<<rbplace.evalHPWL(false)<<endl;
  cout << "  RBPl Est Pin-to-Pin HPWL: "<<rbplace.evalHPWL(true)<<endl;;
  cout << "  RBPl Est             WWL: "<<rbplace.evalWeightedWL(false)<<endl;;
  cout << "  RBPl Est Pin-to-Pin  WWL: "<<rbplace.evalWeightedWL(true)<<endl;;

  if (saveDef.found()) db.saveDEF("out.def");

  const HGraphFixed& hg=rbplace.getHGraph();
  bit_vector fixed(hg.getNumNodes()), storageElts(hg.getNumNodes());
  if (db.spatial.constraints.hasFGC())
      db.spatial.constraints.getFGCbitVec(fixed); 

  bit_vector storElts(fixed.size());
  unsigned numStor=0;
  for(unsigned j=0; j!=fixed.size(); j++)
  {
    const dbCell&        cell=db.getNetlist().getCellByIdx(j);
    const dbMasterCell& mcell=cell.getMaster();
    if (mcell.isCore())
//  if (mcell.isFlipFlop() && mcell.isLatch()) 
    if (cell.isSC()) 
    { storElts[j]=true; numStor++; }
  }

  cout << " Found " << numStor << " store elements " << endl;

  TDPlace tdpl(hg, storElts, rbplace, fixed);

  tdpl.computeEdgeDelays();
  tdpl.computeCriticalities();
  tdpl.plotEverything(5);

  return 0;
}

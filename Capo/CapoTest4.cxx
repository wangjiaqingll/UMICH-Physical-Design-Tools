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
#include "RBPlace/RBPlaceFromDB.h"
#include "HGraphWDims/hgWDimsFromDB.h"
//#include "greedyOrientOpt.h"
//#include "rowdp.h"
//#include "rownet.h"
//#include "netopt.h"
#include "PlaceEvals/edgePlEval.h"

int main(int argc, const char *argv[])
{
  NoParams         noParams       (argc,argv);
  BoolParam        helpRequest    ("help",argc,argv);
  BoolParam        helpRequest1   ("h",   argc,argv);
  BoolParam        saveDef        ("save",argc,argv);
  BoolParam        saveOrigDef    ("saveOrig",argc,argv);
  BoolParam        doCellFlipping ("flip",argc,argv);
  BoolParam        doCellSpacing  ("cellSpace",argc,argv);
  BoolParam check("checkDB",argc,argv);
  DB::Parameters dbparams;
  dbparams.alwaysCheckConsistency=check;

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

  capoParams.replaceSmallBlocks=Never;
  cout << capoParams << endl;

  StringParam 	 auxFileName 	("f", 	argc,argv);
  abkfatal(auxFileName.found(),"Usage: prog -f filename.aux <more params>");

  DB db(auxFileName,dbparams);

  if(saveOrigDef.found())
	db.saveDEF("origDef");

  cerr<<"Design Name: "<<db.getDesignName()<<endl;


  RBPlaceParams rbParam(argc,argv);
  RBPlaceFromDB rbplace(db, rbParam);   

  vector<const char*> hierCellNames;

 {
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

  capo.getHierCellNames(hierCellNames);
 }

//for(unsigned k=0; k!=hierCellNames.size(); k++)
//   cout << setw(6)<< k << ". " << hierCellNames[k]  << endl;
  HGNodeNamesMap      hierNamesMap, tmpNamesMap;
  for(unsigned k=0; k!=hierCellNames.size(); k++)
     hierNamesMap[hierCellNames[k]]=k;

 cout << " ======== Launching another Capo run  --- with hierarchy ========= "
      << endl;

 capoParams.splitterParams.useNameHierarchy=true;
 capoParams.splitterParams.delimiters="_";
 capoParams.replaceSmallBlocks=AtTheEnd;

 
  // now go to RBPlace and swap the old names for the new ones

  HGraphWDimensions&  hg=const_cast<HGraphWDimensions&>(rbplace.getHGraph());
  abkfatal(hg.getNumNodes()==hierCellNames.size(),
            " Numbers of cell names do not match");
  vector<const char*> tmpNames=hg._nodeNames;
  hg._nodeNames=hierCellNames;
  tmpNamesMap=hg._namesMap;
  hg._namesMap =hierNamesMap;

 {
  cout<<capoParams<<endl;

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

 }

  if (saveDef.found()) db.saveDEF("out.def");

/*
  hg._nodeNames=tmpNames;
  hg._namesMap=tmpNamesMap;
*/

  return 0;
}

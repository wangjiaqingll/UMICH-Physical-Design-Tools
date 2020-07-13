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
#include "DB/DB.h"
#include "RBPlaceFromDB.h"

using std::vector;

int main(int argc, const char *argv[])
{
  StringParam auxFileName("f",argc,argv);
  abkfatal(auxFileName.found(),"Usage: prog -f filename.aux");

  BoolParam check("checkDB",argc,argv);
  DB::Parameters dbParams;
  dbParams.alwaysCheckConsistency = check;

  DB db(auxFileName,dbParams);

  cout<<" Building RBPlacement"<<endl; 
  RBPlacement::Parameters rbParams(argc,argv);
  RBPlaceFromDB rbp(db, rbParams);

  cout << " Row-based placement ";
  if (rbp.isConsistent()) cout << "consistent " << endl;
  else cout << "inconsistent " << endl;

  itRBPCoreRow crItr;
  //for(crItr = rbp.coreRowsBegin(); crItr != rbp.coreRowsEnd(); crItr++)
  crItr = rbp.coreRowsBegin();
  {
    itRBPSubRow srItr;
    //for(srItr = crItr->subRowsBegin(); srItr != crItr->subRowsEnd(); srItr++)
    srItr = crItr->subRowsBegin();
    {
	vector<unsigned> cellIds;
	vector<unsigned> siteSpacing;
	vector<double>   spacing;

	cout<<"Original SR: "<<*srItr<<endl<<endl;

	RBPSubRow& sr = const_cast<RBPSubRow&>(*srItr);


	//________pick up all the cells, and space them uniformly_____
	cout<<"________pick up all the cells, and space them uniformly"<<endl;

	cout<<" Extracting all cells"<<endl;
	rbp.extractCellsFromSR(sr,cellIds,siteSpacing);
	cout<<" SR After Extraction: "<<sr<<endl;
	cout<<"  Num Cells:   "<<cellIds.size()<<endl;
	cout<<" SiteSpacing:  "<<siteSpacing<<endl;

	cout<<" Embedding cells in original order - equal spacing"<<endl;
	rbp.embedCellsInSR(sr,cellIds);
	cout<<" SR After Embedding: "<<sr<<endl;

	//____________look at the order...see that its uniform__
	cout<<"__________look at the order...see that its uniform"<<endl;

	cout<<" Extracting all cells - siteSpacing requested"<<endl;
	rbp.extractCellsFromSR(sr,cellIds,siteSpacing);
	cout<<" SR After Extraction: "<<sr<<endl;
	cout<<"  Num Cells:   "<<cellIds.size()<<endl;
	cout<<" Spacing:      "<<siteSpacing<<endl;

	cout<<" Embedding cells in original order"<<endl;
	rbp.embedCellsInSR(sr,cellIds,siteSpacing);
	cout<<" SR After Embedding: "<<sr<<endl;

	//____________extract the cells, and get the loc offsets__
	cout<<"________extract the cells, and get the loc offsets"<<endl;

	cout<<" Extracting all cells - spacing requested"<<endl;
	rbp.extractCellsFromSR(sr,cellIds,spacing);
	cout<<" SR After Extraction: "<<sr<<endl;
	cout<<"  Num Cells:   "<<cellIds.size()<<endl;
	cout<<" Spacing:      "<<spacing<<endl;

	cout<<" Embedding cells in original order - with spacings"<<endl;
	rbp.embedCellsInSR(sr,cellIds,spacing);
	cout<<" SR After Embedding: "<<sr<<endl;

	//____________check that the cells are still uniformly spaced__
	cout<<"________check that the cells are still uniformly spaced"<<endl;

	cout<<" Extracting all cells - siteSacing requested"<<endl;
	rbp.extractCellsFromSR(sr,cellIds,siteSpacing);
	cout<<" SR After Extraction: "<<sr<<endl;
	cout<<"  Num Cells:   "<<cellIds.size()<<endl;
	cout<<" Spacing:      "<<siteSpacing<<endl;
    }
  }

  return 0;
}


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
#include "DB.h"

int main(int argc, char *argv[])
{
  StringParam auxFileName("f",argc,argv);
  abkfatal(auxFileName.found(),"Usage: prog -f filename.aux");
  DB db(auxFileName);
//db.saveDEF("test.def",true);

  itCellGlobal  cItr;
  unsigned 	counter;

  counter = 0;
  for(cItr = db.getNetlist().cellsBegin();
      cItr != db.getNetlist().cellsEnd(); cItr++)
  {
    if(!(*cItr)->isTimingNode() && (counter++ < 0) )
    {
	cout<<(*cItr)->getIndex()<<") "<<(*cItr)->getName()<<": ";
	cout<<(*cItr)->getType()<<" (master's type) ";
	cout<<(*cItr)->getMaster().getType()<<endl;

	cout<<"   Pins:"<<(*cItr)->getNumPins();
	cout<<" Nets:"<<(*cItr)->getNumNets()<<endl;

	itPinLocal pItr;
	for(pItr = (*cItr)->pinsBegin(); pItr != (*cItr)->pinsEnd(); pItr++)
	{
	    cout<<"   "<<(*pItr)->getMaster().getName()<<" ";
	    cout<<(*pItr)->getType()<<endl;
	}
    }
  }

  cout<<" Total !isTimingNode cells: "<<counter<<endl;

/*
  cout<<"Nets:"<<endl;
  itNetGlobal nItr;
  counter = 0;
  for(nItr = db.getNetlist().netsBegin();
      nItr != db.getNetlist().netsEnd(); nItr++)
  {
    if((*nItr)->getNumPins() <= 1 && (counter++ < 0))
    {
    	cout<<(*nItr)->getName()<<"   Pins:"<<(*nItr)->getNumPins();
    	cout<<" Cells:"<<(*nItr)->getNumCells()<<endl;

    	itCellLocal lcItr;
    	for(lcItr = (*nItr)->cellsBegin(); lcItr != (*nItr)->cellsEnd(); 
			lcItr++)
		cout<<"   "<<(*lcItr)->getName()<<endl;
    }
  } 

  cout<<" Total Nets with degree 1: "<<counter<<endl;
*/
 
  cout<<" Total CCs "<< db.getNetlist().getNumCCs() << endl;
  cout<<" Total SCs "<< db.getNetlist().getNumSCs() << endl;


  if (!db.spatial.allCellsPlaced())
  {
     cout << " Can't compute wirelength estimates --- "
          << " not all cells are placed. Exitting ... " << endl;
     return 0; 
  }

  cout<<" Half-Perim wirelength estimate : " <<db.evalHPWLcost() << endl;
  cout<<" Weighted half-Perim wirelength estimate : "
      <<db.evalWHPWLcost() << endl;

  cout<<" Half-Perim wirelength estimate (threshold 10) : "
      <<db.evalHPWLcost(10) << endl;
  cout<<" Weighted half-Perim wirelength estimate (10) : "
      <<db.evalWHPWLcost(10) << endl;

  cout<<" Half-Perim wirelength estimate w IO nets only : " 
      <<db.evalHPWLcost(0,true)<<endl;

  cout<<" Weighted half-Perim wirelength estimate w IO nets only : "
      <<db.evalWHPWLcost(0,true)<<endl;

  cout<<" Half-Perim wirelength estimate w IO nets only (100): "
      <<db.evalHPWLcost(100,true)<<endl;

  cout<<" Weighted half-Perim wirelength estimate w IO nets only (100): "
      <<db.evalWHPWLcost(100,true)<<endl;
  
  cout<<endl;
 
/*cout<<" MST        wirelength estimate : ";
  cout<<db.getNetlist().evalMSTCost(db.spatial.locations )<<endl;
  cout<<" MST        wirelength estimate : ";
  cout<<db.getNetlist().evalMSTCost(db.spatial.locations,
                                      db.spatial.orientations);
  cout<<endl;
*/

  db.getNetlist().printNetStats();

  return 0;
}

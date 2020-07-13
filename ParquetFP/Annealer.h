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









#include <fstream>
#include <vector>
#include <algorithm>
#include <math.h>
#include <stdlib.h>
#include "DB.h"
#include "SeqPair.h"
#include "SPeval.h"
#include "AnalSolve.h"

using namespace std;

namespace parquetfp
{
class Command_Line;

class Annealer
{
 private:
  DB* _db;
  Command_Line* _params;
  SeqPair* _sp;
  SPeval* _spEval;
  AnalSolve* _analSolve;
  
  vector<Point> sortedXSlacks;
  vector<Point> sortedYSlacks;

  char _baseFileName[200];

 public:
  Annealer(){}
  Annealer(Command_Line* params, DB* db);

  ~Annealer();
  void parseConfig(void);

  void go(void);
  void anneal(void);
  void solveQP(void);  
  void takeSPfromDB(); //converts the present placement to a sequence pair
  
  void eval(void); //just evaluate the current SP and set up required data structures and values
  void evalSlacks(void); //evaluate slacks
  void evalCompact(bool whichDir); //just evaluate the current SP with compaction and set up required data structures and values

  int makeMove(vector<unsigned>& tempX, vector<unsigned>& tempY);
  int makeMoveSlacks(vector<unsigned>& tempX, vector<unsigned>& tempY);
  int makeMoveSlacksOrient(vector<unsigned>& A, vector<unsigned>& B,
		         unsigned& index, ORIENT& oldOrient, ORIENT& newOrient);
  int makeMoveOrient(unsigned& index, ORIENT& oldOrient, ORIENT& newOrient);
  int makeARMove(vector<unsigned>& A, vector<unsigned>& B, double currAR);
  int makeSoftBlMove(vector<unsigned>& A, vector<unsigned>& B,
		     unsigned &index, double &newWidth, double &newHeight);
  int makeIndexSoftBlMove(vector<unsigned>& A, vector<unsigned>& B,
	   	          unsigned index, double &newWidth, double &newHeight);

  int makeHPWLMove(vector<unsigned>& tempX, vector<unsigned>& tempY);
  int makeARWLMove(vector<unsigned>& tempX, vector<unsigned>& tempY, 
		   double currAR);

  void sortSlacks(vector<Point>& sortedXSlacks, vector<Point>& sortedYSlacks);

  double getXSize(void);      //requires that spEvaluations be done earlier
  double getYSize(void);      //requires that spEvaluations be done earlier

  int packSoftBlocks(unsigned numIter); //need to update DB after this
  void updatePlacement(void); //update the placement of DB with spEval info
};

//used for std::sort function to sort slacks
struct sort_slacks
{
  bool operator()(Point pt1, Point pt2)
  {
    return (pt1.x < pt2.x);
  }
};
}
//using namespace parquetfp;

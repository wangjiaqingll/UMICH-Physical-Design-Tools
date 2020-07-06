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





#include <vector>
#include <algorithm>
#include <math.h>
#include <stdlib.h>
#include "Nets.h"
#include "Nodes.h"


namespace parquetfp
{
class DB
{
 protected:
  Nodes* _nodes;
  Nets* _nets;
  
  Nodes* _nodesBestCopy;

  double _area;
  bool _initArea;

 public:
  bool successAR;
  DB(char* baseName);
  DB(DB * db, vector<int>& subBlocksIndices, Point& dbLoc, double reqdAR);
  DB(void);

  ~DB();

  DB& operator=(DB& db2);
  void clean(void);

  unsigned getNumNodes(void);
  Nodes* getNodes(void);
  Nets* getNets(void);

  vector<double> getNodeWidths();
  vector<double> getNodeHeights();
  vector<double> getXLocs();
  vector<double> getYLocs();

  double getNodesArea(void);

  void updatePlacement(vector<double>& xloc, vector<double>& yloc);
  void initPlacement(Point& loc);
  void updateSlacks(vector<double>& xSlack, vector<double>& ySlack);
  void plot(char* fileName, double area, double whitespace, double aspectRatio,
      double time, double HPWL, bool plotSlacks, bool plotNets, bool plotNames);

  void saveCapo(char *baseFileName, double reqdAR);  //save the data in Capo format
  void saveCapoNets(char* baseFileName);

  void save(char* baseFileName); //save data in floorplan format
  void saveNets(char* baseFileName);

  void saveBestCopyPl(char* baseFileName);
  void saveInBestCopy(void);

  double evalHPWL(void);  //assumes that placement is updated
 //assumes that placement is updated & terminals to be moved to floorplan edges
  double evalHPWL(double xSize, double ySize);  
  double evalArea(void);  //assumes that placement is updated
  double getXSize(void);
  double getYSize(void);
  double getAvgHeight(void);

  void shiftDesign(Point& offset); //shift the entire placement by an offset

  //marks all nodes with height > avgHeight as macros
  void markTallNodesAsMacros(double maxHeight);

  //reduce the core cells area of design excluding macros to maxWS whitespace
  //assumes macros are marked using DB::markTallNodesAsMacros()
  void reduceCoreCellsArea(double layoutArea, double maxWS);
};
}
//using namespace parquetfp;

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














// Created July 17, 1997 by Igor Markov, VLSI CAD ABKGROUP, UCLA/CS

// CHANGES
// 970804   ilm     unified consistency checks
// 970804   mro     DB::saveDEF()
// 970814   ilm     added initialization of designname and technoName
// 970814   ilm     added output of DESIGN/END DESIGN; tags to saveDEF()
// 970814   ilm     corrected ";" in saveDEF() to be exactly 
//                                    like in qplace.out.def
// 970814   ilm     added "UNITS DISTANCE" output into saveDEF()
// 970815   ilm     saveDEF() now can tell FIXED cells from PLACED
// 970815   ilm     added consistency checks for fixed cells with invalid locs
//                     and unsatisfied constraints                
// 970829   ilm     added a hack with ABKDEBUG as a workaround for
//                     a bug in SunCC4.2 iropt
// 970904   ilm     added DB::Parameters
// 970924   ilm     added DB::getAuxName()
// 971018   mro     removed blocks to ParserLEFDEF
// 971130   ilm     op<< setting a placement into DB 
//                    now makes all locations valid
// 971130   ilm     added added DB::evalHPWLcost() and DB::evalWHPWLcost()
//                    they assume valid placement in DB (e.g. do DB<<pl).
//                    Note that if, for speed, you don't need to check for
//                    valid placement every time (slow), you need to use
//                    dbNetlist::evalHalfPerimCost() and
//                    dbNetlist::evalWHalfPerimCost() directly.
//                     The new additions are goof for slow benchmarking only.
// 971130   ilm  added DB::saveXYLineDrawing()
//               to view the resulting "xy" file in gnuplot plot type
//                   plot "filename.xy" with lines
//                 to produce a PS file (e.g. if you don't have an X terminal)
//                 type
//                   set terminal postscript
//                   set output   "myfile.ps"
//                 before issuing the plot command
// 980121 ilm  added optional thresholding to WL estimators
//             as well as IONetsOnly flag
// 990116 s.m  added route information for saveDEF to save routing information
// 990211 s.m  added DB::locatePin()

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include  <iostream>
#include  <iomanip>
#include  <fstream>
#include  <stdlib.h>
#include  <stdio.h>
#include  "ABKCommon/pathDelims.h"
#include  "ABKCommon/abkcommon.h"
#include  "Placement/xyDraw.h"
#include  "DB.h"
using namespace std;

#ifndef  ABKDEBUG
#define  ABKDEBUG
#endif
// DB::Parameters构造函数 - 初始化参数列表
DB::Parameters::Parameters():ignoreLogicalPins(true),
                             ignoreGlobalRoutingInfo(true),
                             ignoreDetailedRoutingInfo(true),
                             ignoreTimingLibs(true),
                             fixPlacedPads(true),
							 verbose("0 0 0"),
                             routedDesign(false),
                             alwaysCheckConsistency(false)
{}
// 成员函数定义 - 接收参数为.aux文件名称和初始化参数列表
// 判断文件类型并读取 支持LEFDEF, LEFDEFLGC, LEFDEFq, LEFDEFqLGC几种文件
DB::DB(const char *auxFileName,Parameters params):_params(params)
{   
    // 时间
    Timer tm;
    // 无法构造数据，.aux文件未找到
   abkfatal(auxFileName!=NULL,"Can\'t construct DB: NULL aux file name passed");
    // 读取.aux文件
    ifstream auxFile(auxFileName);
    // 无法读取输出提示信息
    abkfatal(auxFile,"Can\'t open aux file to construct DB");
    
    timing = NULL;
    
    strcpy(designName,"");
    strcpy(technoName,"");  
    strcpy(origFileName,auxFileName);
    strcpy(origFileFormat,"No database format found"); // initial value
    delimiter=' ';                   // to be changed by LEFDEF reader etc
    
    cout << "Reading " << auxFileName << endl; // 输出文件读取信息
    // 处理读取的文件
    char word1[100],word2[100],word3[100];
    char tmpw1[255],tmpw2[255],tmpw3[255];
    char * LGCFileName=NULL;
    bool found=false; int i=0;
//  char dir[255];
    const char *auxFileDirEnd=strrchr(auxFileName,pathDelim);
    if (auxFileDirEnd)
    { 
       strncpy(dirName,auxFileName,auxFileDirEnd-auxFileName+1);
       dirName[auxFileDirEnd-auxFileName+1]=0; 
    }
    else strcpy(dirName,"");
    
    while (!found && !auxFile.eof()) 
    {
        int lineNo=1;
        auxFile >> eathash(lineNo) >> word1
                >> noeol(lineNo) >> word2 >> noeol(lineNo);
        abkfatal(!strcmp(word2,":"),
            " Error in aux file: space-separated column expected");
        if (!abk_strcasecmp(word1,"CD")) // 原strcasecmp改为abk_strcasecmp,定义在abkstring.cxx中
        {
            auxFile >> word1;
            auxFile >> needeol(lineNo++);
            if (word1[0]==pathDelimWindows || word1[0] == pathDelimUnix)
                strcpy(dirName,word1);
            else strcat(dirName,word1);
            char     fDel[2];  
            sprintf(fDel,"%c",pathDelim);
            if (word1[strlen(word1)-1]!=pathDelimWindows 
                || word1[0] == pathDelimUnix)
                strcat(dirName,fDel);
        }
        else if (!abk_strcasecmp(word1,"LEFDEFq"))
        {
            cout << " Entering LEFDEFq parser based on Cadence code " << endl;
            found=true; 
            auxFile >> word1 >> noeol(lineNo) >> word2;
            auxFile >> needeol(lineNo++);
            if (word1[0]==pathDelimWindows || word1[0] == pathDelimUnix)
                strcpy(tmpw1,word1);
            else { strcpy(tmpw1,dirName), strcat(tmpw1,word1); }
            if (word2[0]==pathDelimWindows || word1[0] == pathDelimUnix) 
                strcpy(tmpw2,word2);
            else { strcpy(tmpw2,dirName),strcat(tmpw2,word2);  }
            // 读取文件内容
            readLEFDEFq(_abk_cpd(tmpw1),_abk_cpd(tmpw2));
            strcpy(origFileFormat,"LEFDEFq");
            cout << "LEF/DEF q pair read successfully" << endl;
         }
        else if (!abk_strcasecmp(word1,"LEFDEF"))
        {
//		abkfatal(0, "readLEFDEF not currently supported");
            found=true; 
            auxFile >> word1 >> noeol(lineNo) >> word2;
            auxFile >> needeol(lineNo++);
            if (word1[0]==pathDelimWindows || word1[0] == pathDelimUnix)
                strcpy(tmpw1,word1);
            else { strcpy(tmpw1,dirName), strcat(tmpw1,word1); }
            if (word2[0]==pathDelimWindows || word1[0] == pathDelimUnix) 
                strcpy(tmpw2,word2);
            else { strcpy(tmpw2,dirName),strcat(tmpw2,word2);  }
            // 读取文件内容
            readLEFDEF(_abk_cpd(tmpw1),_abk_cpd(tmpw2)); 
            strcpy(origFileFormat,"LEFDEF");
            cout << "LEF/DEF pair read successfully" << endl;

         }
         else if (!abk_strcasecmp(word1,"LEFDEFLGC"))
         {
//		abkfatal(0, "readLEFDEF not currently supported");
            found=true; 
            auxFile >> word1 >> noeol(lineNo) >> word2 
                    >> noeol(lineNo) >> word3 ;
            auxFile >> needeol(lineNo++); 
            if (word1[0]==pathDelimWindows || word1[0] == pathDelimUnix) 
                strcpy(tmpw1,word1);
            else { strcpy(tmpw1,dirName), strcat(tmpw1,word1); }
            if (word2[0]==pathDelimWindows || word1[0] == pathDelimUnix) 
                strcpy(tmpw2,word2);
            else { strcpy(tmpw2,dirName),strcat(tmpw2,word2);  }
            if (word3[0]==pathDelimWindows || word1[0] == pathDelimUnix) 
                strcpy(tmpw3,word3);
            else { strcpy(tmpw3,dirName),strcat(tmpw3,word3);  }
            readLEFDEF(_abk_cpd(tmpw1),_abk_cpd(tmpw2));
            LGCFileName=_abk_cpd(tmpw3);
            strcpy(origFileFormat,"LEFDEFLGC");
            cout << "LEF/DEF pair read successfully "
                 << "(LGC file passed to postprocessor)" << endl;
         }
         else if (!abk_strcasecmp(word1,"LEFDEFqLGC"))
         {
            found=true; 
            auxFile >> word1 >> noeol(lineNo) >> word2 >> noeol(lineNo)>> word3;
            auxFile >> needeol(lineNo++);
            if (word1[0]==pathDelimWindows || word1[0] == pathDelimUnix) 
                strcpy(tmpw1,word1);
            else { strcpy(tmpw1,dirName), strcat(tmpw1,word1); }
            if (word2[0]==pathDelimWindows || word1[0] == pathDelimUnix) 
                strcpy(tmpw2,word2);
            else { strcpy(tmpw2,dirName),strcat(tmpw2,word2);  }
            if (word3[0]==pathDelimWindows || word1[0] == pathDelimUnix) 
                strcpy(tmpw3,word3);
            else { strcpy(tmpw3,dirName),strcat(tmpw3,word3);  }
            readLEFDEFq(_abk_cpd(tmpw1),_abk_cpd(tmpw2));
            LGCFileName=_abk_cpd(tmpw3);
            strcpy(origFileFormat,"LEFDEFLGC");
            cout << "LEF/DEF q pair read successfully "
                 << "(LGC file passed to postprocessor)" << endl;
         }
        auxFile >> skiptoeol;
        if (found)
        while ( ! auxFile.eof() && allFileNamesInAUX.size() < 20)
        {
            auxFile >> word1;
            allFileNamesInAUX.push_back(strdup(word1));
        }
        if ( i++==5) abkfatal(0,"Done"); 
   }
   abkfatal(found, "No supported database input format found\n\
   (currently LEFDEF, LEFDEFLGC, LEFDEFq, LEFDEFqLGC)");
   postProcessor(LGCFileName); 
   Timer tm1;
#ifdef ABKDEBUG
   abkfatal(isConsistent(),"Database constructor failed: DB object corrupted");
#else
   if (_params.alwaysCheckConsistency)
   {
       abkwarn(isConsistent(),"Database constructor failed: DB object corrupted");
   }
#endif
   tm1.stop();
   tm.stop();
   cout << "Consistency check took " << tm1 << endl;
   cout << "Database setup took " << tm << endl;
}
// 存储文件函数
void DB::saveDEF(const char *defFileName,bool bSaveUnPlacedLocs,
                 bool bSaveRoutingInfo) const
{
    int cellIdx;
    int nExtPins=0;

    cout << " Opening " << defFileName << " ... ";
    ofstream os(defFileName); //

    abkfatal(os," Unable to open DEF file for writing");
    cout << " Ok " << endl;

    itCellGlobal iC;

    for (iC=design.cellsBegin();iC!=design.cellsEnd();iC++)
        if ((*iC)->getMaster().isExtPin()) nExtPins++;

    bit_vector cellIsFixed;
    if (spatial.constraints.isEmpty())
      cellIsFixed=bit_vector(spatial.locations.getSize(),false); 
    else
      spatial.constraints.getFGCbitVec(cellIsFixed);

    os << endl;
    os << TimeStamp();
    os << Platform();
    os << User();
    os << MemUsage();


    os << "DESIGN "<< (hasDesignName() ? getDesignName() : "NO_DESIGN_NAME")
       << " ;" << endl << endl;
 
    os << "UNITS DISTANCE MICRONS " << context.getUnits() << " ;" <<endl<<endl;
    
    os << "COMPONENTS " << design.getNumCells() - nExtPins << " ;" << endl;

    for (iC=design.cellsBegin();iC!=design.cellsEnd();iC++)
    {
        typedef dbCell const &rDbCell;
        rDbCell cell(*(*iC));
        cellIdx = cell.getIndex();
        if (!cell.getMaster().isExtPin())
        {
            os << "  - " << setw(10) << cell.getName() << " "
               << setw(10) << cell.getMaster().getName() << " ";

            if (spatial.locationValid[cellIdx])
                os << " + " << setw(11) 
                   << ( cellIsFixed[cellIdx] ? "FIXED ( " : "PLACED ( ")
		   << static_cast <long long int>(spatial.locations[cellIdx].x)
		   <<"  "
		   <<static_cast <long long int> (spatial.locations[cellIdx].y)
                   << " ) "<< spatial.orientations[cellIdx] << " ";

            else if (bSaveUnPlacedLocs)
                os << " + " << setw(11)
                   << "UNPLACED ( " 
		   << static_cast <long long int>(spatial.locations[cellIdx].x)
		   <<"  "
		   <<static_cast <long long int> (spatial.locations[cellIdx].y)
		   << " ) "<<  spatial.orientations[cellIdx] << " ";

            os << ";" << endl;
        }
    }
    os << "END COMPONENTS " << endl << endl;
    os << "NETS " << design.getNumNets() << " ;" << endl;

    unsigned colNum = 0;
    itNetGlobal iN;
    itPinLocal iP;
    bool existExtPins=false;

    if(routedNets.isRouted() && bSaveRoutingInfo)
    {        // if the net is routed then print the routed segments
        for (itRoutedNet iRN=routedNets.routedNetsBegin();
             iRN != routedNets.routedNetsEnd(); iRN++)
        {
            unsigned rnetIdx = (*iRN)->getNetIdx();
            if((*iRN)->isInDB())
            {
                dbNet const &net=design.getNetByIdx(rnetIdx);
                char const *netName = net.getName();
                abkfatal(!abk_strcasecmp(netName, (*iRN)->getName()), 
                         "net name does not match");
                os << "  - " << netName << " ";

                for (iP=net.pinsBegin();iP!=net.pinsEnd();iP++)
                {
                    dbPin const &pin = *(*iP);
                    dbCell const &cell = pin.getCell();
                    char const *pinName = pin.getMasterPinName();
                    char const *cellName = cell.getName();

                    if (cell.getMaster().isExtPin())
                    {
                        existExtPins=true;
                        continue;
                    }
                    os << "\n    ( " << cellName << " " << pinName << " ) ";
                }
                if((*iRN)->isRouted())
                {
                    os << "\n    + " << (*iRN)->getType() << " ";
                    double cX, cY, nX, nY;     // current and next (X,Y)
                    unsigned cL;               // current layer
                    itRoutingElement iRE = (*iRN)->elementsBegin();
                    bool afterVia = false;
//                  bool newElement = true;
                    bool firstElement = true;
                    cX = cY = DBL_MAX;
                    cL = UINT_MAX;
                    for(; iRE != (*iRN)->elementsEnd(); iRE++)
                    {
                        if(iRE->isVia())
                        {
                            os << "\n      ";
                            if (cX != iRE->x || cY != iRE->y || 
                                cL != iRE->getLayer1() &&
                                cL != iRE->getLayer2())
                            {
                                cX = iRE->x;
                                cY = iRE->y;
                                cL = iRE->getLayer1();
                                if (firstElement) firstElement = false;
                                else os << "NEW ";
                                os << (**(context.beginLayers()+cL)).getName()
                                   << " ( " << cX << " " << cY << " ) \n      ";
                            }
                            os << routedNets.getMasterViaByIdx(
                                  iRE->getMasterViaIndex()).getName() << " ";
                            afterVia = true;
                            if (cL == iRE->getLayer2()) cL = iRE->getLayer1();
                            else cL = iRE->getLayer2();
                        }
                        else
                        {
                            if (cX != iRE->x || cY != iRE->y || 
                                cL != iRE->getLayer1() &&
                                cL != iRE->getLayer2())
                            {
                                cX = iRE->x;
                                cY = iRE->y;
                                cL = iRE->getLayer1();
                                os << "\n      ";
                                if (firstElement) firstElement = false;
                                else os << "NEW ";
                                os <<(**(context.beginLayers()+cL)).getName()
                                   << " ( " << cX << " " << cY << " ) ";
                                afterVia = false;
                            }
                            nX = iRE->getXEnd();
                            nY = iRE->getYEnd();
                            if (afterVia) 
                            { afterVia = false; os << "\n      "; }
                            os << "( ";
                            if (nX == cX) os << "* ";
                            else os << nX << " ";
                            if (nY == cY) os << "* ) ";
                            else os << nY << " ) ";
                            cX = nX;
                            cY = nY;
                        }
                    }
                }
                os << ";" << endl;
            }
        }
    }
    else
    {        // otherwise just print the netlist
        for (iN=design.netsBegin();iN!=design.netsEnd();iN++)
        {
            dbNet const &net=*(*iN);
            char const *netName = net.getName();
            os << "- " << netName << " " ;
            colNum = 3+strlen(netName);
        
            for (iP=net.pinsBegin();iP!=net.pinsEnd();iP++)
            {
                dbPin const &pin = *(*iP);
                dbCell const &cell = pin.getCell();
                char const *pinName = pin.getMasterPinName();
                char const *cellName = cell.getName();

                if (cell.getMaster().isExtPin())
                {
                    existExtPins=true;
                    continue;
                }
                colNum += strlen(pinName)+strlen(cellName)+6;
                if (colNum > 75)
                {
                    os << endl << "    ";
                    colNum=4 + strlen(pinName)+strlen(cellName)+6;
                }

                os << "( " << cellName << " " << pinName << " ) ";

            }
            os << ";" << endl;

        }
    }

    os << "END NETS \n\n END DESIGN " << endl;
}

bool  DB::isConsistent() const
{   
    bool IamFine, tmp;
    unsigned i;

//  check individual sections
    //IamFine =  temporal.isConsistent() && spatial.isConsistent()
    IamFine =  spatial.isConsistent()
        && context.isConsistent()  && design.isConsistent()
	&& easyAccess.isConsistent(context, design);

// now perform unified checks (more to come)
   
    tmp     =  (spatial.getSize() == design.getNumCells());
    abkwarn(tmp, "Spatial and Design report difft numbers of cells");
    IamFine =  IamFine && tmp;

    BBox  layoutBBox(context.getBBox());

    bit_vector cellIsFixed;
    if (spatial.constraints.isEmpty())
      cellIsFixed=bit_vector(spatial.locations.getSize(),false);
    else
      spatial.constraints.getFGCbitVec(cellIsFixed);

    bool bboxViolated = false;
    bool fixedLocInvalid = false;

    for (tmp = true, i=0; i < spatial.getSize(); i++)
    { 
        if  (spatial.locationValid[i] 
             && !layoutBBox.contains(spatial.locations[i]))
        { 
           bboxViolated = true;
           break;
        }

        if  (cellIsFixed[i] && !spatial.locationValid[i])
        {
          fixedLocInvalid = true;
          break;
        }
    }

    abkwarn(!bboxViolated,
             " spatial.locations has points beyond context.getBBox()\n");
    if(bboxViolated)
      {
	cerr<<" user can increase the size of the DIEAREA in DEF"<<endl;
      }
    abkwarn(!fixedLocInvalid,
           " spatial.locations has fixed points with invalid location\n");

    //liberalized 16 jan 00 mro
    //liberalized more by sadya
    IamFine = IamFine; //&& ! bboxViolated ;//&& ! fixedLocInvalid;

    cout <<  spatial.constraints;

    if (! spatial.constraints.allSatisfied())
        cout << "   *WARNING* spatial.constraints are not satisfied " << endl;

    tmp = spatial.constraints.inBBox(layoutBBox);
    abkwarn(tmp, " spatial.constraints spill beyond context.getBBox()\n");

    //liberalized 16 jan 00 mro
    //IamFine = IamFine && tmp;
 
    return IamFine;
}

const char *DB::getAuxName()        const
{
   const char *left=strrchr(origFileName,pathDelim);
   if (left==NULL) left=origFileName;
   else            left++;;


   const char *right=strrchr(left,'.');
   if (right==NULL) return left;

   static char buf[256];
   strncpy(buf,left,right-left);
   if (right-left==0) strcpy(buf,"noname");
   return buf;
}

const DB& operator>>(const DB& db,Placement& pl)
{  
  pl = db.spatial.locations;
  return db;
}

const DB& operator>>(const DB& db, vector<Orient>& ori)
{ 
  ori = db.spatial.orientations; 
  return db;
}

DB& operator<<(DB& db, const Placement& pl)
{ 
  db.spatial.locations=pl; 
  std::fill(db.spatial.locationValid.begin(),db.spatial.locationValid.end(),true);
  return db;
}

DB& operator<<(DB& db, const vector<Orient>& ori)
{ 
  db.spatial.orientations=ori; 
  return db;
}

double DB::evalHPWLcost(unsigned threshold, bool IONetsOnly) const
{
    abkfatal(spatial.allCellsPlaced(),
      "Can't compute WL cost, some cells unplaced");
    return 
     design.evalHalfPerimCost(spatial.locations,
                              spatial.orientations,threshold, IONetsOnly);
}

double DB::evalWHPWLcost(unsigned threshold, bool IONetsOnly) const
{
    abkfatal(spatial.allCellsPlaced(),
      "Can't compute WL cost, some cells unplaced");
    return 
    design.evalWHalfPerimCost(spatial.locations,
                              spatial.orientations,threshold, IONetsOnly);
}

void DB::saveXYLineDrawing(const char *xyFileName) const
{
    abkfatal(spatial.allCellsPlaced(),
      "Can't save XY line drawing because some cells are not placed");

    cout << " Opening " << xyFileName << " ... ";
    ofstream xystream(xyFileName); //,ios::out);
    abkfatal(xystream," Unable to open DEF file for writing");
    cout << " Ok " << endl;

    for(unsigned k=0;k<design.getNumCells();k++)
    {
        const dbCell& cell = design.getCellByIdx(k);
        Rectangle 
            cellOutline=Rectangle(0, 0, cell.getWidth(), cell.getHeight());
        if (cellOutline.isEmpty())
        {
           abkwarn(0,"Drawing a cell with empty outline");
           continue;
        }
        cellOutline.TranslateBy(spatial.locations[k]);
        xyDrawRectangle(xystream,cellOutline);
        xystream << endl;
    }
    BBox   layoutBBox=context.getBBox();
    Point  center=layoutBBox.getGeomCenter();
    double dx=0.525*(layoutBBox.xMax-layoutBBox.xMin);
    double dy=0.525*(layoutBBox.yMax-layoutBBox.yMin);

    xystream << center+Point( dx, dy) << endl << endl
             << center+Point(-dx, dy) << endl << endl
             << center+Point(-dx,-dy) << endl << endl
             << center+Point( dx,-dy) << endl << endl;
}

Point DB::locatePin(const dbPin& pin, 
                    const Placement& placement,
                    const vector<Orient>& orientations) const
{
    const dbCell& cell = pin.getCell();
    unsigned cellIdx = cell.getIndex();
    double cellH = cell.getHeight(), cellW = cell.getWidth();
    Point cellLoc = placement[cellIdx];
    Point pinRelLoc = pin.getMaster().getCenter();
    double x, y;
    const Orientation& ori = orientations[cellIdx];
    switch(ori.getAngle())
    {
        case 0:
            y = cellLoc.y + pinRelLoc.y;
            if (ori.isFaceUp())
                x = cellLoc.x + pinRelLoc.x;
            else
                x = cellLoc.x + cellW - pinRelLoc.x;
            break;
        case 90:
            x = cellLoc.x + pinRelLoc.y;
            if (ori.isFaceUp())
                y = cellLoc.y + cellW - pinRelLoc.x;
            else
                y = cellLoc.y + pinRelLoc.x;
            break;
        case 180:
            y = cellLoc.y + cellH - pinRelLoc.y;
            if (ori.isFaceUp())
                x = cellLoc.x + cellW - pinRelLoc.x;
            else
                x = cellLoc.x + pinRelLoc.x;
            break;
        case 270:
            x = cellLoc.x + cellH - pinRelLoc.y;
            if (ori.isFaceUp())
                y = cellLoc.y + pinRelLoc.x;
            else
                y = cellLoc.y + cellW - pinRelLoc.x;
            break;
    }
    //abkfatal(x >= cellLoc.x && y >= cellLoc.y, "invalid pin coordinate");
    //990405 sm commented out because for external pins, the actual pin
    //          can be outside the cell bbox (example: SAMSUNG_BM)
    return Point(x,y);
}

void DB::setPlaceAndOrient(const PlacementWOrient& plWOri)
{
    spatial.locations = static_cast<Placement>(plWOri);
    for(unsigned i = 0; i < plWOri.size(); i++)
    {
	spatial.orientations[i]  = plWOri.getOrient(i);
	spatial.locationValid[i] = true;
    }
}

void DB::parseDEFforGCell(const char * DEFFileName)
{
  char word1[700], word2[100];
  double num1, num2;
  int lineNo = 1;
  cout << "Opening " << DEFFileName << " for GCell\n";
  ifstream defFile(DEFFileName);
  abkfatal (defFile, "Can\'t open DEF file");
  while(!defFile.eof())
  {
    defFile >> eathash(lineNo) >> word1;
    if(!abk_strcasecmp(word1,"GCELLGRID"))
    {
        bool horiz;
        unsigned numDivs;
        defFile >> noeol(lineNo) >> word2;
        if(!abk_strcasecmp(word2,"Y")) horiz = false;
        else if(!abk_strcasecmp(word2,"X")) horiz = true;
        else abkfatal(0, "Invalid direction for GCELLGRID");
        defFile >> noeol(lineNo) >> num1 >> noeol(lineNo) >> word1
                >> noeol(lineNo) >> numDivs >> noeol(lineNo) >> word2
                >> noeol(lineNo) >> num2 >> skiptoeol;
        lineNo++;
        context.addGCellGrid(horiz, num1, numDivs, num2);
    }
    else if(!abk_strcasecmp(word1,"COMPONENTS")) break;
    else
    {
      defFile >> skiptoeol;
      lineNo++;
      defFile >> eathash(lineNo);
    }
  }
  defFile.close();
  cout << "DEF file " << DEFFileName << " parsed\n";
}

DB::~DB()
{
   unsigned k;
   for(k=0; k!=allFileNamesInAUX.size(); k++) delete [] allFileNamesInAUX[k];
}

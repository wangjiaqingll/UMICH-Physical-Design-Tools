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








//! author=" Igor Markov July 17, 1997" 
//
//   CHANGES
//
// 970812 aec  added includes for intervals and easy access.
// 970814 mro  changed declaration for saveDEF.
// 970820 mro  added flag to parseLEFDEF to include logical pins in netlist
// 970824 ilm  overloaded operator<< and operator>> for DB
// 970104 ilm  added DB::Parameters
// 970924 ilm  added DB::getAuxName()
// 971130 ilm  added DB::evalHPWLcost() and DB::evalWHPWLcost()
//                    they assume valid placement in DB (e.g. do DB<<pl).
//                    Note that if, for speed, you don't need to check for
//                    valid placement every time (slow), you need to use
//                    dbNetlist::evalHalfPerimCost() and
//                    dbNetlist::evalWHalfPerimCost() directly.
//                     The new additions are goof for slow benchmarking only.
// 971130 ilm  added DB::saveXYLineDrawing()
//             to view the resulting "xy" file in gnuplot plot type 
//                 plot "filename.xy" with lines
//               to produce a PS file (e.g. if you don't have an X terminal)
//               type
//                 set terminal postscript
//                 set output   "myfile.ps"
//               before issuing the plot command
// 971214 ilm  added DB::getNetlistForSurgery()
// 980121 ilm  added optional thresholding to WL estimators
//             as well as IONetsOnly flag
// 990112 s.m  added DB::Parameters::routedDesign, DB::routedNets
// 990211 s.m  added DB::locatePin()

#ifndef _DB_H
#define _DB_H

#include "string.h"

#include "dbTypes.h"
#include "dbContext.h"
#include "dbDesign.h"
#include "dbSpatial.h"
#include "dbEasyAccess.h"
#include "dbRoutingInfo.h"
#include "Placement/placeWOri.h"

//: ABKGOURP/UCLA Database parser 数据库解析器
class DB
{
public:

    class Parameters
    {
      public:
          bool  ignoreLogicalPins;            // 忽略逻辑引脚
          bool  ignoreGlobalRoutingInfo;      // 忽略全局布线信息
          bool  ignoreDetailedRoutingInfo;    // 忽略详细布线信息
          bool  ignoreTimingLibs;             // 忽略时间库？
          bool  fixPlacedPads;                // 修复放置垫
          Verbosity  verbose;                 // 错误诊断
          bool  routedDesign;                 // 设计已布线？  
          bool  alwaysCheckConsistency;       // 始终检查一致性
          unsigned placeHolder2;              // 放置座1，2...
          unsigned placeHolder3;
          double placeHolder4;
          double placeHolder5;

      Parameters();                           // 函数声明
    };

private:
    MakeFriendsWithParsers
 
    Parameters _params;

    char origFileFormat[30];
    char origFileName[255];
    char dirName  [1023];
    vector<char*>  allFileNamesInAUX;
    char designName[255];
    char technoName[255];
    char delimiter;             // no code to set delimiter yet

    void  postProcessor(const char * LGCFileName=NULL);
                           //always called after parsing in every DB
                           //constructor, before consistency check
  
    void      readLEFDEF(const char * LEFFileName, const char * DEFFileName);

    void      parseLEFDEF(const char * LEFFileName,
                          const char * DEFFileName);
    void      parseDEFforGCell(const char * DEFFileName);
    void      readLEFDEFq(const char * LEFFileName, const char * DEFFileName);
    void      parseLEFDEFq(const char * LEFFileName,
                          const char * DEFFileName); // {}
    // merely checks that the files are valid and calls
#ifdef HAVE_Q_PARSER    
    // defined in the ParserLEFDEF package; does all the work
    // of populating the database
#endif
    
    /*void      readXXX(const char * XXX1Filename, const char * XXX2Filename);*/
    void      parseXXX(const char * XXX1Filename, const char * XXX2Filename);
    // sample API calls for parser of XXX [multi-]file format (see above)
    
    
    dbLayout    context;
    // "the four sections of DB" are context, design, spatial and temporal
    dbNetlist   design;
    
 public:   
        
        dbSpatial   	spatial;
	dbEasyAccess	easyAccess;
        dbRoutingInfo   routedNets;
        dbTemporal  	*timing;
        
        DB(const char *auxFileName, Parameters params=Parameters());
       ~DB();
        bool  isConsistent()            const;
        const char *getOrigFileName()   const { return origFileName;   }
        const char *getAuxName()        const; // derived from origFileName
        const char *getDirName()         const { return dirName;       }
        const vector<char*>& getFileNames() const { return allFileNamesInAUX; }
        const char *getOrigFileFormat() const { return origFileFormat; }
        char        getDelimiter()      const { return delimiter;      }
        
        const dbLayout&  getLayout()  const { return context; }
        const dbNetlist& getNetlist() const { return design;  }
              dbNetlist& getNetlistForSurgery() { return design;  }
        const dbTemporal &getTiming() const { return *timing; }

        void saveDEF(const char *defFileName,
                     bool bSaveUnPlacedLocs=false,
                     bool bSaveRoutingInfo=true) const;
        void saveXYLineDrawing(const char *xyFileName) const;

        bool hasDesignName() const { return strlen(designName)>0; }
        bool hasTechnoName() const { return strlen(technoName)>0; }

        const char* getDesignName() const { return designName; }
        const char* getTechnoName() const { return technoName; }

        double evalHPWLcost(unsigned threshold=0, bool IONetsOnly=false) const;
        // Half-Perimeter wirelength cost estimate (0 -- no thresholding)
        double evalWHPWLcost(unsigned threshold=0, bool IONetsOnly=false) const;
        // same, but net costs are weighted

        void printNetStats(ostream& out) const  { design.printNetStats(out); }
        void printNetStats()             const  { printNetStats(cout); }

        Point locatePin(const dbPin& pin, const Placement& placement,
                        const vector<Orient>& orientations) const;
        // These two methods are for calculating the actual location
        // of a given pin in the layout
        Point locatePin(const dbPin& pin) const
        { return locatePin(pin, spatial.locations, spatial.orientations); }
        Point locatePin(const dbPin& pin,
                        const vector<Orient>& orientations) const
        { return locatePin(pin, spatial.locations, orientations); }

	void setPlaceAndOrient(const PlacementWOrient& plWOri);
        
};

const DB& operator>>(const DB& db, Placement& pl);
const DB& operator>>(const DB& db, std::vector<Orient>& ori);
DB& operator<<(DB& db, const Placement& pl);
DB& operator<<(DB& db, const std::vector<Orient>& ori);

#endif


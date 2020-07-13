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
#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include  <fstream>
#include  <stdlib.h>
#include  <stdio.h>
#include  "ABKCommon/pathDelims.h"
#include  "ABKCommon/abkcommon.h"
#include  "DB.h"
using namespace std;
// 读取文件 - 此函数仅仅判断文件是否存在以及能否被打开，随即调用parse函数处理文件
// ****parse函数是重点
void DB::readLEFDEF
(const char * LEFFileName, const char * DEFFileName)
// merely checks that the files exist and can be opened, calls parse LEFDEF
{  
   abkfatal(LEFFileName!=NULL,"Can\'t construct DB: NULL LEF file name passed");
   abkfatal(DEFFileName!=NULL,"Can\'t construct DB: NULL DEF file name passed");
   const char *lefExt=LEFFileName+strlen(LEFFileName)-4; // 提取文件后缀
   // 比较文件后缀
   abkfatal3(strcmp(lefExt,".lef")==0 || strcmp(lefExt,".LEF")==0,
       "LEF file expected instead of ",LEFFileName,"\n");
   const char *defExt=DEFFileName+strlen(DEFFileName)-4;
   abkfatal3(strcmp(defExt,".def")==0 || strcmp(defExt,".DEF")==0,
       "DEF file expected instead of ",DEFFileName,"\n");
    cout << "Opening " << LEFFileName << endl;
    ifstream leff(LEFFileName);
    abkfatal(leff,"Can\'t open LEF file");
    
    cout << "Opening " << DEFFileName << endl;
    ifstream deff(DEFFileName);
    abkfatal(deff,"Can\'t open DEF file");
    
    parseLEFDEF(LEFFileName,DEFFileName); // does all the work
//  parseLEFDEF(LEFFileName,DEFFileName,!_params.ignoreLogicalPins);
    parseDEFforGCell(DEFFileName);    // extra DEF parsing for GCELL

    allFileNamesInAUX.push_back(strdup(LEFFileName));
    allFileNamesInAUX.push_back(strdup(DEFFileName));
}


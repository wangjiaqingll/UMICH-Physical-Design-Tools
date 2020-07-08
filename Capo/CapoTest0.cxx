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
// argc = argument count,表示传入main函数的数组元素的个数，int 类型
// argv = argument vector, 表示传入main函数的指针数组， char**类型
// 第一个数组元素argv[0]是程序名称，包含程序所在完整路径
// 两个参数通过终端输入获得
int main(int argc, const char *argv[])
{ // 声明几个类型类，以便接收处理终端信息
  NoParams         noParams       (argc,argv);
  BoolParam        helpRequest    ("help",argc,argv); // help - key 
  BoolParam        helpRequest1   ("h",   argc,argv);
  BoolParam        saveDef        ("save",argc,argv);
  BoolParam        saveOrigDef    ("saveOrig",argc,argv);
// BoolParam        doCellFlipping ("flip",argc,argv);
// BoolParam        doCellSpacing  ("cellSpace",argc,argv);

  Verbosity	   verb(argc, argv); // 诊断
  // 打印出版本信息
  PRINT_VERSION_INFO
  cout << CmdLine(argc,argv); // 输出终端

// ------------- Process parameters before getting bogged down in computations
  // 利用CapoParameters类接收终端命令
  CapoParameters capoParams(argc,argv);
  // 如果终端没有输入参数或者接收到帮助请求，输出如下信息
  if (noParams.found() || helpRequest.found() || helpRequest1.found())
  {
     cout << "  Use '-help' or '-f filename.aux' " << endl;
     cout << "  Other options: \n"
          << "   -save          (saves output into out.def)\n"
          << "   -saveOrig      (saves input  in orig.def) \n"
          << endl;
     return 0;
  }
  // 输出capoParams参数信息
  cout << capoParams << endl;
  // 处理输入数据 - 接收处理文件名信息
  StringParam 	 auxFileName 	("f", 	argc,argv);
  // 如果未找到.aux文件，执行以下函数输出错误信息
  abkfatal(auxFileName.found(),"Usage: prog -f filename.aux <more params>"); // 错误警告函数

  // 检查数据内容 - checkDB命令在哪里声明的，没有找到。 返回_b或者_on
  BoolParam check("checkDB",argc,argv);
  DB::Parameters dbparams; // 声明一个Parameters类 - 嵌套于DB中
  dbparams.alwaysCheckConsistency=check; // _b or _on
  // 文件数据读取处理 - 输入参数为.aux文件以及Parameters参数列表
  // 最后将文件读取进内存？或者以什么方式存放？
  DB db(auxFileName,dbparams);
  // 如果接收到命令，存储文件为orig.def
  if(saveOrigDef.found())
	db.saveDEF("orig.def");
  // 输出设计名称
  cerr<<"Design Name: "<<db.getDesignName()<<endl;

  // 主要是设置了三个参数 
  // remCongestion --> 移除拥塞？ 
  // numRowsToRemove --> 移除的行数量
  // spaceCellsAlg --> ? (是一个枚举)
  RBPlaceParams rbParam(argc,argv); // 处理参数 - 返回什么
  RBPlaceFromDB rbplace(db, rbParam); // 一个参数是处理过后文件信息，一个是设置的一些参数 

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
/*

  EdgePlEval eval3 (rbplace.getHGraph(),capo.getPlacement(),3);
  EdgePlEval eval5 (rbplace.getHGraph(),capo.getPlacement(),5);
  EdgePlEval eval10(rbplace.getHGraph(),capo.getPlacement(),10);
  EdgePlEval eval15(rbplace.getHGraph(),capo.getPlacement(),15);
  EdgePlEval eval20(rbplace.getHGraph(),capo.getPlacement(),20);
  EdgePlEval eval30(rbplace.getHGraph(),capo.getPlacement(),30);
  cout << "Eval3  " << eval3;
  cout << "Eval5  " << eval5;
  cout << "Eval10 " << eval10;
  cout << "Eval15 " << eval15;
  cout << "Eval20 " << eval20;
  cout << "Eval30 " << eval30;
*/

  if (saveDef.found()) db.saveDEF("out.def");

  return 0;
}

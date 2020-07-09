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
#include "capoPlacer.h"
#include "RBPlace/RBPlacement.h"
//#include "greedyOrientOpt.h"
//#include "rowdp.h"
//#include "rownet.h"
//#include "netopt.h"

int main(int argc, const char *argv[])
{
  // 创建几个类对象，以便接收终端信息
  NoParams         noParams       (argc,argv);
  BoolParam        helpRequest    ("help",argc,argv);
  BoolParam        helpRequest1   ("h",   argc,argv);
  StringParam      savePl         ("savePl",argc,argv);
// BoolParam        doCellFlipping ("flip",argc,argv);
//  BoolParam        doCellSpacing  ("cellSpace",argc,argv);

  Verbosity	   verb(argc, argv); // 诊断？

  cout<<"\n(c) copyright, 1998-1999 Regents of the University of California\n";
  cout<<" Authors: Andrew E. Caldwell, Andrew B. Kahng, Igor L. Markov\n";
  cout<<" Email:   {caldwell,abk,imarkov}@cs.ucla.edu"<<endl<<endl;
  PRINT_VERSION_INFO // 打印版本信息
  cout << CmdLine(argc,argv); // 命令输入提示 --> 终端

// ------------- Process parameters before getting bogged down in computations
  // 创建CapoParameters类接收终端命令 --> 根据命令设置一些系统参数
  CapoParameters capoParams(argc,argv);
  // 如果终端没有输入参数或者接收到帮助请求时，输出如下帮助信息
  if (noParams.found() || helpRequest.found() || helpRequest1.found())
  {
     cout << "  Use '-help' or '-f filename.aux' " << endl;
     cout << "  To save output, use -savePl filename " << endl;
     return 0;
  }
  // 输出系统参数设置信息
  cout << capoParams << endl;
  // 创建StringParam类接收文件名参数
  StringParam 	 auxFileName 	("f", 	argc,argv);
  // 如果未找到.aux文件名信息，执行abkfatal函数输出提示信息
  abkfatal(auxFileName.found(),"Usage: prog -f filename.aux <more params>");
  // 接收终端信息设置三个系统参数
  // remCongestion --> 移除拥塞?
  // numRowsToRemove --> 移除行的数量？
  // spaceCellsAlg --> 是什么，三个值可选:EQUAL, PIN1, PIN2
  RBPlaceParams rbParam(argc,argv);
  // 输出系统参数
  cout << rbParam <<endl;
  // 
  RBPlacement rbplace(auxFileName, rbParam);   

  Timer capoTimer;
  CapoPlacer capo(rbplace, capoParams);
  capoTimer.stop();

  cout << "After Capo  "<<endl;
  cout << "  Capo Runtime: "<<capoTimer.getUserTime()<<endl;

  cout << "  RBPl Est HPWL: "<<rbplace.evalHPWL(false)<<endl;;
  cout << "  RBPl Est Pin-to-Pin HPWL: "<<rbplace.evalHPWL(true)<<endl;;
  cout << "  RBPl Est  WWL: "<<rbplace.evalWeightedWL(false)<<endl;;
  cout << "  RBPl Est Pin-to-Pin  WWL: "<<rbplace.evalWeightedWL(true)<<endl;;

  if(savePl.found())
  	rbplace.savePlacement(savePl);

  return 0;
}

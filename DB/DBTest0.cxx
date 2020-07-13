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








#ifdef _MSC_VER
#pragma warning (disable:4786)
#endif

#include "ABKCommon/abkcommon.h"
#include "DB.h"

int main(int argc, char *argv[])
{
  using std::cout;
  using std::endl;
  StringParam auxFileName("f",argc,argv);
  BoolParam check("checkDB",argc,argv);
  DB::Parameters params; // 创建对象 --> 初始化系统参数
  params.alwaysCheckConsistency=check; // 检查一致性，布尔值
  abkfatal(auxFileName.found(),"Usage: prog -f filename.aux"); // 判断文件是否存在
  DB db(auxFileName,params); // 创建对象，初始化数据
  cout << " Testcase : " << db.getAuxName() << endl;
  
  cout<<" Total CCs       "<< db.getNetlist().getNumCCs()       << endl;
  cout<<" Total SCs       "<< db.getNetlist().getNumSCs()       << endl;
  cout<<" Total CoreCells "<< db.getNetlist().getNumCoreCells() << endl;
  cout<<" Total AllPads   "<< db.getNetlist().getNumAllPads()   << endl;
  cout<<" Total IOPads    "<< db.getNetlist().getNumIOPads()    << endl;
  cout<<" Total IOPins    "<< db.getNetlist().getNumIOPins()    << endl;
  const Constraints &cons=db.spatial.constraints;
  if (cons.hasFGC())
  {
	cout<<" Total XY-fixed  "<< cons.getFGC().getSize()<<endl;
  }
  else
	  cout << " No fixed-group constraints" << endl;

  db.getNetlist().printNetStats();

//db.saveDEF("test.def",true);
  cout << MemUsage() << endl;
  return 0;
}

/*
void DB::parseLEFDEF(const char*,const char*,bool)
{ }
*/

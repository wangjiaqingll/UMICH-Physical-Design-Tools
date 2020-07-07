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







//! author="Igor Markov, June 15, 1997"

/*
 970622 ilm    Added conversions into double and char * for Param
 970622 ilm    Added type NOPARAM for no parameters.
 970723 ilm    Changed #include <bool.h> to #include <stl_config.h>
               for STL 2.01 compliance
 970820 ilm    moved enum ParamType inside class Param
               added new ctor for use with NOPARAM
               allowed for +option as well as -option
               added member bool on() to tell if it was +option
 970824 ilm    took off conversions in class Param
               defined classes NoParams, BoolParam, UnsignedParam etc
                  with unambiguous conversions and clever found()
 980313 ilm    fixed const-correctness
*/ 
/*此文件用于对终端获取的参数进行相应处理*/
#ifndef _PARAMPROC_H_
#define _PARAMPROC_H__

#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <limits.h>

//#ifndef _MSC_VER
///* Includes from STL for min/max/abs/bool/true/false/etc */
//#include <stl_config.h>
//#endif 

#include "abkstring.h"
#include "abkassert.h"

//:(base class) Catches a given parameter from the command line 
// 从终端获取参数
class Param            
{
// 公有成员为一个枚举类型              
public:
    enum Type { NOPARAM, BOOL, INT, UNSIGNED, DOUBLE, STRING };
    // NOPARAM means "empty command line"
    // BOOL means "no value: either found in command line or not"
// 私有成员
private:
    bool          _b;  // found
    bool          _on; 
    int           _i;
    unsigned      _u;
    double        _d;
    const char *  _s;
    Type          _pt;
    const char *  _key;
// 成员函数声明
public:
    // 构造函数
    Param(const char * keyy, Type part, int argc, const char * const argv[]);
    // 为何有两个？输入参数不一样
    Param(Type part, int argc, const char * const argv[]); // for NOPARAM only
   ~Param() {}; // 析构函数
   // found函数声明
    bool      found()       const; 
    // for any Param::Type, always need to check before anything else
    bool      on()          const;  
    // for any Param::Type; true if the option was invoked with +
    // 声明不同类型参数获取函数
    int       getInt()      const;
    unsigned  getUnsigned() const;
    double    getDouble()   const;
    const char* getString() const;

/*  operator  double()      const;  // deprecated : use below classes */
/*  operator  char* ()      const;  //              instead of Param */
};

//:Constructed from argc/argv, returns to true 
// if the command line had no parameters
// 如果终端名为没有输入信息，则用NoParams类处理，继承了Parm类的私有成员，即_b等
class NoParams  : private Param
{
public:
    // 构造函数声明 - 使用了基类Param的NoParam情况下的成员函数
     NoParams(int argc, const char * const argv[]):Param(Param::NOPARAM,argc,argv) {}
     bool found()    const { return Param::found(); }
     operator bool() const { return Param::found(); }
     Param::on;      // base class member access adjustment
};

//: Catches a given boolean parameter
// 获取一个布尔型参数 - 私有继承Param
class BoolParam : private Param
{
public:
    // 构造函数声明 - 使用了基类Param的有参数情况下的成员函数 -->下同
    BoolParam(const char * key, int argc, const char * const argv[]) 
    : Param(key,Param::BOOL,argc,argv) {}
    bool found() const    { return Param::found(); }
    operator bool() const { return Param::found(); }
    Param::on;      // base class member access adjustment
};

//: Catches a given Unsigned parameter
class UnsignedParam : private Param
{
public:
    UnsignedParam(const char * key, int argc, const char *const argv[])
    : Param(key,Param::UNSIGNED,argc,argv) {}
    bool found() const { return Param::found() && getUnsigned()!=unsigned(-1); }
    operator unsigned() const { return getUnsigned();  }
    Param::on;     // base class member access adjustment
};

//: Catches a given integer parameter
class IntParam : private Param
{
public:
    IntParam(const char * key, int argc, const char * const argv[])
    : Param(key,Param::INT,argc,argv) {}
    bool found()   const { return Param::found();   }
    operator int() const { return getInt();  }
    Param::on;      // base class member access adjustment
};

//: Catches a given double parameter
class DoubleParam : private Param
{
public:
    DoubleParam(const char * key, int argc, const char * const argv[])
    : Param(key,Param::DOUBLE,argc,argv) {}
    bool found() const { return Param::found() && getDouble()!=-1.29384756657; }
    operator double() const { return getDouble();  }
    Param::on;      // base class member access adjustment
};

//: Catches a given string parameter
class StringParam : private Param
{
public:
    StringParam(const char * key, int argc, const char * const argv[])
    : Param(key,Param::STRING,argc,argv) {}
    bool found()     const       
          { return Param::found() && strcmp(getString(),"Uninitialized"); }
    operator const char*() const  { return getString();  }
    Param::on;      // base class member access adjustment
};

#endif

/**************************************************************************
***
*** Copyright (c) 1995-2000 Regents of the University of California,
***               Andrew E. Caldwell, Andrew B. Kahng and Igor L. Markov
*** Copyright (c) 2000-2010 Regents of the University of Michigan,
***               Saurabh N. Adya, Jarrod A. Roy, David A. Papa and
***               Igor L. Markov
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


#ifndef GCC_VERSION
#define GCC_VERSION (__GNUC__ * 10000 \
	+ __GNUC_MINOR__ * 100 \
	+ __GNUC_PATCHLEVEL__)
#endif


#include "sgi_stl_compat.h"

#ifndef _SGI_HASHSET_H
#define _SGI_HASHSET_H
#ifdef __GNUC__
 #if( __GNUC__ >= 3)
   #include <ext/hash_set>
   #if(GCC_VERSION >= 30100)
     using __gnu_cxx::hash;
     using __gnu_cxx::hash_set;
   #else
     using std::hash;
     using std::hash_set;
   #endif
 #else
   #include <hash_set>
 #endif
#else
  #ifdef _MSC_VER
    #include<hash_set>
    #ifndef _ONEHASH
    #define _ONEHASH
      struct lessstr
      {
        bool operator()(char const *s1,char const *s2) const
        {
          return (strcmp(s1, s2) == -1);
        }
      };
      template<class T> class hash : public stdext::hash_compare<T, std::less<T> > {};
      template <> class hash<const char *> : public stdext::hash_compare<const char *, lessstr> {};
    #endif
      template<class A, class B, class C> class hash_set : public stdext::hash_set<A,B> {};
  #else
    #include "ABKCommon/SGI_STL_COMPAT/hash_set.h"
  #endif

#endif
#endif

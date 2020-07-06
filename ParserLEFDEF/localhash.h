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





#ifndef _LOCALHASH_H
#define _LOCALHASH_H

#ifdef USHRT_MAX
#define MAX_HASH_VALUE USHRT_MAX
#else
#define MAX_HASH_VALUE 0xffff
#endif

#define HASH_MULT 259
#define SMALL_HASH_MODULUS 1000
#define REALLY_SMALL_HASH_MODULUS 16
extern int bCaseSens;

inline size_t MyHash(char const *string)
    {
    short unsigned int runval=0;
    char c;

    while ( (c = *string) )
        {
        runval += c;
        runval *= HASH_MULT;
        string++;
        }
    return runval;
    }

struct eqstr
    {
    bool operator()(char const *s1,char const *s2) const
        {
        return (strcmp(s1, s2) == 0);
        }
    };

struct localhash
    { size_t operator()(char const * s) const { return MyHash(s); } };
#endif

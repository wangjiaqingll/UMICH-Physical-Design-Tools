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




// Created:    Igor Markov,  VLSI CAD ABKGROUP UCLA  Aug 15, 1997

#ifdef _MSC_VER
  #pragma warning(disable:4786)
#endif

#include <iostream>
#include <vector>
using std::vector;
// #if !defined(__GNUC__)
// typedef vector<bool> bit_vector;
// #endif
  
#define GCC_VERSION (__GNUC__ * 10000 \
	+ __GNUC_MINOR__ * 100 \
	+ __GNUC_PATCHLEVEL__)

#if (GCC_VERSION >= 30100)
 #include <ext/algorithm>
#endif

#include <algorithm>
#include <utility>
#include "ABKCommon/abkcommon.h"
#include "mapping.h"
#include "ABKCommon/sgi_stl_compat.h"

//using std::vector;
using std::swap;
using std::min;
using std::max;
using std::sort;

#if(GCC_VERSION >= 30100)
 using __gnu_cxx::random_sample;
#else
 using std::random_sample;
#endif
using std::random_shuffle;


/* #ifdef __SUNPRO_CC
  void random_sample(unsigned*,unsigned*,unsigned*,unsigned*,RandomUnsigned&)
  { }
  void random_sample_n(unsigned*,unsigned*,unsigned*,unsigned,RandomUnsigned&)
  { }
  void random_shuffle(unsigned*,unsigned*,RandomUnsigned&) {}
#endif
*/

Mapping::Mapping(unsigned sourceSize, unsigned targetSize, Mapping::Example ex):
_destSize(targetSize), _meat(sourceSize)
{
	abkassert(sourceSize<=targetSize,
	" Map not injective: source set bigger than target set" );        
        unsigned k=0;
	switch (ex) 
        {
		case Identity :
                        for(k=0; k!=_meat.size(); k++) _meat[k]=k;
                        return;
		case _Reverse : 
                     {
                        unsigned N=_meat.size();
                        for(k=0; k!=N; k++) _meat[k]=(N-1)-k;
                        return;
                     }
		case _Random  :
                     {
			std::vector<unsigned> zero2n(targetSize);
                        for(unsigned k=0; k!=targetSize; k++) zero2n[k]=k;
			//RandomUnsigned rng(0,10);
                        RandomRawUnsigned rng("Mapping::Mapping,case _Random,rng");
			random_sample(zero2n.begin(),zero2n.end(),
			_meat.begin(),_meat.end(),rng);
			random_shuffle(_meat.begin(),_meat.end(),rng);
			return;
		     }
		case _RandomOrdered  :
                     {
			std::vector<unsigned> zero2n(targetSize);
                        for(unsigned k=0; k!=targetSize; k++) zero2n[k]=k;
			//RandomUnsigned rng(0,10);
                        RandomRawUnsigned rng("Mapping::Mapping,case _RandomOrdered,rng");
                        random_sample_n(zero2n.begin(),zero2n.end(),
                        _meat.begin(), sourceSize, rng);
			return;
	             }
		default: abkfatal(0," Unknown mapping example");
	}
}


Mapping::Mapping(UnaryOp unop, const Mapping& arg) {
	unsigned i=0;
	switch (unop) {
		case COMPLEMENT : {
			_destSize=arg.getDestSize();
			std::vector<unsigned> partInv;
			arg.getPartialInverse(partInv);
			for(i=0;i<arg.getDestSize();i++)
			if ( partInv[i]==unsigned(-1) )
						_meat.push_back(i);
			break;
		}

		default         : abkfatal(0," Unknown unary mapping operation ");
	}
}


Mapping::Mapping(const Mapping& arg1, BinaryOp binop, const Mapping& arg2) {
	unsigned i=0;
	switch (binop) {
		case COMPOSE  : {
			abkassert(arg1.getSourceSize()>=arg2.getDestSize(),
			"Can\'t compose mappings : size mismatch ");
			_destSize=arg1.getDestSize();
			_meat=std::vector<unsigned>(arg2.getSourceSize());
			for(i=0;i<_meat.size();i++) _meat[i]=arg1[arg2[i]];
			break;
		}

		case MINUS    : {
			abkassert(arg1.getDestSize()>=arg2.getDestSize(),
			"Can\'t subtract subsets : size mismatch");
			_destSize=arg1.getDestSize();
			std::vector<unsigned> partInv1, partInv2;
			arg1.getPartialInverse(partInv1);
			arg2.getPartialInverse(partInv2);
			for(i=0;i<arg1.getDestSize();i++)
			if ( partInv1[i]!=unsigned(-1) &&
				partInv2[i]==unsigned(-1)    )
							_meat.push_back(i);
			break;
		}

		case UNION   : {
			abkassert(arg1.getDestSize()>=arg2.getDestSize(),
			"Can\'t union subsets : size mismatch ");
			_destSize=arg1.getDestSize();
			std::vector<unsigned> partInv1, partInv2;
			arg1.getPartialInverse(partInv1);
			arg2.getPartialInverse(partInv2);
			for(i=0;i<arg1.getDestSize();i++)
			if ( partInv1[i]!=unsigned(-1) ||
				partInv2[i]!=unsigned(-1)    )
							_meat.push_back(i);
			break;
		}

		case XSECT   : {
			abkassert(arg1.getDestSize()>=arg2.getDestSize(),
			"Can\'t union subsets : size mismatch");
			_destSize=arg1.getDestSize();
			std::vector<unsigned> partInv1, partInv2;
			arg1.getPartialInverse(partInv1);
			arg2.getPartialInverse(partInv2);
			for(i=0;i<arg1.getDestSize();i++)
			if ( partInv1[i]!=unsigned(-1) &&
				partInv2[i]!=unsigned(-1)    )
							_meat.push_back(i);
			break;
		}
		default       : abkfatal(0," Unknown binary mapping operation ");
	}
}


std::vector<unsigned>& Mapping::
getPartialInverse(std::vector<unsigned>& result) const {
	// see comments next to declaration
	unsigned i;
	result.reserve(getDestSize());
	for(i=0;i<getDestSize();i++) result.push_back((unsigned)-1);
	for(i=0;i<getSourceSize();i++) {
		abkassert(result[_meat[i]]==(unsigned)-1,"Mapping not injective");
		result[_meat[i]]=i;
	};
	return result;
};


ostream& operator<<(ostream& out, const Mapping& mp) {
	out << " Mapping source size : " << mp.getSourceSize()
	<< "    destination size : " << mp.getDestSize() << endl;
	for(unsigned i=0;i<mp.getSourceSize();i++)
		out << i << " : " << mp[i] << endl;
	return out;
}



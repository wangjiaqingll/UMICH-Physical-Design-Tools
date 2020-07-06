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






/*
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Hewlett-Packard Company makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 *
 * Copyright (c) 1996
 * Silicon Graphics Computer Systems, Inc.
 *                 
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Silicon Graphics makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 */

#ifndef __SGI_STL_COMPAT
#define __SGI_STL_COMPAT


#if defined(__GNUC__)
  #define ported_from_sgi std
#else
      
#include <utility>
#include <algorithm>
#include <iterator>
#include <functional>

// #include <ABKCommon/SGI_STL_COMPAT/hash_map.h>

#ifdef _MSC_VER
  //HP and SGI copyrights do not apply to these definitions of
  //std::min and std::max
  namespace std
  {
      template<class T> const T &min(const T &x, const T &y)
      {
          if (x<y) return x;
          else return y;
      }
      template<class T> const T &max(const T &x, const T &y)
      {
          if (x>y) return x;
          else return y;
      }
  };
#endif

namespace std
{

template <class _ForwardIterator, class _Tp>
void
iota(_ForwardIterator __first, _ForwardIterator __last, _Tp __value)
{
  while (__first != __last)
    *__first++ = __value++;
}       

template <class _Base, class _Integer>
_Base power(_Base __x, _Integer __n)
{
  if (__n == 0) return 1;
  else {
    while ((__n & 1) == 0) {
      __n >>= 1;
      __x = __x * __x;
    }

    _Base __result = __x;
    __n >>= 1;
    while (__n != 0) {
      __x = __x * __x;
      if ((__n & 1) != 0)
        __result = __result * __x;
      __n >>= 1;
    }
    return __result;
  }
}        



template <class _ForwardIter, class _OutputIter, class _Distance>
_OutputIter random_sample_n(_ForwardIter __first, _ForwardIter __last,
                            _OutputIter __out, const _Distance __n)
{
  _Distance __remaining = __last - __first;
  _Distance __m = std::min(__n, __remaining);

  while (__m > 0) {
    if (__random_number(__remaining) < __m) {
      *__out = *__first;
      ++__out;
      --__m;
    }

    --__remaining;
    ++__first;
  }
  return __out;
}

template <class _ForwardIter, class _OutputIter, class _Distance,
          class _RandomNumberGenerator>
_OutputIter random_sample_n(_ForwardIter __first, _ForwardIter __last,
                            _OutputIter __out, const _Distance __n,
                            _RandomNumberGenerator& __rand)
{
  _Distance __remaining = __last - __first;
  _Distance __m = std::min(__n, __remaining);

  while (__m > 0) {
    if (__rand(__remaining) < __m) {
      *__out = *__first;
      ++__out;
      --__m;
    }

    --__remaining;
    ++__first;
  }
  return __out;
}       

template <class _InputIter, class _RandomAccessIter, class _Distance>
_RandomAccessIter __random_sample(_InputIter __first, _InputIter __last,
                                  _RandomAccessIter __out,
                                  const _Distance __n)
{
  _Distance __m = 0;
  _Distance __t = __n;
  for ( ; __first != __last && __m < __n; ++__m, ++__first)
    __out[__m] = *__first;

  while (__first != __last) {
    ++__t;
    _Distance __M = __random_number(__t);
    if (__M < __n)
      __out[__M] = *__first;
    ++__first;
  }

  return __out + __m;
}                      

template <class _InputIter, class _RandomAccessIter,
          class _RandomNumberGenerator, class _Distance>
_RandomAccessIter __random_sample(_InputIter __first, _InputIter __last,
                                  _RandomAccessIter __out,
                                  _RandomNumberGenerator& __rand,
                                  const _Distance __n)
{
  _Distance __m = 0;
  _Distance __t = __n;
  for ( ; __first != __last && __m < __n; ++__m, ++__first)
    __out[__m] = *__first;

  while (__first != __last) {
    ++__t;
    _Distance __M = __rand(__t);
    if (__M < __n)
      __out[__M] = *__first;
    ++__first;
  }

  return __out + __m;
}            

template <class _InputIter, class _RandomAccessIter>
inline _RandomAccessIter
random_sample(_InputIter __first, _InputIter __last,
              _RandomAccessIter __out_first, _RandomAccessIter __out_last)
{
  return __random_sample(__first, __last,
                         __out_first, __out_last - __out_first);
}


template <class _InputIter, class _RandomAccessIter,
          class _RandomNumberGenerator>
inline _RandomAccessIter
random_sample(_InputIter __first, _InputIter __last,
              _RandomAccessIter __out_first, _RandomAccessIter __out_last,
              _RandomNumberGenerator& __rand)
{
  return __random_sample(__first, __last,
                         __out_first, __rand,
                         __out_last - __out_first);
}    


template <class _ForwardIter>
bool is_sorted(_ForwardIter __first, _ForwardIter __last)
{
  if (__first == __last)
    return true;

  _ForwardIter __next = __first;
  for (++__next; __next != __last; __first = __next, ++__next) {
    if (*__next < *__first)
      return false;
  }

  return true;
}

template <class _ForwardIter, class _StrictWeakOrdering>
bool is_sorted(_ForwardIter __first, _ForwardIter __last,
               _StrictWeakOrdering __comp)
{
  if (__first == __last)
    return true;

  _ForwardIter __next = __first;
  for (++__next; __next != __last; __first = __next, ++__next) {
    if (__comp(*__next, *__first))
      return false;
  }

  return true;
}            

// identity_element (not part of the C++ standard).
/* #ifdef __NO_SUCH_SYMBOL__

template <class _Tp> inline _Tp identity_element(plus<_Tp>) 
{ return _Tp(0); }

template <class _Tp> inline _Tp identity_element(multiplies<_Tp>)
{ return _Tp(1); }                

#endif
*/


// identity is an extensions: it is not part of the standard.

template <class _Tp>
struct _Identity : public unary_function<_Tp,_Tp> 
{ const _Tp& operator()(const _Tp& __x) const { return __x; } };

template <class _Tp> struct identity : public _Identity<_Tp> {};
                          

template <class Pair, class U>
// JDJ (CW Pro1 doesn't like const when first_type is also const)
struct __select1st_hint : public unary_function<Pair, U>
{ const U& operator () (const Pair& x) const { return x.first; } };

template <class Pair>
struct select1st : public unary_function<Pair, typename Pair::first_type>
{
  const typename Pair::first_type& operator()(const Pair& x) const
  { return x.first; }
};

template <class Pair>
struct select2nd : public unary_function<Pair, typename Pair::second_type>
{
  const typename Pair::second_type& operator()(const Pair& x) const
  { return x.second; }
};


};    

#endif

#endif



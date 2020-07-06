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





















//created by Andrew Caldwell and Igor Markov on 09/19/98

#ifndef __BASE_GAIN_BUCKET_H__
#define __BASE_GAIN_BUCKET_H__

#include "ABKCommon/abkcommon.h"
#include "ABKCommon/sgi_stl_compat.h" /* added by Xiaojian 08-12-02 */
#include <vector>
using std::vector;
#include <algorithm>
using std::min;
using std::max;
#include "svGainElmt.h"
#include "gainList.h"

class BucketArray : public vector<GainList>
{
  protected:

    int 		_highestNonEmpty;
    int		    	_highestValid;
    SVGainElement*	_lastMoveSuggested;
    int			_gainOffset;
    int			_maxBucketIdx;

  public:
    BucketArray() 
	:vector<GainList>(), _highestNonEmpty(-1), _highestValid(-1),
	 _lastMoveSuggested(NULL), _maxBucketIdx(-1)
	{}

    ~BucketArray(){}

//  virtual void setMaxGain(unsigned maxG)=0;

//  virtual void push(SVGainElement& elmt)=0;

    SVGainElement* getHighest();	//NUL => no more valid moves

    void resetAfterGainUpdate();	//must be called after the
					//gain updates are done
    void removeAll();	//remove all elements

    bool invalidateBucket();
    bool invalidateElement();

    int  getHighestValidGain() { return _highestValid; }

    void setupForClip();

    bool isConsistent();

    friend ostream& operator<<(ostream& os, const BucketArray& buckets);

	void setMaxGain(unsigned maxG);
    void push(SVGainElement& elmt);
};

//____________________IMPLEMENTATIONS__________________________//

//________BucketArray functions________//

inline SVGainElement* BucketArray::getHighest()
{	//assumes _highestValid indexes a bucket with some valid	
	//move remaining or -1, and that lastMoveSuggested is either
	//an element in that bucket, or NULL if _highestValie == -1
    abkassert(_maxBucketIdx != -1, 
	"must call setMaxGain before using the BucketArray");

    return _lastMoveSuggested;
}

inline void BucketArray::resetAfterGainUpdate()
{
    //bring the _highestNonEmpty index down to the first non-empty
    //bucket (it may be too high as a result of incremental gain updates)
    //and clear any invalidations

    abkassert(_maxBucketIdx != -1, 
	"must call setMaxGain before using the BucketArray");

    while( _highestNonEmpty >= 0 && (*this)[_highestNonEmpty].isEmpty())
	_highestNonEmpty--;
    
    _highestValid = _highestNonEmpty;
    if(_highestValid > -1)	//there is something left in this array
	_lastMoveSuggested = (*this)[_highestValid]._next;
    else
	_lastMoveSuggested = NULL;
}

inline bool BucketArray::invalidateBucket()
{
    abkassert(_maxBucketIdx != -1, 
	"must call setMaxGain before using the BucketArray");

	//invalidate _highestValid..
    _highestValid--;
    while( _highestValid >= 0 && (*this)[_highestValid].isEmpty())
	_highestValid--;

    if(_highestValid > -1)	//there is something left in this array
	_lastMoveSuggested = (*this)[_highestValid]._next;
    else
	_lastMoveSuggested = NULL;

    return true;
}

inline bool BucketArray::invalidateElement()
{
    abkassert(_maxBucketIdx != -1, 
	"must call setMaxGain before using the BucketArray");

    abkassert(_lastMoveSuggested != NULL, "can't invalidate a NULL move");
    _lastMoveSuggested = _lastMoveSuggested->_next;

    if(_lastMoveSuggested == NULL)	//last one in this bucket
	return invalidateBucket();
    else
    	return false;
}

inline void BucketArray::removeAll()
{
    for(unsigned g = 0; g < size(); g++)
    {
	SVGainElement* elmt = (*this)[g]._next;
	while(elmt != NULL)
	{
	    elmt->disconnect();
	    elmt = (*this)[g]._next;
	}
    }

    _highestNonEmpty = 0;
    resetAfterGainUpdate();
}
inline bool BucketArray::isConsistent()
{
  bool consistent = true;
  for(unsigned g = 0; g < size(); g++)
 	if(!(*this)[g].isConsistent())
	    consistent = false;

  return consistent;
}

//NOTE: gain offset is the offset that gives use the bucket
//coresponding to gain ==0.  Note that this is a vector
//of size 2*_gainOffset+1.
inline void BucketArray::setupForClip()
{
    int g;
    for(g = _gainOffset-1; g >= 0; g--)
	(*this)[_gainOffset].insertBehind( (*this)[g] );
    for(g = _gainOffset+1; g < static_cast<int>(this->size()); g++)
	(*this)[_gainOffset].insertAhead( (*this)[g] );

    SVGainElement* e;
    for(e = (*this)[_gainOffset]._next; e != NULL; e = e->_next)
    {
	e->_gainOffset = e->_gain;
	e->_gain = 0;
    }
}

inline ostream& operator<<(ostream& os, const BucketArray& bucket)
{
    os<<" BucketArray:"<<endl;
    os<<"  MaxGain:  "<<bucket._gainOffset<<endl;

    for(int g = bucket.size()-1; g >= 0; g--)
	os<<g - bucket._gainOffset <<") "<<bucket[g];

    os<<endl;
    return os;
}

inline void BucketArray::setMaxGain(unsigned maxG)
{ 	
    _gainOffset = maxG;
    _maxBucketIdx =  2 * _gainOffset;
    this->insert(end(), _maxBucketIdx+1-size(), GainList());
    _highestNonEmpty = _highestValid = 0;
}

inline void BucketArray::push(SVGainElement& elmt)
{
    abkassert(_maxBucketIdx != -1, 
	"must call setMaxGain before using the BucketArray");

    int bucket = max(min(_maxBucketIdx,  elmt._gain+_gainOffset), 0);
    (*this)[bucket].push(elmt);
    _highestNonEmpty = max(_highestNonEmpty, bucket);
}

#endif




















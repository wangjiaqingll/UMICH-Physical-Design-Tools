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


#include "ABKCommon/abkcommon.h"
#include "RBPlace/RBPlacement.h"
#include "DB/DB.h"
#include "SimAnneal/simAnneal.h"
#define initTemp 40000
#define maxIter 150
#define maxUpdate 25
#define stopTemp 0.01

SimAnneal::SimAnneal(RBPlacement& rbplace)
	: rb(rbplace), 
		oldCost(0), newCost(0),
		oldPlace(0), newPlace(0)
{

	acceptCount=0;
	curRate=1.0;

	totalIter = 500/maxIter*pow(rb.getNumCells(),(4/3));
	cout << "Total iterations: " << maxIter*totalIter << endl;

	hpwl=rb.evalHPWL();
	overlap=rb.calcOverlap();
	oldCost = cost();
	cout << "Initial:\t HWPL: " << rb.evalHPWL() << " Over: " << rb.calcOverlap() << endl;
	// we want the initial temperature to be of the same magnitude as the cost
	// so that deltaC is approximately equal to temp.
	curTemp=oldCost;

	// first, check if i've got an initial placement, then grab it
	setupAndCheck();
	

  rbplace.saveAsPlot("plotWNames",-DBL_MAX,DBL_MAX,-DBL_MAX,DBL_MAX,"initial");

	itCount = 0;
	// for a certain number of iterations
	while (itCount++ < maxIter)
	{
		acceptCount=0;
		totalCount=0;
		unsigned updateCount=0;

		// we want to maintain a acceptance rate schedule
		// first 15% of iterations decrease exonentially
		if (itCount < 0.15*maxIter)
			curRate*=exp(log(0.33)/(0.15*maxIter));
		// middle 50% of iterations remain constant
		else if (itCount < 0.68*maxIter)
			curRate=0.33;
		// last 35% decrease exponentially
		else
			curRate*=exp(log(0.01/0.33)/(0.35*maxIter));

		// for the maximum number of configs at this iteration
		while (totalCount++ < totalIter)
		{
			updateCount++;
			// we must update the temperature every so often
			if (updateCount==maxUpdate) {
				updateCount=0;
				update(curTemp);
			}

			generate();

			hpwl -= rb.calcInstHPWL(movables); // remove the net lengths attached to updated cells
			overlap -= rb.calcInstOverlap(movables);
			rb.updateCells(movables, newPlace); 
			newCost = cost();

			if (!accept(newCost, oldCost, curTemp))
			{
				// this undoes the moves
				hpwl -= rb.calcInstHPWL(movables); // remove net lengths attached to updated cells
				overlap -= rb.calcInstOverlap(movables);
				rb.updateCells(movables, oldPlace);
				hpwl += rb.calcInstHPWL(movables); // add back the net lengths (since we don't call cost)
				overlap += rb.calcInstOverlap(movables);
			}
			else
			{
				//cout << "Accept!" << endl;
				acceptCount++;
				oldCost = newCost;
			}

		/*if ((hpwl != rb.evalHPWL()) || (overlap != rb.calcOverlap()))
		{
			cout << "\t HWPL: " << rb.evalHPWL() << " Over: " << rb.calcOverlap() << endl;
			cout << "\t HWPL: " << hpwl << " Over: " << overlap << endl;
			cout << endl;

  		rb.saveAsPlot("plotWNames",-DBL_MAX,DBL_MAX,-DBL_MAX,DBL_MAX,"overlap");
			exit(-1);
		}*/

		}
		if (false) {
			char name[20];
			sprintf(name,"saved%d",totalCount/maxIter);
   		rbplace.saveAsPlot("plotWNames",-DBL_MAX,DBL_MAX,-DBL_MAX,DBL_MAX,name);
		}
		double acceptRate = acceptCount/totalCount;
		cout << "Iter: " << itCount << " Cost: " << oldCost << "(" << hpwl << "+" << overlap <<")"<< endl;
	cout << "\tCR: " << curRate << " AR: " << acceptRate  << " NewTemp: " << curTemp << endl;
		//cout << "\t HWPL: " << rb.evalHPWL() << " Over: " << rb.calcOverlap() << endl;
	}

	cout << "Final:\t HWPL: " << rb.evalHPWL() << " Over: " << rb.calcOverlap() << endl;
  rbplace.saveAsPlot("plotWNames",-DBL_MAX,DBL_MAX,-DBL_MAX,DBL_MAX,"final");
}

// generates a set of movable cells and locations (movables, newPlace)
void
SimAnneal::generate()
{
	movables.clear();
	oldPlace.clear();
	newPlace.clear();

	// select random cell
	RandomUnsigned randcell(0,rb.getNumCells());
	unsigned randIdx1=randcell;
	movables.push_back(randIdx1);
	oldPlace.push_back(rb[randIdx1]);

	// figure out window size to maintain 44% acceptance rate
	
	// maintain a ratio of single/double of about 4
	if (RandomUnsigned(0,5)<4)
	{
		unsigned crow=RandomUnsigned(0,rb.getNumCoreRows());
		const RBPCoreRow &cr = rb.getCoreRow(crow);
		unsigned srow=RandomUnsigned(0,cr.getNumSubRows());
		unsigned site=RandomUnsigned(0,cr[srow].getNumSites());

		newPlace.push_back(Point(cr[srow].getXStart()+site*cr[srow].getSiteWidth(),cr[srow].getYCoord()));
		//cout << "displace: " << randIdx1 << "->" <<  crow << "," << srow << "," << site << endl;
	}
	else
	{
		unsigned randIdx2=randcell;
		// pair it to swap
		movables.push_back(randIdx2);
		oldPlace.push_back(rb[randIdx2]);
		// push them back backwards
		newPlace.push_back(rb[randIdx2]);
		newPlace.push_back(rb[randIdx1]);

		//cout << "swap:";
		//cout << "\t" << randIdx1 << ":" << rb[randIdx1] << "->" << rb[randIdx2] << endl;
		//cout << "\t" << randIdx2 << ":" << rb[randIdx2] << "->" << rb[randIdx1] << endl;
	}
}

// cost() calculates the cost function INCREMENTALLY. It finds the HPWL
// of the vector of cells being moved (movables) and the overlap of them too.
double
SimAnneal::cost()
{
	// find HPWL for each net attached to moved cells
	hpwl += rb.calcInstHPWL(movables);
	//double hpwl = rb.evalHPWL();
	// find overlap from introduced cells
	overlap += rb.calcInstOverlap(movables);
	/*cout << "hpwl:" << hpwl << endl;
	cout << "overlap:" << overlap << endl;
	cout << "cost: " << hpwl+overlap << endl;
	*/
	
	return(hpwl+10*overlap);
}

void
SimAnneal::setupAndCheck()
{
	// make sure every cell has a location?
	abkassert(_cellsNotInRows>0, "Cells not in rows");

}

bool
SimAnneal::accept(double newCost, double oldCost, double curTemp)
{
	RandomDouble randIdx(0,1);
	//double y=randIdx;
	//cout << "delta: " << newCost << "-" << oldCost << "=" << newCost-oldCost << endl;
	//cout <<exp(-abs(newCost-oldCost)/curTemp) << endl;
	if (newCost <= oldCost)
		return(true);
	//if (randIdx < exp(-initTemp/6*abs(newCost-oldCost)/curTemp))
	if (randIdx < exp(-abs(newCost-oldCost)/curTemp))
		return(true);
	return(false);	
}

void
SimAnneal::update(double& curTemp)
{

	// temperature can go up or down depending on how well
	// the acceptance rate matches the desired rate
	double acceptRate = acceptCount/totalCount;
	// damping factor (3) can be tweaked
	double change = (1-(acceptRate-curRate)/3);
	curTemp *= change;

	//cout << "\tCR: " << curRate << " AR: " << acceptRate << " " << change << " NewTemp: " << curTemp << endl;

	// old simple schedule
/*	if (curTemp > 40000)
		curTemp *= 0.8;
	else if (curTemp > 5000)
		curTemp *=0.91;
	else 
		curTemp *=0.10;
*/
}

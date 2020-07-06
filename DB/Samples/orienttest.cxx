/**************************************************************************
***
***    Copyright (c) 1995-1997 by ABKGroup, UCLA VLSI CAD Laboratory,
***    UCLA Computer Science Department, Los Angeles, CA 90095-1596 USA,
***    and by the Regents of the University of California.
***    All rights reserved.
***
***    No portion of this file may be used, copied, or transmitted for
***    any purpose, including any research or commercial purpose,
***    without the express written consent of Professor Andrew B. Kahng,
***    UCLA Computer Science Department, 3713 Boelter Hall, Los Angeles,
***    CA 90095-1596 USA.   Any copy made of this file must include this
***    notice.    ABKGroup and the Regents of the University of California
***    retain title to all copies of this file and/or its compiled
***    derivatives.
***
***************************************************************************/

#include <iostream>
#include "abkcommon.h"
#include "constraints.h"
#include "placement.h"
#include "transf.h"
#include "DB.h"

int main()
{
    Orient o1("N");
    Symmetry s1("Y");
    Orient r1("FS");

    unsigned i,j;
    for (i=0; i<16; i++)
    {
      OrientationSet os(i);
      for (j=0; j<16; j++)
      {
	OrientationSet o2(j);
	cout << os << " & " << o2 << " = " << OrientationSet(os & o2) << endl;
      }
    }
}



#!/bin/csh -f

#**************************************************************************
#**
#**    Copyright (c) 1995-1997 by ABKGroup, UCLA VLSI CAD Laboratory,
#**    UCLA Computer Science Department, Los Angeles, CA 90095-1596 USA,
#**    and by the Regents of the University of California.
#**    All rights reserved.
#**
#**    No portion of this file may be used, copied, or transmitted for
#**    any purpose, including any research or commercial purpose,
#**    without the express written consent of Professor Andrew B. Kahng,
#**    UCLA Computer Science Department, 3713 Boelter Hall, Los Angeles,
#**    CA 90095-1596 USA.   Any copy made of this file must include this
#**    notice.    ABKGroup and the Regents of the University of California
#**    retain title to all copies of this file and/or its compiled
#**    derivatives.
#**
#**************************************************************************/

set DIFF=/usr/bin/diff
set WC=/usr/bin/wc
set PROG="SmallPlacersTest0.exe"

/bin/rm -f seeds.out seeds.in

echo "___ size 6 tests _____" > new.out
$PROG -f TESTS/amd_C6.aux -num 100 -verb "10_10_10" >> new.out
echo "___ size 8 tests _____" >> new.out
$PROG -f TESTS/pl_N8_E14_281.aux -num 20 -verb "10_10_10" >> new.out
$PROG -f TESTS/pl_N8_E18_591.aux -num 20 -verb "10_10_10" >> new.out
$PROG -f TESTS/pl_N8_E20_695.aux -num 20 -verb "10_10_10" >> new.out
echo "___ size 9 tests _____" >> new.out
$PROG -f TESTS/pl_N9_E19_365.aux -num 10 -verb "10_10_10" >> new.out
$PROG -f TESTS/pl_N9_E25_213.aux -num 10 -verb "10_10_10" >> new.out
$PROG -f TESTS/pl_N9_E32_337.aux -num 10 -verb "10_10_10" >> new.out
echo "___ size 10 tests _____" >> new.out
$PROG -f TESTS/pl_N10_E19_297.aux -num 5 -verb "10_10_10" >> new.out
$PROG -f TESTS/pl_N10_E24_669.aux -num 5 -verb "10_10_10" >> new.out
$PROG -f TESTS/pl_N10_E25_293.aux -num 2 -verb "10_10_10" >> new.out
echo "___ size 11 tests _____" >> new.out
$PROG -f TESTS/pl_N11_E26_569.aux -num 2 -verb "10_10_10" >> new.out


egrep -v "(Created|took|User|Platform|Memory)" new.out > newout.notime
egrep -v "(Created|took|User|Platform|Memory)" expected.out > expectedout.notime

$DIFF newout.notime expectedout.notime    >  diffs.notime

echo " "
echo Differences from precomputed results
$WC diffs.notime

rm -f seeds.{out,in} expectedout.notime


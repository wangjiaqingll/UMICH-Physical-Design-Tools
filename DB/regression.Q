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
set PROG=dbTest 

echo "------------  Running test 1a..." > new.out
echo -n Running test 1a...
${PROG}1 -f TESTS/smallq.aux >> new.out
echo Ok
echo "------------  Running test 1b..." >> new.out
echo -n Running test 1b...
${PROG}1 -f TESTS/mini4ql.aux >> new.out
echo Ok
echo "------------  Running test 1c..." >> new.out
echo -n Running test 1c...
${PROG}1 -f TESTS/quasiHPql.aux >> new.out
echo Ok

echo "" > placed.def
echo "" > easyAccess.out
echo "" > rbplacemt.out
echo "------------  Running test 2a..." > new.out
echo -n Running test 2a...
${PROG}2 -f TESTS/smallq.aux >> new.out
cat placed.def     >> new.out
cat easyAccess.out >> new.out
cat rbplacemt.out  >> new.out
echo Ok
echo "------------  Running test 2b..." >> new.out
echo -n Running test 2b...
echo "" > placed.def
echo "" > easyAccess.out
echo "" > rbplacemt.out
${PROG}2 -f TESTS/mini4ql.aux >> new.out
cat placed.def     >> new.out
cat easyAccess.out >> new.out
cat rbplacemt.out  >> new.out
echo Ok
echo "------------  Running test 2c..." >> new.out
echo -n Running test 2c...
echo "" > placed.def
echo "" > easyAccess.out
echo "" > rbplacemt.out
${PROG}2 -f TESTS/quasiHPql.aux >> new.out
cat placed.def     >> new.out
cat easyAccess.out >> new.out
cat rbplacemt.out  >> new.out
echo Ok
echo "------------  Running test 3a..." >> new.out
echo -n Running test 3a...
${PROG}3 -f TESTS/smallq.aux >> new.out
echo Ok
echo "------------  Running test 3b..." >> new.out
echo -n Running test 3b...
${PROG}3 -f TESTS/mini4ql.aux >> new.out
echo Ok
echo "------------  Running test 3c..." >> new.out
echo -n Running test 3c...
${PROG}3 -f TESTS/quasiHPql.aux >> new.out
echo Ok
echo "------------  Running test 4a..." >> new.out
echo -n Running test 4a...
${PROG}4 -f TESTS/smallq.aux >> new.out
echo Ok
echo "------------  Running test 4b..." >> new.out
echo -n Running test 4b...
${PROG}4 -f TESTS/mini4ql.aux >> new.out
echo Ok
echo "------------  Running test 4c..." >> new.out
echo -n Running test 4c...
${PROG}4 -f TESTS/quasiHPql.aux >> new.out
echo Ok
echo "------------  Running test 5 ..." >> new.out
echo -n Running test 5 ...
${PROG}5 -f TESTS/quasiHPl.aux >> new.out
cat dbpic.xy >> new.out
echo Ok



egrep -v "(Created|sec|User|Platform|Memory use)" new.out > newout.notime
egrep -v "(Created|sec|User|Platform|Memory use)" expected_Q.out > expectedout.notime
$DIFF newout.notime expectedout.notime > diffs.notime

echo " "
echo Differences from precomputed results
$WC diffs.notime

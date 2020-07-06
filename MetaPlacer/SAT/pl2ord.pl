#!/usr/bin/perl


# Created By Igor Markov, Winter 2001

# This script converts placement results of the Capo placer (a .pl file)
# into a linear ordering (assumption: the original placement was 1-dim)


@ord=();

while(<>)
{
   next unless /v(\d+)\s+(\d+)/;
   $ord[$2]=$1;
}

foreach $v (@ord) { print "v$v\n"; }

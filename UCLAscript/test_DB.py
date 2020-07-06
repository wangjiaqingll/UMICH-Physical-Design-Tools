#!/usr/bin/env python
from UCLAscript import *
print "UCLAscript loaded OK!"
params=DB_Parameters()
params.alwaysCheckConsistency=1
db=DB("./alt0.aux",params)
netlist=db.getNetlist()
netlist.thisown=0
print "Testcase:",db.getAuxName()
print "Total CCs ",netlist.getNumCCs()
print "Total SCs ",netlist.getNumSCs()
print "Total AllPads ",netlist.getNumAllPads() 
netlist.printNetStats()

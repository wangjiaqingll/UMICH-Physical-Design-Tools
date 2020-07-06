#!/bin/csh -f
\cp UCLAscript.i i_file
swig -c++ -perl5 -compat -shadow UCLAscript.i
\cp UCLAscript_wrap.c UCLAscript.pm wrap4perl
swig -c++ -python -shadow UCLAscript.i
\cp UCLAscript_wrap.c UCLAscript.py wrap4python
swig -c++ -tcl8 UCLAscript.i
\cp UCLAscript_wrap.c wrap4tcl


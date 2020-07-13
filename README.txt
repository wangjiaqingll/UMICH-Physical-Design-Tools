
Contact email: <imarkov@umich.edu>

To configure UCLApack, type 

script install.log
perl config 

and answer a few questions (make sure you use PERL 5+ by first typing perl -v).
When you see command prompt type 

exit

If something goes wrong, send install.log along with your bug report.

It is recommended that you let the installation program build all the 
libraries for you and then let it test them (i.e. answer yes to those 
questions) --- just lay back and watch the screen for a while. No compiler 
errors / warnings should appear. Test results should not DIFFer from 
precomputed output files.

---------------------

  Use notes:
         0. You need 300-400Mb of disk space available to install 
            UCLApack and run regression tests.
         1. Currently supported platforms are Solaris/CC5.1, Solaris/g++2.95.2
            and Linux/g++2.95.2 (you can get g++ from sunfreeware.com).
            Solaris version must be 2.7 or higher. Earlier compilers will not
            work; Solaris 2.5 or earlier will not work.
            CC -V, g++ -v and uname -a should tell you the version numbers
            of compilers and systems. Please include those numbers into all
            bugs reports that you send to developers.
            g++ 2.7.2, CC 4.1, CC4.2 and CC5.0 compilers are not and 
            will not be supported. The C++ code compiled with MSVC++ 5.0 
            and 6.0, but we currently do not support automated installation
            for MS Windows.
         2. Installation program requires PERL 5+
            If you have perl4, you are in the Stone Age ---
            please get perl5 or higher (or try perl5 on your system,
            if it works alias perl to perl5 before running this script).
         3. The LEF/DEF parser that may be enclosed with distribution
            requires flex and bison. Those come preinstalled on Linux, but
            may need to be downloaded from sunfreeware.com for Solaris.
         4. The latest g++ RPMs for RedHat Linux can be downloaded from
            ftp://ftp.varesearch.com/pub/support/hjl/gcc/gcc-2.95.x/
         5. In order to link the package ParserLEFDEF and all packages 
            that use it, the library libfl.a must be available to the linker
            using its default search path or in the lib/ subdirectory of
            UCLApack (at the moment when linking starts). libfl.a is a part of
            GNU flex (similar to lex) and comes with most Linux systems.
            On Solaris, if you already have that library, you can put a 
            symbolic link into one of those locations rather than copy 
            the actual library. 
            Successful installations were reported for GNU bison 1.25 and GNU 
            flex 2.4.5, 2.5.3 or 2.5.4 (all work). We provide a binary libfl.a
            for download that is compiled from flex 2.5.4. 


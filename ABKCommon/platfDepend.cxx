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








// August   22, 1997   created   by Igor Markov  VLSI CAD UCLA ABKGROUP
// November 30, 1997   additions by Max Moroz,   VLSI CAD UCLA ABKGROUP

// note: not thread-safe

// This file to be included into all projects in the group
// it contains platform-specific code portability of which relies
// on symbols defined by compilers

// 970825 mro made corrections to conditional compilation in ctors for
//            Platform and User:
//            i)   _MSC_VER not __MSC_VER (starts with single underscore)
//            ii)  allocated more space for _infoLines
//            iii) changed nested #ifdefs to #if ... #elif ... #else
// 970923 ilm added abk_dump_stack()
// 971130 ilm added Max Moroz code for memory estimate and reworked
//            class MemUsage()
// 980822 mm  corrected MemUsage() to work with Solaris 2.6

#ifdef _MSC_VER
 #pragma warning(disable:4786)
#define _X86_
#include <cstdarg>
#include <windef.h>
#include <winbase.h>
//#include <winreg.h>
#include <direct.h>

#include <cstdio>
#include <cstdlib>
#include <string>
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif 

#include "infolines.h"
#include "abktypes.h"
#include "abkassert.h"

#if defined(sun) || defined(__SUNPRO_CC)
 #include <sys/systeminfo.h>
#endif

#ifdef linux
 #include <sys/utsname.h>
#endif     

#if defined(linux) || defined(sun) || defined(__SUNPRO_CC)
 #include <unistd.h>
 #include <pwd.h>
 #include <sys/stat.h>
#endif

/* ======================== IMPLEMENTATIONS ======================== */

void abk_dump_stack()
{
    return;
#if defined(linux) || defined(sun) || defined(__SUNPRO_CC)
    printf("\n  --- ABK GROUP DELIBERATE STACK DUMP  ----------- \n\n");
    fflush(stdout);
    char s[160];
    unsigned ProcId=getpid();
 #if defined(linux)
    sprintf(s, "/bin/echo \"bt   \nquit   \\n\" | "
               "gdb -q /proc/%d/exe %d", ProcId,ProcId);
 #else
    sprintf(s, "/bin/echo \"where\\ndetach \\n\" | dbx - %d", ProcId);
 #endif
    system(s);
    printf("  ------------------ END STACK DUMP -------------- \n");
    fflush(stdout);
#else
// fprintf(stderr," abk_dump_stack(): Can't dump stack on this platform\n");
// fflush(stderr);
#endif
    return;
}

void abk_call_debugger()
{
#if defined(linux) || defined(sun) || defined(__SUNPRO_CC)
    unsigned ProcId=getpid();
    printf("\n  --- ATTACHING DEBUGGER to process %d ", ProcId);
    printf(" (an ABKGROUP utility) --- \n\n");
    fflush(stdout);
    char s[160];
 #if defined(linux)
    sprintf(s, "gdb -q /proc/%d/exe %d", ProcId,ProcId);
 #else // must be Solaris
    sprintf(s, "dbx - %d", ProcId);
 #endif
    system(s);
    printf("  ------------------ CONTINUING -------------- \n");
    fflush(stdout);
#else
 fprintf(stderr," abk_call_debugger(): Can't call debugger on this platform\n");
 fflush(stderr);
#endif
    return;
}

double getPeakMemoryUsage()
{

#if ! (defined(sun) || defined(linux) || defined(__SUNPRO_CC))
  return -1;
#endif

#if defined(sun) || defined(__SUNPRO_CC)
  char procFileName[20];
  unsigned pid=getpid();
  sprintf(procFileName,"/proc/%d/as",pid);
  struct stat buf;
  if (stat(procFileName,&buf))  // no such file on Solaris 2.5 and earlier
  {                             // so we stat another file now
    char procFileNameOld[20];
    sprintf(procFileNameOld,"/proc/%d",pid);
    if (stat(procFileNameOld,&buf)) 
        return -1.0;
  }
  return (1.0/(1024*1024)) * buf.st_size;
#elif defined(linux)
  char buf[1000];
  sprintf(buf,"/proc/%d/stat", static_cast<unsigned>(getpid()));
  ifstream ifs(buf);
  for(unsigned k=0; k!=23; k++) ifs >> buf;
      //  cout << k << ": " << buf << endl; }
  return (1.0/(1024*1024)) * atoi(buf);
#else
  return -1;
#endif
}

static double getMemoryUsageEstimate();

MemUsage::MemUsage()
{
  _peak=getPeakMemoryUsage();
  #if (defined(sun) || defined(__SUNPRO_CC) || defined(linux))
    _estimate=_peak;
    return ;
  #endif
  _estimate=getMemoryUsageEstimate();
}

void MemChange::resetMark()
{
  _memUsageMark=getMemoryUsageEstimate();
}

MemChange::MemChange()
{
  resetMark();
}

MemChange::operator double() const
{
#if defined(sun) || defined(linux) || defined(__SUNPRO_CC)
  return getPeakMemoryUsage()-_memUsageMark;
#else
  return -1.0;
#endif
}

Platform::Platform()
{
#if defined(sun) || defined(__SUNPRO_CC)
  Str31 sys, rel, arch, platf;
  sysinfo(SI_SYSNAME, sys, 31);
  sysinfo(SI_RELEASE, rel, 31);
  sysinfo(SI_ARCHITECTURE,arch, 31);
  sysinfo(SI_PLATFORM,platf, 31);
  _infoLine= new char[strlen(sys)+strlen(rel)+strlen(arch)+strlen(platf)+30];
  sprintf(_infoLine,"# Platform     : %s %s %s %s \n",sys,rel,arch,platf);
#elif defined(linux)
  struct utsname buf;
  uname(&buf);
  _infoLine= new char[strlen(buf.sysname)+strlen(buf.release)
                     +strlen(buf.version)+strlen(buf.machine)+30];
  sprintf(_infoLine,"# Platform     : %s %s %s %s \n",buf.sysname,buf.release,
                                                      buf.version,buf.machine);
#elif defined(_MSC_VER)
  _infoLine= new char[40];
  strcpy(_infoLine, "# Platform     : MS Windows \n");
#else
  _infoLine= new char[40];
  strcpy(_infoLine, "# Platform     : unknown \n");
#endif
}

#if !defined(sun) && !defined(linux)
extern  int gethostname(char*, int);
#endif

User::User()
{

//#if defined(linux) || defined(sun) || defined(__SUNPRO_CC)
#if defined(linux) 
  char host[100]; 
  gethostname(host,31);
  struct passwd *pwr=getpwuid(getuid());
  if (pwr==NULL)
  {
    _infoLine= new char[40];
     strcpy(_infoLine, "# User         : unknown \n");
     return;
  }

  _infoLine= new char[strlen(pwr->pw_name)+strlen(pwr->pw_gecos)+130];
  sprintf(_infoLine,"# User         : %s@%s (%s) \n",
                      pwr->pw_name,host,pwr->pw_gecos);
#elif defined(_MSC_VER)
  _infoLine= new char[40];
  strcpy(_infoLine, "# User         : unknown \n");
#else
  _infoLine= new char[40];
  strcpy(_infoLine, "# User         : unknown \n");
#endif
}

// code by Max Moroz

const int kMegaByte=1024*1024;
const int kSmallChunks=1000;
const int kMaxAllocs=20000;
const double MemUsageEps=3;

// everything in bytes

inline int memused()
{
// cout << " Peak memory " << getPeakMemoryUsage() << endl;
   return static_cast<int>(getPeakMemoryUsage()*kMegaByte);
}

double getMemoryUsageEstimate()
{
#if ! (defined(linux) || defined(sun) || defined(__SUNPRO_CC))
        return -1;
#endif
        static int prevMem=0;
        static int extra;
        static int fail=0;

        if (fail) return -1;

//      new_handler oldHndl;
//      oldHndl=set_new_handler(mH);

        void* ptr[kMaxAllocs];
        int numAllocs=0;

        int last=memused();
//       cout << "Memused : " << last << endl;
        if (last<=0) return -1;
//      cerr<<"memused()="<<memused()<<endl;
        int chunk=8192; // system allocates 8K chunks
        int allocated=0;
        int countSmallChunks=kSmallChunks;
        // if we allocate <8K, we'd get memused()+=8K, and allocated<8K - error
        while (1)
        {
//              abkfatal(numAllocs<kMaxAllocs, "too many allocs");
                if (numAllocs>=kMaxAllocs) 
                {   
                   abkwarn(0,"too many allocs (may be okay for 64-bit builds)");
                   // FIFO destruction
                   for (int i=0; i!=numAllocs; ++i) free(ptr[i]);
                    return -1.0;     
                }
//              cerr<<"old: "<<memused()<<"; allocating "<<chunk<<"; now: ";
                if (!(ptr[numAllocs++]=malloc(chunk)))
                {
                        fail=1;
                        return -1;
                }
//              cerr<<memused()<<endl;
                allocated+=chunk;
                if (memused()>last+MemUsageEps)
                        break;
                if (chunk<=kMegaByte && !countSmallChunks--)
                {
                                chunk*=2;
                                countSmallChunks=kSmallChunks;
                }
        }

//     int result=memused()-allocated-prevMem;

/* LIFO destruction
        while (numAllocs) free(ptr[--numAllocs]); */

        // FIFO destruction
        for (int i=0; i!=numAllocs; ++i) free(ptr[i]);

        extra=memused()-last;
// handle extra correctly:
// in some cases we need to add its prev value to current,
// in some just store the new value

        prevMem=memused()-allocated;
//      set_new_handler(oldHndl);
        return prevMem/double(kMegaByte);
}

UserHomeDir::UserHomeDir()
{
#if defined(sun) || defined(linux)
  struct passwd *pwrec;
  pwrec=getpwuid(getuid()); 
  _infoLine=strdup(pwrec->pw_dir);
#elif defined(_MSC_VER)
  char *homedr=getenv("HOMEDRIVE");
  char *homepath=getenv("HOMEPATH");
  char *pathbuf=new char[_MAX_PATH];
  if (homedr==NULL || homepath==NULL)
      strcpy(pathbuf, "c:\\users\\default");
  else
      _makepath(pathbuf,homedr,homepath,NULL,NULL);
  int len=strlen(pathbuf);
  if (pathbuf[len-1]=='\\') pathbuf[len-1]='\0';
  _infoLine=strdup(pathbuf);
  delete [] pathbuf;
#endif
}

ExecLocation::ExecLocation()
{ 
  char buf[1000]="";
#ifdef linux
  readlink("/proc/self/exe", buf, 1000); 
  int pos=strlen(buf)-1;
  for (;pos>=0;pos--)
    {
    if (buf[pos]=='/')
      {
      buf[pos]='\0';
      break;
      }
    }
#endif

#if defined(sun) || defined(__SUNPRO_CC)
  pid_t pid=getpid();
  char tempfname[100]="";
  sprintf(tempfname,"/tmp/atempf%d",static_cast<int>(pid));
  sprintf(buf,"/usr/proc/bin/ptree %d | tail -4 > %s",
				  static_cast<int>(pid),tempfname);
  system(buf);
  { ifstream ifs(tempfname); ifs >> buf; ifs >> buf;}
  unlink(tempfname);
  if (buf[0]!='/')
  {
    if (strrchr(buf,'/')==NULL) 
    { getcwd(buf,1000); _infoLine=strdup(buf); return; }
    else
    { 
      char buf1[500]="",buf2[1000]="";
      getcwd(buf1,500);
      strncpy(buf2,buf,500);
      sprintf(buf,"%s/%s",buf1,buf2);
    }
  }
  char *lastDelim=strrchr(buf,'/');
  if (lastDelim==NULL) sprintf(buf,"Cannot find");
  else                 *lastDelim='\0';
#elif defined(_MSC_VER)
  ::GetModuleFileName(NULL,buf,995);
  char drv[_MAX_DRIVE],dir[_MAX_DIR],fname[_MAX_FNAME],ext[_MAX_EXT];
  _splitpath(buf,drv,dir,fname,ext);
  _makepath(buf,drv,dir,"","");
  int len=strlen(buf);
  if (buf[len-1]=='\\') buf[len-1]='\0';
#endif

  _infoLine=strdup(buf);
}

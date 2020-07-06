
typedef struct lexsstr
    {
    char *str;
    int  DoDelete;
    } lexemeStruct;

#ifndef alloca
#ifdef __GNUC__
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined ( __sparc) || defined (__sgi)
#include <alloca.h>
#else /* not sparc */
#if defined (MSDOS) && !defined (__TURBOC__)
#include <malloc.h>
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
#include <malloc.h>
 #pragma alloca
#else /* not MSDOS, __TURBOC__, or _AIX */
#ifdef __hpux
#ifdef __cplusplus
extern "C" {
void *alloca (unsigned int);
};
#else /* not __cplusplus */
void *alloca ();
#endif /* not __cplusplus */
#endif /* __hpux */
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc.  */
#endif /* not GNU C.  */
#endif /* alloca not defined.  */


#include <DB/DB.h>
#include "LEFDEFpoint.h"
#include "lefparse.tab.h"
#include "parserLEFDEF.h"

#if 0
int leflex(void);
YYSTYPE leflval;
int bCaseSens;

int lefparse()
{
leflex();
alloca(5);
ParserLEFDEF::regionHashMapType r;

ParserLEFDEF::masterCellHashMapType h;
                    ParserLEFDEF::regionHashMapType::iterator iR =
                                r.find((char const*)(NULL));

    pair<char const* const,dbMasterCell*> const &pref =
        pair<char const* const,dbMasterCell*>(
                                              (const char*)NULL,
                                              (dbMasterCell*)NULL);



    pair<ParserLEFDEF::masterCellHashMapType::iterator,bool> p =
        h.insert(pref);

return 0;
}

#endif

int defparse()
{
alloca(5);
ParserLEFDEF::regionHashMapType r;

ParserLEFDEF::masterCellHashMapType h;
                    ParserLEFDEF::regionHashMapType::iterator iR =
                                r.find((char const*)(NULL));

    pair<char const* const,dbMasterCell*> const &pref =
        pair<char const* const,dbMasterCell*>(
                                              (const char*)NULL,
                                              (dbMasterCell*)NULL);
                                 
                                         

    pair<ParserLEFDEF::masterCellHashMapType::iterator,bool> p =
        h.insert(pref);

return 0;
}

FILE *lefin, *defin;

//void DB::parseLEFDEF(const char*,const char*,bool)
//{
//}


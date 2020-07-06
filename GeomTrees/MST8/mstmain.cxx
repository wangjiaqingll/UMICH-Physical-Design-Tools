#include <stdio.h>
#include "mst8.h"
#include "abkcommon.h"

extern int _MST_Nsoln;

main() // int argc, char **argv)
{
    int NPts=0;
    int x,y;
    int i,j;
    char what;
    mst_package_init(1000);
    for(start_net(), i=j=0;;) {
        if (scanf("%1s", &what) != 1) break;
        if (what == 'P') {
            scanf("%d %d", &x, &y); 
            add_new_point(x,y,(void *)(i++));
            }
        if (what == 'E') {
            if(j++ == 0) end_net();
            scanf("%d %d", &x, &y); 
            make_equiv((void *)x,(void *)y);
            }
        NPts++;
        }
    printf("%d points, %d equivs\n", i,j);
    mst();
    for(i=0; i<NPts; i++) {
        int ie, ee;
        mst_used(i, &ie, &ee);
        printf("Point %d used %d times by internal, %d times by external\n", 
        i, ie, ee);
        }
    j = 0;
    for(i=0; i<_MST_Nsoln; i++) {
        int x1, y1, x2, y2;
        void *p1, *p2;
        int st = mst_arc(i, &x1, &y1, &p1, &x2, &y2, &p2);
        if (st)
           j += abs(x1-x2) + abs(y1-y2);
        printf("%d to %d (%s)\n", (int)p1, (int)p2, st?"Normal":"Internal");
        }
    printf("total = %d\n", j);
    mst_package_done();
    return 0;
} 
/* double mst(double (*fn)(int,int,void*,int,int,void*) ) */


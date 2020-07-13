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






//created by Stefanus Mantik on 02/14/98  stefanus@cs.ucla.edu
#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#ifndef __PLACE_EVALUATOR_TABLE_FOR_WLEST_CXX__
#define __PLACE_EVALUATOR_TABLE_FOR_WLEST_CXX__

#include <math.h>
#include <algorithm>
#include "ABKCommon/abkassert.h"
#include "Geoms/bbox.h"
#include "placeEvalTables.h"

using std::min;
using std::max;

double lookupTable(const BBox & box, const unsigned & numPins)
{
    if(numPins <= 1) return 0.0;
    if(numPins <= 3) return box.getHalfPerim();

    unsigned pinId = numPins - 4;

    double width = box.xMax - box.xMin,
           height = box.yMax - box.yMin,
           maxSide = max(width, height),
           minSide = min(width, height);
    if (minSide == 0) return maxSide; 
        // one side is 0 means that the WL is the size of the other side

    double aspectRatio = maxSide / minSide;
    abkfatal(numPins <= 30,"number of pins exceed the threshold limit(30)");
    if (aspectRatio < 32)
    {
        unsigned aspectRatioId = unsigned(floor(aspectRatio) - 1);
        return minSide * (ERSMTTable[pinId][aspectRatioId]+
                          ((aspectRatio - floor(aspectRatio))*
                           (ERSMTTable[pinId][aspectRatioId+1]-
                            ERSMTTable[pinId][aspectRatioId])));
    }
    else return (box.getHalfPerim() + ERSMTTable[pinId][31] - 32);
}

extern const double ERSMTTable[28][32] = 
{
 {  2.12942,  3.17482,  4.19069,  5.19699,  6.20573,  7.19893,
    8.21274,  9.20484, 10.20989, 11.21051, 12.21770, 13.20783,
   14.20547, 15.21232, 16.21514, 17.21568, 18.21555, 19.21428,
   20.21872, 21.21840, 22.21460, 23.21450, 24.21853, 25.21443,
   26.22247, 27.20792, 28.22206, 29.21732, 30.21477, 31.21979,
   32.22113, 33.22117 },
 {  2.26240,  3.33733,  4.35881,  5.37226,  6.37646,  7.37748,
    8.39534,  9.39369, 10.40070, 11.39258, 12.39974, 13.40137,
   14.39643, 15.39754, 16.39722, 17.40516, 18.40308, 19.40747,
   20.40685, 21.40514, 22.40307, 23.40943, 24.41403, 25.41093,
   26.40391, 27.41251, 28.41158, 29.40001, 30.40535, 31.40474,
   32.40420, 33.40674 },
 {  2.39489,  3.50085,  4.53621,  5.55868,  6.57365,  7.57674,
    8.57987,  9.58780, 10.59098, 11.59970, 12.59637, 13.60245,
   14.60193, 15.60538, 16.60216, 17.61530, 18.60697, 19.60539,
   20.60495, 21.60797, 22.60079, 23.61556, 24.60466, 25.60995,
   26.59999, 27.61592, 28.60985, 29.61422, 30.60191, 31.61052,
   32.61465, 33.61113 },
 {  2.52169,  3.66161,  4.70584,  5.74342,  6.75529,  7.76270,
    8.77532,  9.78083, 10.78531, 11.79951, 12.79701, 13.79850,
   14.80397, 15.80290, 16.80254, 17.81509, 18.80208, 19.79902,
   20.80590, 21.81712, 22.80672, 23.81842, 24.80775, 25.81498,
   26.81688, 27.81740, 28.82241, 29.81904, 30.81852, 31.82063,
   32.82182, 33.82182 },
 {  2.64050,  3.81709,  4.88377,  5.92140,  6.93963,  7.96794,
    8.97480,  9.98281, 10.98553, 11.98832, 12.99228, 14.00001,
   14.99628, 15.99395, 16.99203, 18.01430, 19.01157, 20.00358,
   21.01476, 22.01299, 23.02380, 24.01959, 25.00779, 26.00714,
   27.02205, 28.02408, 29.01619, 30.02622, 31.02851, 32.02235,
   33.02521, 34.03016 },
 {  2.74504,  3.96924,  5.05824,  6.09777,  7.12756,  8.15390,
    9.16377, 10.17189, 11.18217, 12.19498, 13.19134, 14.20315,
   15.19978, 16.20734, 17.21343, 18.20972, 19.21556, 20.21686,
   21.22053, 22.22584, 23.22184, 24.22074, 25.23362, 26.22507,
   27.22820, 28.22321, 29.22814, 30.22953, 31.22924, 32.22938,
   33.23735, 34.23065 },
 {  2.85161,  4.10914,  5.21666,  6.27438,  7.31346,  8.32957,
    9.35590, 10.37172, 11.37027, 12.37556, 13.39362, 14.39453,
   15.40714, 16.41702, 17.41425, 18.41436, 19.42236, 20.41419,
   21.42393, 22.42885, 23.42042, 24.41530, 25.43497, 26.42445,
   27.44238, 28.42316, 29.42762, 30.43453, 31.44896, 32.43474,
   33.43236, 34.44008 },
 {  2.96382,  4.24608,  5.38377,  6.44378,  7.49037,  8.52496,
    9.53482, 10.56998, 11.57452, 12.59806, 13.59204, 14.60155,
   15.61468, 16.61088, 17.61483, 18.62355, 19.62810, 20.62075,
   21.63659, 22.62514, 23.62854, 24.64093, 25.64008, 26.64090,
   27.62873, 28.63807, 29.63499, 30.64295, 31.64313, 32.65229,
   33.65284, 34.64808 },
 {  3.05852,  4.38817,  5.53685,  6.61132,  7.66391,  8.70330,
    9.71867, 10.75357, 11.75842, 12.77235, 13.78767, 14.80144,
   15.80256, 16.81284, 17.80867, 18.82006, 19.83042, 20.82635,
   21.81840, 22.82779, 23.82250, 24.82705, 25.83750, 26.83632,
   27.83720, 28.85050, 29.84018, 30.84143, 31.84530, 32.86174,
   33.84969, 34.85049 },
 {  3.15125,  4.52011,  5.68651,  6.79116,  7.84102,  8.87679,
    9.91440, 10.94321, 11.95065, 12.96709, 13.97464, 14.97836,
   15.99832, 17.01064, 18.00555, 19.01542, 20.02829, 21.03081,
   22.02224, 23.04391, 24.05076, 25.03441, 26.04465, 27.05216,
   28.03970, 29.05954, 30.06292, 31.05173, 32.06082, 33.05741,
   34.04675, 35.08010 },
 {  3.23750,  4.65023,  5.84147,  6.94744,  8.02515,  9.06621,
   10.10051, 11.12364, 12.14951, 13.16379, 14.18269, 15.18226,
   16.20624, 17.21542, 18.19657, 19.21709, 20.22328, 21.23375,
   22.22662, 23.23777, 24.23776, 25.24475, 26.25564, 27.25860,
   28.23304, 29.25530, 30.26543, 31.26961, 32.26066, 33.26600,
   34.26319, 35.26654 },
 {  3.33277,  4.77053,  5.98552,  7.09648,  8.18008,  9.24902,
   10.28499, 11.30230, 12.33978, 13.36411, 14.35675, 15.36914,
   16.39121, 17.39499, 18.39683, 19.40786, 20.42319, 21.43490,
   22.43601, 23.44659, 24.44131, 25.44847, 26.44667, 27.45367,
   28.44982, 29.46652, 30.46296, 31.46645, 32.46081, 33.46832,
   34.47193, 35.47032 },
 {  3.41492,  4.89483,  6.12670,  7.25576,  8.34522,  9.41457,
   10.46385, 11.49334, 12.51762, 13.55817, 14.55932, 15.57834,
   16.58858, 17.59753, 18.60768, 19.60422, 20.61368, 21.63163,
   22.64679, 23.63432, 24.63568, 25.64210, 26.66427, 27.67160,
   28.66703, 29.67275, 30.68191, 31.65677, 32.67690, 33.66334,
   34.68184, 35.66722 },
 {  3.50141,  5.00681,  6.26801,  7.41949,  8.51096,  9.58896,
   10.64413, 11.67384, 12.70241, 13.72804, 14.74509, 15.77340,
   16.77749, 17.79616, 18.80394, 19.81869, 20.81794, 21.83675,
   22.83093, 23.83663, 24.83860, 25.84202, 26.84848, 27.85928,
   28.87384, 29.86051, 30.87141, 31.86710, 32.86682, 33.89239,
   34.89605, 35.88142 },
 {  3.58854,  5.12624,  6.40376,  7.57655,  8.67850,  9.76014,
   10.81802, 11.84833, 12.86721, 13.92305, 14.94522, 15.96455,
   16.96438, 17.99177, 19.01429, 19.99730, 20.99963, 22.03046,
   23.03880, 24.03269, 25.04161, 26.04838, 27.05688, 28.06408,
   29.06513, 30.05693, 31.07698, 32.07890, 33.09295, 34.09367,
   35.08183, 36.08920 },
 {  3.66937,  5.24333,  6.54311,  7.71715,  8.84258,  9.91124,
   10.97850, 12.03651, 13.05969, 14.09910, 15.12121, 16.14902,
   17.16408, 18.18992, 19.19152, 20.21243, 21.22022, 22.22142,
   23.23063, 24.23758, 25.24249, 26.24531, 27.26006, 28.28139,
   29.27099, 30.26923, 31.27741, 32.27708, 33.28627, 34.29137,
   35.29025, 36.30345 },
 {  3.74561,  5.34149,  6.68068,  7.87283,  8.99449, 10.09557,
   11.15654, 12.21528, 13.25792, 14.27711, 15.31518, 16.33292,
   17.33877, 18.36338, 19.37802, 20.38989, 21.41120, 22.41808,
   23.41555, 24.43143, 25.44057, 26.45370, 27.47893, 28.46176,
   29.47262, 30.47584, 31.47754, 32.47747, 33.49367, 34.49236,
   35.49839, 36.48839 },
 {  3.81739,  5.46279,  6.79453,  8.01382,  9.15298, 10.24170,
   11.32715, 12.38267, 13.42030, 14.45803, 15.50489, 16.52348,
   17.53719, 18.56537, 19.57773, 20.58216, 21.60400, 22.62555,
   23.62993, 24.62864, 25.64084, 26.64881, 27.64456, 28.64961,
   29.66573, 30.67089, 31.68247, 32.69199, 33.67455, 34.68654,
   35.69561, 36.71333 },
 {  3.89147,  5.55373,  6.92165,  8.14566,  9.30385, 10.41826,
   11.49923, 12.56791, 13.60555, 14.65453, 15.67482, 16.70909,
   17.72743, 18.73952, 19.76962, 20.78845, 21.79206, 22.79845,
   23.80796, 24.82919, 25.84622, 26.83892, 27.86716, 28.86908,
   29.85376, 30.87523, 31.87762, 32.88133, 33.89453, 34.87957,
   35.90416, 36.91163 },
 {  3.96543,  5.66668,  7.05579,  8.29153,  9.45685, 10.55711,
   11.66004, 12.72779, 13.78417, 14.82687, 15.84714, 16.89460,
   17.91475, 18.95140, 19.96376, 20.98148, 21.99333, 22.99274,
   24.02628, 25.03224, 26.05683, 27.04029, 28.05661, 29.06590,
   30.06088, 31.07913, 32.08681, 33.09236, 34.10392, 35.11322,
   36.10599, 37.10658 },
 {  4.03395,  5.76623,  7.16297,  8.42470,  9.59779, 10.72257,
   11.81908, 12.88205, 13.96029, 14.98597, 16.04922, 17.07373,
   18.10499, 19.12613, 20.14762, 21.15156, 22.19112, 23.18881,
   24.20825, 25.23637, 26.23592, 27.23654, 28.24403, 29.27927,
   30.28221, 31.27476, 32.27399, 33.27022, 34.28724, 35.28981,
   36.28970, 37.31618 },
 {  4.10470,  5.86677,  7.28738,  8.55433,  9.75546, 10.88802,
   11.98445, 13.05721, 14.11593, 15.17013, 16.22717, 17.25249,
   18.28566, 19.31501, 20.33261, 21.36755, 22.37868, 23.37829,
   24.39706, 25.41636, 26.42995, 27.43355, 28.42446, 29.46213,
   30.45397, 31.46501, 32.48968, 33.48782, 34.48402, 35.49353,
   36.51516, 37.51342 },
 {  4.17528,  5.94257,  7.40931,  8.68943,  9.90079, 11.05002,
   12.14686, 13.23528, 14.29145, 15.35044, 16.39244, 17.43700,
   18.47688, 19.49823, 20.51017, 21.55551, 22.57719, 23.58789,
   24.60411, 25.61653, 26.59793, 27.62486, 28.63106, 29.67191,
   30.66724, 31.68057, 32.68452, 33.69825, 34.67902, 35.69986,
   36.70096, 37.71157 },
 {  4.24320,  6.05796,  7.52300,  8.82501, 10.03864, 11.18562,
   12.30808, 13.37693, 14.47957, 15.54143, 16.57363, 17.63341,
   18.65598, 19.66778, 20.70751, 21.74289, 22.75425, 23.76897,
   24.78877, 25.80008, 26.81614, 27.84229, 28.84223, 29.85826,
   30.85508, 31.87829, 32.88247, 33.87387, 34.90811, 35.90443,
   36.90361, 37.90043 },
 {  4.30620,  6.14084,  7.64417,  8.96024, 10.17845, 11.34377,
   12.46345, 13.57146, 14.64853, 15.70181, 16.76275, 17.80310,
   18.84599, 19.86226, 20.88948, 21.92561, 22.94287, 23.96673,
   24.99225, 25.99319, 27.00743, 28.01716, 29.04400, 30.06036,
   31.05471, 32.05959, 33.07117, 34.07927, 35.08883, 36.09320,
   37.10245, 38.11476 },
 {  4.37562,  6.23038,  7.75121,  9.07543, 10.32140, 11.50078,
   12.61161, 13.72487, 14.80566, 15.87720, 16.92832, 17.98407,
   19.00425, 20.02515, 21.09021, 22.10532, 23.10666, 24.15676,
   25.16315, 26.19020, 27.19676, 28.22330, 29.23551, 30.23346,
   31.25179, 32.26163, 33.25566, 34.28221, 35.29904, 36.29744,
   37.32750, 38.31281 },
 {  4.43959,  6.32859,  7.86044,  9.20039, 10.46335, 11.64954,
   12.77599, 13.89044, 14.96935, 16.03667, 17.09348, 18.15226,
   19.18460, 20.22655, 21.26287, 22.27838, 23.32610, 24.36190,
   25.36003, 26.39073, 27.39664, 28.43734, 29.41397, 30.42889,
   31.45255, 32.44796, 33.49642, 34.47632, 35.49371, 36.48769,
   37.49720, 38.51638 }
};

extern const double ChengTable[51]=
{
                0.0,
                1.000000, 1.000000, 1.000000, 1.082797, 1.153598,
                1.220592, 1.282322, 1.338501, 1.399094, 1.449260,
                1.497380, 1.545500, 1.593619, 1.641739, 1.689859,
                1.730376, 1.770893, 1.811410, 1.851927, 1.892444,
                1.928814, 1.965184, 2.001553, 2.037923, 2.074293,
                2.106117, 2.137941, 2.169766, 2.201590, 2.233414,
                2.264622, 2.295830, 2.327038, 2.358246, 2.389454,
                2.418676, 2.447898, 2.477119, 2.506341, 2.535563,
                2.560954, 2.586346, 2.611737, 2.637129, 2.662520,
                2.688684, 2.714848, 2.741013, 2.767177, 2.793341
};

#endif

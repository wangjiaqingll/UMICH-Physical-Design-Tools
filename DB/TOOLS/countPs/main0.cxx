#include "DB.h"


main(int argc, char *argv[]) 
    {

    StringParam auxFileName("f", argc, argv);
    abkfatal(auxFileName.found(),"Usage: prog -f filename.aux");
    DB db(auxFileName);

    unsigned nPads=0,nPIsExclusive=0,nPOsExclusive=0,nPIOs=0,
             nPadsOther=0,nPadsUnknown=0,nPadsPower=0,nPadsSpacer=0;

    const dbNetlist &netlist = db.getNetlist();

    itCellGlobal iC;

    for (iC=netlist.cellsBegin();iC!=netlist.cellsEnd();iC++)
        {
        dbCell &cell = *(*iC);
        dbMasterCell const &mCell = cell.getMaster();

        if (mCell.isPad())
            {
            nPads++;
            if (cell.isPIO())
                nPIOs++;
            else
                {
                if (cell.isPI())
                    nPIsExclusive++;
                if (cell.isPO())
                    nPOsExclusive++;
                abkfatal(!cell.isPI() || !cell.isPO(),
                         "cell both PI and PO but not PIO");
                }

            if (cell.isPOther())
                nPadsOther++;
            if (cell.isPUnknown())
                nPadsUnknown++;
            if (mCell.isPPower())
                nPadsPower++;
            if (mCell.isPSpacer())
                nPadsSpacer++;


            }
        }

    cout << "Design " << db.getDesignName() << ":" << endl;
    cout << "   Total pads: " << nPads << endl;
    cout << "       PIOs  :           " << nPIOs << endl;
    cout << "       PIs (exclusive) : " << nPIsExclusive << endl;
    cout << "       POs (exclusive) : " << nPOsExclusive << endl;
    cout << "       POthers:        : " << nPadsOther << endl;
    cout << "       PUnknowns:      : " << nPadsUnknown << endl;
    cout << "           (power)     : " << nPadsPower   << endl;
    cout << "           (spacer)    : " << nPadsSpacer << endl;

    return 0;
    }
	

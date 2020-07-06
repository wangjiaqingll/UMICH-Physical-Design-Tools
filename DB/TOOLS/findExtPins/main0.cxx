#include "DB.h"


main(int argc, char *argv[]) 
    {

    StringParam auxFileName("f", argc, argv);
    abkfatal(auxFileName.found(),"Usage: prog -f filename.aux");
    DB db(auxFileName);

    const dbNetlist &netlist = db.getNetlist();

    itCellGlobal iC;

    for (iC=netlist.cellsBegin();iC!=netlist.cellsEnd();iC++)
        {
        dbCell const &cell = *(*iC);
        dbMasterCell const &mCell = cell.getMaster();

        if (mCell.isExtPin())
            {
            cout << "Cell: " << cell.getName() << endl;
            cout << "  Cell index:   " << cell.getIndex() << endl;
            cout << "  Master cell:  " << mCell.getName() << endl;
            dbMasterPin const &pin = *(*(mCell.pinsBegin()));
            cout << "  Master pin:   " << pin.getName() << endl;
            dbSite const &site = mCell.getSite();
            cout << "  Site:         " << site.getName() << endl << endl;
            }
        }


    return 0;
    }
	

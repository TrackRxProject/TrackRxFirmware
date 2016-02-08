/*
 * flash.c
 *
 *  Created on: Feb 7, 2016
 *      Author: Jake
 */

#include <stdlib.h>
#include <string.h>

// Simplelink includes
#include "simplelink.h"

//Driverlib includes
#include "hw_types.h"
#include "hw_ints.h"
#include "rom.h"
#include "rom_map.h"
#include "interrupt.h"
#include "prcm.h"

#include "flash.h"

int writeInterval_flash(float interval)
{
	return 0;
}

static long WriteFileToDevice(unsigned long *ulToken, long *lFileHandle, unsigned char * fileName)
{
    long lRetVal = -1;
    int iLoopCnt = 0;

    //
    //  create a user file
    //
    lRetVal = sl_FsOpen(fileName,
                FS_MODE_OPEN_CREATE(65536, \
                          _FS_FILE_OPEN_FLAG_COMMIT|_FS_FILE_PUBLIC_WRITE),
                        ulToken,
                        lFileHandle);
    if(lRetVal < 0)
    {
        //
        // File may already be created
        //
        lRetVal = sl_FsClose(*lFileHandle, 0, 0, 0);
        return -1; //TODO error code
    }
    else
    {
        //
        // close the user file
        //
        lRetVal = sl_FsClose(*lFileHandle, 0, 0, 0);
        if (SL_RET_CODE_OK != lRetVal)
        {
            return -1; //TODO error code
        }
    }

    //
    //  open a user file for writing
    //
    lRetVal = sl_FsOpen(fileName,
                        FS_MODE_OPEN_WRITE,
                        ulToken,
                        lFileHandle);
    if(lRetVal < 0)
    {
        lRetVal = sl_FsClose(*lFileHandle, 0, 0, 0);
        return -1; //TODO error code
    }


    //
    // close the user file
    //
    lRetVal = sl_FsClose(*lFileHandle, 0, 0, 0);
    if (SL_RET_CODE_OK != lRetVal)
    {
        return -1; //TODO Error code
    }

    return 0;
}

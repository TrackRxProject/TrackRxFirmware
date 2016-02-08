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

#include "trackrxfirmware.h"

long writeInterval_flash(int interval)
{
	unsigned char charArray[4];
	intToCharArray(interval, 4, charArray);
	return writeFileToDevice("interval", 0, charArray, 4);
}

static long writeFileToDevice(unsigned char * fileName,
								unsigned int offset,
								unsigned char * writeBuff,
								int length)
{
	long lFileHandle;
	unsigned long ulToken;

    long lRetVal = -1;

    //
    //  open a user file for writing
    //
    lRetVal = sl_FsOpen(fileName,
                        FS_MODE_OPEN_WRITE,
                        &ulToken,
                        &lFileHandle);

    if (lRetVal == SL_FS_ERR_FILE_NOT_EXISTS)
    {
    	//
    	//  if the file does not exist, create it
    	//
    	lRetVal = sl_FsOpen(fileName,
    	             FS_MODE_OPEN_CREATE(65536, \
    	                     _FS_FILE_OPEN_FLAG_COMMIT|_FS_FILE_PUBLIC_WRITE),
    	                     &ulToken,
    	                     &lFileHandle);

    	if(lRetVal < 0)
    	{
    	    lRetVal = sl_FsClose(lFileHandle, 0, 0, 0);
    	    return -1; //TODO error code
    	}
    	else
    	{
    	    //
    	    // close the user file
    	    //
    	    lRetVal = sl_FsClose(lFileHandle, 0, 0, 0);
    	    if (SL_RET_CODE_OK != lRetVal)
    	    {
    	        return -1; //TODO error code
    	    }
    	}
    }
    else if(lRetVal < 0)
    {
        lRetVal = sl_FsClose(lFileHandle, 0, 0, 0);
        return -1; //TODO error code
    }


    lRetVal = sl_FsWrite(lFileHandle, offset, writeBuff, length);
    if (lRetVal < 0)
    {
    	long oldRetVal = lRetVal;
    	lRetVal = sl_FsClose(lFileHandle,0,0,0);
    	return oldRetVal;
    }

    //
    // close the user file
    //
    lRetVal = sl_FsClose(lFileHandle, 0, 0, 0);
    if (SL_RET_CODE_OK != lRetVal)
    {
        return -1; //TODO Error code
    }

    return 0;
}

int readInterval_flash()
{
	int interval;
	unsigned char readBuff[4];
	readFileFromDevice("interval", 0, readBuff, 4); // TODO error handle
	return charArrayToInt(readBuff,4);
}

static long readFileFromDevice(unsigned char * fileName,
								unsigned int offset,
								unsigned char * readBuff,
								int length)
{
	unsigned long ulToken;
	long lFileHandle;
    long lRetVal = -1;
    int iLoopCnt = 0;

    //
    // open a user file for reading
    //
    lRetVal = sl_FsOpen(fileName,
                        FS_MODE_OPEN_READ,
                        &ulToken,
                        &lFileHandle);
    if(lRetVal < 0)
    {
        lRetVal = sl_FsClose(lFileHandle, 0, 0, 0);
        return lRetVal;
    }

    //
    // read the data
    //
    lRetVal = sl_FsRead(lFileHandle, offset, readBuff, length);
    if ((lRetVal < 0) || (lRetVal != length))
    {
        lRetVal = sl_FsClose(lFileHandle, 0, 0, 0);
        return lRetVal;
    }

    //
    // close the user file
    //
    lRetVal = sl_FsClose(lFileHandle, 0, 0, 0);
    if (SL_RET_CODE_OK != lRetVal)
    {
        return lRetVal;
    }

    return 0;
}

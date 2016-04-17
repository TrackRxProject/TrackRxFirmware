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
    	             FS_MODE_OPEN_CREATE(3584, \
    	                     _FS_FILE_OPEN_FLAG_COMMIT|_FS_FILE_PUBLIC_WRITE),
    	                     &ulToken,
    	                     &lFileHandle);

    	if(lRetVal < 0)
    	{
    	    lRetVal = sl_FsClose(lFileHandle, 0, 0, 0);
    	    return lRetVal;
    	}
    	else
    	{
    	    //
    	    // close the user file
    	    //
    	    lRetVal = sl_FsClose(lFileHandle, 0, 0, 0);
    	    if (SL_RET_CODE_OK != lRetVal)
    	    {
    	        return lRetVal;
    	    }
    	    // Now, open it for writing
    	    lRetVal = sl_FsOpen(fileName,
    	                            FS_MODE_OPEN_WRITE,
    	                            &ulToken,
    	                            &lFileHandle);
    	    if (SL_RET_CODE_OK != lRetVal)
    	    {
    	    	return lRetVal;
    	    }
    	}
    }
    else if(lRetVal < 0)
    {
        lRetVal = sl_FsClose(lFileHandle, 0, 0, 0);
        return lRetVal;
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
        return lRetVal;
    }

    return 0;
}

static long readFileFromDevice(unsigned char * fileName,
								unsigned int offset,
								unsigned char * readBuff,
								int length)
{
	unsigned long ulToken;
	long lFileHandle;
    long lRetVal = -1;

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

//-----------------------------------------------------------------------------
// Following methods are used externally as the flash API
//-----------------------------------------------------------------------------

/* Return an integer representation of the dosage interval */
int readInterval_flash()
{
	unsigned char readBuff[4];
	readFileFromDevice("interval", 0, readBuff, 4); // TODO error handle
	return charArrayToInt(readBuff,4);
}

/* Write the integer representation of the dosage interval to flash */
long writeInterval_flash(int interval)
{
	unsigned char charArray[4];
	intToCharArray(interval, 4, charArray);
	return writeFileToDevice("interval", 0, charArray, 4);
}

long writeUUID_flash(unsigned char * uuid)
{
	return writeFileToDevice("uuid", 0, uuid, UUID_LENGTH);
}

long readUUID_flash(unsigned char * uuid)
{
	return readFileFromDevice("uuid", 0, uuid, UUID_LENGTH);
}

long writeSSID_flash(unsigned char * ssid, unsigned char length)
{
	int ret = writeFileToDevice("ssid", 0, ssid, length);
	if (ret < 0)
		return ret;
	return writeFileToDevice("ssidlen", 0, &length, 1);
}

long readSSID_flash(unsigned char * ssid)
{
	unsigned char length;
	int ret = readFileFromDevice("ssidlen", 0, &length, 1);
	if (ret < 0)
		return ret;
	return readFileFromDevice("ssid", 0, ssid, length);
}

long readSSIDLen_flash(unsigned char * length)
{
	return readFileFromDevice("ssidlen", 0, &length, 1);
}

long writeSecurityKey_flash(unsigned char * pw, unsigned char length)
{
	int ret = writeFileToDevice("pwlen", 0, &length, 1);
	if (ret < 0)
		return ret;
	return writeFileToDevice("pw", 0, pw, length);
}

long readSecurityKey_flash(unsigned char * pw)
{
	unsigned char length;
	int ret = readFileFromDevice("pwlen", 0, &length, 1);
	if (ret < 0)
		return ret;
	return readFileFromDevice("pw", 0, pw, length);
}

long initAdherence_flash()
{
	return writeFileToDevice("history", 0, 0, 1);
}

long writeAdherence_flash(unsigned char adhereBool)
{
	unsigned char adherence[ADHERENCE_LENGTH];
	int ret = readAdherenceHistory_flash(adherence);
	if (ret<0)
		return ret;
	unsigned char length;
	ret = readFileFromDevice("historylen", 0, &length, 1);
	if (ret < 0)
		return ret;
	adherence[length] = adhereBool;
	length++;
	ret = writeFileToDevice("history", 0, adherence, length);
	if (ret < 0)
		return ret;
	return writeFileToDevice("historylen", 0, &length, 1);
}

long readAdherenceHistory_flash(unsigned char * history)
{
	unsigned char length;
	int ret = readFileFromDevice("historylen", 0, &length, 1);
	if (ret < 0)
		return ret;
	return readFileFromDevice("history", 0, history, length);
}

long readHistoryLength_flash(unsigned char * length)
{
	return readFileFromDevice("historylen", 0, length, 1);
}

long writeHistoryLength_flash(unsigned char * length)
{
	return writeFileToDevice("historylen", 0, length, 1);
}

long readActivationFlag_flash(unsigned char * activationFlag)
{
	return readFileFromDevice("activation", 0, activationFlag, 1);
}

long writeActivationFlag_flash(unsigned char * activationFlag)
{
	return writeFileToDevice("activation", 0, activationFlag, 1);
}

long readMissingDose_flash(unsigned char * missingDoseFlag)
{
	return readFileFromDevice("missingdose", 0, missingDoseFlag, 1);
}

long writeMissingDose_flash(unsigned char * missingDoseFlag)
{
	return writeFileToDevice("missingdose", 0, missingDoseFlag, 1);
}

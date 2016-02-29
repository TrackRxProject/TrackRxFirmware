/*
 * flash.h
 *
 *  Created on: Feb 7, 2016
 *      Author: Jake
 */

#ifndef FLASH_H_
#define FLASH_H_

#define SL_MAX_FILE_SIZE        64L*1024L       /* 64KB file */
#define BUF_SIZE                2048

/* Application specific status/error codes */
typedef enum{
    // Choosing this number to avoid overlap w/ host-driver's error codes
    FILE_ALREADY_EXIST = -0x7D0,
    FILE_CLOSE_ERROR = FILE_ALREADY_EXIST - 1,
    FILE_NOT_MATCHED = FILE_CLOSE_ERROR - 1,
    FILE_OPEN_READ_FAILED = FILE_NOT_MATCHED - 1,
    FILE_OPEN_WRITE_FAILED = FILE_OPEN_READ_FAILED -1,
    FILE_READ_FAILED = FILE_OPEN_WRITE_FAILED - 1,
    FILE_WRITE_FAILED = FILE_READ_FAILED - 1,

    STATUS_CODE_MAX = -0xBB8
}e_AppStatusCodes;

static long writeFileToDevice(unsigned char * fileName,
								unsigned int offset,
								unsigned char * writeBuff,
								int length);
static long readFileFromDevice(unsigned char * fileName,
								unsigned int offset,
								unsigned char * readBuff,
								int length);

int readInterval_flash();
long writeInterval_flash(int interval);
long writeUUID_flash(unsigned char * uuid);
long readUUID_flash(unsigned char * uuid);
long writeSSID_flash(unsigned char * ssid, unsigned char length);
long readSSID_flash(unsigned char * ssid);
long writeAdherence_flash(unsigned char adhereBool);
long readHistoryLength_flash(unsigned char * length)
long readAdherenceHistory_flash(unsigned char * history);
long writeHistoryLength_flash(unsigned char * length)
long readSecurityKey_flash(unsigned char * pw);
long writeSecurityKey_flash(unsigned char * pw, unsigned char length);
long readActivationFlag_flash(unsigned char * activationFlag);
long writeActivationFlag_flash(unsigned char * activationFlag);


#endif /* FLASH_H_ */

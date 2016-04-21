/*
 * http.h
 *
 *  Created on: Mar 3, 2016
 *      Author: Jake
 */

#ifndef HTTP_H_
#define HTTP_H_

void postActivate_http();
int getInterval_http();
void postAdherence_http();
void putAdherence_http(unsigned char * data, int length);
int checkAuthorization_http();
void notify_http();
int SmartConfigConnect_http();

#endif /* HTTP_H_ */

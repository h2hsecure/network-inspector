/*
 * test_endian.c
 *
 *  Created on: 20 Oct 2016
 *      Author: root
 */
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>

static uint8_t myip[] = {0xa,0x14,0x28,0xa};


static uint32_t
bytetoint(u_int8_t ip[]) {
	uint32_t i=0;

	memcpy(&i, ip, 4);

	return i;
}

int
main2(int argc, char *argv[])
{
	printf("r:%x\n", bytetoint(myip));

}

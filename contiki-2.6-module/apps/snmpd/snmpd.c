/* -----------------------------------------------------------------------------
 * SNMP implementation for Contiki
 *
 * Copyright (C) 2010 Siarhei Kuryla <kurilo@gmail.com>
 *
 * This program is part of free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * 09-07-2012 added RFC3414 confrom non volatile snmpEngineBoots functionality -> see lines marked with "sz"
 * 16-07-2012 added pseudo random value generation for 64bit integer at boot time, used to generate IV in AES
 * algorithm. defined in RFC3826 -> see lines marked with "sz"
 * Sven Zehl - sven@zehl.co.cc
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "contiki.h"

#include "snmpd.h"
#include "snmpd-conf.h"
#include "dispatcher.h"
#include "mib-init.h"
#include "logging.h"
#include "keytools.h"
/*sz*/
#include <avr/eeprom.h>
#include <avr/io.h>
/*sz*/

/*sz*/
/*Enable Printf() Debugging*/
/** \brief Enable Printf() Debugging*/
#define PDEBUG 1
/*sz*/

#define CHECK_STACK_SIZE 0

#define UDP_IP_BUF   ((struct uip_udpip_hdr *)&uip_buf[UIP_LLH_LEN])

/* UDP connection */
static struct uip_udp_conn *udpconn;

/*sz*/
#if ENABLE_SNMPv3
u32t EEMEM MsgAuthoritativeEngineBoots = 0;
u32t privacyLow;
u32t privacyHigh;
#endif
/*sz*/

PROCESS(snmpd_process, "SNMP daemon process");

/*sz*/
#if ENABLE_SNMPv3
/** \brief function for incrementing the value of msgAuthoritativeEngineBoots, called at startup */
u8t incMsgAuthoritativeEngineBoots()
{
	/*Increments the Value of MsgAuthoritativeEngineBoots when booting.*/
	/*Checks if the maximum value of 2147483647 (RFC3414) is reached.*/
	if((eeprom_read_dword(&MsgAuthoritativeEngineBoots))<2147483647)
	{
		eeprom_update_dword(&MsgAuthoritativeEngineBoots, (eeprom_read_dword(&MsgAuthoritativeEngineBoots)+1));
	}
	else
	{
#if PDEBUG
		printf("Maximum Number of MsgAuthoritativeEngineBoots reached, please reconfigure all secret values and reinstall SNMP Agent\n");
#endif
	}
#if PDEBUG
	printf("MsgAuthoritativeEngineBoots = %lu\n",eeprom_read_dword(&MsgAuthoritativeEngineBoots));
#endif

	return 0;
}
#endif
/*sz*/

#if CONTIKI_TARGET_AVR_RAVEN || CONTIKI_TARGET_AVR_ZIGBIT
extern unsigned long seconds;
#else
clock_time_t systemStartTime;
#endif

u32t getSysUpTime()
{
    #if CONTIKI_TARGET_AVR_RAVEN || CONTIKI_TARGET_AVR_ZIGBIT
		/*sz*/
		//Check if the Maximum value of AuthoritativeEngineTime mentioned in RFC3414
		//is reached. If the value is greater, reset seconds and increment AuthoritativeEngineBoots
		if((seconds*100)>2147483647)
		{
		seconds=0;
		#if ENABLE_SNMPv3
		incMsgAuthoritativeEngineBoots();
		#endif
		}
		/*sz*/
        return seconds * 100;
    #else
        return (clock_time() - systemStartTime)/ 10;
    #endif
}

/*sz*/
#if ENABLE_SNMPv3
/** \brief function that returns the actual value ofmsgAuthoritativeEngineBoots*/
u32t getMsgAuthoritativeEngineBoots()
{
	u32t returnParameter;
	returnParameter = eeprom_read_dword(&MsgAuthoritativeEngineBoots);
    return returnParameter;
}
#endif
/*sz*/

/*sz*/
#if ENABLE_SNMPv3
/** \brief generates the seed for the random function that generates the pseudo random 64Bit value for the IV, used in the usm*/
u32t get_seed()
{
   u32t seed = 0;
   u32t *p = (u32t*) (RAMEND+1);
   extern u32t __heap_start;
#if PDEBUG
   printf("RAMEND: %X\n__heap_start: %X",RAMEND, &__heap_start);
#endif
   while (p >= &__heap_start + 1)
      seed ^= * (--p);

   return seed;
}

u32t getLPrivacyParameters()
{
    privacyLow++;
    return privacyLow;
}

u32t getHPrivacyParameters()
{
    privacyHigh++;
    return privacyHigh;
}
#endif
/*sz*/





#if CHECK_STACK_SIZE
int max = 0;
u32t* marker;
#endif

/*-----------------------------------------------------------------------------------*/
/*
 * UDP handler.
 */
static void udp_handler(process_event_t ev, process_data_t data)
{
    u8t response[MAX_BUF_SIZE];
    u16t resp_len;
    #if CHECK_STACK_SIZE
    memset(response, 0, sizeof(response));
	printf("Speicherplatz besorgt\n");
    #endif

    #if DEBUG && (CONTIKI_TARGET_AVR_RAVEN || CONTIKI_TARGET_AVR_ZIGBIT)
    u8t request[MAX_BUF_SIZE];
    u16t req_len;
    #endif /* DEBUG && CONTIKI_TARGET_AVR_RAVEN */
    if (ev == tcpip_event && uip_newdata()) {
        #if INFO
            uip_ipaddr_t ripaddr;
            u16_t rport;
            uip_ipaddr_copy(&ripaddr, &UDP_IP_BUF->srcipaddr);
            rport = UDP_IP_BUF->srcport;
        #else
            uip_ipaddr_copy(&udpconn->ripaddr, &UDP_IP_BUF->srcipaddr);
            udpconn->rport = UDP_IP_BUF->srcport;
        #endif

        #if DEBUG && (CONTIKI_TARGET_AVR_RAVEN || CONTIKI_TARGET_AVR_ZIGBIT)
        req_len = uip_datalen();
        memcpy(request, uip_appdata, req_len);
        if (dispatch(request, &req_len, response, resp_len, MAX_BUF_SIZE) != ERR_NO_ERROR) {
            udpconn->rport = 0;
            memset(&udpconn->ripaddr, 0, sizeof(udpconn->ripaddr));
            return;
        }
        #else
		#if PDEBUG
			printf("UDP Handler: Info: handler started, recognized new paket\n");
		#endif
        if (dispatch((u8t*)uip_appdata, uip_datalen(), response, &resp_len, MAX_BUF_SIZE) != ERR_NO_ERROR) {
            udpconn->rport = 0;
            memset(&udpconn->ripaddr, 0, sizeof(udpconn->ripaddr));
            return;
        }

        #endif /* DEBUG && CONTIKI_TARGET_AVR_RAVEN */

        #if CHECK_STACK_SIZE
		printf("z�hle Speicher ab\n");
        u32t *p = marker - 1;
        u16t i = 0;
        while (*p != 0xAAAAAAAA || *(p - 1) != 0xAAAAAAAA || *(p - 2) != 0xAAAAAAAA) {
            i+=4;
            p--;
        }
        if (i > max) {
            max = i;
        }
        //snmp_info(" %d", max);
		printf("Check Stacksize: %d\n",max);
        #endif


        #if INFO
            uip_ipaddr_copy(&udpconn->ripaddr, &ripaddr);
            udpconn->rport = rport;
        #endif

        uip_udp_packet_send(udpconn, response, resp_len);

        memset(&udpconn->ripaddr, 0, sizeof(udpconn->ripaddr));
        udpconn->rport = 0;
    }
}
/*-----------------------------------------------------------------------------------*/

#include "md5.h"

/*-----------------------------------------------------------------------------------*/
/*
 *  Entry point of the SNMP server.
 */
PROCESS_THREAD(snmpd_process, ev, data) {
	PROCESS_BEGIN();

        #if !(CONTIKI_TARGET_AVR_RAVEN || CONTIKI_TARGET_AVR_ZIGBIT)
        systemStartTime = clock_time();
        #endif
	
	/*Switch LED on @ANY Brick On if SNMPD started and set other LEDs as Output*/
	DDRB |= (1 << PIN5);
	DDRB |= (1 << PIN6);
	DDRB |= (1 << PIN7);
	PORTB &= ~(1 << PIN7);
	PORTB |= (1 << PIN6);
	PORTB |= (1 << PIN5);

	/*END LED @ANY Brick*/

	
        #if CHECK_STACK_SIZE
		printf("Starte f�llen mit Bit Pattern\n");
        u16t i = 0;
        u32t pointer;
        u32t* p = &pointer;
        for (i = 0; i < 500; i++) {
            *p = 0xAAAAAAAA;
            p--;
        }
        marker = &pointer;
        #endif

	udpconn = udp_new(NULL, UIP_HTONS(0), NULL);
	udp_bind(udpconn, UIP_HTONS(LISTEN_PORT));

	/*sz*/
	#if ENABLE_SNMPv3
	incMsgAuthoritativeEngineBoots();
	#endif
	/*sz*/

	/*sz*/
	#if ENABLE_SNMPv3
	srandom(get_seed());
	privacyLow = random();
	//srandom(get_seed());
	privacyHigh = random();
#if PDEBUG
	printf("Privacy 32Bit Pseudo Random Number: \n%u \n%u\n",privacyLow, privacyHigh);
#endif
	#endif
	/*sz*/


        /* init MIB */
        if (mib_init() != -1) {

            while(1) {
                PROCESS_YIELD();
                udp_handler(ev, data);
            }
        } else {
            snmp_log("error occurs while initializing the MIB\n");
        }
	PROCESS_END();
}

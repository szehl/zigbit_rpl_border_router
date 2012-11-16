/* -----------------------------------------------------------------------------
 * SNMP implementation for Contiki
 *
 * Copyright (C) 2010 Siarhei Kuryla
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

/**
 * \file
 *         Configuration of the SNMP protocol.
 * \author
 *         Siarhei Kuryla <kurilo@gmail.com>
 */

#ifndef __SNMP_CONF_H__
#define	__SNMP_CONF_H__

#include "snmpd-types.h"

#define ENABLE_SNMPv1   0

#define ENABLE_SNMPv3   1

#define ENABLE_PRIVACY  1

#define ENABLE_AUTH     1

/** \brief maximum length of an SNMP message. */
#define MAX_BUF_SIZE    484

#define TIME_WINDOW     150

#define CHECK_STACK_SIZE 0

/** \brief community string. */
#define COMMUNITY_STRING        "public"

#if ENABLE_SNMPv3
	/*sz*/
    /*u32t getMsgAuthoritativeEngineBoots();*/
	/*moved to snmpd.h*/
	/*sz*/

    ptr_t* getEngineID();

    u8t* getUserName();
	/*sz*/
    /*u32t getLPrivacyParameters();*/
    /*u32t getHPrivacyParameters();*/
	/*moved to snmpd.h*/
	/*sz*/
#endif

#endif	/* __SNMP_CONF_H__ */


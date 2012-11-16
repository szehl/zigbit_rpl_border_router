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
 */

/**
 * \file
 *         Defines an SNMP agent process.
 * \author
 *         Siarhei Kuryla <kurilo@gmail.com>
 */


#ifndef __SNMPD_H__
#define __SNMPD_H__

#include "contiki-net.h"
#include "snmpd-types.h"

/** \brief port listened by the SNMP agent */
#define LISTEN_PORT 161

/** \brief SNMP agent process. */
PROCESS_NAME(snmpd_process);

/** \brief Time in seconds since the system started. */
u32t getSysUpTime();

/*sz*/
#if ENABLE_SNMPv3
/** \brief Value of SNMP Engine Boots used for USM */
u32t getMsgAuthoritativeEngineBoots();
#endif
/*sz*/

/*sz*/
/** \brief Returns the lower part of the 64 Bit pseudo random integer variable, generated at startup*/
u32t getLPrivacyParameters();
/** \brief Returns the higher part of the 64 Bit pseudo random integer variable, generated at startup*/
u32t getHPrivacyParameters();
/*sz*/


#endif /* __SNMPD_H__ */

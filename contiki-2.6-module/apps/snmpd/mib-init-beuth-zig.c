#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "mib-init.h"
#include "ber.h"
#include "utils.h"
#include "logging.h"
#include "dispatcher.h"
#include "radio.h"
#include "rf230bb.h"
#include <avr/io.h>
#include "snmpd.h"
#include "snmpd-types.h"

#if (CONTIKI_TARGET_AVR_RAVEN || CONTIKI_TARGET_AVR_ZIGBIT) && ENABLE_PROGMEM
#include <avr/pgmspace.h>
#else
#define PROGMEM
#endif

/*sz*/
/*Enable Printf() Debugging*/
/** \brief Enable Printf() Debugging*/
#define PDEBUG 1
/*sz*/

/* common oid prefixes*/
static u8t ber_oid_system_desc[] PROGMEM  				= {0x2b, 0x06, 0x01, 0x02, 0x01, 0x01, 0x01, 0x00};
static ptr_t oid_system_desc PROGMEM      				= {ber_oid_system_desc, 8};
static u8t ber_oid_system_time[] PROGMEM  				= {0x2b, 0x06, 0x01, 0x02, 0x01, 0x01, 0x03, 0x00};
static ptr_t oid_system_time PROGMEM      				= {ber_oid_system_time, 8};

static u8t ber_oid_system_sysContact [] PROGMEM         = {0x2b, 0x06, 0x01, 0x02, 0x01, 0x01, 0x04, 0x00};
static ptr_t oid_system_sysContact PROGMEM              = {ber_oid_system_sysContact, 8};
static u8t ber_oid_system_sysName [] PROGMEM            = {0x2b, 0x06, 0x01, 0x02, 0x01, 0x01, 0x05, 0x00};
static ptr_t oid_system_sysName PROGMEM                 = {ber_oid_system_sysName, 8};
static u8t ber_oid_system_sysLocation [] PROGMEM        = {0x2b, 0x06, 0x01, 0x02, 0x01, 0x01, 0x06, 0x00};
static ptr_t oid_system_sysLocation PROGMEM             = {ber_oid_system_sysLocation, 8};

static u8t ber_oid_system_str[] PROGMEM   				= {0x2b, 0x06, 0x01, 0x02, 0x01, 0x01, 0x0B, 0x00};
static ptr_t oid_system_str PROGMEM       				= {ber_oid_system_str, 8};
static u8t ber_oid_system_tick[] PROGMEM  				= {0x2b, 0x06, 0x01, 0x02, 0x01, 0x01, 0x0D, 0x00};
static ptr_t oid_system_tick PROGMEM      				= {ber_oid_system_tick, 8};

/* SNMP group */
static u8t ber_oid_snmpInPkts[] PROGMEM                 = {0x2b, 0x06, 0x01, 0x02, 0x01, 0x0B, 0x01, 0x00};
static ptr_t oid_snmpInPkts PROGMEM                     = {ber_oid_snmpInPkts, 8};
static u8t ber_oid_snmpInBadVersions[] PROGMEM          = {0x2b, 0x06, 0x01, 0x02, 0x01, 0x0B, 0x03, 0x00};
static ptr_t oid_snmpInBadVersions PROGMEM              = {ber_oid_snmpInBadVersions, 8};
static u8t ber_oid_snmpInASNParseErrs[] PROGMEM         = {0x2b, 0x06, 0x01, 0x02, 0x01, 0x0B, 0x06, 0x00};
static ptr_t oid_snmpInASNParseErrs PROGMEM             = {ber_oid_snmpInASNParseErrs, 8};


/* Beuth Socket*/
static u8t ber_oid_steckdose_int[] PROGMEM     			= {0x2b, 0x06, 0x01, 0x04, 0x01, 0x81, 0xac, 0x5d, 0x64, 0x84, 0x58, 0x01, 0x00};
static ptr_t oid_steckdose_int PROGMEM         			= {ber_oid_steckdose_int, 13};

/*RSSI Value*/
static u8t ber_oid_rssi_int[] PROGMEM     				= {0x2b, 0x06, 0x01, 0x04, 0x01, 0x81, 0xac, 0x5d, 0x64, 0x84, 0x58, 0x02, 0x00};
static ptr_t oid_rssi_int PROGMEM         				= {ber_oid_rssi_int, 13};

/*Battery Power Value*/
static u8t ber_oid_batt_int[] PROGMEM     				= {0x2b, 0x06, 0x01, 0x04, 0x01, 0x81, 0xac, 0x5d, 0x64, 0x84, 0x58, 0x04, 0x00};
static ptr_t oid_batt_int PROGMEM         				= {ber_oid_batt_int, 13};




s8t getSysDescr(mib_object_t* object, u8t* oid, u8t len)
{
    if (!object->varbind.value.p_value.len) {
        object->varbind.value.p_value.ptr = (u8t*)"@ANY Brick running Contiki SNMP";
        object->varbind.value.p_value.len = 31;
    }
    return 0;
}

s8t setSysDescr(mib_object_t* object, u8t* oid, u8t len, varbind_value_t value)
{
    object->varbind.value.p_value.ptr = (u8t*)"BEUTH HOCHSCHULE";
    object->varbind.value.p_value.len = 16;
    return 0;
}

s8t getTimeTicks(mib_object_t* object, u8t* oid, u8t len)
{
    object->varbind.value.u_value = getSysUpTime();
#if PDEBUG
	printf("aktuelle Zeit mit clock time: %d",clock_time());
	printf("aktuelle Zeit mit getSysUpTime: %d",getSysUpTime());
#endif
    return 0;
}

/* LED DS1 for @ANY module start */
/** \brief LED DS1 for @ANY module get function*/
s8t getBeuthState(mib_object_t* object, u8t* oid, u8t len)
{
    object->varbind.value.i_value = !((PORTB >> PIN5) & 1);
#if PDEBUG
	printf("Get Pin State ausgef�hrt, Ergebnis %d\n",object->varbind.value.i_value);
#endif
    return 0;
}


/** \brief set LED DS1 for @ANY module*/
s8t setBeuthState(mib_object_t* object, u8t* oid, u8t len, varbind_value_t value)
{
	//DDRB |= (1 << PIN5); //moved to SNMPD Process Start
	//DDRB |= (1 << PIN7);
    if (value.i_value == 1) {
        PORTB &= ~(1 << PIN5);
	//PORTB |= (1 << PIN6);
    } else {
        PORTB |= (1 << PIN5);
	//PORTB &= ~(1 << PIN7);
    }

	return 0;
}
/* LED DS1 for @ANY module end */

/* RSSI value*/
/** \brief RSSI value get function*/
s8t getRssiValue(mib_object_t* object, u8t* oid, u8t len)
{
	int rssi_temp;
	rssi_temp=rf230_get_raw_rssi();
    object->varbind.value.i_value = (-91)+(rssi_temp); //Already multiplicated with three
#if PDEBUG
	printf("Get RSSI Value ausgef�hrt, iValue Ergebnis %d\n",object->varbind.value.i_value);
#endif
    return 0;
}

/* RSSI value end */

/* battery power level*/
/************************/

/** \brief function for initialization of the adc*/
void adc_init()
{
    // AREF = AVcc
    ADMUX = (1<<REFS0)|(1<<REFS1); //intern 2.54V voltage reference

    // ADC Enable and prescaler of 128
    // 16000000/128 = 125000
    ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
}

/** \brief adc read function */
u16t adc_read()
{
    ADMUX = (ADMUX & 0xF8)|0;     // clears the bottom 3 bits before ORing
    // start single convertion
    // write '1' to ADSC
    ADCSRA |= (1<<ADSC);
    // wait for conversion to complete
    // ADSC becomes '0' again
    // till then, run loop continuously
    while(ADCSRA & (1<<ADSC));

    return (ADC);
}

/** \brief get function for the battery power level mib object*/
s8t getBattValue(mib_object_t* object, u8t* oid, u8t len)
{
	adc_init();
	u16t adc_value;
	int batt_level;
	adc_value=adc_read();
	batt_level=(((25400)/1024)*adc_value*2); //Reference Voltage 2.54Volts, 10Bit ADC, 
        //multiplied with ADC Read, multiplied with two because of the 1:1 voltage divider
    object->varbind.value.i_value = batt_level;
    return 0;
}
/*Battery Level end*/

/*
 * SNMP group
 */
s8t getMIBSnmpInPkts(mib_object_t* object, u8t* oid, u8t len)
{
    object->varbind.value.u_value = getSnmpInPkts();
#if PDEBUG
	printf("Get InPakets.value = %d\n Get InPakets.function=%d \n",object->varbind.value.u_value, getSnmpInPkts());
#endif
    return 0;
}

s8t getMIBSnmpInBadVersions(mib_object_t* object, u8t* oid, u8t len)
{
    object->varbind.value.u_value = getSnmpInBadVersions();
    return 0;
}


s8t getMIBSnmpInASNParseErrs(mib_object_t* object, u8t* oid, u8t len)
{
    object->varbind.value.u_value = getSnmpInASNParseErrs();
    return 0;
}
/*


/*-----------------------------------------------------------------------------------*/
/*
 * Initialize the MIB.
 */
s8t mib_init()
{


    if (add_scalar(&oid_system_desc, 0, BER_TYPE_OCTET_STRING, 0, &getSysDescr, &setSysDescr) == -1 ||
        add_scalar(&oid_system_time, FLAG_ACCESS_READONLY, BER_TYPE_TIME_TICKS, 0, &getTimeTicks, 0) == -1  ||

		add_scalar(&oid_system_sysContact, FLAG_ACCESS_READONLY, BER_TYPE_OCTET_STRING, "<sven@zehl.co.cc>", 0, 0) == -1 ||
        add_scalar(&oid_system_sysName, 0, BER_TYPE_OCTET_STRING, "@ANY Brick", 0, 0) == -1 ||
        add_scalar(&oid_system_sysLocation, 0, BER_TYPE_OCTET_STRING, "Beuth Hochschule Berlin - IPv6-Lab", 0, 0) == -1)

		{
        return -1;
    }

	// snmp group

	    if (add_scalar(&oid_snmpInPkts, FLAG_ACCESS_READONLY, BER_TYPE_COUNTER, 0, &getMIBSnmpInPkts, 0) != ERR_NO_ERROR ||
        add_scalar(&oid_snmpInBadVersions, FLAG_ACCESS_READONLY, BER_TYPE_COUNTER, 0, &getMIBSnmpInBadVersions, 0) != ERR_NO_ERROR ||
        add_scalar(&oid_snmpInASNParseErrs, FLAG_ACCESS_READONLY, BER_TYPE_COUNTER, 0, &getMIBSnmpInASNParseErrs, 0) != ERR_NO_ERROR) {
        return -1;
    }

	if (add_scalar(&oid_steckdose_int, 0, BER_TYPE_INTEGER, 0, &getBeuthState, &setBeuthState) == -1)
	{
        return -1;
    }

		if (add_scalar(&oid_rssi_int, FLAG_ACCESS_READONLY, BER_TYPE_INTEGER, 0, &getRssiValue, 0) == -1)
	{
        return -1;
    }

	if (add_scalar(&oid_batt_int, FLAG_ACCESS_READONLY, BER_TYPE_INTEGER, 0, &getBattValue, 0) == -1)
	{
        return -1;
    }

    return 0;
}

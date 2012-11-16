TARGET=native
PROJECT=snmp-server

#UIP_CONF_IPV6=1
#UIP_CONF_RPL=1
#CFLAGS+= -DUIP_CONF_IPV6
#CFLAGS+= -DUIP_CONF_IPV6_RPL

CFLAGS+= -DUIP_CONF_IPV6
CFLAGS+= -DUIP_CONF_IPV6_RPL
CFLAGS+= -DUIP_CONF_ROUTER

#activate to enable extern PA on 2,4GHz modules
CFLAGS+= -DRF230BB_EXTERNAL_PA_ENABLED

#activate to test meshed networks
CFLAGS+= -DRF230_MAX_TX_POWER=15
CFLAGS+= -DRF230_MIN_RX_POWER=30

#add one byte to the echo reply (uip6.c)
#used for sending echo request and reply
#with same frame length.
#for running the test send echo with 73byte data
#example= ping6 [dest] -s 73

#CFLAGS+= -DICMP_ECHO_MAX_TRANS_TEST

#disable ICMP Checksum check with UIP_CONF_IPV6_CHECKS=0 
#in /platform/avr-zigbit/contiki-conf.h

UIP_CONF_IPV6=1
UIP_CONF_ROUTER=1
UIP_CONF_IPV6_RPL=1


#CFLAGS+= -DMAC_CONF_FREQUENCY=0x09 #900MHz
CFLAGS+= -DMAC_CONF_FREQUENCY=0x24 #2400MHz
CFLAGS+= -DMAC_CONF_NUMBER=0x08
#for using .sh-script initial value must
#CFLAGS+= -DMAC_CONF_NUMBER=0x08



APPS=snmpd 

CONTIKI = ../..

include $(CONTIKI)/Makefile.include

#CFLAGS   += -DENABLE_SNMPv1
#CFLAGS   += -DENABLE_SNMPv3


common:

zigbit: common
	make TARGET=avr-zigbit MIB_INIT=mib-init-beuth-zig.c $(PROJECT).elf 
	avr-objcopy -O ihex -R .eeprom -R .fuse -R .signature $(PROJECT).elf $(PROJECT).hex
	avr-size -C --mcu=atmega1281 $(PROJECT).elf
	rm -rf obj_native
rfa1281: common
	make TARGET=avr-atmega128rfa1 MIB_INIT=mib-init-beuth-zig.c $(PROJECT).elf 
	avr-objcopy -O ihex -R .eeprom -R .fuse -R .signature $(PROJECT).elf $(PROJECT).hex
	avr-size -C --mcu=atmega1281 $(PROJECT).elf
	rm -rf obj_native
	
zigbit-empty:
	make TARGET=avr-zigbit MIB_INIT=mib-init.c $(PROJECT).elf -DMAC_CONF_NUM=0x55 -DMAC_CONF_FREQ=0x33 
	avr-objcopy -O ihex -R .eeprom -R .fuse -R .signature $(PROJECT).elf $(PROJECT).hex
	avr-size -C --mcu=atmega1281 $(PROJECT).elf
	rm -rf obj_native

minimal-net: common
	make TARGET=minimal-net MIB_INIT=mib-init.c $(PROJECT).minimal-net

minimal-net-mib: common
	make TARGET=minimal-net MIB_INIT=mib-init-raven.c $(PROJECT).minimal-net

raven-beuth:
	make TARGET=avr-raven MIB_INIT=mib-init-beuth.c $(PROJECT).elf
	avr-objcopy -O ihex -R .eeprom -R .fuse -R .signature $(PROJECT).elf $(PROJECT).hex
	avr-size -C --mcu=atmega1284p $(PROJECT).elf
	rm -rf obj_native

raven: 	
	make TARGET=avr-raven MIB_INIT=mib-init.c $(PROJECT).elf
	avr-objcopy -O ihex -R .eeprom -R .fuse -R .signature $(PROJECT).elf $(PROJECT).hex
	avr-size -C --mcu=atmega1284p $(PROJECT).elf
	rm -rf obj_native

raven-mib: 	
	make TARGET=avr-raven MIB_INIT=mib-init-raven.c $(PROJECT).elf
	avr-objcopy -O ihex -R .eeprom -R .fuse -R .signature $(PROJECT).elf $(PROJECT).hex
	avr-size -C --mcu=atmega1284p $(PROJECT).elf
	rm -rf obj_native

raven-empty-mib:
	make TARGET=avr-raven MIB_INIT=mib-init-empty.c $(PROJECT).elf
	avr-objcopy -O ihex -R .eeprom -R .fuse -R .signature $(PROJECT).elf $(PROJECT).hex
	avr-size -C --mcu=atmega1284p $(PROJECT).elf
	rm -rf obj_native

#Clean Routines

cl-all:
	make TARGET=avr-raven clean
	make TARGET=minimal-net clean
	rm -rf obj_native
	rm symbols.c symbols.h $(PROJECT).elf $(PROJECT).hex
	rm -rf obj_avr-raven
	rm -rf obj_minimal-net
	rm $(PROJECT).minimal-net
	
cl-minnet:
	make TARGET=minimal-net clean
	rm -rf obj_native
	rm symbols.c symbols.h
	rm -rf obj_minimal-net
	rm $(PROJECT).minimal-net

cl-raven:
	make TARGET=avr-raven clean
	rm -rf obj_native
	rm symbols.c symbols.h $(PROJECT).elf $(PROJECT).hex
	rm -rf obj_avr-raven

upload:
	avrdude -p m1284p -c jtag2 -P usb -v -F -e -U flash:w:"./$(PROJECT).hex"

run:
	sudo ./snmp-server.minimal-net

#!/bin/bash
clear
echo "Good morning, world."
make clean TARGET=avr-zigbit
echo "Changing MAC to 0x01"
sed -i s/"CFLAGS+= -DMAC_CONF_NUMBER=0x08"/"CFLAGS+= -DMAC_CONF_NUMBER=0x01"/g Makefile
echo "Compiling 0x01"
make zigbit
mv snmp-server.elf snmp-server-1.elf

make clean TARGET=avr-zigbit
echo "Changing MAC to 0x02"
sed -i s/"CFLAGS+= -DMAC_CONF_NUMBER=0x01"/"CFLAGS+= -DMAC_CONF_NUMBER=0x02"/g Makefile
echo "Compiling 0x02"
make zigbit
mv snmp-server.elf snmp-server-2.elf

make clean TARGET=avr-zigbit
echo "Changing MAC to 0x03"
sed -i s/"CFLAGS+= -DMAC_CONF_NUMBER=0x02"/"CFLAGS+= -DMAC_CONF_NUMBER=0x03"/g Makefile
echo "Compiling 0x03"
make zigbit
mv snmp-server.elf snmp-server-3.elf

make clean TARGET=avr-zigbit
echo "Changing MAC to 0x04"
sed -i s/"CFLAGS+= -DMAC_CONF_NUMBER=0x03"/"CFLAGS+= -DMAC_CONF_NUMBER=0x04"/g Makefile
echo "Compiling 0x04"
make zigbit
mv snmp-server.elf snmp-server-4.elf

make clean TARGET=avr-zigbit
echo "Changing MAC to 0x05"
sed -i s/"CFLAGS+= -DMAC_CONF_NUMBER=0x04"/"CFLAGS+= -DMAC_CONF_NUMBER=0x05"/g Makefile
echo "Compiling 0x05"
make zigbit
mv snmp-server.elf snmp-server-5.elf

make clean TARGET=avr-zigbit
echo "Changing MAC to 0x06"
sed -i s/"CFLAGS+= -DMAC_CONF_NUMBER=0x05"/"CFLAGS+= -DMAC_CONF_NUMBER=0x06"/g Makefile
echo "Compiling 0x06"
make zigbit
mv snmp-server.elf snmp-server-6.elf

make clean TARGET=avr-zigbit
echo "Changing MAC to 0x07"
sed -i s/"CFLAGS+= -DMAC_CONF_NUMBER=0x06"/"CFLAGS+= -DMAC_CONF_NUMBER=0x07"/g Makefile
echo "Compiling 0x07"
make zigbit
mv snmp-server.elf snmp-server-7.elf

make clean TARGET=avr-zigbit
echo "Changing MAC to 0x08"
sed -i s/"CFLAGS+= -DMAC_CONF_NUMBER=0x07"/"CFLAGS+= -DMAC_CONF_NUMBER=0x08"/g Makefile
echo "Compiling 0x08"
make zigbit
mv snmp-server.elf snmp-server-8.elf

echo "Finished all eight ladies man!"
 

########################################
# Makefile for RTES Exercise 3	       #
# 				       #
# Kostas Mylonakis -- mylonakk@auth.gr #
########################################

#Note: searchWifi.sh works only on zSun, probably not in any OS.

#remember to change this for cross-compile
CC=/home/panagiotis/Desktop/Enswmatomena/openwrt-zsun-zsun/staging_dir/toolchain-mips_mips32_gcc-4.8-linaro_uClibc-0.9.33.2/bin/mips-openwrt-linux-gcc

#GCC=gcc

#remember to upload coresponding lib @zSun:/lib
LIBS=-lpthread -lrt -O
DIR= src/
EXE_DIR= bin/
RM=rm -f

all: wifi 

wifi: $(DIR)wifi.c $(DIR)utilities.c
	$(CC) $(DIR)wifi.c $(DIR)utilities.c -o $(EXE_DIR)$@ $(LIBS)

#callscript: callScript.c
#	$(CC) callScript.c -o $@ $(LIBS)

clean:
	$(RM) $(EXE_DIR)wifi

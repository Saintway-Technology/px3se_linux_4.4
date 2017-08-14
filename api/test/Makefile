PWD=`pwd`
TOP_DIR=${PWD}/../../../..

TOOLCHAIN_PATH=$(TOP_DIR)/buildroot/output/host/usr
INC_DIR=$(TOOLCHAIN_PATH)/include
LIB_DIR=$(TOOLCHAIN_PATH)/lib

CC = $(TOOLCHAIN_PATH)/bin/arm-rockchip-linux-gnueabihf-gcc
CPP = $(TOOLCHAIN_PATH)/bin/arm-rockchip-linux-gnueabihf-g++
STRIP = $(TOOLCHAIN_PATH)/bin/arm-rockchip-linux-gnueabihf-strip

INCS = -I $(INC_DIR)
LIBS = -L $(LIB_DIR)
CFLAGS = -Wno-multichar


BIN = apitest
all:$(BIN) 
###########################server.c##############################	
SRC = $(wildcard apitest.c PlayBackService.c)
OBJ := $(SRC:.c=.o)
$(OBJ) : %.o : %.c
	@$(CC) -c $< -o $@ $(INCS) $(CFLAGS) $(LIBS)

apitest : $(OBJ)
	@$(CC) -o apitest $(OBJ) $(LIBS)
	@$(STRIP) apitest
	@echo "apitest build success!"
clean:
	@-rm -f PlayBackService.o  apitest.o apitest
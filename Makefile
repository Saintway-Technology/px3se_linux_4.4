PWD=`pwd`
TOP_DIR=${PWD}/../..

TOOLCHAIN_PATH=$(TOP_DIR)/buildroot/output/host/usr
MM_PATH =$(TOP_DIR)/buildroot/output/target/usr/lib/gstreamer-1.0
INC_DIR=$(TOOLCHAIN_PATH)/include
LIB_DIR=$(TOOLCHAIN_PATH)/lib

CC = $(TOOLCHAIN_PATH)/bin/arm-rockchip-linux-gnueabihf-gcc
CPP = $(TOOLCHAIN_PATH)/bin/arm-rockchip-linux-gnueabihf-g++
STRIP = $(TOOLCHAIN_PATH)/bin/arm-rockchip-linux-gnueabihf-strip

MYINC_DIR=$(TOOLCHAIN_PATH)/arm-rockchip-linux-gnueabihf/sysroot/usr/include
MYINC_LIB=$(TOOLCHAIN_PATH)/arm-rockchip-linux-gnueabihf/sysroot/usr/lib

INCS = -I $(INC_DIR) -I $(MYINC_DIR)/gstreamer-1.0 -I $(MYINC_DIR)/glib-2.0 -I $(MYINC_LIB)/glib-2.0/include

LIBS =	-L $(MYINC_LIB)\
		-lc -lpthread -ldl -lrt \
		-lgobject-2.0 -lgmodule-2.0  -lglib-2.0 \
		-lgstreamer-1.0
  	       
CFLAGS = -Wno-multichar


BIN = audioservice
all:$(BIN) 
	
SRC = $(wildcard audioservice.c aud_fun.c list_file.c message_queue.c)
OBJ := $(SRC:.c=.o)
$(OBJ) : %.o : %.c
	@$(CC) -c $< -o $@ $(INCS) $(CFLAGS) $(LIBS)

$(BIN) : $(OBJ)
	@$(CC) -o $(BIN) $(OBJ) $(LIBS)
	@$(STRIP) $(BIN)
	@cp $(BIN) $(TOP_DIR)/buildroot/output/target/bin
	@cp ./S01_audio_ser_init $(TOP_DIR)/buildroot/output/target/etc/init.d
	@chmod +x $(TOP_DIR)/buildroot/output/target/etc/init.d/S01_audio_ser_init
	@echo "audioservice build success!"
clean:
	@-rm -f $(BIN) $(OBJ)
	@echo "clean success!"

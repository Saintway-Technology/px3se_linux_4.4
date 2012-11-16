#
############################################################################
# (C) Copyright 2008 Novell, Inc. All Rights Reserved.
#
#  GPLv2: This program is free software; you can redistribute it
#  and/or modify it under the terms of version 2 of the GNU General
#  Public License as published by the Free Software Foundation.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
############################################################################

makedir := $(dir $(realpath $(lastword $(MAKEFILE_LIST))))
include $(makedir)/gnu.mk

ifeq ($(BOARD),)
  TARGET = $(shell uname -m)
else
  TARGET = $(BOARD)
endif

optdir  := /opt/punybench
os	:= $(shell uname)
opus    := $(basename $(notdir $(PWD)))
target  := $(TARGET)
objdir  :=.$(target)
sources := $(wildcard *.c)
objects := $(addprefix $(objdir)/, $(sources:.c=.o))
libdir  := $(DESTDIR)$(optdir)/lib

-include $(makedir)/$(target).mk

#CC=/usr/local/bin/gcc
INC+=-I. -I../include -I../../include

# -pg -O -g -DUNOPT -DNDEBUG
CFLAGS += -std=gnu99
CFLAGS += -rdynamic -fPIC
CFLAGS += -g -O -pg -Wall -Wstrict-prototypes -Werror \
	-D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 \
	$(.INCLUDES) $(INC)

ifeq ($(os),Linux)
  AR := ar
  ARFLAGS := rc
else
  ifeq ($(os),Darwin)
    AR := libtool
    ARFLAGS := -static -o
  endif
endif

$(objdir)/%.o : %.c Makefile
	@mkdir -p $(objdir)
	$(CC) $(CFLAGS) -c $< -o $@

$(opus): $(objects)
	@rm -f $(objdir)/$@
	$(AR) $(ARFLAGS) $(objdir)/$@ $(objects)

install:
	@mkdir -p $(libdir)
	cp $(objdir)/$(opus) /tmp/$(opus).a # Can't use sudo in nfs home directory
	sudo mv /tmp/$(opus).a $(libdir)/$(opus).a

clean:
	@rm -f *.core
	@rm -f *.out
	@rm -fr $(objdir)

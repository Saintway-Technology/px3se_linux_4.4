#
# Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that
# can be found in the LICENSE file.
# 

export PUNYBIN=bin
export SCRATCH_DIR=_scratch

books = $(wildcard *.b) $(wildcard *.d) $(wildcard *.m)

default:
	@for i in $(books); do \
		(cd $$i && $(MAKE)) || exit 1; \
	done

install:
	@for i in $(books); do \
		(cd $$i && $(MAKE) install) || exit 1; \
	done

clean:
	@for i in $(books); do \
		(cd $$i && $(MAKE) clean) || exit 1; \
	done

test:
	./scripts/punybench.sh

mktest:
	@echo $(books)


# /bin/bash
#
# Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that
# can be found in the LICENSE file.
#

[[ -z $PUNYBIN ]] && export PUNYBIN=../bin

[[ -z $SCRATCH_DIR ]] && export SCRATCH_DIR=_scratch

export PATH=$PUNYBIN:$PATH

mkdir -p $SCRATCH_DIR
##
## Default pptions for tests
##
file=${SCRATCH_DIR}/xyzzy
dir=${SCRATCH_DIR}/dir
dest=${SCRATCH_DIR}/xxxxx
iter=1000
loop=3


## Just run all the tests once.
## Some tests have been commented out because of the resouces
## they need to run.


af -f$file <<- HERE
	a
	HERE

bigfile -f$file
##bigwrite -f$file

mkdir -p $dir

creat -f$file
dir ${SCRATCH_DIR} .
dirprobe ${SCRATCH_DIR}
execve -fbin/hello
exponential -i$iter -l$loop
rm -r ${SCRATCH_DIR}/dir; fakeclient -d$dir -i2 -l$loop -z0x100 -s1
fiddle -i3 -k2 -z100 -q2 -t11
#fragment  # this is a nasty test to run on a real disk
getdirentries -d$dir
getxattr -f$file -xxyzzy
inplace -f$file -z1 -i$iter
rm -f $dest; link -f$file -e$dest
listxattr -f$file
longname -i$iter -l$loop -f${SCRATCH_DIR}/thisisaverylongfilenamethatneedstobesomethingontheorderoftwohunderedfiftysixcharactersbuthisisnotenoughbutitwilldofornowbutimgoingtomakeitevenlongeruntilthesytembreakswhichmaybeneverwhatrandommusingsofaprogrammerishouldbeusingchinesecharacters
mystat
mystat ${SCRATCH_DIR}
noatime -f$file
open -f$file
randwrite -f$file -z200  # must be paired with randread
randread  -f$file -z200  # must be paired with randwrite
rate -d$dir -i3 -w3 -k3 -rztree -ocopy
readdir -d$dir
rename -f$file -e$dest
rmtree -d$dir
rw -f$file <<- HERE
	12345
	HERE
seek -f$file
sparse -f$file
bin/stat -f$file
statfs -d$dir
rm -f $dest; symlink -n500 -e$dest
t -f$file
telldir -d$dir
testopt
threadtree -d$dir -i3 -w3 -k3 -rztree -ocopy
timer
truncate -f$file -i2
ubigdir -d$dir -k20 -i3
ucrdel -d$dir -i$iter
ucreate -f$file -i$iter
ucreatefiles -d$dir -i2 -q"xyz" -k7 -l$loop
ufstat -f$file -i$iter -l$loop
ugetpid -i$iter -l$loop
bigfile -f$file; uio -f$file -i$iter -l$loop
unlink -f$file
uopenthread -d$dir -i$iter -t11 -l$loop
ulseek -f$file -i$iter -l$loop
uopen -f$file -i$iter -l$loop
uread -f$file -i$iter -l$loop -z0x200000
urename -f$file -i$iter -l$loop
ureaddir -d$dir -i$iter -l$loop -k1013
useekdir -d$dir -k10000 -l$loop
ustat -f$file -i$iter -l$loop
utime -f$file
utimer -i3 -l$loop
uwrite -f$file -i$iter -l$loop -z0x100000 -b13
rm -fr $dir; uwritefiles -d$dir -i$iter -l$loop -z0x100000

testopt -p -f$file -i$iter -l$loop -y

rm -fr ${SCRATCH_DIR}


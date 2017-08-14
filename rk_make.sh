#! /bin/sh

EXT_PROJECT_FILE=audioservice
TOP_DIR=$(pwd)
#EXT_DIR_NAME=$(basename $TOP_DIR)
BUILDROOT_TARGET_PATH=$(pwd)/../../buildroot/output/target/
CC=$(pwd)/../../buildroot/output/host/usr/bin/arm-rockchip-linux-gnueabihf-gcc
STRIP=$(pwd)/../../buildroot/output/host/usr/bin/arm-rockchip-linux-gnueabihf-strip
PRODUCT_NAME=`ls ../../device/rockchip/px3-se`
TARGET_EXECUTABLE=""

#define build err exit function
check_err_exit(){
  if [ $1 -ne "0" ]; then
     echo -e $MSG_ERR
     exit 2
  fi
}

if [ "$PRODUCT_NAME"x = "px3-se"x ];then
sed -i '/DEVICE_EVB/s/^/#&/' $EXT_PROJECT_FILE 
fi

#get parameter for "-j2~8 and clean"
result=$(echo "$1" | grep -Eo '*clean')
if [ "$result" = "" ];then
        mulcore_cmd=$(echo "$1" | grep '^-j[0-9]\{1,2\}$')
elif [ "$1" = "clean" ];then
        make clean
elif [ "$1" = "distclean" ];then
        make clean
else
	mulcore_cmd=-j4
fi

make $mulcore_cmd
check_err_exit $?

for file in ./*
do
    if test -x $file
    then
	elf=$(file -e elf $file)
	result=$(echo $elf | grep "LSB executable")
	if [ -n "$result" ]
	then
		TARGET_EXECUTABLE=$(basename "$file")
		echo "found executable file $TARGET_EXECUTABLE"
		$STRIP $TOP_DIR/$TARGET_EXECUTABLE
		echo "$TARGET_EXECUTABLE audioservice is ready."
	fi
    fi
done

make clean


#call just for buid_all.sh
if [ "$1" = "cleanthen" ] || [ "$2" = "cleanthen" ];then
        make clean
fi

#we should restore the modifcation which is made on this script above.
if [ "$PRODUCT_NAME"x = "px3-se"x ];then
sed -i '/DEVICE_EVB/s/^.//' $EXT_PROJECT_FILE 
fi

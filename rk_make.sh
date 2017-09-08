#! /bin/sh

TOP_DIR=$(pwd)
BUILDROOT_TARGET_PATH=$(pwd)/../../buildroot/output/target
RK3399_PLAT=$(pwd)/../../device/rockchip/rk3399

if [ -d $RK3399_PLAT ]
then
	PM_DEFINES=-DPLATFORM_WAYLAND
else
	PM_DEFINES=
fi

arm-linux-gcc -rdynamic -g -funwind-tables  -O0 -D_GNU_SOURCE $PM_DEFINES -o  power_manager_service power_service.c  power_manager.c  thermal.c  -lpthread -lxml2 -I$(pwd) -I$(pwd)/include -I$(pwd)/include/libxml


cp $TOP_DIR/power_manager_service $BUILDROOT_TARGET_PATH/usr/bin/
cp $TOP_DIR/thermal_sensor_config.xml $BUILDROOT_TARGET_PATH/etc/
cp $TOP_DIR/thermal_throttle_config.xml $BUILDROOT_TARGET_PATH/etc/

rm -rf $TOP_DIR/power_manager_service

echo "power_manager_service is ready on buildroot/output/target/usr/bin/"

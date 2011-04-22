#!/bin/bash
echo "****************************************"
echo "Rebuild Project"
echo "****************************************"

if [ $1 = "debug" ]; then
	echo "****************************************"
	echo "Building to debug"
	echo "****************************************"
	libtoolize
	autoreconf --install
	rm -rf build && mkdir build
	cd build
	../configure --host=arm-none-eabi --build=i686-linux-gnu CFLAGS="-g -O0"
	make
	echo "End building"
	echo "****************************************"
elif [ $1 = "run" ]; then
	echo "****************************************"
	echo "Building to run"
	echo "****************************************"
	libtoolize
	autoreconf --install
	rm -rf build && mkdir build
	cd build
	../configure --host=arm-none-eabi --build=i686-linux-gnu CFLAGS="-g -O2"
	make
	echo "End building"
	echo "****************************************"
else
  echo "Sorry, please input correct parameters"
  exit 1
fi
exit 0

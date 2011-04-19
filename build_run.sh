libtoolize
autoreconf --install
rm -rf build && mkdir build
cd build
../configure --host=arm-none-eabi --build=i686-linux-gnu CFLAGS="-g -O2"
make


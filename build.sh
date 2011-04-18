libtoolize
autoreconf --install
mkdir build
cd build
../configure --host=arm-none-eabi --build=i686-linux-gnu
make


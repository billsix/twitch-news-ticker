mkdir build
mkdir buildInstall
cd build

cmake -DCMAKE_INSTALL_PREFIX=../buildInstall  -DCMAKE_BUILD_TYPE=Debug ../
bear cmake --build  . --target all
cmake --build  . --target install
cd ../buildInstall

mkdir build
mkdir buildInstall
cd build
cmake -DCMAKE_INSTALL_PREFIX=../buildInstall -DCMAKE_BUILD_TYPE=Debug ../
bear cmake --build  . --target all
cmake --build  . --target install
cd ../buildInstall
./bin/twitch-news-ticker


# fascinated that she told me that she was leaving tomorrow morning
# and was unsure when she was coming home
# felt like that was planned before the perhaps-contrived argument

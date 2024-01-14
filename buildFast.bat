
REM g++ -std=c++17 -O3 -march=native src/*.cpp -Iinclude -o tur

g++ -std=c++17 -Os -s -ffunction-sections -fdata-sections -march=native -Wl,--gc-sections src/*.cpp -Iinclude -o tur

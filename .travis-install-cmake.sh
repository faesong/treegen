CMAKE_VERSION=3.18.0
CMAKE_OS=Linux-x86_64
CMAKE_TAR=cmake-$CMAKE_VERSION-$CMAKE_OS.tar.gz
CMAKE_URL=https://github.com/Kitware/CMake/releases/download/v$CMAKE_VERSION/$CMAKE_TAR
CMAKE_DIR=$(pwd)/cmake-kitware

wget --no-check-certificate $CMAKE_URL
mkdir -p $CMAKE_DIR
tar --strip-components=1 -xzf $CMAKE_TAR -C $CMAKE_DIR
chmod +x $CMAKE_DIR/bin/cmake
export PATH=$CMAKE_DIR/bin:$PATH

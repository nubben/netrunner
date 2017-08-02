#!/bin/bash
echo "Set up dir"
mkdir build
cd build

echo "Downloading files"
if [ ! -f "glew-2.0.0.tgz" ]; then
  curl -L https://github.com/nigels-com/glew/releases/download/glew-2.0.0/glew-2.0.0.tgz > glew-2.0.0.tgz
fi
if ! type "cmake" > /dev/null; then
  if [ ! -f "cmake-3.9.0.tar.gz" ]; then
    curl https://cmake.org/files/v3.9/cmake-3.9.0.tar.gz > cmake-3.9.0.tar.gz
  fi
fi
if [ ! -f "glfw3.2.1.tar.gz" ]; then
  curl -L https://github.com/glfw/glfw/archive/3.2.1.tar.gz > glfw3.2.1.tar.gz
fi
if ! type "pkg-config" > /dev/null; then
  if [ ! -f "pkg-config-0.29.tar.gz" ]; then
    curl https://pkg-config.freedesktop.org/releases/pkg-config-0.29.tar.gz > pkg-config-0.29.tar.gz
  fi
fi
if [ ! -f "freetype-2.8.tar.gz" ]; then
  curl -L http://download.savannah.gnu.org/releases/freetype/freetype-2.8.tar.gz > freetype-2.8.tar.gz
fi
if [ ! -f "harfbuzz-1.4.7.tar.bz2" ]; then
  curl -L https://github.com/behdad/harfbuzz/releases/download/1.4.7/harfbuzz-1.4.7.tar.bz2 > harfbuzz-1.4.7.tar.bz2
fi
if [ ! -f "md5sum.c" ]; then
  curl https://spit.mixtape.moe/view/raw/765a1dc1 > md5sum.c
fi

echo "Extracing files"
tar zxf glew-2.0.0.tgz
if ! type "cmake" > /dev/null; then
  tar zxf cmake-3.9.0.tar.gz
fi
tar zxf glfw3.2.1.tar.gz
if ! type "pkg-config" > /dev/null; then
  tar zxf pkg-config-0.29.tar.gz
fi
tar zxf freetype-2.8.tar.gz
tar jxf harfbuzz-1.4.7.tar.bz2

physicalCpuCount=$([[ $(uname) = 'Darwin' ]] &&
                       sysctl -n hw.physicalcpu_max ||
                       lscpu -p | egrep -v '^#' | sort -u -t, -k 2,4 | wc -l)

echo "Configured for $physicalCpuCount CPUs"
if [ -d "cmake-3.9.0" ]; then
  echo "Installing CMake"
  cd cmake-3.9.0
    sh configure --parallel=$physicalCpuCount
    make -j$physicalCpuCount
    echo "Prompting for local user password for installation"
    sudo make install
  cd ..
fi
echo "Installing GLEW"
cd glew-2.0.0
  make -j$physicalCpuCount
  echo "Prompting for local user password for installation"
  sudo make install
cd ..
echo "Installing GLFW"
cd glfw-3.2.1
  cmake .
  make -j$physicalCpuCount
  sudo make install
cd ..
if [ -d "pkg-config-0.29" ]; then
  echo "Installing pkg-config"
  cd pkg-config-0.29
    env LDFLAGS="-framework CoreFoundation -framework Carbon" ./configure --with-internal-glib
    make -j$physicalCpuCount
    sudo make install
  cd ..
fi
echo "Installing FreeType"
cd freetype-2.8
  sh configure
  make -j$physicalCpuCount
  sudo make install
cd ..
echo "Installing HarfBuzz"
cd harfbuzz-1.4.7
  sh configure --with-freetype=yes
  make -j$physicalCpuCount
  sudo make install
cd ..

echo "Installing md5sum"
gcc md5sum.c -o md5sum
chmod u+x md5sum
sudo cp md5sum /usr/local/bin

echo "Restoring pwd"
cd ..


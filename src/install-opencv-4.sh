#!/bin/bash

set -e

OPENCV_PATH=/opt/opencv

mkdir -p ${OPENCV_PATH}
cd ${OPENCV_PATH}
sudo wget https://github.com/opencv/opencv/archive/4.0.0.tar.gz
sudo wget https://github.com/opencv/opencv_contrib/archive/4.0.0.tar.gz
tar -xvzf 4.0.0.tar.gz
tar -xvzf 4.0.0.tar.gz.1
cd opencv-4.0.0/
sudo rm -rdf build
mkdir build
cd build/

cmake \
-DCMAKE_BUILD_TYPE=RELEASE \
-DCMAKE_INSTALL_PREFIX=/usr/local \
-DWITH_TBB=ON \
-DBUILD_NEW_PYTHON_SUPPORT=ON \
-DWITH_V4L=ON \
-DINSTALL_C_EXAMPLES=ON \
-DINSTALL_PYTHON_EXAMPLES=ON \
-DBUILD_EXAMPLES=ON \
-DWITH_QT=ON \
-DWITH_OPENGL=ON \
-DWITH_GTK=ON \
-DWITH_OPENMP=ON \
-DENABLE_PRECOMPILED_HEADERS=OFF \
-DOPENCV_EXTRA_MODULES_PATH=${OPENCV_PATH}/opencv_contrib-4.0.0/modules \
-DOPENCV_ENABLE_NONFREE=ON \
..

make -j8

cd ${OPENCV_PATH}/opencv-4.0.0/build/ ; 
sudo make install ; 

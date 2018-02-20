#!/bin/bash

PROJECT_ROOT=$(pwd)
DEP_ROOT=$(pwd)/dependencies
EZMQ_TARGET_ARCH="$(uname -m)"

if [ -d "./dependencies" ] ; then
    echo "dependencies folder exist"
else
    mkdir dependencies
fi

#build ezmq-cpp
cd $DEP_ROOT
#clone ezmq-protocol-cpp
if [ -d "./protocol-ezmq-cpp" ] ; then
    rm -rf protocol-ezmq-cpp
fi
git clone git@github.sec.samsung.net:RS7-EdgeComputing/protocol-ezmq-cpp.git
cd ./protocol-ezmq-cpp
#./build.sh --with_dependencies=true --target_arch=x86
#./build.sh --target_arch=x86_64
echo "build protocol-ezmq-cpp"
scons TARGET_OS=linux TARGET_ARCH=armhf
echo "done"

#build & install curl lib
cd $DEP_ROOT
if [ -a "./curl-7.58.0.tar.gz" ] ; then
    echo "curl exist"
else
    wget https://github.com/curl/curl/releases/download/curl-7_58_0/curl-7.58.0.tar.gz
fi
tar xvf curl-7.58.0.tar.gz
cd curl-7.58.0
echo "build curl"
./configure
make -j 8
sudo make install
sudo ldconfig
echo "done"

#build ezmq-plus-cpp
cd $PROJECT_ROOT
#scons TARGET_OS=linux TARGET_ARCH=x86_64
echo "build protocol-ezmq-plus-cpp"
scons TARGET_OS=linux TARGET_ARCH=armhf
echo "done"

#copy samples
cp out/linux/armhf/release/samples/publisher ./publisher
cp out/linux/armhf/release/samples/subscriber ./subscriber

#copy libs
mkdir libs
cp out/linux/armhf/release/libezmq_plus.so ./libs/libezmq_plus.so
cp dependencies/protocol-ezmq-cpp/out/linux/armhf/release/libezmq.so ./libs/libezmq.so
cp -r /usr/local/lib/* libs

echo "done"

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
if [ -d "./protocol-ezmq-cpp" ] ; then
    echo "protocol-ezmq-cpp exist"
else
    git clone git@github.sec.samsung.net:RS7-EdgeComputing/protocol-ezmq-cpp.git
    cd ./protocol-ezmq-cpp
    echo "build protocol-ezmq-cpp"
    scons TARGET_OS=linux TARGET_ARCH=armhf
    echo "done"
fi

#build AML
cd $DEP_ROOT
if [ -a "./datamodel-aml-cpp" ] ; then
    echo "datamodel-aml-cpp exist"
else
    git clone git@github.sec.samsung.net:RS7-EdgeComputing/datamodel-aml-cpp.git
    cd datamodel-aml-cpp
    ./build_arm.sh
fi

#build & install curl lib
cd $DEP_ROOT
if [ -a "./curl-7.58.0.tar.gz" ] ; then
    echo "curl exist"
else
    wget https://github.com/curl/curl/releases/download/curl-7_58_0/curl-7.58.0.tar.gz
    tar xvf curl-7.58.0.tar.gz
    cd curl-7.58.0
    echo "build curl"
    ./configure
    make -j 8
    sudo make install
    sudo ldconfig
    echo "done"
fi

#build & install JsonCpp
cd $DEP_ROOT
if [ -d "./jsoncpp" ] ; then
    echo "jsoncpp exist"
else
    git clone https://github.com/open-source-parsers/jsoncpp.git
    cd jsoncpp
    git checkout 1.8.4
    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=release -DBUILD_STATIC_LIBS=OFF -DBUILD_SHARED_LIBS=ON -DARCHIVE_INSTALL_DIR=./ -G "Unix Makefiles" ../
    sudo make -j 8
    sudo make install
    sudo ldconfig
fi


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

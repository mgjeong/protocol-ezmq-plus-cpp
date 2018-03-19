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
    echo "protocol-ezmq-cpp exist"
else
    git clone git@github.sec.samsung.net:RS7-EdgeComputing/protocol-ezmq-cpp.git
    cd ./protocol-ezmq-cpp
    echo "build protocol-ezmq-cpp"
    ./build.sh --target_arch=x86_64
    sudo cp out/linux/x86_64/release/libezmq.so /usr/local/lib
    sudo ldconfig
    echo "done"
fi


#build AML
cd $DEP_ROOT
if [ -a "./datamodel-aml-cpp" ] ; then
    echo "datamodel-aml-cpp exist"
else
    git clone git@github.sec.samsung.net:RS7-EdgeComputing/datamodel-aml-cpp.git
    cd datamodel-aml-cpp
    git checkout alpha
    ./build.sh
    sudo cp out/linux/x86_64/release/libaml.so /usr/local/lib
    sudo ldconfig
fi


#build & install curl lib
cd $DEP_ROOT
if [ -a "./curl-7.58.0.tar.gz" ] ; then
    echo "curl exist"
else
    wget https://github.com/curl/curl/releases/download/curl-7_58_0/curl-7.58.0.tar.gz
    tar xvf curl-7.58.0.tar.gz
    cd curl-7.58.0
    ./configure
    make -j 8
    sudo make install
    sudo ldconfig
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
echo "build protocol-ezmq-plus-cpp"
scons TARGET_OS=linux TARGET_ARCH=x86_64
echo "done"

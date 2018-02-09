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
cd ./dependencies
#clone ezmq-protocol-cpp
git clone git@github.sec.samsung.net:RS7-EdgeComputing/protocol-ezmq-cpp.git
cd ./protocol-ezmq-cpp
#./build.sh --with_dependencies=true --target_arch=x86
./build.sh --target_arch=x86_64


#build ezmq-plus-cpp
cd $PROJECT_ROOT
scons TARGET_OS=linux TARGET_ARCH=x86_64


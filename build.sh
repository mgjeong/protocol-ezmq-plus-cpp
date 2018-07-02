#!/bin/bash

EZMQ_PLUS_WITH_DEP=true

PROJECT_ROOT=$(pwd)
DEP_ROOT=$(pwd)/dependencies
EZMQ_PLUS_BUILD_MODE="release"

install_dependencies() {
    if [ -d "./dependencies" ] ; then
        echo "dependencies folder exist"
        rm -rf dependencies
        mkdir dependencies
    else
        mkdir dependencies
    fi

    #build zmq
    cd $DEP_ROOT
    if [ -d "./libzmq" ] ; then
        echo "libzmq exist"
    else
        git clone https://github.com/zeromq/libzmq.git
        cd ./libzmq
        git checkout v4.2.2
        ./version.sh
        ./autogen.sh
        ./configure
        make -j 8
        sudo make install
        sudo ldconfig
    fi

    #build & install curl lib
    cd $DEP_ROOT
    if [ -d "./curl-7.58.0" ] ; then
        echo "curl lib exist"
    else
        if [ -a "./curl-7.58.0.tar.gz" ] ; then
            echo "curl file exist"
        else
            wget https://github.com/curl/curl/releases/download/curl-7_58_0/curl-7.58.0.tar.gz
        fi
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

    #build ezmq-cpp
    cd $DEP_ROOT
    #clone ezmq-protocol-cpp
    if [ -d "./protocol-ezmq-cpp" ] ; then
        echo "protocol-ezmq-cpp exist"
    else
        git clone git@github.sec.samsung.net:RS7-EdgeComputing/protocol-ezmq-cpp.git
        cd ./protocol-ezmq-cpp
        echo "build protocol-ezmq-cpp"
        if [ "debug" = ${EZMQ_PLUS_BUILD_MODE} ]; then
            ./build_auto.sh --with_dependencies=false --target_arch=x86_64 --build_mode=debug
            sudo cp out/linux/x86_64/debug/libezmq.so /usr/local/lib
        else
            ./build_auto.sh --with_dependencies=false --target_arch=x86_64 --build_mode=release
            sudo cp out/linux/x86_64/release/libezmq.so /usr/local/lib
        fi
            sudo ldconfig
            echo "done"
    fi

    #build AML
    cd $DEP_ROOT
    if [ -d "./datamodel-aml-cpp" ] ; then
        echo "datamodel-aml-cpp exist"
    else
        git clone git@github.sec.samsung.net:RS7-EdgeComputing/datamodel-aml-cpp.git
        cd datamodel-aml-cpp

        if [ "debug" = ${EZMQ_PLUS_BUILD_MODE} ]; then
             ./build_common.sh --target_arch=x86_64 --build_mode=debug --logging=on
             sudo cp out/linux/x86_64/debug/libaml.so /usr/local/lib
        else
             ./build_common.sh --target_arch=x86_64
             sudo cp out/linux/x86_64/release/libaml.so /usr/local/lib
        fi
             sudo ldconfig
             echo "done"
    fi
}

usage() {
    echo "Usage: ./build.sh <option>"
    echo "Options:"
    echo "  --with_dependencies=(default: false)                               :  Build with dependencies"
    echo "  --build_mode=[release|debug](default: release)                     :  Build ezmq plus library and samples in release or debug mode"
    echo "  -h / --help                                                        :  Display help and exit"
    echo "Examples:"
    echo "build:-"
    echo "  $ ./build.sh"
    echo "  $ ./build.sh --with_dependencies=true"
    echo "  $ ./build.sh --with_dependencies=false --build_mode=release"
    echo "  $ ./build.sh --with_dependencies=false --build_mode=debug"
    echo "help:-"
    echo "  $ ./build.sh -h"
}


process_cmd_args() {
    if [ "$#" -eq 0  ]; then
        echo -e "No argument.."
    fi

    while [ "$#" -gt 0  ]; do
        case "$1" in
            --with_dependencies=*)
                EZMQ_PLUS_WITH_DEP="${1#*=}";
                if [ ${EZMQ_PLUS_WITH_DEP} = true ]; then
                    echo -e "Build with depedencies"
                elif [ ${EZMQ_PLUS_WITH_DEP} = false ]; then
                    echo -e "Build without depedencies"
                else
                    echo -e "Wrong option"
                    shift 1; exit 0
                fi
                shift 1;
                ;;
            --build_mode=*)
                EZMQ_PLUS_BUILD_MODE="${1#*=}";
                echo "Build mode is: $EZMQ_PLUS_BUILD_MODE"
                shift 1;
                ;;
            -h)
                usage; exit 0
                ;;
            --help)
                usage; exit 0
                ;;
            -*)
                usage; exit 1
                ;;
            *)
                usage; exit 1
                ;;
        esac
    done
}

process_cmd_args "$@"

if [ -d "./dependencies" ] ; then
    echo "dependencies folder exist"
else
    mkdir dependencies
fi


if [ ${EZMQ_PLUS_WITH_DEP} = true ]; then
    install_dependencies
fi

#build ezmq-plus-cpp
cd $PROJECT_ROOT
echo "build protocol-ezmq-plus-cpp"

if [ "debug" = ${EZMQ_PLUS_BUILD_MODE} ]; then
    scons TARGET_OS=linux LOGGING=1 RELEASE=0 TARGET_ARCH=x86_64 TEST=0
else
    scons TARGET_OS=linux TARGET_ARCH=x86_64 LOGGING=0
fi

echo "done"

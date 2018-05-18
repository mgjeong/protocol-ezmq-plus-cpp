#!/bin/bash

USR_LIB_DIR=/usr/local/lib
USR_INC_DIR=/usr/local/include
KEYWORD_EZMQ_PLUS_CPP=protocol-ezmq-plus-cpp

# COPY shared library to /usr/local/lib
sudo cp out/linux/armhf/release/libezmq_plus.so /usr/local/lib

mkdir tmp
# COPY header file to /usr/local/include/protocol-ezmq-plus-cpp
sudo rm -rf $USR_INC_DIR/$KEYWORD_EZMQ_PLUS_CPP
sudo mkdir $USR_INC_DIR/$KEYWORD_EZMQ_PLUS_CPP
sudo cp -rf ./include/* $USR_INC_DIR/$KEYWORD_EZMQ_PLUS_CPP/

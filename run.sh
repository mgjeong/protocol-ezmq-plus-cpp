#!/bin/sh

#copy libs
cd /
rm -rf /usr/local/lib/*
cp -r libs/* /usr/local/lib/
ldconfig

#excute sample
if [ "publisher" = $1 ]; then
    echo "start publisher with topic: $2"
    ./publisher $2
elif [ "subscriber" = $1 ]; then
    echo "start subscriber with topic $2"
    ./subscriber $2
else
    echo "Wrong arguments!!!"
    echo "Example"
    echo " publisher topic"
    echo " subscriber topic"
fi


#!/bin/sh

if [ "loop" = $1 ]; then
    while true
    do
        echo "Entered infinite loop for test"
        sleep 1
    done
elif [ "publisher" = $1 ]; then
    echo "start publisher with topic: $2"
    ./publisher $2
elif [ "subscriber" = $1 ]; then
    if [ "aml" = $2 ]; then
        echo "start subscriber with topic: $3"
        ./AmlSubscriber $3
    elif [ "xml" = $2 ]; then
        echo "start subscriber with topic: $3"
        ./XmlSubscriber $3
    else
        echo "Wrong arguments!!!"
        echo "Example"
        echo " loop"
        echo " publisher topic"
        echo " subscriber aml topic"
        echo " subscriber xml topic"
    fi
else
    echo "Wrong arguments!!!"
    echo "Example"
    echo " loop"
    echo " publisher topic"
    echo " subscriber aml topic"
    echo " subscriber xml topic"
fi

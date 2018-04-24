#!/bin/sh

PROJECT_ROOT=$(pwd)

#clear dir
if [ -d "./out" ] ; then
    rm -rf ./out
fi
#clear dir
if [ -d "./cov" ] ; then
    rm -rf ./cov
    mkdir cov
else
    mkdir cov
fi

#build
scons LOGGING=1 RELEASE=0 TARGET_ARCH=x86_64 TEST=0

#copy aml file
cp unittests/sample_data_model.aml out/linux/x86_64/debug/unittests/

#start unittest
cd out/linux/x86_64/debug/unittests
./ezmqx_config_test
./ezmqx_publisher_test
./ezmqx_subscriber_test
./ezmqx_topicdiscovery_test
./ezmqx_topicvalidate_test

sleep 3

cd $PROJECT_ROOT
# gen report.html
gcovr -r . \
-e "extlibs.*" \
        -e "extlibs.*" \
        -e "dependenncies.*" \
        -e "out.linux.*" \
        -e "unittests.*" \
        -e "samples.*" \
        -e "src/logger.*" \
--html --html-details -o ./cov/report.html

echo "done"
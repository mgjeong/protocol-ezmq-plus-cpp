./build_arm.sh
cp unittests/sample_data_model.aml out/linux/armhf/release/unittests/
cd out/linux/armhf/release/unittests
./ezmqx_config_test
./ezmqx_publisher_test
./ezmqx_subscriber_test
./ezmqx_topicdiscovery_test
./ezmqx_topicvalidate_test
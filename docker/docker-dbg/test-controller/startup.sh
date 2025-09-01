#!/bin/bash

VERSION=$1
DOCKER_NAME=$2

DOCKER_IMAGE=pscale.artifactory.cec.lab.emc.com/pscale-docker-local/test-controller
MYUSER=mam28
MYTOKEN=cmVmdGtuOjAxOjE3NjA1OTM4MzE6OGJnd1VNOGhpTTZqSmFSTUJBUVhUWUNiR1dE

# Remote Path
BIN_PATH=/mnt/docker/bin
QA_PATH=/mnt/qalogserver/qa

# Local Path
#BIN_PATH=$(realpath ../)/bin
#QA_PATH=/mnt/qalogserver_cn/qa

usage()
{
	echo -e "Usage: \n\t$0 [VERSION]"

	# B_MAIN_3995
	echo -e "Example:\n\t$0 B_MAIN_3995"
}

run_test_controller()
{

	echo "##### Start test-controller: VERSION-${VERSION} DOCKER NAME-${DOCKER_NAME} #####"

	DOCKER_VERSION=$(./artifactory-docker-image.py latest test-controller)
	docker pull ${DOCKER_IMAGE}:${DOCKER_VERSION}
	docker run -d -it --rm --name=$DOCKER_NAME --network=host \
		-v $BIN_PATH:/root/bin \
		-v $QA_PATH:/mnt/qalogserver/qa \
		-e BUILD_ID=${VERSION} \
		-e USE_INTERNAL_EPEL=yes \
		-e PSCALE_ARTIFACTORY_READ_USER=${MYUSER} \
		-e PSCALE_ARTIFACTORY_READ_TOKEN=${MYTOKEN} \
		${DOCKER_IMAGE}:${DOCKER_VERSION} \
		/bin/bash -l
}

check_parameter()
{
	if [ -z $1 ]; then
		usage
		exit -1
	fi

	if [ -z $2 ]; then
		DOCKER_NAME=test-controller
	fi

	if [ ! -d "$QA_PATH" ]; then
		echo "$QA_PATH does not exist"
		exit -1
	fi

	if ! mountpoint -q "$QA_PATH"; then
		echo "$QA_PATH is NOT mounted"
		exit -1
	fi
}

main()
{
	check_parameter $1 $2

	echo $BIN_PATH
	echo $QA_PATH
	echo $DOCKER_NAME
	run_test_controller
}

# Main Entry #
main $VERSION $DOCKER_NAME

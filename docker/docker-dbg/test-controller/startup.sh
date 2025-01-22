#!/bin/bash

# B_MAIN_3995
VERSION=$1
DOCKER_VERSION=$(./artifactory-docker-image.py latest test-controller)
DOCKER_IMAGE=pscale.artifactory.cec.lab.emc.com/pscale-docker-local/test-controller
MYUSER=mam28
MYTOKEN=cmVmdGtuOjAxOjE3NjA1OTM4MzE6OGJnd1VNOGhpTTZqSmFSTUJBUVhUWUNiR1dE

BIN_PATH=$(realpath ../)/bin

usage()
{
	echo -e "Usage: \n\t$0 [VERSION]"
	echo -e "Example:\n\t$0 B_MAIN_3995"
}

run_test_controller()
{

	echo "##### Start test-controller: VERSION-${VERSION} #####"
	docker pull ${DOCKER_IMAGE}:${DOCKER_VERSION}
	docker run -d -it --rm --name=test-controller --network=host \
		-v $BIN_PATH:/root/bin \
		-v /mnt/qalogserver_cn/qa:/mnt/qalogserver/qa \
		-e BUILD_ID=${VERSION} \
		-e USE_INTERNAL_EPEL=yes \
		-e PSCALE_ARTIFACTORY_READ_USER=${MYUSER} \
		-e PSCALE_ARTIFACTORY_READ_TOKEN=${MYTOKEN} \
		${DOCKER_IMAGE}:${DOCKER_VERSION} \
		/bin/bash -l
}

main()
{
	if [ -z $1 ]; then
		usage
		exit -1
	fi

	echo $BIN_PATH
	#run_test_controller $1
}

# Main Entry #
main $VERSION

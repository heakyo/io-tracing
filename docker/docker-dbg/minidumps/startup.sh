#!/bin/bash

VERSION=$1
DOCKER_NAME=$2

#DOCKER_IMAGE=pscale.artifactory.cec.lab.emc.com/pscale-docker-local/test-controller
DOCKER_IMAGE=pscale.artifactory.cec.lab.emc.com/pscale-docker-local/eng-os/minidumps
MYUSER=mam28
MYTOKEN=cmVmdGtuOjAxOjE3NjA1OTM4MzE6OGJnd1VNOGhpTTZqSmFSTUJBUVhUWUNiR1dE

BIN_PATH=$(realpath ../)/bin

usage()
{
	echo -e "Usage: \n\t$0 [VERSION]"

	# B_MAIN_3995
	echo -e "Example:\n\t$0 B_MAIN_3995"
}

run_minidump()
{
	echo "##### Start Minidump: VERSION-${VERSION} DOCKER NAME-${DOCKER_NAME} #####"

	docker pull ${DOCKER_IMAGE}:latest
	docker run -d -it --rm --name=$DOCKER_NAME --network=host \
		-v ./core:/work/core \
		-v $BIN_PATH:/root/bin \
		-v $HOME/isilon/isi-src/onefs:/tmp/isilon_mnt/src \
		-e BUILD_ID=${VERSION} \
		-e USE_INTERNAL_EPEL=yes \
		-e PSCALE_ARTIFACTORY_READ_USER=${MYUSER} \
		-e PSCALE_ARTIFACTORY_READ_TOKEN=${MYTOKEN} \
		${DOCKER_IMAGE}:latest \
		/bin/bash -l
}

main()
{
	if [ -z $1 ]; then
		usage
		exit -1
	fi

	if [ -z $2 ]; then
		DOCKER_NAME=minidump
	fi

	echo $BIN_PATH $DOCKER_NAME
	run_minidump
}

# Main Entry #
main $VERSION $DOCKER_NAME

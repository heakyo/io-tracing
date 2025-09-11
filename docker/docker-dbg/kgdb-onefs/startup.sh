#!/bin/bash

# B_MAIN_3995
VERSION=$1
DOCKER_VERSION=$(./artifactory-docker-image.py latest tools/kgdb-onefs)
DOCKER_IMAGE=pscale.artifactory.cec.lab.emc.com/pscale-docker-local/tools/kgdb-onefs
MYUSER=mam28
MYTOKEN=cmVmdGtuOjAxOjE3NjA1OTM4MzE6OGJnd1VNOGhpTTZqSmFSTUJBUVhUWUNiR1dE

SYM_PATH=$(realpath ./)/symbols
#SRC_PATH=$HOME/isilon/isi-src/onefs

# Remote
SRC_PATH=/export/onefs

usage()
{
	echo -e "Usage: \n\t$0 VERSION"
	echo -e "Example:\n\t$0 B_MAIN_3995"
}

run_test_controller()
{
	echo "##### Start kgdb-onefs: VERSION-${VERSION} #####"

	docker pull ${DOCKER_IMAGE}:${DOCKER_VERSION}
	docker run -d -it --rm --name=kgdb-onefs \
		-v $SYM_PATH:/tmp/symbols \
		-v $SRC_PATH:/tmp/isilon_mnt/src \
		-e BUILD_ID=${VERSION} \
		-e USE_INTERNAL_EPEL=yes \
		-e PSCALE_ARTIFACTORY_READ_USER=${MYUSER} \
		-e PSCALE_ARTIFACTORY_READ_TOKEN=${MYTOKEN} \
		${DOCKER_IMAGE}:${DOCKER_VERSION} \
		/bin/bash -l
}

main()
{
	if [ -z "$1" ]; then
		usage
		exit -1
	fi

	run_test_controller $1
}

# Main Entry #
main $VERSION

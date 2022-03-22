APP_DIR		:= /app
SOURCE_DIR	:= /app/src
BUILD_DIR	:= /tmp/hoverclock_build
OUTPUT_DIR	:= /tmp/hoverclock

CQTDEPLOYER         := cqtdeployer
CQTDEPLOYER_FLAGS   := -confFile ${APP_DIR}/build_config.json

MAKE    := make
QMAKE 	:= qmake

THREADS := $(shell nproc)

all: dir installers 

dir: 
	mkdir -p ${BUILD_DIR}

installers: hoverclock
	${CQTDEPLOYER} ${CQTDEPLOYER_FLAGS}

hoverclock:
	cd ${BUILD_DIR} && \
	${QMAKE} ${SOURCE_DIR} && \
	${MAKE} -j ${THREADS} && \
	${MAKE} install INSTALL_ROOT=${BUILD_DIR}

clean:
	rm -r /tmp/hoverclock*; \
	rm -r ${APP_DIR}/build; \
	rm -r ${APP_DIR}/DistributionKit

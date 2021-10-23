APP_DIR		:= /app
SOURCE_DIR	:= /app/src
BUILD_DIR	:= /tmp/hoverclock_build
BIN_DIR		:= /tmp/hoverclock_bin
OUTPUT_DIR	:= /tmp/hoverclock

LINUX_DEPLOY 			:= /tmp/linuxdeploy-x86_64.AppImage
LINUX_DEPLOY_APP_DIR 	:= --appdir=${OUTPUT_DIR}
LINUX_DEPLOY_FLAGS 		:= --plugin qt --output appimage

MAKE_UTIL 	:= /usr/bin/make
QMAKE_UTIL 	:= /usr/bin/qmake

THREADS := $(shell nproc)

all: dir appimage

dir: 
	mkdir -p ${BUILD_DIR}

appimage: hoverclock
	${LINUX_DEPLOY} ${LINUX_DEPLOY_APP_DIR}

	cp ${BIN_DIR}/opt/hover_clock/bin/hover_clock ${OUTPUT_DIR}/usr/bin/
	cp ${APP_DIR}/hoverclock.desktop ${OUTPUT_DIR}/usr/share/applications/hoverclock.desktop
	cp ${SOURCE_DIR}/resources/icon_light.png ${OUTPUT_DIR}/usr/share/icons/hicolor/64x64/apps/hoverclock.png

	cd ${APP_DIR} && \
	${LINUX_DEPLOY} ${LINUX_DEPLOY_APP_DIR} ${LINUX_DEPLOY_FLAGS}

hoverclock:
	cd ${BUILD_DIR} && \
	${QMAKE_UTIL} ${SOURCE_DIR} && \
	${MAKE_UTIL} -j ${THREADS} && \
	${MAKE_UTIL} install INSTALL_ROOT=${BIN_DIR}

clean:
	rm -r /tmp/hoverclock*
	rm ${APP_DIR}/*.AppImage
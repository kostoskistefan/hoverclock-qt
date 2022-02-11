APP_DIR		:= /app
SOURCE_DIR	:= /app/src
BUILD_DIR	:= /tmp/hoverclock_build
BIN_DIR		:= /tmp/hoverclock_bin
OUTPUT_DIR	:= /tmp/hoverclock

LINUX_DEPLOY 			:= /tmp/linuxdeploy-x86_64.AppImage
LINUX_DEPLOY_QT			:= /tmp/linuxdeploy-plugin-qt-x86_64.AppImage
LINUX_DEPLOY_APP_DIR	:= --appdir=${OUTPUT_DIR}
LINUX_DEPLOY_FLAGS		:= --plugin qt --output appimage --desktop-file=${APP_DIR}/hoverclock.desktop

MAKE_UTIL 	:= make
QMAKE_UTIL 	:= qmake

THREADS := $(shell nproc)

all: dir appimage

dir: 
	mkdir -p ${BUILD_DIR}

appimage: hoverclock appimage-fix
	${LINUX_DEPLOY} ${LINUX_DEPLOY_APP_DIR}

	cp ${BIN_DIR}/opt/hoverclock/bin/hoverclock ${OUTPUT_DIR}/usr/bin/
	cp ${APP_DIR}/hoverclock.desktop ${OUTPUT_DIR}/usr/share/applications/hoverclock.desktop
	cp ${SOURCE_DIR}/resources/icons/hoverclock.svg ${OUTPUT_DIR}/usr/share/icons/hicolor/scalable/apps/hoverclock.svg

	cd ${APP_DIR} && \
	${LINUX_DEPLOY} ${LINUX_DEPLOY_APP_DIR} ${LINUX_DEPLOY_FLAGS}

appimage-fix:
	dd if=/dev/zero bs=1 count=3 seek=8 conv=notrunc of=${LINUX_DEPLOY}
	dd if=/dev/zero bs=1 count=3 seek=8 conv=notrunc of=${LINUX_DEPLOY_QT}

hoverclock:
	cd ${BUILD_DIR} && \
	${QMAKE_UTIL} ${SOURCE_DIR} && \
	${MAKE_UTIL} -j ${THREADS} && \
	${MAKE_UTIL} install INSTALL_ROOT=${BIN_DIR}

clean:
	rm -r /tmp/hoverclock*; \
	rm ${APP_DIR}/*.AppImage

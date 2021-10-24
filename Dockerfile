FROM ubuntu:16.04
ENV DEBIAN_FRONTEND=noninteractive

WORKDIR /tmp

RUN apt-get update && \
    apt-get install -y software-properties-common && \
    add-apt-repository ppa:beineri/opt-qt-5.12.2-xenial && \
    apt-get update && \
    apt-get -y install wget qt512-meta-minimal make g++ fuse libgl1-mesa-dev

ENV PATH="/opt/qt512:/opt/qt512/bin:${PATH}"

RUN wget https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage && \
    wget https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage && \
    chmod +x linuxdeploy*.AppImage

VOLUME /app
WORKDIR /app

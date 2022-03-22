FROM ubuntu:16.04
ENV DEBIAN_FRONTEND=noninteractive

WORKDIR /tmp

RUN apt-get update && \
    apt-get install -y software-properties-common && \
    add-apt-repository ppa:beineri/opt-qt-5.15.2-xenial && \
    apt-get update && \
    apt-get -y install wget qt515base make g++ libgl1-mesa-dev xcb qt515svg

ENV PATH="/opt/qt515:/opt/qt515/bin:${PATH}"

RUN wget https://github.com/QuasarApp/CQtDeployer/releases/download/v1.5.4.14/CQtDeployer_1.5.4.14_Linux_x86_64.deb && \
    chmod +x CQtDeployer_1.5.4.14_Linux_x86_64.deb && \
    dpkg -i CQtDeployer_1.5.4.14_Linux_x86_64.deb

VOLUME /app
WORKDIR /app

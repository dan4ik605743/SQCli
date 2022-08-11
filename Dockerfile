FROM debian:bullseye
MAINTAINER Danil Danevich

RUN apt-get -qq update && \
    # dependency for project
    apt-get -qq install -y g++ make binutils cmake && \

    # boost
    apt-get -qq install -y wget curl libcurl4-openssl-dev build-essential python-dev autotools-dev libicu-dev libbz2-dev libboost-all-dev && \
        wget -O boost_1_78_0.tar.gz https://sourceforge.net/projects/boost/files/boost/1.78.0/boost_1_78_0.tar.gz/download && \
        tar -xvf boost_1_78_0.tar.gz && \
        cd boost_1_78_0/ && \
        ./bootstrap.sh --prefix=/usr/ && \
        ./b2 && \
        ./b2 install \

    # sqlite3
    wget -c https://www.sqlite.org/2022/sqlite-autoconf-3390200.tar.gz && mkdir sqlite && cd sqlite && \
    tar xvfz ../sqlite-autoconf-3390200.tar.gz && cd sqlite-autoconf-3390200 && ./configure && make && make install

WORKDIR /usr/src/SQCli
COPY sqcli sqcli
COPY modules modules
COPY CMakeLists.txt ./

RUN cmake . && \
    make -j$(nproc) && \
    make install && \
    rm -rf /usr/src/tgbot-cpp/*

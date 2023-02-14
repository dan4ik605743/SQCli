FROM debian:sid
LABEL maintainer="6057430gu@gmail.com"

RUN apt-get update -y --no-install-recommends && \
    apt-get install -y --no-install-recommends g++ git ca-certificates make binutils cmake libboost-all-dev libsqlite3-dev && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /usr/src/SQCli
COPY . ./

RUN mkdir build && \ 
  cd build && \
  cmake .. && \
  make -j$(nproc) && \
  make install && \
  rm -rf /usr/src/SQCli/*

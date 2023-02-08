FROM ubuntu:20.04
LABEL maintainer="Niven"

# setup timezone
RUN echo 'Asia/Singapore' > /etc/timezone \
    && ln -s /usr/share/zoneinfo/Asia/Singapore /etc/localtime

RUN apt-get update && apt install -y --no-install-recommends \
    g++ clang git wget build-essential libsnappy-dev libboost-all-dev \
    libleveldb-dev cmake automake binutils autoconf ca-certificates \
    libevent-dev zlib1g-dev libc-ares-dev openssl libsystemd-dev \
    autotools-dev libtool pkg-config libcunit1-dev libjansson-dev \
    libjemalloc-dev ruby-dev bison libelf-dev

RUN git clone --recursive https://github.com/sieniven/blockchain-indexer.git
RUN mkdir /blockchain-indexer/third_party/json/build
WORKDIR /blockchain-indexer/third_party/json/build
RUN cmake .. -DCMAKE_BUILD_TYPE=Release
RUN make -j8 install

RUN mkdir /blockchain-indexer/build
WORKDIR /blockchain-indexer/build
RUN cmake .. -DCMAKE_BUILD_TYPE=Release
RUN make -j8
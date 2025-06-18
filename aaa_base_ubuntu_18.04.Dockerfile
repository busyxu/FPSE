FROM ubuntu:18.04

RUN apt-get update && \
    apt-get -y --no-install-recommends install \
        binutils \
        cmake \
        curl \
        ca-certificates \
        gcc-multilib \
        gcc-5-multilib \
        git \
        g++-multilib \
        g++-5-multilib \
        libgmp-dev \
        libgomp1 \
        libomp5 \
        libomp-dev \
        make \
        ninja-build \
        python3 \
        python3-setuptools \
        sudo\
        ccache && \
    apt-get clean && \
    ln -s /usr/bin/python3 /usr/bin/python

# Create `user` user for container with password `user`.  and give it
# password-less sudo access

ARG UID=1000
ARG GID=1000
RUN groupadd -g $GID aaagroup && \
    useradd -m -u $UID -g $GID -s /bin/bash aaa && \
    echo aaa:aaa | chpasswd && \
    cp /etc/sudoers /etc/sudoers.bak && \
    echo 'aaa  ALL=(root) NOPASSWD: ALL' >> /etc/sudoers
USER aaa
WORKDIR /home/aaa

ENV CCACHE_DIR=/home/aaa/.ccache
RUN mkdir -p $CCACHE_DIR
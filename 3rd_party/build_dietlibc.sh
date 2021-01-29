#!/usr/bin/env bash

DIET_LIBC_VERSION=0.34

TARBALL=dietlibc-${DIET_LIBC_VERSION}.tar.xz
DOWNLOAD_LINK=https://www.fefe.de/dietlibc/${TARBALL}
DIET_LIBC_DIR=dietlibc-${DIET_LIBC_VERSION}
DIET_LIBC_DIST=dietlibc-${DIET_LIBC_VERSION}-dist

if [ ! -d ${DIET_LIBC_DIST} ]; then
    if [ ! -d ${DIET_LIBC_DIR} ]; then
        if [ ! -f ${TARBALL} ]; then
            wget ${DOWNLOAD_LINK}
        fi
        tar -xvf ${TARBALL}
    fi

    cd ${DIET_LIBC_DIR}
    make -j2 
    DESTDIR=../${DIET_LIBC_DIST} make install
    cd -
fi


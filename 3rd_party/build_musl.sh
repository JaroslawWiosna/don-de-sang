#!/usr/bin/env bash

MUSL_VERSION=v1.2.2
MUSL_DIST=musl-${MUSL_VERSION}-dist

if [ ! -d ${MUSL_DIST} ]; then
    if [ ! -d "musl" ]; then
        git clone --depth=1 -b ${MUSL_VERSION} git://git.musl-libc.org/musl
    else
        cd musl
        CHECKOUTED_MUSL_VERSION=`git describe --tags --always`
        if [ "$MUSL_VERSION" = "$CHECKOUTED_MUSL_VERSION" ]; then
            echo Desired version is already checkouted.
        else
            echo Checkouted Musl version is not the one we want!
            git fetch --all --tags
            #TODO: Check if git version is at least x.yy
            # Because in old git git reset might not be supported
            # Consider building git from sources in CI
            git reset --hard HEAD
            git clean -fdx
            git checkout tags/${MUSL_VERSION}
        fi
        cd -
    fi

    cd musl
    ./configure

    mkdir ../${MUSL_DIST}
    DESTDIR=../${MUSL_DIST} make -j2 install
fi


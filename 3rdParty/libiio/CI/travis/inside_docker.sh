#!/bin/sh -xe

LIBNAME="$1"
OS_TYPE="$2"

export INSIDE_DOCKER="1"
export TRAVIS_BUILD_DIR="/$LIBNAME"

cd /$LIBNAME

/$LIBNAME/CI/travis/before_install_linux "$OS_TYPE"

/$LIBNAME/CI/travis/make_linux "$OS_TYPE"

# need to find this out inside the container
. /${LIBNAME}/CI/travis/lib.sh
echo "$(get_ldist)" > /${LIBNAME}/build/.LDIST

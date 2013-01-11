#!/bin/sh

VERSION=$1
HOST=$2
DATE=`date +%Y-%m-%d`

if [ x$VERSION = x -o x$HOST = x ]; then
	echo "Usage: $0 version username@host"
	exit 1
fi

set -e

cd `dirname $0`/../..
ROOT_DIR=`pwd`

echo $ROOT_DIR

rm -rf release
mkdir -p release
mkdir -p release/build
cd release/build
cmake ../..
make dist
cd ..
mv build/blockmodel-*.tar.gz .
for i in ../doc/*.rst; do
	cat $i | sed -e "s/@DATE@/${DATE}/g" | rst2html - >`basename $i .rst`-$VERSION.html
done
rm -rf build

scp * $HOST:www/live/assets/files/blockmodel

echo "Deployment done. Don't forget to: "
echo "  - tag the current revision in git with $VERSION"
echo "  - check the permissions on the remote server"

#!/bin/bash

source=$1
target=$2

libs=`find $source -name *.a`

for lib in $libs
do
    echo "extracting library $lib..."
    objs=`$AR t $lib`
    $AR x $lib
    mv $objs $2
done

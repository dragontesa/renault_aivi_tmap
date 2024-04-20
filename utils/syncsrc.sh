#!/bin/sh
SRC_PATH=/media/sf_DATA0/w0/tmap_rsm2018
rsync -av --exclude='.git' --exclude='.git*' --exclude='.settings' --exclude='navdata' --exclude='navdata_online' --exclude='static' --exclude='dynamic' --exclude='*.pro.user*' --exclude='.project' --exclude='.cproject'  $SRC_PATH .

#find $SRC_PATH -not -path "$SRC_PATH/.git*" -not -path "$SRC_PATH/.settings*" -not -path "$SRC_PATH/navdata" -not -path "$SRC_PATH/navdata/*" -not -path "$SRC_PATH/dynamic" -not -path "$SRC_PATH/dynamic/*" -not -path "$SRC_PATH/static" -not -path "$SRC_PATH/static/*" -not -path "$SRC_PATH/.project" -not -path "$SRC_PATH/.cproject" -not -path "$SRC_PATH/*.pro.user*" -exec cp -ruPv '{}' ./tmap_rsm2018 '{}' \;


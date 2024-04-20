#!/bin/bash
tail -f ~/tmap_rsm_ui_dbg.log | perl -pe 's/[[:alnum:]]\w+.qml/\e[1;34m$&\e[0m/g; s/[[:alnum:]]\w+.cpp/\e[1;31m$&\e[0m/g'

#!/bin/bash
export RSM2018_OUT=${WORKSPACE}/out
export RSM2018_PREFIX=${RSM2018_OUT}/tmp
export RSM2018_TARGET_PREFIX=${RSM2018_OUT}/release

# clean
rm -rf ${RSM2018_OUT}

# create directories
mkdir -p ${RSM2018_OUT}
mkdir -p ${RSM2018_PREFIX}
mkdir -p ${RSM2018_TARGET_PREFIX}

# generate version
re="#define\s+\w+\s+([0-9]+)"
major=0
minor=0
micro=0
line=$(grep "TMAP_VERSION_MAJOR" tmapconfig.h)
if [[ $line =~ $re ]]; then major=${BASH_REMATCH[1]}; fi
line=$(grep "TMAP_VERSION_MINOR" tmapconfig.h)
if [[ $line =~ $re ]]; then minor=${BASH_REMATCH[1]}; fi
line=$(grep "TMAP_VERSION_MICRO" tmapconfig.h)
if [[ $line =~ $re ]]; then micro=${BASH_REMATCH[1]}; fi

export VERSION_STRING="${major}.${minor}.${micro}"
export TMAP_BUILD_ID=$(git rev-parse --short HEAD)

echo version: ${VERSION_STRING}
echo build_id: ${TMAP_BUILD_ID}


# generate version #2 (since v0.18.035 by sangbong.woo)
major2=0
minor2=0
micro2=0

major2=$(grep "TMAP_VERSION_MAJOR" tmapconfig.h)
minor2=$(grep "TMAP_VERSION_MINOR" tmapconfig.h)
micro2=$(grep "TMAP_VERSION_MICRO" tmapconfig.h)

major2=${major2//[^0-9]}
minor2=${minor2//[^0-9]}
micro2=${micro2//[^0-9]}

export VERSION_STRING="${major2}.${minor2}.${micro2}-official"

echo version: ${VERSION_STRING}
echo build_id: ${TMAP_BUILD_ID}




# generate Makefile
cd ${RSM2018_OUT}
${RSM2018_QT_NEW_ROOT}/bin/qmake 'DEFINES+=TMAP_BUILD_ID=\\\"${TMAP_BUILD_ID}\\\"' ../tmap_rsm2018.pro -spec devices/linux-gen3-g++
if [[ $? -ne 0 ]]; then
	exit 1
fi

make qmake_all
if [[ $? -ne 0 ]]; then
	exit 1
fi

# make
make -j4
if [[ $? -ne 0 ]]; then
	exit 1
fi


# install
make install
if [[ $? -ne 0 ]]; then
	exit 1
fi

### cppcheck - static analysis check ###
#	Minimal set of arguments
# cppcheck --xml --xml-version=2 ${WORKSPACE} 2> ${RSM2018_OUT}/cppcheck-result.xml
#	Enable all checks
#if [ "$useCppCheck" = true ] ; then
###  cppcheck --enable=all --inconclusive --xml --xml-version=2 ${WORKSPACE} 2> ${RSM2018_OUT}/cppcheck-result.xml
#fi
#	Analysis in multiple parallel threads
#  cppcheck -j4 --enable=all --inconclusive --xml --xml-version=2 ${WORKSPACE} 2> ${RSM2018_OUT}/cppcheck-result.xml

# packaging
PACKAGE_OUT=${RSM2018_OUT}/package
PACKAGE_VERSION=${releaseDate}

if [ "$todayAsReleaseDate" = true ] ; then
  PACKAGE_DATE=$(date +"%G.%V.%u")
  PACKAGE_VERSION="dev_${PACKAGE_DATE}"
fi

PACKAGE_DIRNAME=${PACKAGE_VERSION}

mkdir -p ${PACKAGE_OUT}
cd ${PACKAGE_OUT}
tar -zxf ${WORKSPACE}/tools/bosch-package-skeleton.tar.gz
mv bosch-package-skeleton ${PACKAGE_DIRNAME}

echo "${PACKAGE_VERSION}" > ${PACKAGE_DIRNAME}/version.txt

mv -f ${RSM2018_TARGET_PREFIX}/* ${PACKAGE_DIRNAME}/artefacts/

export ARCHIVE_FILENAME="tmap_release_${VERSION_STRING}-${TMAP_BUILD_ID}.tar.gz"
tar -czf ${ARCHIVE_FILENAME} ${PACKAGE_DIRNAME}
if [[ $? -ne 0 ]]; then
	exit 1
fi


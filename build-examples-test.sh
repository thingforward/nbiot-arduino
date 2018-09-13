#!/bin/bash

BD=$(mktemp -d -t nbiot-arduino-build)
ED="./examples"

read -r -d '' PIOINI << EOM
[env:builddefault]
platform = atmelsam
board = mkr1000USB
framework = arduino
monitor_speed = 115200
build_flags = -DNBIOT_DEBUG0 -DNBIOT_DEBUG1
EOM

echo Building in ${BD}

RES=0

EXAMPLES=$(cd ${ED} && find . -type d -depth 1)
for EXAMPLE in ${EXAMPLES}; do
  echo building ${EXAMPLE} ...

  mkdir ${BD}/${EXAMPLE}
  mkdir ${BD}/${EXAMPLE}/{src,lib}
  cp ${ED}/${EXAMPLE}/* ${BD}/${EXAMPLE}/src/

  # symlink library to there
  ln -s $(pwd) ${BD}/${EXAMPLE}/lib/nbiot-arduino

  # put in default ini file
  echo "${PIOINI}" >${BD}/${EXAMPLE}/platformio.ini

  ( cd ${BD}/${EXAMPLE} && pio run -s )
  if [[ $? -eq 0 ]]; then
    echo SUCCESS
  else
    echo FAILED
    RES=1
  fi
done 

rm -r ${BD}

exit ${RES}

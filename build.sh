#!/bin/bash 

while getopts ":d:D:r:R" opt; do
  case $opt in
    d) 
      cmake -DCMAKE_BUILD_TYPE=Debug CMakeLists.txt
      ;;
    \?)
      cmake -DCMAKE_BUILD_TYPE=Release CMakeLists.txt
      ;;
  esac
done

make -j12


#!/bin/bash

echo "error 0";
../src/compiler < ../tests/error0.imp
echo "error 1";
../src/compiler < ../tests/error1.imp
echo "error 2";
../src/compiler < ../tests/error2.imp
echo "error 3";
../src/compiler < ../tests/error3.imp
echo "error 4";
../src/compiler < ../tests/error4.imp
echo "error 5";
../src/compiler < ../tests/error5.imp
echo "error 6";
../src/compiler < ../tests/error6.imp
echo "error 7";
../src/compiler < ../tests/error7.imp
echo "error 8";
../src/compiler < ../tests/error8.imp

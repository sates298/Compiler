#!/bin/bash

echo "error 0";
./compiler -v error0.imp error.mr
echo "error 1";
./compiler -v error1.imp error.mr
echo "error 2";
./compiler -v error2.imp error.mr
echo "error 3";
./compiler -v error3.imp error.mr
echo "error 4";
./compiler -v error4.imp error.mr
echo "error 5";
./compiler -v error5.imp error.mr
echo "error 6";
./compiler -v error6.imp error.mr
echo "error 7";
./compiler -v error7.imp error.mr
echo "error 8";
./compiler -v error8.imp error.mr

rm error.mr
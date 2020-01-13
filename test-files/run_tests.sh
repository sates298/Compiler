#!/bin/bash

echo "error 0";
./compiler error0.imp error.mr
echo "error 1";
./compiler error1.imp error.mr
echo "error 2";
./compiler error2.imp error.mr
echo "error 3";
./compiler error3.imp error.mr
echo "error 4";
./compiler error4.imp error.mr
echo "error 5";
./compiler error5.imp error.mr
echo "error 6";
./compiler error6.imp error.mr
echo "error 7";
./compiler error7.imp error.mr
echo "error 8";
./compiler error8.imp error.mr

rm error.mr
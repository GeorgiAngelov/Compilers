#!/bin/bash
echo "===================";
echo "Executing POSITIVE self test";
echo "===================";
echo "";
for i in ../selftest_pos/*.lig;
 do 
	echo "Executing $i:";
	./pa2 $i;
	echo "";
 done
 
echo "============================";
echo "Executing NEGATIVE self test";
echo "============================";
echo "";
for i in ../selftest_neg/*.lig;
 do 
	echo "Executing $i:";
	./pa2 $i;
	echo "";
 done
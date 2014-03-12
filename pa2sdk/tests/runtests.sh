#!/bin/bash

first_test=1
last_test=30

if test $# -lt 1 || test $# -gt 3
then
echo "Usage: $0 <path_to_executable> [first_test] [last_test]"
echo ""
echo "Examples:"
echo "To run tests 1 through $last_test:"
echo "$0 ../src/pa2"
echo "To run tests 1 through 4:"
echo "$0 ../src/pa2 1 4"
echo "To run test 4:"
echo "$0 ../src/pa2 4"
exit 1
fi

if test $# -eq 2
then
first_test=$2
last_test=$2
fi

if test $# -eq 3
then
first_test=$2
last_test=$3
fi

for i in `seq $first_test $last_test`
do
      echo "*** test$i.lig ***"
      $1 "test$i.lig" >& out.tmp
      diff out.tmp "test$i.out"
      rm out.tmp
done

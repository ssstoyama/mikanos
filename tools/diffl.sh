#!/bin/sh

FILES=`ls kernel | grep -e .c$ -e .cpp$ -e .hpp$ -e .asm$ -e Makefile`

for file in $FILES
do
  echo $file
  diff sample/kernel/$file kernel/$file
done
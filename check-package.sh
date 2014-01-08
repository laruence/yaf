#!/bin/bash

for src in *.c */*.c *.h */*.h tests/*phpt tests/*ini
do
  grep -q $(basename $src) package.xml || echo "Missing $src"
done

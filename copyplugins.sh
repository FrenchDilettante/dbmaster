#!/bin/sh
for f in `find ../dbmaster-build-desktop -name lib*.so -print`; do
  cp $f src/
done

#!/bin/bash

echo "****************************************"
echo "Archiving for anti-stealing $1"
git archive --format=tar --prefix=anti-stealing/ $1 | (cd .. && gzip > anti-stealing-$1.tar.gz)
echo "Complete"
echo "****************************************"

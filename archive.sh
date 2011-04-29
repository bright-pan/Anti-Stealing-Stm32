#!/bin/bash

echo "****************************************"
echo "Archiving for anti-stealing $1"
git archive --format=tar --prefix=Anti-Stealing/ $1 | (cd .. && gzip > Anti-Stealing-$1.tar.gz)
echo "Complete"
echo "****************************************"

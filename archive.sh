#!/bin/bash

echo "****************************************"
echo "Archiving for anti-stealing $1"
git archive --format=tar --prefix=hchq/ $1 | (cd .. && gzip > $1.tar.gz)
echo "Complete"
echo "****************************************"

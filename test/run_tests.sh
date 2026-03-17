#!/bin/bash
set -ex

sudo service postgresql start
sudo -u postgres make USE_PGXS=1 installcheck || (cat regression.diffs; cat regression.out)


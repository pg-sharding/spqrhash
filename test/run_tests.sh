#!/bin/bash
set -ex

sudo service postgresql start
sudo -u postgres make installcheck || (cat regression.diffs; cat regression.out)


#!/bin/bash
set -ex

# Initialize PostgreSQL
su postgres -c "pg_ctl init -D /var/lib/postgresql/data"

# Start PostgreSQL
su postgres -c "pg_ctl start -D /var/lib/postgresql/data -o '-c listen_addresses=localhost'"

# Wait for PostgreSQL to be ready
sleep 2


cd /spqrhash
if ! make USE_PGXS=1 installcheck PGUSER=postgres TAP_TESTS=; then
    if [ -f /spqrhash/regression.diffs ]; then
        echo "=== regression.diffs ==="
        cat /spqrhash/regression.diffs
    fi
    if [ -f /spqrhash/output_iso/regression.diffs ]; then
        echo "=== isolation.diffs ==="
        cat /spqrhash/output_iso/regression.diffs
    fi
    exit 1
fi


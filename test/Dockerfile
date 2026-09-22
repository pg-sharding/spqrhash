ARG PG_VERSION=14
FROM postgres:${PG_VERSION}

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    postgresql-server-dev-${PG_MAJOR} \
    python3-docutils \
    && rm -rf /var/lib/apt/lists/*

COPY . /spqrhash
WORKDIR /spqrhash

RUN make USE_PGXS=1 && make USE_PGXS=1 install

ENTRYPOINT [ "/spqrhash/test/run_tests_docker.sh" ]
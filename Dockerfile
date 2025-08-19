ARG codename=jammy
FROM ubuntu:${codename}

ARG POSTGRES_VERSION=15

ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Israel
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone
RUN apt update && apt install -y ca-certificates

RUN apt-get update -o Acquire::AllowInsecureRepositories=true && apt-get install -y --no-install-recommends --allow-unauthenticated \
	curl lsb-release ca-certificates gnupg

RUN install -d /usr/share/postgresql-common/pgdg && \
    curl -o /usr/share/postgresql-common/pgdg/apt.postgresql.org.asc --fail https://www.postgresql.org/media/keys/ACCC4CF8.asc && \
    echo "deb [signed-by=/usr/share/postgresql-common/pgdg/apt.postgresql.org.asc] https://apt.postgresql.org/pub/repos/apt $(lsb_release -cs)-pgdg main" > /etc/apt/sources.list.d/pgdg.list

RUN apt-get update && apt-get install -y --no-install-recommends \
    sudo postgresql-server-dev-${POSTGRES_VERSION} postgresql-${POSTGRES_VERSION} \
    gcc g++ make python3-docutils

ENV PATH=$PATH:"/usr/lib/postgresql/${POSTGRES_VERSION}/lib/pgxs/src/test/regress:/home/build-user/pgbin/lib/postgresql/pgxs/src/test/regress"

RUN groupadd -g 999 build-user && \
    useradd -r -u 999 -g build-user build-user

COPY . /home/build-user
RUN chown build-user:build-user /home -R && usermod -aG sudo build-user

RUN echo 'build-user ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers

USER build-user
WORKDIR /home/build-user

RUN make clean && sudo make install
RUN sudo chmod 777 -R /home/build-user

ENTRYPOINT ["./test/run_tests.sh"]

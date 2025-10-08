FROM ubuntu:22.04 AS builder

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libasio-dev \
    libpq-dev \
    libpqxx-dev \
    libssl-dev \
    pkg-config

WORKDIR /app

COPY . .

RUN cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
RUN cmake --build build

FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    libpq5 \
    libpqxx-6.4 \
    libssl3 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY --from=builder /app/build/AuthServer .

COPY .env .
COPY migrations ./migrations

EXPOSE 18080

CMD ["./AuthServer"]

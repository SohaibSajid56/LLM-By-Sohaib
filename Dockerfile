FROM debian:stable-slim

RUN apt-get update && apt-get install -y \
    g++ \
    curl \
    libcurl4-openssl-dev \
    libasio-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

RUN g++ -std=c++17 server.cpp memory_engine.cpp btree.cpp hashtable.cpp \
    -lcurl -lpthread -o server

CMD ["./server"]

FROM ubuntu:22.04

RUN apt update && apt install -y \
    g++ \
    cmake \
    curl \
    libcurl4-openssl-dev \
    libasio-dev \
    libboost-all-dev

WORKDIR /app

COPY . .

# Build the HTTP server
RUN g++ -std=c++17 server.cpp memory_engine.cpp btree.cpp hashtable.cpp -lcurl -pthread -o server

CMD ["./server"]

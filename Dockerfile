FROM debian:stable-slim

RUN apt-get update && apt-get install -y \
    g++ curl libcurl4-openssl-dev wget unzip \
    && rm -rf /var/lib/apt/lists/*

# Install standalone ASIO (header-only)
RUN wget https://sourceforge.net/projects/asio/files/asio/1.28.1/asio-1.28.1.tar.gz/download -O asio.tar.gz \
    && tar -xzf asio.tar.gz \
    && cp -r asio-1.28.1/asio/include/* /usr/local/include/ \
    && rm -rf asio-1.28.1 asio.tar.gz

WORKDIR /app

COPY . .

RUN g++ -std=c++17 server.cpp memory_engine.cpp btree.cpp hashtable.cpp \
    -lcurl -pthread -o server

EXPOSE 8080

CMD ["./server"]

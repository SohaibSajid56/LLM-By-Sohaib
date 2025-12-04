FROM ubuntu:22.04

RUN apt update && apt install -y g++

WORKDIR /app

COPY . .

RUN g++ -std=c++17 main.cpp memory_engine.cpp btree.cpp hashtable.cpp -o engine

CMD ["./engine"]

# K-D Tree

## Introduction
K-D Tree is widely used for efficiently query the distance among vectors.

## Data
Our K-D Tree exploits a Point class for storing vectors. The users are responsible for 
implementing their favorite I/O, maintaining their own point data. Currently we only
support Google protobuf for serialization. Please specify the /path/to/libprotobuf.so
and /path/to/header-of-protobuf in Makefile if needed.

## Environment Requirment
  * GCC 4.9 or higher

    ```bash
    tar zxf gcc-4.9.2.tar.gz;cd gcc-4.9.2;contrib/download_prerequisites;cd ..;mkdir buildc;cd buildc;../gcc-4.9.2/configure --disable-multilib;make -j 32;sudo make install;cd ..;
    ```
  * Google Protobuf

    ```bash
    sudo apt-get install -y libprotobuf-dev; sudo apt-get install -y protobuf-compiler;
    ```

## Build
  * The first time use this libaray, you should first compile the protobuf implementation of kdtree.proto:

    ```bash
    make proto;make
    ```

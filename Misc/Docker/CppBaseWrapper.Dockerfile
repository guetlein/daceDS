FROM cpp-build-base:0.2 AS build

WORKDIR /cppwrapper
COPY ./cppwrapper ./

RUN make 


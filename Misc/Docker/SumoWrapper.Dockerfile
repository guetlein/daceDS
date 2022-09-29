#FROM cpp-build-base:0.2 AS build
#FROM cppwrapper:0.1.0 AS cppwrapper
FROM cppwrapper:0.2.1 AS cppwrapper
#kafka & avro libs from comfluent repo are built with gcc4.8... -> c++11 string

RUN apt-get update &&  apt-get -y install zip curl

WORKDIR /sumo-1_13_0   
COPY ./sumo-1_13_0 ./
RUN apt-get update && \
	apt-get install -y cmake
RUN mkdir -p /sumo-1_13_0/build/cmake-build
WORKDIR /sumo-1_13_0/build/cmake-build 

RUN cmake -D CMAKE_POSITION_INDEPENDENT_CODE=1 ../..
RUN make -j8
RUN make install


RUN apt install -y libpugixml-dev


#run after sumo, aws.zip changes frequently
#WORKDIR /aws
#RUN curl "https://awscli.amazonaws.com/awscli-exe-linux-x86_64.zip" -o "awscliv2.zip"
#RUN unzip awscliv2.zip
#RUN ./aws/install

RUN chmod 755 -R /opt/librdkafka/

RUN useradd -ms /bin/bash duser
WORKDIR /home/duser
COPY ./wrapper /home/duser/wrapper
WORKDIR /home/duser/wrapper
RUN make 



ENV SUMO_HOME=/usr/local/share/sumo
WORKDIR /home/duser
COPY *.sh ./
RUN chmod 755 *.sh


RUN ln -s /usr/local/bin/sumo /usr/bin/sumo
RUN ln -s /usr/local/bin/sumo /usr/bin/sumo-gui
#ENV LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:.:/wrapper/bin"
ENV LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:.:/home/duser/wrapper/bin"
RUN mkdir -p /home/duser/output
RUN mkdir -p /tmp/SimulationService
#RUN chown duser /tmp/SimulationService
RUN chmod 777 /tmp/SimulationService

RUN chown -R duser:duser /home/duser
USER duser
#RUN mkdir /home/duser/.aws/
#RUN ln -s /home/duser/resource/aws.cred /home/duser/.aws/credentials
ENTRYPOINT ["./run.sh"]





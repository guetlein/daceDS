FROM ubuntu

RUN apt-get update && apt-get install -y software-properties-common

RUN apt-get update && apt-get -y install curl
RUN apt-get update &&  apt-get -y install iproute2
RUN apt-get update &&  apt-get -y install gdb
RUN apt-get update &&  apt-get -y install openjdk-13-jre-headless
RUN apt-get update &&  apt-get -y install zip

#RUN curl "https://awscli.amazonaws.com/awscli-exe-linux-x86_64.zip" -o "awscliv2.zip"
#RUN unzip awscliv2.zip
#RUN ./aws/install


RUN useradd -ms /bin/bash duser
USER duser
WORKDIR /home/duser

COPY *.sh ./
ADD bin bin
COPY log4j.properties ./bin
COPY log4j.properties log4j.properties

USER root
RUN chmod 755 *.sh
RUN chmod -R 755 bin
RUN chown duser:duser -R .
USER duser


#RUN mkdir /home/duser/.aws/
#RUN ln -s /home/duser/resource/aws.cred /home/duser/.aws/credentials
ENTRYPOINT ["./run.sh"]


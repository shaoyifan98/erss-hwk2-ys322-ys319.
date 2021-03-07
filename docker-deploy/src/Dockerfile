FROM ubuntu:18.04
RUN apt-get update && apt-get -y install g++ make
RUN mkdir /src
RUN mkdir /var/log/erss
WORKDIR /src
ADD . /src

CMD ["./testProxy"]

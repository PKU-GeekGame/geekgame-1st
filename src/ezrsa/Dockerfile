FROM ubuntu:20.04

ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update
RUN apt-get install -y python3 xinetd python3-openssl python3-pip

COPY . .
RUN pip install -r requirements.txt

COPY ezrsa.xinetd /etc/xinetd.d/ezrsa

CMD xinetd -dontfork

EXPOSE 3000
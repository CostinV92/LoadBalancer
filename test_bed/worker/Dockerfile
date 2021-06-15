FROM ubuntu:20.04
RUN apt-get update
RUN apt-get install iproute2 inetutils-ping systemctl -y
RUN kill $(pidof worker); exit 0
COPY worker /usr/bin/worker
CMD /vagrant/worker/delayed_start.sh

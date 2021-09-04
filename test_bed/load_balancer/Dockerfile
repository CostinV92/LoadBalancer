FROM ubuntu:20.04
RUN apt-get update
RUN apt-get install iproute2 inetutils-ping systemctl -y
RUN kill $(pidof load_balancer); exit 0
CMD /vagrant/load_balancer/startup.sh


FROM ubuntu:20.04
RUN apt-get update
RUN apt-get install iproute2 inetutils-ping cron systemctl net-tools -y
RUN kill $(pidof client); exit 0
CMD /vagrant/client/startup.sh

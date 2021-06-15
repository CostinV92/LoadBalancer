FROM ubuntu:20.04
RUN apt-get update
RUN apt-get install iproute2 inetutils-ping cron systemctl -y
RUN kill $(pidof client); exit 0
COPY client /usr/bin/client
COPY client_cron /etc/cron.d/client_cron
RUN chmod 0644 /etc/cron.d/client_cron
RUN crontab /etc/cron.d/client_cron
RUN systemctl enable cron
CMD cron -f

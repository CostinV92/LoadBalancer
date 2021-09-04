#!/bin/bash
cp /vagrant/client/client /usr/bin/client
cp /vagrant/client/client_cron /etc/cron.d/client_cron
chmod 0644 /etc/cron.d/client_cron
crontab /etc/cron.d/client_cron
systemctl enable cron
cron -f

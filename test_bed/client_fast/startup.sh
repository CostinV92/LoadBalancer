#!/bin/bash
cp /vagrant/client_fast/client /usr/bin/client

sleep 5
while :
do
    client
    sleep 1
done

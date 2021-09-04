#!/bin/bash
cp /vagrant/worker/worker /usr/bin/worker

sleep 5
while :
do
    worker
    sleep 3
done
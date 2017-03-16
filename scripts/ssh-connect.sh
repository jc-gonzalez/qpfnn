#! /bin/bash

if [ $# -lt 2 ]; then
    s=$(basename $0)
    echo "usage:  $s <thisHostName> <host1> <host2> ... <hostN>"
    exit 1
fi  

this_host=$1
shift
rest_of_hosts=$*

chmod 700 ~/.ssh
chmod 644 ~/.ssh/*

for h in ${rest_of_hosts}; do
    # Copy public key of this host to the rest of hosts
    scp ~/.ssh/id_rsa.pub eucops@${h}:.ssh/id_rsa.${this_host}.pub 
    cat .ssh/id_rsa.pub | ssh eucops@${h} 'cat >> .ssh/authorized_keys'
    cat .ssh/id_rsa.pub | ssh eucops@${h} 'cat >> .ssh/known_hosts'

    # Copy public key from other host to this host 
    scp eucops@${h}:.ssh/id_rsa.pub ~/.ssh/id_rsa.${h}.pub 
    cat ~/.ssh/id_rsa.${h}.pub >> ~/.ssh/known_hosts 
    cat ~/.ssh/id_rsa.${h}.pub >> ~/.ssh/authorized_keys 
done

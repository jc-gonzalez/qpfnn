##############################################################################
# File       : CreateDockerContainer.sh
# Domain     : QPF.scripts
# Version    : 1.0
# Date       : 2016/04/01
# Copyright (C) 2015, 2016 J C Gonzalez
#_____________________________________________________________________________
# Purpose    : Install Docker and create base container
# Created by : J C Gonzalez
# Status     : Prototype
# History    : See <Changelog>
###############################################################################

# Install Docker
sudo yum install docker

# Start Docker daemon
sudo service docker restart

# Create Docker group and include eucops user into it
sudo groupadd docker
sudo usermod -aG docker eucops

# Show Docker info
docker info

# Create base container
if [ ! -f Dockerfile ]; then
    echo "ERROR: Container specification file Dockerfile not found in current folder"
    exit 1
fi

docker build -t qpf-debian:a .
  

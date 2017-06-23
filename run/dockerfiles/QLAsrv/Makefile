##############################################################################
# File       : Makefile
# Domain     : QPF.run.dockerfiles.QLAsrv
# Version    : 1.2
# Date       : 2016/12/18
# Copyright (C) 2015,2016,2017  J C Gonzalez
#_____________________________________________________________________________
# Purpose    : Generates docker images from Dockerfiles
# Created by : J C Gonzalez
# Status     : Prototype
# History    : See <Changelog>
###############################################################################

IMAGE_NAME_BASE=qla-srv-debian:base
IMAGE_NAME=qla-srv-debian:a

.PHONY: image baseimage complete mrproper clean

.SILENT:

all: image

complete: mrproper image

baseimage:
	docker image inspect $(IMAGE_NAME_BASE) >/dev/null 2>&1 \
	&& echo "Image $(IAMGE_NAME_BASE) already exists. Continuing." \
	|| docker build -t $(IMAGE_NAME_BASE) -f Dockerfile-base .

image: baseimage
	docker rmi -f $(IMAGE_NAME)  \
	|| echo "Cannot remove image $(IMAGE_NAME). Continuing."
	docker build -t $(IMAGE_NAME) .

clean:
	-echo "Cleanning . . ."

mrproper:
	docker rmi -f $(IMAGE_NAME_NAME)  \
	|| echo "Cannot remove image $(IMAGE_NAME_BASE). Continuing."
	docker rmi -f $(IMAGE_NAME)  \
	|| echo "Cannot remove image $(IMAGE_NAME). Continuing."

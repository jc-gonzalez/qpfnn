##############################################################################
# File       : Makefile
# Domain     : QPF.run.dockerfiles.QLA
# Version    : 1.2
# Date       : 2016/12/18
# Copyright (C) 2015,2016,2017  J C Gonzalez
#_____________________________________________________________________________
# Purpose    : Generates docker images from Dockerfiles
# Created by : J C Gonzalez
# Status     : Prototype
# History    : See <Changelog>
###############################################################################

IMAGE_NAME=qpf-debian:a

.PHONY: image clean

.SILENT:

image:
	docker rmi -f $(IMAGE_NAME) \
		|| echo "Cannot remove image $(IMAGE_NAME). Continuing."
	docker build -t $(IMAGE_NAME) .

clean:
	-echo "Cleanning . . ."

all: image


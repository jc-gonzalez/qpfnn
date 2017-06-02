##############################################################################
# File       : Makefile
# Domain     : QPF.run.dockerfiles
# Version    : 1.2
# Date       : 2016/12/18
# Copyright (C) 2015,2016,2017  J C Gonzalez
#_____________________________________________________________________________
# Purpose    : Generates docker images from Dockerfiles
# Created by : J C Gonzalez
# Status     : Prototype
# History    : See <Changelog>
###############################################################################

SUBDIRS = QLA LE1sim 

.PHONY: subdirs clean $(SUBDIRS)

.SILENT:

subdirs: $(SUBDIRS)

recreate:
	-for d in $(SUBDIRS); do ($(MAKE) -C $$d clean all ); done

clean:
	-for d in $(SUBDIRS); do ($(MAKE) -C $$d clean ); done

$(SUBDIRS):
	$(MAKE) -C $@

all: subdirs


#==============================================================================
# VERSION:        2.0
# DESCRIPTION:    Create container with its dependencies for the execution of QDT
# AUTHOR:         J. C. Gonzalez <JCGonzalez@sciops.esa.int>
# COMMENTS:
#   This file describes how to build a container for the execution of the
#   QLA Diagnostic Tools, as a service, to be used within a Docker Swarm.
# USAGE:
#   # Build QDT image (for now the image name:tag is always qpf-debian:a)
#   docker build -t qla-srv-debian:a .
#
#   # Run interactive
#   docker run -i -t --priviledged=true \
#       -v {procBinPath}:/qlabin \
#       -v {localPath}/:{imgPath} \
#       qpf-debian:a /bin/bash {imgPath}/{procElem} {imgPath}/dummy.cfg
#
#  Runing script {procElem} assumes the QDT is in /qlabin
#==============================================================================

# Base Docker image
FROM qla-srv-debian:base
MAINTAINER J C Gonzalez <JCGonzalez@sciops.esa.int>

LABEL esa.euclid.soc.qla.content="QDT caller as service" \
      esa.euclid.soc.qla.version="2.0" \
      esa.euclid.soc.qla.release-date="2017-07-31"

# install QDT caller
COPY qdtwrp.tgz /tmp/
RUN mkdir -p /qlabin && tar xvzfC /tmp/qdtwrp.tgz /qlabin && rm -f /tmp/qdtwrp.tgz

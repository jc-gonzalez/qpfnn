#==============================================================================
# VERSION:        1.2
# DESCRIPTION:    Create container with its dependencies for the execution of QDT
# AUTHOR:         J. C. Gonzalez <JCGonzalez@sciops.esa.int>
# COMMENTS:
#   This file describes how to build a container for the execution of the
#   QLA Diagnostic Tools.  The tools are not embeded in the container, but
#   they are rather installed in the host and mapped to internal container
#   paths when the container is executed.
# USAGE:
#   # Build QDT image (for now the image name:tag is always qpf-debian:a)
#   docker build -t qpf-debian:a .
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
FROM debian
MAINTAINER J C Gonzalez <JCGonzalez@sciops.esa.int>

LABEL esa.euclid.soc.qla.content="QDT required packages" \
      esa.euclid.soc.qla.version="1.2-RC2" \
      esa.euclid.soc.qla.release-date="2017-05-18"
      
# see update.sh for why all "apt-get install"s have to stay as one long line
RUN apt-get update && \
    apt-get install -y python2.7 \
                       python-scipy \
                       python-astropy \
                       python-zmq \ 
                    --no-install-recommends && \
    rm -rf /var/lib/apt/lists/*
    

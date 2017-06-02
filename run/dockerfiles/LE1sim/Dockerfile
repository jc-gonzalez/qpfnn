#==============================================================================
# VERSION:        1.2
# DESCRIPTION:    Create container with its dependencies for the execution of
#                 the fake LE1 processor
# AUTHOR:         J. C. Gonzalez <JCGonzalez@sciops.esa.int>
# COMMENTS:
#   This file describes how to build a container for the execution of the
#   fake LE1 processor.  The processor is embeded in the container.
# USAGE:
#   # Build QDT image (for now the image name:tag is always le1-sim-debian:a)
#   docker build -t le1-sim-debian:a .
#
#   # Run interactive
#   docker run -i -t --priviledged=true \
#       -v {procBinPath}:/le1bin \
#       -v {localPath}/:{imgPath} \
#       qpf-debian:a /bin/bash {imgPath}/{procElem} 
#
#  Runing script {procElem} assumes the LE1 processor is in /le1bin
#==============================================================================

# Base Docker image
FROM le1-sim-debian:base
MAINTAINER J C Gonzalez <JCGonzalez@sciops.esa.int>

LABEL esa.euclid.soc.qla.content="LE1 fake processor" \
      esa.euclid.soc.qla.version="1.2-RC2" \
      esa.euclid.soc.qla.release-date="2017-05-18"
      
# install LE1-fake software (placed in current directory)
COPY le1sim.tgz /tmp/
RUN mkdir -p /le1bin && tar xvzfC /tmp/le1sim.tgz /le1bin && rm -f /tmp/le1sim.tgz


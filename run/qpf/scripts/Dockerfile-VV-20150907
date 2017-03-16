FROM debian

# see update.sh for why all "apt-get install"s have to stay as one long line
RUN apt-get update && \
    apt-get install -y python2.7 \
    	    	       python-scipy \
                       python-astropy \
		       python-zmq \ 
    --no-install-recommends && rm -rf /var/lib/apt/lists/*

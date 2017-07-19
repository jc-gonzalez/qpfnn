#!/usr/bin/env python

"""
runContainer.py

This module is an example of a wrapper for a Pythoon script/application,
in change of retrieving information from the Docker Swarm Manager, so that
the script can be used as a service.  If address is not provided, then the
tag option is expected, and the wrapped script is executed in a normal
container.

The communication is done using a nanomsg REQ-REP connection, and the
script retrieves (in addition and prior to the true script arguments) the
command options --address, that provides the address where the wrapper is
to request the next bunch of data to process.

The wrapper provides to the swarm manager with the folder where any input
data file is to be found.  The manager is expected to store there the
files, and then to answer the requester with any additional information it
might need.

An example of how this script is to be called to be executed as a normal
container would be:

$ docker run -it -v /Users/jcgonzalez/ws/QPFnn/run/pth1/pth2:/tsk/proc \
      -v /Users/jcgonzalez/ws/QPFnn/run:/tsk \
      devel \
      python /tsk/runService.py \
          tag: 20180511T120323 \
          session_dir: /tmp/qpf/run/SESSIONID/tsk/ \
          processor: /Users/jcgonzalez/ws/QPFnn/run/pth1/pth2/testsrv_driver.py

An example of this script being used to create a swarm service would be:

$ docker run -it -v /Users/jcgonzalez/ws/QPFnn/run/pth1/pth2:/tsk/proc \
      -v /Users/jcgonzalez/ws/QPFnn/run:/tsk \
      devel \
      python /tsk/runService.py \
          address: 134.23.87.67:5432 \
          session_dir: /tmp/qpf/run/SESSIONID/tsk/ \
          processor: /Users/jcgonzalez/ws/QPFnn/run/pth1/pth2/testsrv_driver.py

"""

from __future__ import print_function


__author__    = 'J C Gonzalez <JCGonzalez@sciops.esa.int>'
__copyright__ = 'Copyright (C) 2015-2017 Euclid SOC Team'
__license__   = 'LGPL'
__vcs_id__    = '$Id$'
__version__   = '0.1.0' #Versioning: http://www.python.org/dev/peps/pep-0386/


#from nanomsg import Socket, REQ, REP
from datetime import datetime

import errno
import os
import sys
import contextlib

service_data = {
    'address': None,
    'session_dir': '/tmp/',
    'processor': None,
    'tag': None
}

@contextlib.contextmanager
def parse_command_line():
    """Parses command line

    This function parses the command line and removes the command line options
    address:, session_dir: and processor:, that are used exclusively by this script.
    It yields the modified arguments list to be passed on to the actual service
    application code, and upon completion the arguments are restored.
    """
    global service_data

    num_of_args = len(sys.argv)
    if num_of_args < 6:
        print('Usage:  runService.py session_dir: <path> processor: <script.py> '
              + '[ address: <IP:PORT> | tag: <datetag> ] '
              + '[ rest of options ]\n')
        print('If address: is provided and tag: command line is missing, it is '
              + 'assumed it works as a service, and requests data to the Task '
              + 'Manager via REQ-REP protocol layer.')
        exit(0)

    new_args = []
    xr = iter(xrange(num_of_args))
    for i in xr:
        is_service_option = False
        for k in service_data.keys():
            if (sys.argv[i] == (k + ":")):
                service_data[k] = sys.argv[i + 1]
                is_service_option = True
                next(xr)
                break
        if not is_service_option:
            new_args.append(sys.argv[i])

    sys._argv = sys.argv[:]
    sys.argv = new_args
    yield
    sys.argv = sys._argv


def mkdir_p(path):
    """Creates the directory provided, and all the needed parent folders.
    """
    try:
        os.makedirs(path)
    except OSError as exc:
        if exc.errno == errno.EEXIST and os.path.isdir(path):
            pass
        else:
            raise


def request_data_and_get_ack(addr, input_path):
    """Connects to the Task Manager to request for new input data

    This function is the main actor in the service wrapper, it connects via a REQ-REP
    connection to the Task Manager and requests new input data, providing information on
    the directory where these data should be placed.  The Task Manager will store the
    data to be processed into that directory, and will answer the request message, in
    order to trigger the service execution start.

    Args:
        addr       (str): The address to connect to the Task Manager (IP:PORT)
        input_path (str): Directory where the input data should be stored

    Returns:
        True/False: True if acknowledge message was received back from Task Manager
    """
    print('address: {}.  input_path: {}'.format(addr, input_path))
    return True

    # Create connection and message strings
    conn_addr = 'tcp://' + addr
    msg = b'SERVICE_DATA_REQUEST|input_path={0}'.format(input_path)

    with Socket(REQ) as sck:
        # Open connection to REQ-REEP protocol layer
        sck.connect(conn_addr)
        # Send message
        sck.send(msg)
        # Wait for answer from Task Manager
        ans = sck.recv()

    answer = ans.startswith('SERVICE_DATA_ACK', beg=0, end=15)
    return answer


def main():
    """Main function

    Parses command line arguments, send data request to Task Manager, and starts
    actual wrapped functionality (QDT) in case the data and acknowledge message
    was received back from the manager.
    """
    # Parse command line arguments, and proceed with new args
    with parse_command_line():
        # Define operation mode
        addr = service_data['address']
        run_as_service = addr != None

        # Create service specific folder to store input/output data
        if run_as_service:
            tag_date = datetime.utcnow().strftime("%Y%m%dT%H%M%S")
        else:
            tag_date = service_data['tag']
        host_dir_in = service_data['session_dir'] + tag_date + '/in/'
        srv_dir_in  = '/tsk/' + tag_date + '/in/'
        srv_dir_out = '/tsk/' + tag_date + '/out/'
        mkdir_p(srv_dir_in)
        mkdir_p(srv_dir_out)

        # Get wrapped module name (driver?)
        module_name = os.path.splitext(os.path.basename(service_data['processor']))[0]
        sys.path.append('/tsk/proc')
        proc_module = __import__(module_name)

        # Run the underlying (wrapped) script, requesting data just if executed as
        # a service
        if (not run_as_service) or request_data_and_get_ack(addr, host_dir_in):
            proc_module.main()


if __name__ == "__main__":
    main()

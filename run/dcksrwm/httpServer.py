import sys
import os.path
from os import curdir, sep
import signal
from threading import Thread
import urlparse
from BaseHTTPServer import HTTPServer, BaseHTTPRequestHandler


PortNum = 8080

class PUTHandler(BaseHTTPRequestHandler):
    def do_PUT(self):
        print "----- SOMETHING WAS PUT!! ------"
        print self.headers
        print "Uploaded file: {}".format(self.path)
        length = int(self.headers['Content-Length'])
        content = self.rfile.read(length)
        self.send_response(200)
        #print content

    # def do_GET(self):
    #     parsed_path = urlparse.urlparse(self.path)
    #     message_parts = [
    #             'CLIENT VALUES:',
    #             'client_address=%s (%s)' % (self.client_address,
    #                                         self.address_string()),
    #             'command=%s' % self.command,
    #             'path=%s' % self.path,
    #             'real path=%s' % parsed_path.path,
    #             'query=%s' % parsed_path.query,
    #             'request_version=%s' % self.request_version,
    #             '',
    #             'SERVER VALUES:',
    #             'server_version=%s' % self.server_version,
    #             'sys_version=%s' % self.sys_version,
    #             'protocol_version=%s' % self.protocol_version,
    #             '',
    #             'HEADERS RECEIVED:',
    #             ]
    #     for name, value in sorted(self.headers.items()):
    #         message_parts.append('%s=%s' % (name, value.rstrip()))
    #     message_parts.append('')
    #     message = '\r\n'.join(message_parts)
    #     self.send_response(200)
    #     self.end_headers()
    #     self.wfile.write(message)
    #     return

    def do_GET(self):
        if self.path=="/":
            print os.path.splitext(self.path)[0]
            print self.path
            print "my code"
            self.path="/index_example3.html"

        try:
            #Check the file extension required and
            #set the right mime type


            extension = os.path.splitext(self.path)[-1]
            mime_types = {
           '.html': 'text/html',
           '.jpg': 'image/jpg',
           '.png': 'image/png',
           '.gif': 'image/gif',
           '.js':  'application/javascript',
           '.css': 'text/css',
            }

            mimetype = mime_types.get(extension)
            sendReply = mimetype is not None

            if sendReply == True:
                with open(curdir+sep+self.path) as f:
                    #self.send_header('Content-type',mimetype)
                    self.send_response(200)
                    self.end_headers()
                    self.wfile.write(f.read())

            return
        except IOError:
            self.send_error(404,'File Not Found'% self.path)

def run_on(port=PortNum):
    print("Starting a server on port %i" % port)
    server_address = ('localhost', port)
    httpd = HTTPServer(server_address, PUTHandler)
    httpd.serve_forever()

if __name__ == "__main__":
    ports = [int(arg) for arg in sys.argv[1:]]
    for port_number in ports:
        server = Thread(target=run_on, args=[port_number])
        server.daemon = True # Do not make us wait for you to exit
        server.start()
    signal.pause() # Wait for interrupt signal, e.g. KeyboardInterrupt

import mimetypes, sys
import http.server
import socketserver
from urllib.parse import urlparse, parse_qs

ext2type = mimetypes.types_map.copy()

class MyServer(http.server.BaseHTTPRequestHandler):
  def do_GET(self):
    if self.path[:7] == '/something_complicated':
      args = parse_qs(urlparse(self.path).query)
      self.something_complicated(args)
    else:
      self.servefile()
  
  def servefile(self):
    path = self.path[1:]
    try:
      f = open(path, 'rb')
    except IOError:
      self.send_error(404, 'File not found')
      return
    self.send_response(200)
    self.send_header('Content-type', ext2type['.' + self.path.split('.')[-1]])
    self.end_headers()
    self.wfile.write(f.read())
    f.close()

  def something_complicated(self, args):
    print(f'something complicated {args}')
    raise NotImplementedError('Not Yet Implemented')

PORT = int(sys.argv[1])

with socketserver.TCPServer(("", PORT), MyServer) as httpd:
  print(f'serving @ {PORT}')
  httpd.serve_forever()


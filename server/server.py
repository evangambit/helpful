import cgi ,mimetypes, re, sqlite3, sys, time
import http.server
import socketserver
from urllib.parse import unquote, urlparse, parse_qs
import pystache

# conn = sqlite3.connect('comments.db')
# c = conn.cursor()

ext2type = mimetypes.types_map.copy()

class MyServer(http.server.BaseHTTPRequestHandler):
  def __init__(self, *args, **kwargs):
    super().__init__(*args, **kwargs)
    print('init')
  def do_GET(self):
    args = parse_qs(urlparse(self.path).query)
    if self.path == '/':
      self.index(*args)
    else:
      self.servefile()
  
  def do_PUT(self):
    if self.path[:5] != '/data':
      self.send_error(400, 'Put request\'s path must begin with "/data"')
      return
    path = '.' + self.path

    args = parse_qs(urlparse(path).query)
    if 'Content-Length' not in self.headers:
      self.send_error(400, 'Request\'s header does not have Content-Length')
      return
    try:
      content_length = int(self.headers['Content-Length'])
      assert content_length >= 0
    except:
      self.send_error(400, 'Content-Length must be an unsigned integer')
      return
    
    ctype = self.headers['Content-type']
    if 'Content-type' not in self.headers:
      self.send_error(400, 'Request\'s header does not have Content-type')
      return

    data = self.rfile.read(content_length)
    ctype, pdict = cgi.parse_header(self.headers['content-type'])
    if ctype == 'text/plain':
      with open(path, 'w+') as f:
        f.write(data.decode('utf8'))
    else:
      self.send_error(400, f'Unrecognized Content-type "{ctype}"')
      return
    
    self.send_response(200)
    self.end_headers()
  
  def servefile(self):
    print('servefile', self.path)
    try:
      f = open(self.path[1:], 'rb')
    except IOError:
      self.send_error(404, 'File not found')
      return
    self.send_response(200)
    # self.send_header('Content-type', 'image/png')
    self.send_header('Content-type', ext2type['.' + self.path.split('.')[-1]])
    self.end_headers()
    self.wfile.write(f.read())
    f.close()

  def index(self, *args):
    with open('template.html', 'r') as f:
      template_html = f.read()
    result = pystache.render(template_html, {
      'title': 'Lorem Ipsum!'
    })
    self.send_response(200)
    self.send_header("Content-type", "text/html")
    self.end_headers()
    self.wfile.write(result.encode())

PORT = int(sys.argv[1])

with socketserver.TCPServer(("", PORT), MyServer) as httpd:
  print(f'serving @ {PORT}')
  httpd.serve_forever()


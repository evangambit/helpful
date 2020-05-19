/**
 @param request   string             "www.foo.com/bar"
 @param headers   {string: string}   {"Content-type": "text/plain"}
 @param method    string             "GET"
 @param data      string
 @param callback  function(string request, string response)
 @param error     function(string request, string response)
 */
 function ajax(request, headers, method, data, callback, error) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = () => {
    if (xhttp.readyState == 4) {
      if (xhttp.status == 200) {
        if (callback) {
          callback(request, xhttp.responseText);
        }
      } else {
        if (error) {
          error(request, xhttp.responseText)
        }
      }
    }
  }
  xhttp.open(method, request, true);
  if (headers) {
    for (key in headers) {
      xhttp.setRequestHeader(key, headers[key])
    }
  }
  xhttp.send(data);
}

function get(path, callback) {
  if (!path.startsWith('./')) {
    path = './' + path;
  }
  let ctype = '';
  if (path.endsWith('.txt')) {
    ctype = 'text/plain';
  }
  if (!ctype) {
    throw Error('File "' + path + '" has unrecognized extension.');
  }
  ajax(path, {'Content-type': ctype}, 'GET', '', callback);
}

function put_textfile(path, text, callback) {
  // Paths should all start with './data/...'
  path = './' + path;
  ajax(path, {'Content-type': 'text/plain'}, 'PUT', text, callback);
}
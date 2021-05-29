/**
 * Get the last element of an array.
 */
Array.prototype.back = function() {
  return this[this.length - 1];
}

/**
 * Performs an asynchronous request.
 * @param {string} url to request
 * @param {object}
 * @param {string} GET, POST PUT, PATCH, or DELETE
 * @return {Promise}
 */
function ajax(url, headers, method, data, isJson) {
  if (!headers) {
    headers = {};
  }
  if (!method) {
    method = 'GET';
  }

  return new Promise((resolve, reject) => {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = () => {
      if (xhttp.readyState == 4) {
        if (xhttp.status == 200) {
          let response = xhttp.responseText;
          if (isJson) {
            response = JSON.parse(response);
          }
          resolve(response);
        } else {
          reject(xhttp.responseText)
        }
      }
    }
    xhttp.open(method, url, true);
    if (headers) {
      for (let key in headers) {
        xhttp.setRequestHeader(key, headers[key])
      }
    }
    xhttp.send(data);
  });
}

// Shuffle an array.
function shuffle(A) {
  for (let i = A.length - 1; i > 0; --i) {
    let idx = Math.random() * (i + 1) | 0;
    let s = A[i];
    A[i] = A[idx];
    A[idx] = s;
  }
}

// Example of a custom HTML tag.
class MyCustomView extends HTMLElement {
  constructor(backend) {
    super();
  }
  connectedCallback() {
    // called when added to the DOM
  }
}
window.customElements.define('my-custom-view', MyCustomView);

// Wrapper for cookies.
const cookies = {
  "set": (key, value) => {
    value = JSON.stringify(value)
    let newCookie = key + "=" + value + "; path=/; expires=Fri, 01 Jan 2038 01:01:01 GMT;"
    if (newCookie.length > 1000) {
      return false;
    }
    document.cookie = newCookie;
    return true;
  },
  "get": (key, defaultValue) => {
    let index = document.cookie.search(key + "=");
    if (index == -1) {
      return defaultValue;
    }
    let cookiesAfterKey = document.cookie.substr(index + key.length + 1);
    let semicolonIndex = cookiesAfterKey.search(";");
    if (semicolonIndex == -1) {
      semicolonIndex = cookiesAfterKey.length;
    }
    return JSON.parse(cookiesAfterKey.substr(0, semicolonIndex));
  },
  "contains": (key) => {
    return document.cookie.search(key + "=") !== -1;
  },
  "del": (key) => {
    document.cookie = key + '=; path=/; expires=Tue, 01 Oct 2019 01:01:01 GMT;';
  }
};

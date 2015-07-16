var assert = require('assert');
var hasp = require('./');

var client = hasp.Hasp();
assert.equal(typeof client.login, 'function');
assert.equal(client.login(), 0);
assert.equal(client.getSize(), 4032);
client.write("Hello");
assert.equal(client.read(), "Hello");
client.write("Hello World With A Long String");
assert.equal(client.read(), "Hello World With A Long String");

var assert = require('assert');
var hasp = require('./');

var client = hasp.Hasp();
assert.equal(typeof client.login, 'function');
assert.equal(client.login(), 0);
assert.equal(client.getSize(), 4032);

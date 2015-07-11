var assert = require('assert');
var hasp = require('./');

assert.equal(typeof hasp.login, 'function');
assert.equal(hasp.login(), 7); // Drive is not connected

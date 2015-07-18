var hasp = require('../build/Release/hasp');
var fixture = require('./fixture');

var client = hasp.Hasp();

exports.testFunctionsExist = function(assert) {
  assert.equals(typeof client.login, 'function');
  assert.equals(typeof client.logout, 'function');
  assert.equals(typeof client.getSize, 'function');
  assert.equals(typeof client.read, 'function');
  assert.equals(typeof client.write, 'function');
  assert.done();
}

exports.testLoginAndLogout = function(assert) {
  assert.ok(client.login());
  assert.ok(client.logout());
  assert.done();
}

exports.testLogout = function(assert) {
  assert.throws(client.logout, Error, 'Invalid handle');
  assert.done();
}

exports.testSize = function(assert) {
  client.login();
  assert.equals(client.getSize(), 4032);
  client.logout();
  assert.done();
}

exports.testReadWrite = function(assert) {
  var shortData = 'Hello World',
      longData = 'Lorem ipsum dolor sit amet, consectetur adipiscing elit.';
  client.login();
  client.write(shortData);
  assert.equals(client.read(), shortData);
  client.write(longData);
  assert.equals(client.read(), longData);
  client.logout();
  assert.done();
}

exports.testExceedSize = function(assert) {
  client.login();
  assert.throws(function() {
    client.write(fixture.fourKbiteString);
  }, Error, 'Storage max size exceeded');
  client.logout();
  assert.done();
}

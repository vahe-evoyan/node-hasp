var hasp = require('../build/Release/hasp');
var fixture = require('./fixture');
var configs = require('./configs');
var crypto = require('crypto');

var client = hasp.Hasp();

console.log(client);
/*
exports.testFunctionsExist = function(assert) {
  assert.equals(typeof client.login, 'function');
  assert.equals(typeof client.logout, 'function');
  assert.equals(typeof client.getSize, 'function');
  assert.equals(typeof client.read, 'function');
  assert.equals(typeof client.write, 'function');
  assert.done();
};
*/

exports.testLoginAndLogout = function(assert) {
  console.log('THIS0', client, client instanceof hasp.Hasp);
  client.login(configs.vendorCode, function(err, status){
    assert.ok(status);
    assert.equals(err, null);
    assert.done();
  });
  // assert.ok(client.logout());
};

/*
exports.testLogout = function(assert) {
  assert.throws(client.logout, Error, 'Invalid handle');
  assert.done();
};

exports.testSize = function(assert) {
  client.login(configs.vendorCode);
  assert.equals(client.getSize(), 4032);
  client.logout();
  assert.done();
};

exports.testReadWrite = function(assert) {
  var shortData = 'Hello World',
      longData = 'Lorem ipsum dolor sit amet, consectetur adipiscing elit.';
  client.login(configs.vendorCode);
  client.write(shortData);
  assert.equals(client.read(), shortData);
  client.write(longData);
  assert.equals(client.read(), longData);
  client.logout();
  assert.done();
};

exports.testReadWriteZero = function(assert) {
  var data = 'Hello \0 World';
  client.login(configs.vendorCode);
  client.write(data);
  assert.equals(client.read(), data);
  client.logout();
  assert.done();
};

exports.testRandowReadWrites = function(assert) {
  client.login(configs.vendorCode);
  for (var i = 0; i < 10; ++i) {
    var dataLength = parseInt(Math.random() * 2016),
        data = crypto.randomBytes(dataLength).toString();
    client.write(data);
    assert.equals(client.read(), data);
  }
  assert.done();
};

exports.testExceedSize = function(assert) {
  client.login(configs.vendorCode);
  assert.throws(function() {
    client.write(fixture.fourKbiteString);
  }, Error, 'Storage max size exceeded');
  client.logout();
  assert.done();
};
*/

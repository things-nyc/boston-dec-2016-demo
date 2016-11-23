var ttn = require('ttn');

var handleData = require('./handle-data');

var appEUI = process.env.NODE_TTN_EUI;
var accessKey = process.env.NODE_TTN_KEY;
var client = new ttn.Client('staging.thethingsnetwork.org', appEUI, accessKey);

client.on('uplink', function(msg) {
  console.log('Received message');
  handleData(msg);
});

client.on('error', function(err) {
  console.log('[ERROR] ', err.message);
});

client.on('activation', function(msg) {
  console.log('Device activated: ', msg.devEUI);
});


var documents = require('document');

var msg = JSON.parse(request.rawBody);

/*
var mock-msg = {
  devEUI: '00000000688E64E5',
  fields: {
     air_humidity: 21.875,
     air_pressure: 1012,
     air_temperature: 24.203125,
     ambient_light: 0,
     packet_type: 17,
     payload: 'ET1//xg0AP04AAAWsRj2/w==',
     soil_humidity: 99.609375,
     soil_temperature: 24.9609375,
     vbat: 7.999755859375,
     water_temperature: 22.69140625
  },
  counter: 48,
  port: 1,
  metadata: { 
     frequency: 904.3,
     datarate: 'SF7BW125',
     codingrate: '4/5',
     gateway_timestamp: 4143630827,
     channel: 2,
     server_time: '2016-12-02T20:31:52.429859147Z',
     rssi: -63,
     lsnr: 10.5,
     rfchain: 0,
     crc: 1,
     modulation: 'LORA',
     gateway_eui: '8DEDC7F4BF59AA10',
     altitude: 15,
     longitude: -73.98869,
     latitude: 40.68539
  }
};
*/

var fields = {
  counter: msg.counter,
  devEUI: msg.devEUI,
  port: msg.port,
  air_humidity: msg.fields.air_humidity,
  air_pressure: msg.fields.air_pressure,
  air_temperature: msg.fields.air_temperature,
  ambient_light: msg.fields.ambient_light,
  soil_humidity: msg.fields.soil_humidity,
  soil_temperature: msg.fields.soil_temperature,
  vbat: msg.fields.vbat,
  water_temperature: msg.fields.water_temperature,
  packet_type: msg.fields.packet_type,
  payload: msg.fields.payload,
  frequency: msg.metadata.frequency,
  datarate: msg.metadata.datarate,
  codingrate: msg.metadata.codingrate,
  gateway_timestamp: msg.metadata.gateway_timestamp,
  channel: msg.metadata.channel,
  server_time: msg.metadata.server_time,
  rssi: msg.metadata.rssi,
  lsnr: msg.metadata.lsnr,
  rfchain: msg.metadata.rfchain,
  crc: msg.metadata.crc,
  modulation: msg.metadata.modulation,
  gateway_eui: msg.metadata.gateway_eui,
  altitude: msg.metadata.altitude,
  longitude: msg.metadata.longitude,
  latitude: msg.metadata.latitude
};

if (msg.fields.packet_type == 0x11) {  
  // save data
  return documents.save(fields);
} else {
  return {error: 'Unsupported packet format.'};
}

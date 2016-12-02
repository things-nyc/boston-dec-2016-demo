var handleData = require('./handle-data');

var mock = {
  devEUI: '00000000688E64E5',
  fields: 
   { air_humidity: 21.875,
     air_pressure: 1012,
     air_temperature: 24.203125,
     ambient_light: 0,
     packet_type: 17,
     payload: 'ET1//xg0AP04AAAWsRj2/w==',
     soil_humidity: 99.609375,
     soil_temperature: 24.9609375,
     vbat: 7.999755859375,
     water_temperature: 22.69140625 },
  counter: 48,
  port: 1,
  metadata: 
   { frequency: 904.3,
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

handleData(mock);
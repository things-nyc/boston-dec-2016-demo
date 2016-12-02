var documents = require('document');
var moment = require('/modules/momentjs/moment.min.js');

var results = documents.query({"fields":"*"});

var data = results.result.documents;

var airHumidity = [];

data.forEach(function(item){
  var msg = JSON.parse(item.message);
  var time = moment.moment(msg.metadata.server_time).format('HH:mm Z');
  var dataItem = [
    time,
    msg.fields.air_humidity
  ];
  airHumidity.push(dataItem);
});

var raingardenModel = [[
  'Time',
  'Air Humidity',
]];

return raingardenModel.concat(airHumidity);

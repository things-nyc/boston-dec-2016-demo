var documents = require('document');
var moment = require('/modules/momentjs/moment.min.js');

var parseData = function(label, key) {
  var results = documents.query({"fields":"*"});
  var data = results.result.documents;
  var chartData = [];

  var raingardenModel = [[
    'Time',
    label,
  ]];
  
  data.forEach(function(item){
    var time = moment.moment(item.server_time).format('HH:mm Z');
    var dataItem = [
      time,
      parseInt(item[key])
    ];
    chartData.push(dataItem);
  });

  return raingardenModel.concat(chartData);
}

var data = parseData('Air Pressure', 'air_pressure');

return data;
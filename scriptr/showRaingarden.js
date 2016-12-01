var documents = require('document');

var results = documents.query({"fields":"*"});

var data = results.result.documents;

return data;

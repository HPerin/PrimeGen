/**
 * Created by lucas on 6/3/17.
 */

const net = require('net');
const Client = require('./client');
const Scheduler = require('./scheduler');
const Config = require('./config.json');

var clientList = [];
var scheduler = new Scheduler(Config);

net.createServer(function (socket) {
    clientList.push(new Client(socket, scheduler));
}).listen(Config.port);


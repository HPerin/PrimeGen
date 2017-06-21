/**
 * Created by lucas on 6/3/17.
 */

const net = require('net');
const Client = require('./client');
const Scheduler = require('./scheduler');

var clientList = [];
var scheduler = new Scheduler();

net.createServer(function (socket) {
    clientList.push(new Client(socket, scheduler));
}).listen(3000);


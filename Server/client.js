/**
 * Created by lucas on 6/3/17.
 */

var method = Client.prototype;

function Client(socket, scheduler) {
    this.socket = socket;
    this.scheduler = scheduler;
    this.last_block_sent = -1;
    this.current_blocks = [];

    this.current_message = '';

    var t = this;
    this.socket.on('data', function (data) {
        try {
            t.current_message += String(data);
            if (t.current_message.indexOf('$') !== -1) {
                var message_list = t.current_message.split('$');
                if (message_list.length > 1) {
                    for (var i = 0; i < message_list.length - 1; i++) {
                        //console.log('READ: ' + message_list[i]);
                        var message = JSON.parse(message_list[i]);
                        t._processMessage(message);
                    }
                }
                t.current_message = message_list[message_list.length - 1];
            }
        } catch (e) {
            t._sendMessage(JSON.stringify({
                type: 'ERROR',
                report: 'Invalid message. Should be in JSON format.'
            }), 'utf8', function () {
                t.socket.destroy();
            });
        }
    });

    this.socket.on('close', function () {
        t.current_blocks.forEach(function (id) {
            t.scheduler.releaseUnfinishedBlock(id);
        });
        t.current_blocks = [];
    });

    this.socket.on('error', function () { });
}

method._processMessage = function (message) {
    var t = this;
    if ('type' in message) {
        switch (message.type) {
            case 'BLOCK_REQUEST': {
                if (t.current_blocks.length < 128) {
                    t.scheduler.getUnfinishedBlock(function (row) {
                        t.current_blocks.push(String(row.id));
                        t._sendMessage(JSON.stringify({
                            type: 'BLOCK_REQUEST_RESPONSE',
                            block_start: row.block_start,
                            block_end: row.block_end,
                            id: String(row.id)
                        }));
                    });
                } else {
                    t._sendMessage(JSON.stringify({
                        type: 'ERROR',
                        report: 'Cannot request more than 10 blocks at the same time.'
                    }));
                }
            } break;
            case 'BLOCK_RESULT': {
                var index = t.current_blocks.indexOf(message.id);
                if (index !== -1) {
                    t.current_blocks.splice(index, 1);
                    t.scheduler.updateBlockData(message.data, message.block_start, message.block_end, message.id, function (valid) {
                        t._sendMessage(JSON.stringify({
                            type: 'BLOCK_RESULT',
                            valid: valid
                        }));
                    });
                } else {
                    t._sendMessage(JSON.stringify({
                        type: 'ERROR',
                        report: 'Invalid block id. id = ' + message.id + '.'
                    }), 'utf8', function () {
                        t.socket.destroy();
                    });
                }
            } break;
            case 'SNAPSHOT_REQUEST': {
                t._sendSnapshot(message.since);
            } break;
            default: {
                t._sendMessage(JSON.stringify({
                    type: 'ERROR',
                    report: 'Invalid message. Unknown type = ' + message.type + '.'
                }), 'utf8', function () {
                    t.socket.destroy();
                });
            }
        }
    } else {
        t._sendMessage(JSON.stringify({
            type: 'ERROR',
            report: 'Invalid message. Should have a type field.'
        }), 'utf8', function () {
            t.socket.destroy();
        });
    }
};

method._sendSnapshot = function () {
    var t = this;

    t.scheduler.getFinishedBlocks(t.last_block_sent, function (rows) {
        if (rows.length > 0) t.last_block_sent = rows[rows.length - 1].id;
        t._sendMessage(JSON.stringify(rows));
    });
};

method._sendMessage = function (message, format, callback) {
    console.log('WRITE: ' + message);
    this.socket.write(message + '$', format, callback);
};

module.exports = Client;

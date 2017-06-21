const sqlite3 = require('sqlite3');
const fs = require('fs');
const bignum = require('bignum');

var method = Scheduler.prototype;

var lock = false;

function Scheduler() {
    this.db = new sqlite3.Database('./prime.db');
    this.lastBlockStart = bignum("0");
    this.blocksOnProgress = [];

    var t = this;
    this.db.run(fs.readFileSync('./data.sql', 'utf8'), function (err) {
        if (err) throw err;

        t.db.prepare('select block_start from prime_blocks order by block_start desc limit 1').all(function (err, rows) {
            if (err) throw err;

            if (rows !== undefined && rows.length > 0) {
                t.lastBlockStart = bignum(rows[0].block_start).add(10000);
            }

            console.log('LAST BLOCK START: ' + t.lastBlockStart);
        });
    });

}

method.getFinishedBlocks = function (first_id, callback) {
    if (first_id === undefined) first_id = -1;

    this.db.prepare('select data from prime_blocks where confirmations >= 2 and id > ?').all(first_id, function (err, rows) {
        if (err) throw err;

        if (rows === undefined) rows = [];
        callback(rows);
    });
};

method.updateBlockData = function (data, block_start, block_end, id, callback) {
    var t = this;
    function insertData() {
        if (!lock) {
            lock = true;

            t.releaseUnfinishedBlock(id);
            t.db.prepare('select data from prime_blocks where block_start = ? and block_end = ?').all(block_start, block_end,
                function (err, rows) {

                var row = rows[0];
                if (row.data === null) {
                    t.db.prepare('update prime_blocks set data = ?, confirmations = ? where block_start = ? and block_end = ?').run(JSON.stringify(data), 1, block_start, block_end,
                        function (err) {
                        if (err) throw err;
                        lock = false;
                        callback(true);
                    });
                } else {
                    var new_data = JSON.parse(row.data);
                    new_data.sort(function (a, b) { return a - b; });
                    data.sort(function (a, b) { return a - b; });

                    var valid = true;
                    if (data.length !== new_data.length) valid = false;
                    if (valid) {
                        for (var i = 0; i < data.length; i++) {
                            if (data[i] !== new_data[i]) {
                                valid = false;
                                break;
                            }
                        }
                    }

                    if (valid) {
                        t.db.prepare('update prime_blocks set confirmations = confirmations + 1 where block_start = ? and block_end = ?').run(block_start, block_end,
                            function (err) {
                            if (err) throw err;
                            lock = false;
                            callback(true);
                        });
                    } else {
                        t.db.prepare('update prime_blocks set confirmations = ? where block_start = ? and block_end = ?').run(-1, block_start, block_end,
                            function (err) {
                            if (err) throw err;
                            lock = false;
                            callback(false);
                        });
                    }
                }
            });
        } else {
            setTimeout(function () {
                insertData();
            }, 0);
        }
    }

    insertData();
};

method.getUnfinishedBlock = function (callback) {
    var t = this;

    t.db.all('select * from prime_blocks where confirmations < 2', function (err, rows) {
        if (err) throw err;

        var found = false;
        if (rows !== undefined) {
            for (var i = 0; i < rows.length; i++) {
                if (t.blocksOnProgress.indexOf(String(rows[i].id)) === -1) {
                    found = true;
                    t.blocksOnProgress.push(String(rows[i].id));
                    callback(rows[i]);
                    break;
                }
            }
        }

        if (!found) {
            function insertBlock() {
                if (!lock) {
                    lock = true;
                    t.db.prepare('insert into prime_blocks (block_start, block_end) values (?, ?)').run(t.lastBlockStart.toString(), t.lastBlockStart.add(9999).toString(), function (err) {
                        if (err) throw err;
                    }).finalize(function (err) {
                        if (err) throw err;

                        t.lastBlockStart = t.lastBlockStart.add(10000);
                        lock = false;
                        t.getUnfinishedBlock(callback);
                    });
                } else {
                    setTimeout(function () {
                        insertBlock();
                    }, 0);
                }
            }

            insertBlock();
        }
    });
};

method.releaseUnfinishedBlock = function (id) {
    this.blocksOnProgress.splice(this.blocksOnProgress.indexOf(String(id)), 1);
};

module.exports = Scheduler;
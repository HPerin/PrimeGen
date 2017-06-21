create table if not exists prime_blocks (
    id integer primary key autoincrement,
    block_start text unique not null,
    block_end text not null,
    data text,
    confirmations integer default 0
);
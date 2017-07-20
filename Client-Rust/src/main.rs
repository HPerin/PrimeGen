#[macro_use]
extern crate json;

use std::net::TcpStream;
use std::io::Write;
use std::io::BufRead;
use std::io::BufReader;
use std::vec::Vec;
use std::thread;

pub struct Client {
    connection : TcpStream
}

pub struct Block {
    id : u64,
    block_start: u64,
    block_end: u64
}

impl Client {
    pub fn new() -> Client {
        let c = Client {
            connection : TcpStream::connect("127.0.0.1:5000").unwrap()
        };
        c.connection.set_nodelay(true).unwrap();
        return c;
    }

    fn read_until(&mut self, c : char) -> String {
        let mut reader = BufReader::new(&self.connection);
        let mut buffer : Vec<u8> = Vec::new();
        reader.read_until(c as u8, &mut buffer).unwrap();

        let buffer_size = buffer.len();
        buffer.remove(buffer_size-1);

        return String::from_utf8(buffer).unwrap();
    }

    fn request_block(&mut self) -> Block {
        self.connection.write("{ \"type\": \"BLOCK_REQUEST\" }$".as_bytes()).unwrap();
        let parsed_block = json::parse(&self.read_until('$')).unwrap();
        return Block {
            id : parsed_block["id"].as_str().unwrap().parse().unwrap(),
            block_start : parsed_block["block_start"].as_str().unwrap().parse().unwrap(),
            block_end : parsed_block["block_end"].as_str().unwrap().parse().unwrap(),
        };
    }

    fn send_result(&mut self, b : Block, v : Vec<json::JsonValue>) {
        let r = object!{
            "type" => "BLOCK_RESULT",
            "id" => b.id.to_string(),
            "block_start" => b.block_start.to_string(),
            "block_end" => b.block_end.to_string(),
            "data" => v
        };
        self.connection.write(r.dump().as_bytes()).unwrap();
        self.connection.write("$".as_bytes()).unwrap();
        self.read_until('$');
    }

    fn is_prime(&mut self, p : &u64) -> bool {
        if p % 2 == 0 { return false };
        let mut i : u64 = 3;
        let max = p / 2;
        while i < max {
            if p % i == 0 {
                return false;
            }

            i += 2;
        }
        return true;
    }

    fn process_primes(&mut self, b : &Block) -> Vec<json::JsonValue> {
        let mut v : Vec<json::JsonValue> = Vec::new();
        for p in b.block_start..(b.block_end+1) {
            if self.is_prime(&p) {
                v.push(p.into());
            }
        }
        return v;
    }

    pub fn run_once(&mut self) {
        let b : Block = self.request_block();
        let v = self.process_primes(&b);
        self.send_result(b, v);
    }
}

fn main() {
    let mut children = vec![];

    for _ in 0..10 {
        children.push(thread::spawn(move || {
            let mut c = Client::new();
            loop {
                c.run_once();
            }
        }));
    }

    for child in children {
        let _ = child.join();
    }
}
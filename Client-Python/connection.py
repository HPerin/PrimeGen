import socket
import threading
import json


class Connection:
    def __init__(self, target_address, target_port):
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.connect((target_address, target_port))
        self.lock = threading.Lock()

    def request_snapshot(self, since):
        self.lock.acquire()
        msg = {
            'type': 'SNAPSHOT_REQUEST',
            'since': since
        }
        self.socket.send(bytes(json.dumps(msg) + '$', 'UTF-8'))
        result = self.__read_until('$')
        self.lock.release()
        return result

    def request_block(self):
        self.lock.acquire()
        msg = {
            'type': 'BLOCK_REQUEST'
        }
        self.socket.send(bytes(json.dumps(msg) + '$', 'UTF-8'))
        result = self.__read_until('$')
        self.lock.release()
        return result

    def block_result(self, block_id, block_start, block_end, data):
        self.lock.acquire()
        msg = {
            'type': 'BLOCK_RESULT',
            'id': block_id,
            'block_start': block_start,
            'block_end': block_end,
            'data': data
        }
        self.socket.send(bytes(json.dumps(msg) + '$', 'UTF-8'))
        result = self.__read_until('$')
        self.lock.release()
        return result

    def __read_until(self, separator):
        line = ""
        while True:
            part = self.socket.recv(1)
            if part.decode('UTF-8') != separator:
                line += part.decode('UTF-8')
            elif part.decode('UTF-8') == separator:
                break
        return line
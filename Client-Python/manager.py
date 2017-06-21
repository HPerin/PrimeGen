from connection import Connection
import threading
import json
import multiprocessing
from block import Block


class Manager:
    def __init__(self, thread_count, target_address, target_port):
        self.connection = Connection(target_address, target_port)
        self.threads = []
        for i in range(0, thread_count):
            self.threads.append(threading.Thread(target=self.__run_single_thread))

    def run(self):
        for i in range(0, len(self.threads) - 1):
            self.threads[i].start()
        self.threads[len(self.threads) - 1].start()

    def __run_single_thread(self):
        while True:
            block = json.loads(self.connection.request_block())

            queue = multiprocessing.Queue()
            p = Block(block['block_start'], block['block_end'], queue)
            p.start()
            p.join()

            result = []
            while queue.qsize() > 0:
                result.append(queue.get())

            self.connection.block_result(block['id'], block['block_start'], block['block_end'], result)
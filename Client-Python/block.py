import multiprocessing


class Block(multiprocessing.Process):
    def __init__(self, block_start, block_end, queue):
        super().__init__()
        self.block_start = int(block_start)
        self.block_end = int(block_end)
        self.queue = queue

    def run(self):
        for i in range(self.block_start, self.block_end+1):
            if self.__is_prime(i):
                self.queue.put(i)

    def __is_prime(self, p):
        if p % 2 == 0:
            return False

        i = 3
        i_end = int(p / 2)
        while i <= i_end:
            if p % i == 0:
                return False
            i += 2
        return True
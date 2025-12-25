import queue
import time

import multiprocessing as mp
from multiprocessing import Queue, Process

from .tileGenerator import TileRenderer


def tileWorker(taskQueue):
    tile = TileRenderer()
    while True:
        try:
            chunk_data: dict = taskQueue.get(timeout=5)
            if chunk_data is None:
                break
                
            tile.generateTile(chunk_data)
            
        except queue.Empty:
            continue


class TileQueueManager:
    def __init__(self, maxWorkers=None):
        self.workers = []
        self.tileQueue = Queue()
        self.maxWorkers = maxWorkers or min(4, mp.cpu_count())
        self.recentChunks = {}
        self.dedupeWindow = 30
        
    def startWorkers(self):
        for _ in range(self.maxWorkers):
            worker = Process(target=tileWorker, args=(self.tileQueue,))
            worker.start()

            self.workers.append(worker)
    
    def stopWorkers(self):
        for _ in self.workers:
            self.tileQueue.put(None)

        for worker in self.workers:
            worker.join(timeout=10)
            if worker.is_alive():
                worker.terminate()
        
        self.workers.clear()
    
    def addTask(self, chunk_data):
        chunk_key = self._getChunkKey(chunk_data)
        now = time.time()
        self._pruneRecent(now)
        if chunk_key in self.recentChunks:
            return self.tileQueue.qsize()

        self.recentChunks[chunk_key] = now
        self.tileQueue.put(chunk_data)
        return self.tileQueue.qsize()

    def _getChunkKey(self, chunk_data):
        dimension = chunk_data.dimension
        chunk_x = chunk_data.chunkX
        chunk_z = chunk_data.chunkZ

        if chunk_x is None or chunk_z is None:
            min_x = min(block.x for block in chunk_data.blocks)
            min_z = min(block.z for block in chunk_data.blocks)
            chunk_x = min_x // 16
            chunk_z = min_z // 16

        return (dimension, chunk_x, chunk_z)

    def _pruneRecent(self, now):
        expired = [key for key, ts in self.recentChunks.items() if now - ts > self.dedupeWindow]
        for key in expired:
            self.recentChunks.pop(key, None)

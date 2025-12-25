import asyncio
import aiohttp

import multiprocessing as mp


class ChunksSender:
    def __init__(self, config: dict, resultQueue: mp.Queue):
        self.timeout = 5
        self.resultQueue = resultQueue
        self.url = config.get("api").get("chunks")
        self.maxConcurrency = config.get("chunkSending", {}).get("maxConcurrency", 4)
        self.semaphore = asyncio.Semaphore(self.maxConcurrency)
        
    async def _sendChunkData(self, session: aiohttp.ClientSession, data: dict) -> None:
        chunkX = data.get("chunkX")
        chunkZ = data.get("chunkZ")
        dimension = data.get("dimension")
        
        payload = {"chunk": data.get("chunk")}
        
        try:
            async with session.post(self.url, json=payload, timeout=self.timeout) as response:
                if response.status == 200:
                    self.resultQueue.put(("success", dimension, chunkX, chunkZ))

                else:
                    errorText = await response.text()
                    self.resultQueue.put(("error", dimension, chunkX, chunkZ))

                    print(f"[Mipmap] HTTP error {response.status} for chunk ({chunkX}, {chunkZ}): {errorText}")
                
        except aiohttp.ClientError as e:
            print(f"[Mipmap] Network error sending chunk ({chunkX}, {chunkZ}): {e}")
            self.resultQueue.put(("error", dimension, chunkX, chunkZ))
        
        except asyncio.TimeoutError as e:
            print(f"[Mipmap] Timeout sending chunk ({chunkX}, {chunkZ}): {e}")
            self.resultQueue.put(("error", dimension, chunkX, chunkZ))
        finally:
            self.semaphore.release()
            
    async def run(self, queue: mp.Queue) -> None:
        async with aiohttp.ClientSession() as session:
            while True:
                if not queue.empty():
                    await self.semaphore.acquire()
                    chunkData = queue.get()
                    asyncio.create_task(self._sendChunkData(session, chunkData))
                else:
                    await asyncio.sleep(0.1)

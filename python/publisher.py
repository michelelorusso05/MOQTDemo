# publisher.py - invia binari codificati via MoQ
# Il file invia file preprocessati dalla cartella ./frames/,
# li comprime e li divide in chunk di lunghezza predefinita.

import asyncio, moq
import glob
import time
import lz4.block

async def main():
    async with moq.Client("https://127.0.0.1:4443", tls_verify=False) as client:
        broadcast = client.create_broadcast("moqtest/1")
        print("Client creato")

        track = broadcast.publish_track("frames")
        
        broadcast.announce()
        print("Broadcast annunciato. Aspetto 2 secondi prima di iniziare a trasmettere...")
        
        await asyncio.sleep(2)

        files = sorted(glob.glob("./frames/*.bin"))
        print(f"Inizio invio di {len(files)} frame...")

        start_time = time.perf_counter()

        CHUNK_SIZE = 9 * 10 * 1024

        for index, f_name in enumerate(files):
            with open(f_name, "rb") as f:
                payload = lz4.block.compress(f.read(), store_size=False)
                
                # Calcola i microsecondi passati dall'inizio della trasmissione
                timestamp_us = int((time.perf_counter() - start_time) * 1_000_000)

                group = track.append_group()
                
                for i in range(0, len(payload), CHUNK_SIZE):
                    chunk = payload[i : i + CHUNK_SIZE]
                    group.write_frame(chunk, timestamp_us)

                print(f"Frame {index} inviato")

                group.finish()

            # Mantieni il framerate a 30 fps
            # Idealmente, bisognerebbe aspettare 1/30 di secondo meno il tempo impiegato
            # a svolgere il resto delle operazioni
            await asyncio.sleep(1 / 30)

        print("Pubblicato, ora aspetto...")
        await asyncio.sleep(3)

        track.finish()
        await asyncio.sleep(1)
        broadcast.close()
        print("Broadcast terminato")

if __name__ == "__main__":
    asyncio.run(main())
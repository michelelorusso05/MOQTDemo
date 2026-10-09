# MOQTDemo

## Introduzione
La demo qui presente permette di inviare dei file `.ply` preconvertiti su un canale di comunicazione MoQ per essere poi mostrati su un secondo programma e renderizzati in una finestra Raylib.

### Publisher
Il publisher è un semplice script Python, locato in python/publisher.py. Legge i frame da una cartella `./frames/` rispetto alla working directory dello script.

### Subscriber
Il subscriber è un programma C++ che legge i frame inviati e li mostra a schermo in una finestra Raylib con ambiente 3D. È possibile muoversi all'interno di questo ambiente e vedere la nuvola di punti da diversi angoli.

#### Nota su Raylib
L'eseguibile Raylib non è autocontenuto, necessita della cartella `shaders` nella stessa working directory, altrimenti non sarà in grado di caricare gli shader necessari per le instanced mesh. Bisogna solo assicurarsi di copiare la cartella la prima volta dopo la compilazione.

## Esecuzione
Il progetto del subscriber è la soluzione Visual Studio 2026 contenuta in questa cartella. Il progetto dovrebbe essere configurato a dovere per permettere la compilazione immediata.

Il publisher invece richiede solo l'esecuzione dello script e dell'installazione dei prerequisiti contenuti in `requirements.txt`. Bisogna prima generare la cartella dei frame partendo da una serie di `.ply`, come ad esempio la [Microsoft Voxelized Upper Bodies Collection](https://plenodb.jpeg.org/pc/microsoft).

La connessione tra i due è gestita da un terzo componente, il Relay MoQ. Entrambi i componenti cercano una connessione su https://localhost:4443 (non è un typo). È possibile utilizzare l'immagine Docker ufficiale del Relay oppure eseguirlo dalla Crate Rust apposita. È necessario eseguirlo in ogni caso in modalità di sviluppo, autogenerando i certificati e disabilitando l'autenticazione.

```batch
moq-relay --listen "[::]:4443" --listen-tls-generate "localhost" --auth-public "**"
```
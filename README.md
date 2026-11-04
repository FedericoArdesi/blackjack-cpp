# Blackjack da terminale

[![Licenza: MIT](https://img.shields.io/badge/licenza-MIT-blue.svg)](LICENSE)
![C++17](https://img.shields.io/badge/C%2B%2B-17-00599C.svg)

Un blackjack completo che si gioca nel terminale, scritto in **C++17 senza librerie esterne**.
Carte disegnate in ASCII/Unicode, colori, statistiche di sessione e le regole del casinò:
split, raddoppio, resa e assicurazione.

<!-- TODO: aggiungere uno screenshot in docs/images/screenshot.png e decommentare la riga sotto -->
<!-- ![Screenshot](docs/images/screenshot.png) -->

## Caratteristiche

- Sei mazzi mescolati insieme, con rimescolamento automatico
- Split (fino a 4 mani), raddoppio, resa e assicurazione
- Blackjack naturale pagato 3:2
- Statistiche di sessione: mani vinte e perse, serie di vittorie, fiches massime e minime
- Architettura a strati, con la logica separata da grafica e input
- Test automatici sulla logica del gioco

## Requisiti

- Un compilatore C++17 (`g++` o `clang++`)
- `make`

| Sistema | Come ottenerli |
| --- | --- |
| Linux | `sudo apt install build-essential` (Debian/Ubuntu) |
| macOS | `xcode-select --install` |
| Windows | [w64devkit](https://github.com/skeeto/w64devkit) o [MSYS2](https://www.msys2.org/) |

## Compilare e giocare

```bash
git clone https://github.com/FedericoArdesi/blackjack-cpp.git
cd blackjack-cpp
make run
```

Altri comandi:

| Comando | Cosa fa |
| --- | --- |
| `make` | compila e crea l'eseguibile `blackjack` (`blackjack.exe` su Windows) |
| `make run` | compila e avvia il gioco |
| `make test` | compila ed esegue i test della logica |
| `make demo SCENARIO=split` | avvia il gioco con carte decise (vedi sotto) |
| `make clean` | cancella i file generati |

Su Windows conviene usare [Windows Terminal](https://aka.ms/terminal) per vedere bene i caratteri
delle carte. Se compaiono simboli strani, imposta `USA_COLORI = false` in
`src/base/costanti.h` e ricompila.

## Come si gioca

Si parte con 100 fiches. Dal menu puoi giocare, vedere le statistiche o leggere le regole.
Durante la mano scegli l'azione dal menu a numeri.

- Vince chi si avvicina di più a 21 senza superarlo. Asso = 1 o 11, figure = 10.
- Il banco pesca finché ha meno di 17 e sta su tutti i 17.
- Puntate pari, minimo 2. Vittoria 1:1, blackjack naturale 3:2, pareggio restituisce la puntata.
- **Raddoppia**: puntata x2, una sola carta.
- **Dividi**: due carte di pari valore diventano due mani (massimo 4). Dopo lo split un 21 non è blackjack.
- **Arrenditi**: perdi metà puntata (non dopo uno split).
- **Assicurazione**: se il banco mostra un asso, puoi puntare metà puntata che abbia blackjack (paga 2:1).
- Se non hai più fiches per la puntata minima, la partita finisce.

Le regole principali (numero di mazzi, soglia del banco, pagamenti, fiches iniziali) sono costanti
in `src/base/costanti.h`.

## Struttura del progetto

```
blackjack/
├── src/
│   ├── main.cpp          punto d'ingresso e menu principale
│   ├── base/             costanti.h (impostazioni e regole), dati.h (enum e struct)
│   ├── terminale/        colori, pulizia schermo, pause
│   ├── logica/           regole del gioco (punteggi, mazzo, esiti)
│   ├── grafica/          disegno delle schermate e lettura dell'input
│   └── partita/          svolgimento di una mano e di una sessione
├── tests/                test automatici della logica
├── tools/                demo con carte decise, per provare i casi rari
├── docs/images/          diagrammi dell'architettura
├── Makefile
└── LICENSE
```

Il codice è diviso in strati: la **logica** non disegna niente, la **grafica** non decide le regole,
la **partita** le collega. Per questo la logica si può testare senza avviare il gioco.

## Architettura

I diagrammi in `docs/images/` mostrano come lavorano insieme i vari strati.

**I tre strati.** `main()` chiama la partita, che collega grafica e logica; terminale e dati sono usati da tutti.

![I tre strati: partita, grafica, logica](docs/images/architettura-strati.png)

**Il percorso di una mano**, dalla puntata al pagamento.

![Il percorso di una mano](docs/images/percorso-di-una-mano.png)

**La risoluzione della mano**: come la logica decide l'esito e aggiorna le fiches.

![Risoluzione della mano](docs/images/risoluzione-mano.png)

**Lo split**: come una mano diventa due (fino a un massimo di quattro).

![Lo split](docs/images/split.png)

## Test e demo

`make test` controlla punteggi, assi, split, raddoppio, assicurazione, pagamenti e statistiche.

Per provare a mano una situazione rara senza aspettare che esca per caso:

```bash
make demo SCENARIO=split
```

Scenari disponibili: `split`, `assicurazione`, `blackjack`, `asso`, `resa`.

## Licenza

Distribuito con licenza MIT. Vedi il file [LICENSE](LICENSE).

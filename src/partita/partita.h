// PARTITA: collega grafica e logica.
// La grafica disegna e legge la scelta, la logica dice cosa è permesso
// e cosa succede. L'unica funzione che serve fuori da qui è giocaSessione.
#pragma once

#include "base/dati.h"

// Più mani di fila, senza tornare ogni volta al menu.
void giocaSessione(Sessione& sessione);

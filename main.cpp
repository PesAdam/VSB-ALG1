/**
 * @file main.cpp
 * @brief Overenie DAG a doplnenie na kompletný DAG pomocou DFS.
 *
 * Program načíta orientovaný graf zo súboru, overí pomocou prehľadávania
 * do hĺbky (DFS), či je graf orientovaný acyklický graf (DAG). Ak áno,
 * doplní ho ďalšími hranami na kompletný DAG — teda na graf s maximálnym
 * počtom hrán @f$ |E| = \frac{n(n-1)}{2} @f$. Výsledok zapíše do súboru.
 *
 * @section Vstup
 * Textový súbor kde každý riadok obsahuje dvojicu celých čísel @c i @c j
 * oddelených bielym znakom — reprezentujú orientovanú hranu z vrcholu @c i
 * do vrcholu @c j. Vrcholy sú číslované od 0.
 *
 * @section Vystup
 * Na štandardný výstup sa vypíše:
 *  -# Či je graf DAG.
 *  -# Počet vrcholov a hrán vstupného grafu.
 *  -# Očakávaný počet hrán kompletného DAG podľa vzorca.
 *  -# Počet vrcholov a hrán výsledného kompletného DAG.
 *
 * Kompletný DAG sa zapíše do súboru s príponou @c _complete (napr.
 * @c graph1.txt → @c graph1_complete.txt).
 *
 * @author Projekt — Algoritmy 1
 * @date 2026
 */

#include "src/graph.hpp"

/**
 * @brief Hlavná funkcia — vstupný bod programu.
 *
 * Postup:
 *  -# Načíta meno vstupného súboru od používateľa.
 *  -# Načíta hrany grafu zo súboru a zostaví zoznam susedov @c adj[].
 *  -# Zavolá jeDAG() — ak graf nie je DAG, vypíše hlásenie a skončí.
 *  -# Vypíše body 1 a 2 na stdout (či je DAG, počty vrcholov a hrán).
 *  -# Odvodí meno výstupného súboru (@c vstup_complete.txt).
 *  -# Zavolá doplnDAG() — doplní hrany, zapíše súbor, vypíše body 3 a 4.
 *
 * @return @c 0 pri úspešnom behu,
 *         @c 1 ak sa súbor nedá otvoriť alebo je prázdny.
 */
int main() {
    // --- Načítanie súboru ---

    // Premenná pre meno súboru — používateľ ho zadá zo štandardného vstupu.
    string subor;
    cout << "Zadaj meno suboru: ";  // Výzva pre používateľa.
    cin >> subor;                   // Načítame meno súboru (jedno slovo, bez medzier).

    // Otvoríme súbor na čítanie (ifstream = input file stream).
    ifstream f(subor);

    // Ak sa súbor nepodarilo otvoriť (neexistuje, nemáme práva, ...), skončíme s chybou.
    if (!f) {
        cerr << "Chyba: subor '" << subor << "' sa neda otvorit!\n";
        return 1;   // Nenulový návratový kód signalizuje chybu.
    }

    // Dočasné pole na uloženie načítaných hrán.
    // Každá hrana je dvojica (from, to) — z akého vrcholu do akého.
    vector<pair<int,int>> hranyZoSuboru;

    // maxIdx sleduje najväčší číselný index vrcholu, ktorý sme videli.
    // Začíname na -1 — ak súbor bude prázdny, ostane -1 a to odhalíme neskôr.
    int maxIdx = -1, u, v;

    // Čítame súbor riadok po riadku — každý riadok obsahuje dve čísla: u a v.
    // Operátor >> číta číslo a preskočí biele znaky (medzery, nové riadky).
    // Cyklus skončí keď narazíme na koniec súboru alebo chybný formát.
    while (f >> u >> v) {
        hranyZoSuboru.push_back({u, v});          // Pridáme hranu do zoznamu.
        maxIdx = max(maxIdx, max(u, v));           // Aktualizujeme maximálny index vrcholu.
    }
    f.close();  // Zatvoríme súbor — už ho nepotrebujeme.

    // Ak maxIdx zostal -1, súbor bol prázdny alebo mal zlý formát.
    if (maxIdx < 0) {
        cout << "Subor je prazdny alebo nema spravny format.\n";
        return 1;
    }

    // Vrcholy sú číslované od 0 po maxIdx, teda ich je celkom maxIdx+1.
    n = maxIdx + 1;

    // Inicializujeme zoznam susedov — n prázdnych vektorov, jeden pre každý vrchol.
    adj.assign(n, {});

    // Naplníme zoznam susedov z načítaných hrán.
    // Pre každú hranu (from → to) pridáme 'to' do zoznamu susedov vrcholu 'from'.
    for (auto& [from, to] : hranyZoSuboru)   // Štruktúrovaná väzba (C++17) — rozbalíme dvojicu.
        adj[from].push_back(to);

    // --- Výstup podľa zadania: bod 1 a bod 2 ---

    // Bod 1: Informácia, či zadaný graf je DAG.
    // Bod 2: Počet vrcholov a počet hrán zadaného grafu.
    // Obe informácie vypíšeme po zistení výsledku DFS.

    // Zavoláme funkciu jeDAG(), ktorá:
    //   1. Spustí DFS na celý graf.
    //   2. Ak nájde cyklus, vráti false.
    //   3. Ak je to DAG, vráti true a vyplní pole topo[] topologickým poradím.
    if (!jeDAG()) {
        // Bod 1: Graf nie je DAG.
        cout << "Graf NIE JE DAG — obsahuje orientovany cyklus.\n";
        // Bod 2: Vypíšeme počty aj pre prípad, že graf nie je DAG.
        cout << "Pocet vrcholov: " << n
             << ", pocet hran: " << hranyZoSuboru.size() << "\n";
        return 0;   // Nemá zmysel doplňovať hrany — skončíme.
    }

    // Bod 1: Graf je DAG.
    cout << "Graf JE DAG.\n";
    // Bod 2: Počet vrcholov a hrán vstupného grafu.
    cout << "Pocet vrcholov: " << n
         << ", pocet hran: " << hranyZoSuboru.size() << "\n";

    // --- Doplnenie na kompletný DAG a zápis do súboru ---

    // Odvodíme meno výstupného súboru z mena vstupného:
    // nájdeme poslednú bodku v názve súboru a vložíme "_complete" pred príponu.
    // Napríklad: "graph1.txt" → "graph1_complete.txt"
    // Ak súbor nemá príponu, jednoducho pridáme "_complete".
    string vystupSubor;
    size_t bodka = subor.rfind('.');   // Nájdeme poslednú bodku v názve súboru.
    if (bodka != string::npos)         // Ak bodka existuje (súbor má príponu)...
        vystupSubor = subor.substr(0, bodka) + "_complete" + subor.substr(bodka);
    else                               // Ak bodka neexistuje (bez prípony)...
        vystupSubor = subor + "_complete";

    // Zavoláme funkciu, ktorá doplní chýbajúce hrany, zapíše výsledok do súboru
    // a vypíše body 3 a 4 na stdout.
    doplnDAG(vystupSubor);

    // Program skončil úspešne.
    return 0;
}

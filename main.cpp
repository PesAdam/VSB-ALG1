// Knižnica pre vstup a výstup (cout, cin, cerr)
#include <iostream>
// Knižnica pre prácu so súbormi (ifstream — čítanie zo súboru)
#include <fstream>
// Knižnica pre dynamické polia — vector<> (zoznam susedov, stavy, poradie)
#include <vector>
// Knižnica pre množinu — set<> (rýchle overenie, či hrana už existuje)
#include <set>
// Knižnica pre algoritmy — reverse() (otočenie topologického poradia)
#include <algorithm>

// Použijeme štandardný menný priestor, aby sme nemusali písať std::cout atď.
using namespace std;

// ============================================================
//  Globálne premenné — dostupné vo všetkých funkciách
// ============================================================

// Celkový počet vrcholov grafu (zistíme ho pri načítaní súboru)
int n;

// Zoznam susedov: adj[u] obsahuje zoznam vrcholov, do ktorých vedie hrana Z vrcholu u.
// Napríklad ak existuje hrana 0→1 a 0→3, tak adj[0] = {1, 3}.
// Vonkajší vektor má n prvkov (jeden pre každý vrchol),
// vnútorné vektory obsahujú susedov daného vrcholu.
vector<vector<int>> adj;

// Pole stavov každého vrcholu počas DFS:
//   stav[u] == 0  →  biely: vrchol u sme ešte vôbec nenavštívili
//   stav[u] == 1  →  sivý:  vrchol u práve navštevujeme (je na zásobníku rekurzie)
//   stav[u] == 2  →  čierny: vrchol u je hotový, všetci jeho susedia boli spracovaní
vector<int> stav;

// Výsledok DFS — topologické poradie vrcholov.
// Vrchol sa sem pridá až keď sú všetci jeho nástupcovia spracovaní (keď ho farbíme na čierno).
// DFS ich pridáva v opačnom poradí → na konci zavoláme reverse().
vector<int> topo;

// Príznak cyklu — nastavíme ho na true hneď ako nájdeme spätnú hranu (smeruje do sivého vrcholu).
// Po nastavení prerušíme DFS čo najrýchlejšie.
bool cyklus;

// ============================================================
//  DFS — prehľadávanie do hĺbky
//  Zároveň detekuje cyklus (spätná hrana do sivého vrcholu)
//  a zostavuje topologické poradie.
//  Parameter u: vrchol, ktorý práve spracovávame.
// ============================================================

void dfs(int u) {
    // Označíme vrchol u ako "sivý" — práve ho navštevujeme.
    // Od tohto momentu je u na zásobníku rekurzie.
    stav[u] = 1;

    // Prejdeme všetkých susedov vrcholu u — teda všetky vrcholy v,
    // do ktorých vedie hrana u → v.
    for (int v : adj[u]) {

        // Ak je sused v sivý, znamená to, že v je práve na zásobníku rekurzie —
        // teda sme sa dostali späť k vrcholu, cez ktorý sme ešte len prechádzali.
        // To je definícia SPÄTNEJ HRANY a teda cyklu. Nastavíme príznak a hneď skončíme.
        if (stav[v] == 1) {
            cyklus = true;        // Našli sme cyklus — zapisujeme to do globálnej premennej.
            return;               // Ukončíme túto vetvu DFS ihneď — nemá zmysel pokračovať.
        }

        // Ak je sused v biely (ešte nenavštívený), rekurzívne ho navštívime.
        // Toto je to "ísť hlbšie" v prehľadávaní do hĺbky.
        if (stav[v] == 0)
            dfs(v);               // Rekurzívne zavolanie DFS pre vrchol v.

        // Ak počas rekurzívneho volania bol nájdený cyklus, okamžite sa vraciame späť.
        // Tým zastavíme celú rekurziu — nema zmysel pokračovať ak vieme, že graf nie je DAG.
        if (cyklus) return;
    }

    // Keď sme sem prišli, znamená to že sme prešli VŠETKÝCH susedov vrcholu u
    // a nenašli sme žiadny cyklus. Vrchol u môžeme označiť ako "čierny" — hotový.
    stav[u] = 2;

    // Vrchol u pridáme do topologického poradia.
    // Pridáva sa AŽ TERAZ, keď sú všetci nástupcovia (podstrom za u) hotový.
    // DFS pridáva vrcholy od "najhlbších" po "najplytšie" — teda v obrátenom topologickom poradí.
    // Preto na konci zavoláme reverse().
    topo.push_back(u);
}

// ============================================================
//  jeDAG — Overenie, či je graf orientovaný acyklický graf (DAG).
//  Spustí DFS na všetky nenavštívené vrcholy (pre prípad, že graf nie je súvislý).
//  Vedľajší efekt: ak je graf DAG, pole topo[] bude obsahovať topologické poradie.
//  Návratová hodnota: true ak je DAG, false ak obsahuje cyklus.
// ============================================================

bool jeDAG() {
    // Inicializujeme stavové pole — všetky vrcholy sú na začiatku biele (0 = nenavštívené).
    stav.assign(n, 0);

    // Vynulujeme príznak cyklu pred každým novým behom.
    cyklus = false;

    // Vyprázdnime výsledné topologické poradie pred novým behom.
    topo.clear();

    // Spustíme DFS zo každého vrcholu, ktorý ešte nebol navštívený.
    // Toto je dôležité pre grafy, ktoré nie sú súvislé —
    // teda také, kde sa z vrcholu 0 nedá dosiahnuť každý iný vrchol.
    for (int i = 0; i < n; i++)
        if (stav[i] == 0)         // Ak vrchol i ešte nebol navštívený, spustíme z neho DFS.
            dfs(i);

    // DFS pridáva vrcholy do topo[] od najhlbšieho po najplytší —
    // teda v opačnom topologickom poradí.
    // Otočíme pole, aby sme dostali správne poradie (od začiatku k závislostiam).
    reverse(topo.begin(), topo.end());

    // Vrátime true (graf je DAG) ak sa počas DFS nenašiel žiadny cyklus.
    return !cyklus;
}

// ============================================================
//  doplnDAG — Doplnenie grafu na kompletný DAG a zápis do výstupného súboru.
//  Pre každú dvojicu vrcholov (u, v) kde u je pred v v topologickom poradí,
//  pridáme hranu u → v ak ešte neexistuje.
//  Cieľový počet hrán: n*(n-1)/2 (každá dvojica má práve jednu hranu).
//  Výsledný graf zapíše do súboru vystupSubor vo formáte "u v" na každý riadok.
//  Táto funkcia sa volá len ak je graf DAG (overili sme to v jeDAG()).
//  Parameter vystupSubor: meno výstupného súboru.
// ============================================================

void doplnDAG(const string& vystupSubor) {
    // Vytvoríme množinu existujúcich hrán.
    // set<pair<int,int>> je množina dvojíc (u, v), kde u→v je hrana.
    // Množina nám umožňuje v čase O(log n) zistiť, či nejaká hrana už existuje.
    // set automaticky uchováva hrany zoradené — výstupný súbor bude prehľadný.
    set<pair<int,int>> hrany;

    // Prejdeme všetky existujúce hrany zo zoznamu susedov (adj)
    // a vložíme ich do množiny hrany.
    for (int u = 0; u < n; u++)            // Pre každý vrchol u...
        for (int v : adj[u])               // ...pre každého suseda v vrcholu u...
            hrany.insert({u, v});           // ...vložíme hranu (u→v) do množiny.

    // Prechádzame všetky usporiadané dvojice vrcholov podľa topologického poradia.
    // i je index prvého vrcholu (predchodca), j je index druhého vrcholu (nasledovník).
    // Podmienka j > i zaručuje, že hrana ide "dopredu" v topologickom poradí → nikdy nevznikne cyklus.
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {

            // Vyberieme konkrétne čísla vrcholov na pozíciách i a j v topologickom poradí.
            int u = topo[i];   // u je vrchol na i-tej pozícii — je "skôr" v poradí
            int v = topo[j];   // v je vrchol na j-tej pozícii — je "neskôr" v poradí

            // Skontrolujeme, či hrana u→v ešte neexistuje.
            // Ak nie, pridáme ju — žiadny cyklus nevznikne, lebo ideme vždy dopredu v topo. poradí.
            if (!hrany.count({u, v}))
                hrany.insert({u, v});      // Hrana chýbala — pridáme ju do množiny.
        }
    }

    // Zapíšeme kompletný DAG do výstupného súboru vo formáte rovnakom ako vstup:
    // každý riadok obsahuje dvojicu "u v" oddelenú medzerou.
    ofstream out(vystupSubor);             // Otvoríme výstupný súbor na zápis.
    if (!out) {
        cerr << "Chyba: nepodarilo sa vytvorit vystupny subor '" << vystupSubor << "'!\n";
        return;
    }
    for (auto& [u, v] : hrany)             // Prejdeme všetky hrany (set ich má zoradené)...
        out << u << " " << v << "\n";     // ...a zapíšeme každú hranu ako "u v".
    out.close();                           // Zatvoríme výstupný súbor.

    // --- Výstup na stdout podľa zadania: body 3 a 4 ---

    // Bod 3: Očakávaný počet hrán kompletného DAG podľa vzorca n*(n-1)/2.
    long long ocakavane = (long long)n * (n - 1) / 2;
    cout << "Ocakavany pocet hran kompletneho DAG: " << ocakavane << "\n";

    // Bod 4: Skutočný počet vrcholov a hrán výsledného kompletného DAG.
    // hrany.size() musí byť rovnaké ako ocakavane — ak nie, je niekde chyba.
    cout << "Kompletny DAG — pocet vrcholov: " << n
         << ", pocet hran: " << (long long)hrany.size() << "\n";

    cout << "Kompletny DAG zapísany do suboru: " << vystupSubor << "\n";
}

// ============================================================
//  main — hlavná funkcia, vstupný bod programu
// ============================================================

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

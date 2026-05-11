/**
 * @file graph.cpp
 * @brief Implementácia globálnych premenných a algoritmov pre DAG.
 *
 * Obsahuje definície globálnych premenných a implementácie funkcií
 * @c dfs(), @c jeDAG() a @c doplnDAG().
 */

#include "graph.hpp"
/// @copydoc n
int n;
//pocet vrcholov v grafe

/// @copydoc adj
vector<vector<int>> adj;
// adj zoznam susedov v grafe

/// @copydoc stav
vector<int> stav;
// stav pre DFS

/// @copydoc topo
vector<int> topo;
// vysledne topologicke poradie vrcholov

/// @copydoc cyklus
bool cyklus;

void dfs(int u) {
    stav[u] = 1; // sivy — prave navstevujeme (oznacime na sivo je na zasobniku rekurzie)
    
    //prejdi vsetkych susedov vrcholu U
    // ak je adj[] = {1, 3} tak prva iteracia je 1 a druha je 3
    for (int v : adj[u]) {
        if (stav[v] == 1) {
            // ak je sused sivy tak mame cyklus a koncime
            cyklus = true;
            return;
        }
        if (stav[v] == 0) {
            dfs(v); // rekurzia
        }

        if (cyklus) return; // ak je cyklus tak koncime rekurziu
    }

    // ked oznacime vrchol na cierno priadme ho do topo
    stav[u] = 2;        // cierny — vsetci susedia spracovani
    topo.push_back(u);  // pridame do topologickeho poradia (odzadu)
}

bool jeDAG() {
    stav.assign(n, 0); // vsetky vrcholy su biele, pripravene
    cyklus = false;
    topo.clear();

    // spustime DFS z kazdeho vrcholu
    for (int i = 0; i < n; i++) {
        if (stav[i] == 0) {
            dfs(i);
        }
    }

    // DFS pridava odzadu preto my potrebujeme topo otocit
    reverse(topo.begin(), topo.end());

    return !cyklus;
}

void doplnDAG(const string& vystupSubor) {
    // mnozina existujucich hran
    // set nam zaruci to ze bude jedinecna hrana takze mame DAG (neopakuju sa hrany)
    set<pair<int, int>> hrany;

    // kazdu hranu ktoru najdeme ulozime do set
    for (int u = 0; u < n; u++) {
        for (int v : adj[u]) {
            hrany.insert({ u, v });
        }
    }

    // pridame chybajuce hrany podla topologickeho poradia (i < j) 
    // u - cislo odkial hrana ide
    // v - cislo kam hrana ide
    for (int i = 0; i < n; i++) {
        // i je vzdy mennsie ako j takze ideme vzdy dopredu
        for (int j = i + 1; j < n; j++) {
            int u = topo[i]; // vrchol ktory je skor v poradi
            int v = topo[j]; // nasledovnik
            if (!hrany.count({ u, v })) {
                hrany.insert({ u, v });
            }
        }
    }

    // zapiseme kompletny DAG do suboru
    ofstream out(vystupSubor);
    if (!out) {
        cerr << "Chyba: nepodarilo sa vytvorit vystupny subor '" << vystupSubor << "'!\n";
        return;
    }
    for (auto& hrana : hrany) {
        out << hrana.first << " " << hrana.second << "\n";
    }
    // ocakavane hrany podla vzorca (n-1) / 2 = pocet hran
    long long ocakavane = (long long)n * (n - 1) / 2;
    cout << "Ocakavany pocet hran kompletneho DAG: " << ocakavane << "\n";

    // vypis skutocnych poctov
    cout << "Kompletny DAG — pocet vrcholov: " << n
        << ", pocet hran: " << (long long)hrany.size() << "\n";
    cout << "Kompletny DAG zapísany do suboru: " << vystupSubor << "\n";
}

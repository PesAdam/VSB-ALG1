/**
 * @file graph.cpp
 * @brief Implementácia globálnych premenných a algoritmov pre DAG.
 *
 * Obsahuje definície globálnych premenných a implementácie funkcií
 * @c dfs(), @c jeDAG() a @c doplnDAG().
 */

#include "graph.hpp"

 // ============================================================
 // globalne premene
 // ============================================================

 /// @copydoc n
int n;
/// @copydoc adj
vector<vector<int>> adj;
/// @copydoc stav
vector<int> stav;
/// @copydoc topo
vector<int> topo;
/// @copydoc cyklus
bool cyklus;

// ============================================================
//  implementacie funkcii
// ============================================================

void dfs(int u) {
    stav[u] = 1; // sivy — prave navstevujeme u (je na zasobniku rekurzie)

    for (int v : adj[u]) {
        if (stav[v] == 1) {
            // ak je sused sivy tak mame cyklus
            cyklus = true;
            return;
        }
        if (stav[v] == 0) {
            dfs(v); // rekurzia
        }

        if (cyklus) return; // ak je cyklus tak koncime rekurziu
    }

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
    set<pair<int, int>> hrany;

    for (int u = 0; u < n; u++) {
        for (int v : adj[u]) {
            hrany.insert({ u, v });
        }
    }

    // pridame chybajuce hrany podla topologickeho poradia (i < j → bez cyklu)
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            int u = topo[i]; // predchodca
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

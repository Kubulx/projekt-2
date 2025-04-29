#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>

using namespace std;

int pobierz_poprawny_start(const vector<vector<int>>& graf, int numer_grafu) {
    int start;
    cout << "[Graf " << numer_grafu << "] Podaj wierzcholek startowy: ";
    cin >> start;

    while (start < 0 || start >= graf.size() || graf[start].empty()) {
        cout << "Nieprawidlowy wierzcholek startowy lub brak polaczen.\n";
        cout << "Podaj poprawny wierzcholek startowy (0 do " << graf.size() - 1 << "): ";
        cin >> start;
    }

    return start;
}

vector<vector<int>> inicjalizuj_graf(int numer_grafu) {
    vector<vector<int>> graf;
    cout << "\n[Graf " << numer_grafu << "] Podaj krawedzie (np. 0 1). Wpisz -1 -1 aby zakonczyc:\n";

    while (true) {
        int w1, w2;
        cin >> w1 >> w2;
        if (w1 == -1 && w2 == -1)
            break;

        if (w1 >= 0 && w2 >= 0) {
            int max_w = max(w1, w2);
            if (graf.size() <= max_w)
                graf.resize(max_w + 1);

            graf[w1].push_back(w2);
            graf[w2].push_back(w1);
        }
        else {
            cout << "Nieprawidlowa krawedz. Sprobuj ponownie.\n";
        }
    }

    cout << "\n[Graf " << numer_grafu << "] Lista sasiedztwa:\n";
    for (int i = 0; i < graf.size(); ++i) {
        if (!graf[i].empty()) {
            cout << i << ": ";
            for (int sasiad : graf[i]) {
                cout << sasiad << " ";
            }
            cout << endl;
        }
    }

    return graf;
}

void bfs(int start, const vector<vector<int>>& graf, int numer_grafu, mutex& cout_mutex) {
    int n = graf.size();
    vector<bool> odwiedzony(n, false);
    queue<int> kolejka;

    odwiedzony[start] = true;
    kolejka.push(start);

    {
        lock_guard<mutex> lock(cout_mutex);
        cout << "\n[Graf " << numer_grafu << "] Kolejnosc przeszukiwania BFS: ";
    }

    while (!kolejka.empty()) {
        int punkt = kolejka.front();
        kolejka.pop();

        {
            lock_guard<mutex> lock(cout_mutex);
            cout << punkt << " ";
        }

        for (int sasiad : graf[punkt]) {
            if (!odwiedzony[sasiad]) {
                odwiedzony[sasiad] = true;
                kolejka.push(sasiad);
            }
        }
    }

    {
        lock_guard<mutex> lock(cout_mutex);
        cout << endl;
    }
}

int main() {
    vector<vector<int>> graf1 = inicjalizuj_graf(1);
    vector<vector<int>> graf2 = inicjalizuj_graf(2);

    int start_1 = pobierz_poprawny_start(graf1, 1);
    int start_2 = pobierz_poprawny_start(graf2, 2);

    mutex cout_mutex;

    thread watek1(bfs, start_1, cref(graf1), 1, ref(cout_mutex));
    thread watek2(bfs, start_2, cref(graf2), 2, ref(cout_mutex));

    watek1.join();
    watek2.join();

    return 0;
}

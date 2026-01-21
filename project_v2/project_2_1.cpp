#include <iostream>
#include <cmath>
#include <string>
#include <limits>
#include <queue>
#include <algorithm>
#include <stack>
#include <vector>

using namespace std;

// ============================================================================
// ГЛОБАЛЬНЫЕ КОНСТАНТЫ И ПЕРЕМЕННЫЕ
// ============================================================================

const int MAX_STATIONS = 1000;
const int MODES = 3;        // 0 Metro, 1 Bus, 2 Train
const int START_MODE = 3;   // "ещё не ехали"
const long double INF = 1e100L;
const int BIG_TRANSFERS = 1000000000;

int N = 0;  // Количество станций
int M = 0;  // Количество ребер

long double sensitivity[MODES];
long double transfer_penalty[MODES][MODES];
long double local_penalty[MAX_STATIONS + 1];

bool station_ok(int v) { return v >= 1 && v <= N; }
bool mode_ok(int m) { return m >= 0 && m < MODES; }

// ============================================================================
// 1. Структуры данных графа (graph.cpp)
//    - Отвечает за хранение графа
//    - Реализация функций добавления ребер
//    - Вычисление фактического времени
//    - Инициализация структур данных
// ============================================================================

vector<vector<uint32_t>> edge_to;
vector<vector<uint32_t>> edge_mode;
vector<vector<uint32_t>> edge_base_time;
vector<vector<long double>> edge_load;

void graph_init(uint32_t n) {
    edge_to.assign((size_t)n + 1, vector<int>());
    edge_mode.assign((size_t)n + 1, vector<int>());
    edge_base_time.assign((size_t)n + 1, vector<int>());
    edge_load.assign((size_t)n + 1, vector<long double>());
}

void add_edge(int from, int to, int mode, int base_time, long double load) {
    edge_to[from].push_back(to);
    edge_mode[from].push_back(mode);
    edge_base_time[from].push_back(base_time);
    edge_load[from].push_back(load);

    edge_to[to].push_back(from);
    edge_mode[to].push_back(mode);
    edge_base_time[to].push_back(base_time);
    edge_load[to].push_back(load);
}

long double calculate_actual_time(int base_time, long double load, int mode) {
    return (long double)base_time * (1.0L + load * sensitivity[mode]);
}

// ============================================================================
// 2. Компоненты связности (dfs_module.cpp)
//    - Реализация алгоритма DFS
//    - Поиск компонент связности
//    - Сортировка компонент
//    - Визуализация результатов
// ============================================================================

void dfs_component(
    int start_vertex,
    vector<int>& component,
    vector<char>& visited,
    int mode_filter,
    bool use_filter)
{
    stack<int> st;
    st.push(start_vertex);

    while (!st.empty()) {
        int v = st.top();
        st.pop();

        if (visited[v]) continue;
        visited[v] = 1;
        component.push_back(v);

        int deg = (int)edge_to[v].size();
        for (int i = 0; i < deg; i++) {
            if (use_filter && edge_mode[v][i] != mode_filter) continue;
            int to = edge_to[v][i];
            if (!visited[to]) st.push(to);
        }
    }
}

bool components_before(const vector<int>& a, const vector<int>& b) {
    if (a.size() != b.size()) return a.size() > b.size(); // size desc
    return lexicographical_compare(a.begin(), a.end(), b.begin(), b.end()); // tie: lexicographic
}

vector<vector<int>> find_connected_components(int mode_filter, bool use_filter)
{
    vector<char> visited((size_t)N + 1, 0);
    vector<vector<int>> comps;

    for (int v = 1; v <= N; v++) {
        if (!visited[v]) {
            vector<int> comp;
            dfs_component(v, comp, visited, mode_filter, use_filter);

            // внутри компоненты станции по возрастанию (разрешено std::sort для DFS)
            sort(comp.begin(), comp.end());
            comps.push_back(comp);
        }
    }

    // компоненты по убыванию размера; tie — лексикографически (разумный и стабильный вариант)
    sort(comps.begin(), comps.end(), components_before);
    return comps;
}

void print_isolated_zones(const string& title, int mode_filter, bool use_filter) {
    vector<vector<int>> comps = find_connected_components(mode_filter, use_filter);

    cout << title << "\n";

    // если 0 или 1 компонента — изолированных зон нет
    if ((int)comps.size() <= 1) {
        cout << 0 << "\n\n";
        return;
    }

    // "крупнейшая компонента" = comps[0], остальные — изолированные зоны
    cout << (int)comps.size() - 1 << "\n";
    for (int i = 1; i < (int)comps.size(); i++) {
        const vector<int>& zone = comps[i];
        for (int j = 0; j < (int)zone.size(); j++) {
            if (j) cout << ' ';
            cout << zone[j];
        }
        cout << "\n";
    }
    cout << "\n";
}

// ============================================================================
// 3. Алгоритм Дейкстры (dijkstra_module.cpp)
//    - Реализация алгоритма Дейкстры с приоритетной очередью (своя куча)
//    - Учет штрафов за пересадки
//    - Хранение состояний (станция + вид транспорта)
//    - Оптимизация путей (tie-break по пересадкам при равном времени)
// ============================================================================

// В куче сравниваем по (time, station, mode) — чтобы поведение было детерминированным
bool heap_is_better(
    long double t1, int v1, int m1,
    long double t2, int v2, int m2)
{
    if (t1 != t2)
        return t1 < t2;
    if (v1 != v2)
        return v1 < v2;
    return m1 < m2;
}

void heap_swap(
    int i, int j,
    vector<long double>& ht,
    vector<int>& hv,
    vector<int>& hm)
{
    long double tt = ht[i];
    ht[i] = ht[j];
    ht[j] = tt;
    int tv = hv[i]; hv[i] = hv[j]; hv[j] = tv;
    int tm = hm[i]; hm[i] = hm[j]; hm[j] = tm;
}

void heap_push(
    long double t, int v, int m,
    vector<long double>& ht,
    vector<int>& hv,
    vector<int>& hm)
{
    ht.push_back(t);
    hv.push_back(v);
    hm.push_back(m);

    int idx = (int)ht.size() - 1;
    while (idx > 0) {
        int p = (idx - 1) / 2;
        if (heap_is_better(ht[idx], hv[idx], hm[idx], ht[p], hv[p], hm[p])) {
            heap_swap(idx, p, ht, hv, hm);
            idx = p;
        } else break;
    }
}

bool heap_pop(
    long double& t, int& v, int& m,
    vector<long double>& ht,
    vector<int>& hv,
    vector<int>& hm)
{
    if (ht.empty())
        return false;

    t = ht[0];
    v = hv[0];
    m = hm[0];

    int last = (int)ht.size() - 1;
    ht[0] = ht[last];
    hv[0] = hv[last];
    hm[0] = hm[last];
    ht.pop_back();
    hv.pop_back();
    hm.pop_back();

    int n = (int)ht.size();
    int idx = 0;
    while (true) {
        int l = idx * 2 + 1;
        int r = idx * 2 + 2;
        int best = idx;

        if (l < n && heap_is_better(ht[l], hv[l], hm[l], ht[best], hv[best], hm[best]))
            best = l;
        if (r < n && heap_is_better(ht[r], hv[r], hm[r], ht[best], hv[best], hm[best]))
            best = r;

        if (best == idx)
            break;
        heap_swap(idx, best, ht, hv, hm);
        idx = best;
    }

    return true;
}

void dijkstra(
    int start_station,
    vector<vector<long double>>& dist,
    vector<vector<int>>& parent_station,
    vector<vector<int>>& parent_mode,
    vector<vector<int>>& transfers)
{
    // init
    for (int v = 1; v <= N; v++) {
        for (int m = 0; m <= START_MODE; m++) {
            dist[v][m] = INF;
            parent_station[v][m] = -1;
            parent_mode[v][m] = -1;
            transfers[v][m] = BIG_TRANSFERS;
        }
    }

    vector<long double> heap_time;
    vector<int> heap_station;
    vector<int> heap_mode;

    dist[start_station][START_MODE] = 0.0L;
    transfers[start_station][START_MODE] = 0;
    heap_push(0.0L, start_station, START_MODE, heap_time, heap_station, heap_mode);

    while (!heap_time.empty()) {
        long double cur_t;
        int u, last_mode;
        if (!heap_pop(cur_t, u, last_mode, heap_time, heap_station, heap_mode)) break;

        if (cur_t != dist[u][last_mode]) continue;

        int deg = (int)edge_to[u].size();
        for (int i = 0; i < deg; i++) {
            int v = edge_to[u][i];
            int edge_m = edge_mode[u][i];
            int base_t = edge_base_time[u][i];
            long double load_val = edge_load[u][i];

            long double actual = calculate_actual_time(base_t, load_val, edge_m);
            long double new_t = cur_t + actual;
            int new_tr = transfers[u][last_mode];

            if (last_mode != START_MODE && last_mode != edge_m) {
                new_t += transfer_penalty[last_mode][edge_m];
                new_t += local_penalty[u];
                new_tr++;
            }

            if (new_t < dist[v][edge_m]) {
                dist[v][edge_m] = new_t;
                transfers[v][edge_m] = new_tr;
                parent_station[v][edge_m] = u;
                parent_mode[v][edge_m] = last_mode;
                heap_push(new_t, v, edge_m, heap_time, heap_station, heap_mode);
            } else if (new_t == dist[v][edge_m] && new_tr < transfers[v][edge_m]) {
                transfers[v][edge_m] = new_tr;
                parent_station[v][edge_m] = u;
                parent_mode[v][edge_m] = last_mode;
                heap_push(new_t, v, edge_m, heap_time, heap_station, heap_mode);
            }
        }
    }
}

// ============================================================================
// 4. Быстрая сортировка (quicksort_module.cpp)
//    - Реализация быстрой сортировки
//    - Учет всех тай-брейков
//    - Сортировка результатов запросов
// ============================================================================

void swap_result(
    int a, int b,
    vector<int>& destinations,
    vector<char>& reachable,
    vector<long double>& time_total,
    vector<int>& transfers_cnt,
    vector<long double>& comfort,
    vector<vector<int>>& path_station,
    vector<vector<int>>& path_mode)
{
    if (a == b) return;

    int ti = destinations[a];
    destinations[a] = destinations[b];
    destinations[b] = ti;

    char tc = reachable[a];
    reachable[a] = reachable[b];
    reachable[b] = tc;

    long double tld = time_total[a];
    time_total[a] = time_total[b];
    time_total[b] = tld;
    ti = transfers_cnt[a];
    transfers_cnt[a] = transfers_cnt[b];
    transfers_cnt[b] = ti;
    tld = comfort[a];
    comfort[a] = comfort[b];
    comfort[b] = tld;

    vector<int> tvs = path_station[a];
    path_station[a] = path_station[b];
    path_station[b] = tvs;

    vector<int> tvm = path_mode[a];
    path_mode[a] = path_mode[b];
    path_mode[b] = tvm;
}

bool result_before(
    int a, int b,
    const vector<int>& destinations,
    const vector<char>& reachable,
    const vector<long double>& time_total,
    const vector<int>& transfers_cnt,
    const vector<long double>& comfort)
{
    // reachable (достижимые раньше недостижимых)
    if (reachable[a] != reachable[b]) return reachable[a] > reachable[b];

    // оба недостижимы — по номеру цели
    if (!reachable[a] && !reachable[b]) return destinations[a] < destinations[b];

    // оба достижимы — comfort, time, transfers, destination
    if (comfort[a] != comfort[b]) return comfort[a] < comfort[b];
    if (time_total[a] != time_total[b]) return time_total[a] < time_total[b];
    if (transfers_cnt[a] != transfers_cnt[b]) return transfers_cnt[a] < transfers_cnt[b];
    return destinations[a] < destinations[b];
}

int partition_results(
    int low, int high,
    vector<int>& destinations,
    vector<char>& reachable,
    vector<long double>& time_total,
    vector<int>& transfers_cnt,
    vector<long double>& comfort,
    vector<vector<int>>& path_station,
    vector<vector<int>>& path_mode)
{
    // pivot = middle (снижает шанс худшего случая по сравнению с "последний элемент всегда")
    int mid = low + (high - low) / 2;
    swap_result(mid, high, destinations, reachable, time_total, transfers_cnt, comfort, path_station, path_mode);

    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (result_before(j, high, destinations, reachable, time_total, transfers_cnt, comfort)) {
            i++;
            swap_result(i, j, destinations, reachable, time_total, transfers_cnt, comfort, path_station, path_mode);
        }
    }
    swap_result(i + 1, high, destinations, reachable, time_total, transfers_cnt, comfort, path_station, path_mode);
    return i + 1;
}

void quick_sort_results(
    int low, int high,
    vector<int>& destinations,
    vector<char>& reachable,
    vector<long double>& time_total,
    vector<int>& transfers_cnt,
    vector<long double>& comfort,
    vector<vector<int>>& path_station,
    vector<vector<int>>& path_mode)
{
    // рекурсивно — K обычно не огромный; pivot улучшен
    if (low < high) {
        int pi = partition_results(low, high, destinations, reachable, time_total, transfers_cnt, comfort, path_station, path_mode);
        quick_sort_results(low, pi - 1, destinations, reachable, time_total, transfers_cnt, comfort, path_station, path_mode);
        quick_sort_results(pi + 1, high, destinations, reachable, time_total, transfers_cnt, comfort, path_station, path_mode);
    }
}

// ============================================================================
// 5. Восстановление пути (path_reconstruction.cpp)
//    - Восстановление оптимального пути
//    - Вычисление метрики удобства
//    - Формирование результатов для каждой станции
//    - Обработка недостижимых станций
// ============================================================================

void reconstruct_path(
    int dest, int best_mode,
    const vector<vector<int>>& parent_station,
    const vector<vector<int>>& parent_mode,
    vector<int>& out_station,
    vector<int>& out_mode)
{
    out_station.clear();
    out_mode.clear();

    stack<int> st_station;
    stack<int> st_mode;

    int cur_station = dest;
    int cur_mode = best_mode;

    while (cur_station != -1) {
        st_station.push(cur_station);
        st_mode.push(cur_mode);

        int ps = parent_station[cur_station][cur_mode];
        int pm = parent_mode[cur_station][cur_mode];
        cur_station = ps;
        cur_mode = pm;
    }

    while (!st_station.empty()) {
        out_station.push_back(st_station.top());
        out_mode.push_back(st_mode.top());
        st_station.pop();
        st_mode.pop();
    }
}

void print_path_with_modes(const vector<int>& stations, const vector<int>& modes)
{
    if (stations.empty()) return;

    cout << stations[0];
    for (int i = 1; i < (int)stations.size(); i++) {
        int used_mode = modes[i]; // режим, которым пришли в stations[i]
        cout << " -" << used_mode << "-> " << stations[i];
    }
}

// ============================================================================
// 6. Ввод-вывод (io_module.cpp)
//    - Чтение входных данных
//    - Вывод результатов
//    - Форматирование вывода
//    - Валидация входных данных
// ============================================================================

bool read_input()
{
    if (!(cin >> N >> M)) return false;
    if (N < 1 || N > MAX_STATIONS) return false;
    if (M < 0) return false;

    graph_init(N);

    for (int i = 0; i < MODES; i++) {
        if (!(cin >> sensitivity[i])) return false;
    }

    for (int r = 0; r < MODES; r++) {
        for (int c = 0; c < MODES; c++) {
            if (!(cin >> transfer_penalty[r][c])) return false;
        }
    }

    for (int s = 1; s <= N; s++) {
        if (!(cin >> local_penalty[s])) return false;
    }

    for (int e = 0; e < M; e++) {
        int from, to, mode, base_time;
        long double load;
        if (!(cin >> from >> to >> mode >> base_time >> load)) return false;

        if (!station_ok(from) || !station_ok(to)) return false;
        if (!mode_ok(mode)) return false;
        if (base_time < 0) return false;
        if (load < 0.0L) return false;

        // load допускается как доля (0..1) или проценты (0..100)
        if (load > 1.0L) load /= 100.0L;
        if (load < 0.0L || load > 1.0L) return false;

        add_edge(from, to, mode, base_time, load);
    }

    return true;
}

// ============================================================================
// MAIN
// ============================================================================

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    if (!read_input()) return 0;

    // 1) Связность и изолированные зоны (4 графа)
    print_isolated_zones("METRO", 0, true);
    print_isolated_zones("BUS", 1, true);
    print_isolated_zones("TRAIN", 2, true);
    print_isolated_zones("ALL", -1, false);

    // 2) Запросы пассажира
    int Q;
    if (!(cin >> Q)) return 0;
    if (Q < 0) return 0;

    for (int query_idx = 1; query_idx <= Q; query_idx++) {
        int start_station;
        int K;
        long double coef;

        if (!(cin >> start_station >> K >> coef)) return 0;
        if (!station_ok(start_station)) return 0;
        if (K < 0) return 0;

        vector<int> targets((size_t)K);
        for (int i = 0; i < K; i++) {
            if (!(cin >> targets[i])) return 0;
            if (!station_ok(targets[i])) return 0;
        }

        vector<vector<long double>> dist((size_t)N + 1, vector<long double>((size_t)START_MODE + 1, INF));
        vector<vector<int>> parent_station((size_t)N + 1, vector<int>((size_t)START_MODE + 1, -1));
        vector<vector<int>> parent_mode((size_t)N + 1, vector<int>((size_t)START_MODE + 1, -1));
        vector<vector<int>> transfers((size_t)N + 1, vector<int>((size_t)START_MODE + 1, BIG_TRANSFERS));

        dijkstra(start_station, dist, parent_station, parent_mode, transfers);

        vector<int> result_destinations;
        vector<char> result_reachable;
        vector<long double> result_times;
        vector<int> result_transfers;
        vector<long double> result_comfort;
        vector<vector<int>> result_path_station;
        vector<vector<int>> result_path_mode;

        result_destinations.reserve((size_t)K);
        result_reachable.reserve((size_t)K);
        result_times.reserve((size_t)K);
        result_transfers.reserve((size_t)K);
        result_comfort.reserve((size_t)K);
        result_path_station.reserve((size_t)K);
        result_path_mode.reserve((size_t)K);

        for (int i = 0; i < K; i++)
        {
            int dest = targets[i];

            long double best_time = INF;
            int best_mode = -1;
            int best_tr = BIG_TRANSFERS;

            for (int m = 0; m <= START_MODE; m++)
            {
                long double t = dist[dest][m];
                int tr = transfers[dest][m];

                if (t < best_time) {
                    best_time = t;
                    best_mode = m;
                    best_tr = tr;
                } else if (t == best_time && t < INF && tr < best_tr) {
                    best_mode = m;
                    best_tr = tr;
                }
            }

            result_destinations.push_back(dest);

            if (best_mode == -1 || best_time >= INF)
            {
                result_reachable.push_back(0);
                result_times.push_back(0.0L);
                result_transfers.push_back(0);
                result_comfort.push_back(0.0L);
                result_path_station.push_back(vector<int>());
                result_path_mode.push_back(vector<int>());
            } else {
                result_reachable.push_back(1);
                result_times.push_back(best_time);
                result_transfers.push_back(best_tr);

                long double comfort = best_time + coef * (long double)best_tr;
                result_comfort.push_back(comfort);

                vector<int> p_station, p_mode;
                reconstruct_path(dest, best_mode, parent_station, parent_mode, p_station, p_mode);
                result_path_station.push_back(p_station);
                result_path_mode.push_back(p_mode);
            }
        }

        if (!result_destinations.empty())
        {
            quick_sort_results(0, (int)result_destinations.size() - 1,
                               result_destinations, result_reachable, result_times,
                               result_transfers, result_comfort,
                               result_path_station, result_path_mode);
        }

        cout << "QUERY " << query_idx << "\n";
        for (int i = 0; i < (int)result_destinations.size(); i++)
        {
            cout << "TARGET " << result_destinations[i] << "\n";
            if (!result_reachable[i]) {
                cout << "UNREACHABLE\n";
            } else {
                cout << "TIME " << (double)result_times[i] << "\n";
                cout << "TRANSFERS " << result_transfers[i] << "\n";
                cout << "COMFORT " << (double)result_comfort[i] << "\n";
                cout << "PATH ";
                print_path_with_modes(result_path_station[i], result_path_mode[i]);
                cout << "\n";
            }
        }
        cout << "\n";
    }

    return 0;
}

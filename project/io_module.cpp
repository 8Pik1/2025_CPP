#include "project.hpp"

bool readInput(Graph& graph, std::vector<Query>& queries) {
    int stationCount = 0;
    int edgeCount = 0;

    if (!(std::cin >> stationCount >> edgeCount)) return false;

    graph.init(stationCount);

    // sensitivity[0..2]
    for (int mode = 0; mode < MODES; ++mode) {
        double s = 0.0;
        std::cin >> s;
        graph.setSensitivity(mode, s);
    }

    // transfer penalty 3x3
    for (int i = 0; i < MODES; ++i) {
        for (int j = 0; j < MODES; ++j) {
            double p = 0.0;
            std::cin >> p;
            graph.setTransferPenalty(i, j, p);
        }
    }

    // local transfer time for stations 1..N
    for (int v = 1; v <= stationCount; ++v) {
        double t = 0.0;
        std::cin >> t;
        graph.setLocalTransferTime(v, t);
    }

    // edges
    for (int i = 0; i < edgeCount; ++i) {
        int u = 0, v = 0, mode = 0, baseTime = 0;
        double load = 0.0;
        std::cin >> u >> v >> mode >> baseTime >> load;
        graph.addUndirectedEdge(u, v, mode, baseTime, normalizeLoad(load));
    }

    // queries
    int queryCount = 0;
    std::cin >> queryCount;

    queries.clear();
    queries.reserve(queryCount);

    for (int qi = 0; qi < queryCount; ++qi) {
        Query q;
        int k = 0;

        std::cin >> q.start >> q.coef >> k;
        q.targets.resize(k);

        for (int i = 0; i < k; ++i) {
            std::cin >> q.targets[i];
        }

        queries.push_back(q);
    }

    return true;
}

// -------------------- OUTPUT --------------------

static std::string graphNameRu(const std::string& key) {
    if (key == "METRO") return "МЕТРО";
    if (key == "BUS") return "АВТОБУСЫ";
    if (key == "TRAIN") return "ЖЕЛЕЗНАЯ ДОРОГА";
    if (key == "ALL") return "ВСЕ ВИДЫ ТРАНСПОРТА";
    return key;
}

static const char* modeNameRu(int mode) {
    // mode: 0 metro, 1 bus, 2 train
    switch (mode) {
        case 0: return "МЕТРО";
        case 1: return "АВТОБУС";
        case 2: return "Ж/Д";
        default: return "НЕИЗВ.";
    }
}

void printZonesBlock(const std::string& name,
                     const std::vector<std::vector<int>>& zones) {
    std::cout << "ИЗОЛИРОВАННЫЕ ЗОНЫ (" << graphNameRu(name) << ")\n";
    std::cout << "КОЛИЧЕСТВО " << zones.size() << "\n";

    for (size_t i = 0; i < zones.size(); ++i) {
        const std::vector<int>& z = zones[i];
        std::cout << "ЗОНА " << (i + 1) << " РАЗМЕР " << z.size() << ":";

        for (size_t k = 0; k < z.size(); ++k) {
            std::cout << " " << z[k];
        }
        std::cout << "\n";
    }
}

static void printRouteLine(const RouteResult& r) {
    if (!r.reachable) {
        std::cout << "ЦЕЛЬ " << r.target << " НЕДОСТИЖИМА\n";
        return;
    }

    std::cout << "ЦЕЛЬ " << r.target
              << " ВРЕМЯ " << std::fixed << std::setprecision(6) << r.time
              << " ПЕРЕСАДКИ " << r.transfers
              << " УДОБСТВО " << std::fixed << std::setprecision(6) << r.comfort
              << " ПУТЬ ";

    if (r.stations.empty()) {
        std::cout << "\n";
        return;
    }

    std::cout << r.stations[0];
    for (size_t i = 0; i < r.modes.size(); ++i) {
        std::cout << " -(" << modeNameRu(r.modes[i]) << ")-> " << r.stations[i + 1];
    }
    std::cout << "\n";
}

void printQueryBlock(int index,
                     const Query& query,
                     const std::vector<RouteResult>& routes) {
    std::cout << "ЗАПРОС " << index
              << " СТАРТ " << query.start
              << " КОЭФФИЦИЕНТ " << std::fixed << std::setprecision(6) << query.coef
              << "\n";

    for (size_t i = 0; i < routes.size(); ++i) {
        printRouteLine(routes[i]);
    }
}

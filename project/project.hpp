#ifndef PROJECT_HPP
#define PROJECT_HPP

#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <limits>
#include <cmath>
#include <iomanip>
#include <string>
#include <algorithm>

static const int MODES = 3;        // 0 metro, 1 bus, 2 train
static const int START_MODE = 3;   // before first ride
static const double EPS = 1e-9;

inline bool doubleLess(double a, double b) {
    return a + EPS < b;
}

inline bool doubleEqual(double a, double b) {
    return std::fabs(a - b) <= EPS;
}

inline double clamp01(double x) {
    if (x < 0.0) return 0.0;
    if (x > 1.0) return 1.0;
    return x;
}

// load can be in [0..1] or [0..100]
inline double normalizeLoad(double load) {
    if (load > 1.0 + EPS) {
        if (load <= 100.0 + EPS) load /= 100.0;
    }
    return clamp01(load);
}

template <class T>
inline void swapValues(T& a, T& b) {
    T tmp = a;
    a = b;
    b = tmp;
}

struct Edge {
    int to;
    int mode;        // 0..2
    int baseTime;    // >= 0
    double load;     // 0..1
};

struct Query {
    int start;
    double coef;
    std::vector<int> targets;
};

struct RouteResult {
    int target;
    bool reachable;
    double time;
    int transfers;
    double comfort;
    std::vector<int> stations;  // path vertices
    std::vector<int> modes;     // mode per step (size = stations-1)
};

class Graph {
public:
    Graph();

    void init(int stationCount);

    int stationCount() const;
    const std::vector<Edge>& neighbors(int v) const;

    void setSensitivity(int mode, double value);
    void setTransferPenalty(int fromMode, int toMode, double value);
    void setLocalTransferTime(int station, double value);

    void addUndirectedEdge(int u, int v, int mode,
                           int baseTime, double load);

    double edgeActualTime(const Edge& e) const;

    // transfer penalty at station when switching prevMode->nextMode
    // first ride (prevMode==START_MODE) => 0
    double transferCost(int station, int prevMode, int nextMode) const;

private:
    int n;
    std::vector<std::vector<Edge>> adj;
    double sensitivity[MODES];
    double transferPenalty[MODES][MODES];
    std::vector<double> localTransferTime;
};

struct DijkstraResult {
    int n;
    int start;

    // dist[station][mode], mode in 0..3
    std::vector<std::vector<double>> dist;
    std::vector<std::vector<int>> transfers;

    // for path restore (0 means "no parent")
    std::vector<std::vector<int>> prevStation;
    std::vector<std::vector<int>> prevMode;
    std::vector<std::vector<int>> prevEdgeMode;
};

// -------------------- modules --------------------
bool readInput(Graph& graph, std::vector<Query>& queries);

std::vector<std::vector<int>> isolatedZones(const Graph& graph, int modeFilter);

DijkstraResult runDijkstra(const Graph& graph, int startStation);

RouteResult buildRouteResult(const DijkstraResult& dres,
                             int startStation,
                             int targetStation,
                             double coef);

void quickSortRoutes(std::vector<RouteResult>& routes);

// output is also part of IO module (no separate file per your structure)
void printZonesBlock(const std::string& name,
                     const std::vector<std::vector<int>>& zones);

void printQueryBlock(int index,
                     const Query& query,
                     const std::vector<RouteResult>& routes);

#endif

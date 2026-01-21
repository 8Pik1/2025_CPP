#include "project.hpp"

Graph::Graph() : n(0) {
    for (int m = 0; m < MODES; ++m) {
        sensitivity[m] = 0.0;
        for (int k = 0; k < MODES; ++k) {
            transferPenalty[m][k] = 0.0;
        }
    }
}

void Graph::init(int stationCount) {
    n = stationCount;
    adj.assign(n + 1, std::vector<Edge>());
    localTransferTime.assign(n + 1, 0.0);
}

int Graph::stationCount() const {
    return n;
}

const std::vector<Edge>& Graph::neighbors(int v) const {
    return adj[v];
}

void Graph::setSensitivity(int mode, double value) {
    if (mode < 0 || mode >= MODES) return;
    sensitivity[mode] = value;
}

void Graph::setTransferPenalty(int fromMode, int toMode, double value) {
    if (fromMode < 0 || fromMode >= MODES) return;
    if (toMode < 0 || toMode >= MODES) return;
    transferPenalty[fromMode][toMode] = value;
}

void Graph::setLocalTransferTime(int station, double value) {
    if (station < 1 || station > n) return;
    localTransferTime[station] = value;
}

void Graph::addUndirectedEdge(int u, int v, int mode,
                              int baseTime, double load) {
    if (u < 1 || u > n || v < 1 || v > n) return;
    if (mode < 0 || mode >= MODES) return;
    if (baseTime < 0) baseTime = 0;

    Edge e1;
    e1.to = v;
    e1.mode = mode;
    e1.baseTime = baseTime;
    e1.load = clamp01(load);

    Edge e2 = e1;
    e2.to = u;

    adj[u].push_back(e1);
    adj[v].push_back(e2);
}

double Graph::edgeActualTime(const Edge& e) const {
    double factor = 1.0 + e.load * sensitivity[e.mode];
    return static_cast<double>(e.baseTime) * factor;
}

double Graph::transferCost(int station, int prevMode, int nextMode) const {
    if (prevMode == START_MODE) return 0.0; // first ride
    if (prevMode == nextMode) return 0.0;   // no switch
    return transferPenalty[prevMode][nextMode] + localTransferTime[station];
}

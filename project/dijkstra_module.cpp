#include "project.hpp"

static bool pairBetter(double t1, int tr1, double t2, int tr2) {
    if (doubleLess(t1, t2)) return true;
    if (doubleEqual(t1, t2) && tr1 < tr2) return true;
    return false;
}

static int transfersDelta(int prevMode, int nextMode) {
    if (prevMode == START_MODE) return 0; // first ride is not a transfer
    if (prevMode == nextMode) return 0;
    return 1;
}

struct DijkstraNode {
    double time;
    int transfers;
    int station;
    int lastMode;
};

struct NodeCompare {
    bool operator()(const DijkstraNode& a, const DijkstraNode& b) const {
        if (!doubleEqual(a.time, b.time)) return a.time > b.time;
        return a.transfers > b.transfers;
    }
};

DijkstraResult runDijkstra(const Graph& graph, int startStation) {
    DijkstraResult res;
    res.n = graph.stationCount();
    res.start = startStation;

    int n = res.n;

    res.dist.assign(n + 1, std::vector<double>(4, std::numeric_limits<double>::infinity()));
    res.transfers.assign(n + 1, std::vector<int>(4, std::numeric_limits<int>::max()));

    res.prevStation.assign(n + 1, std::vector<int>(4, 0));
    res.prevMode.assign(n + 1, std::vector<int>(4, -1));
    res.prevEdgeMode.assign(n + 1, std::vector<int>(4, -1));

    res.dist[startStation][START_MODE] = 0.0;
    res.transfers[startStation][START_MODE] = 0;

    std::priority_queue<DijkstraNode, std::vector<DijkstraNode>, NodeCompare> pq;

    pq.push(DijkstraNode{0.0, 0, startStation, START_MODE});

    while (!pq.empty()) {
        DijkstraNode cur = pq.top();
        pq.pop();

        double bestTime = res.dist[cur.station][cur.lastMode];
        int bestTransfers = res.transfers[cur.station][cur.lastMode];

        // stale states
        if (!doubleEqual(cur.time, bestTime)) continue;
        if (cur.transfers != bestTransfers) continue;

        const std::vector<Edge>& list = graph.neighbors(cur.station);

        for (size_t i = 0; i < list.size(); ++i) {
            const Edge& e = list[i];

            int nextStation = e.to;
            int nextMode = e.mode;

            double edgeTime = graph.edgeActualTime(e);
            double transferTime = graph.transferCost(cur.station, cur.lastMode, nextMode);

            double candTime = cur.time + edgeTime + transferTime;
            int candTransfers = cur.transfers + transfersDelta(cur.lastMode, nextMode);

            double storedTime = res.dist[nextStation][nextMode];
            int storedTransfers = res.transfers[nextStation][nextMode];

            if (pairBetter(candTime, candTransfers, storedTime, storedTransfers)) {
                res.dist[nextStation][nextMode] = candTime;
                res.transfers[nextStation][nextMode] = candTransfers;

                res.prevStation[nextStation][nextMode] = cur.station;
                res.prevMode[nextStation][nextMode] = cur.lastMode;
                res.prevEdgeMode[nextStation][nextMode] = nextMode;

                pq.push(DijkstraNode{candTime, candTransfers, nextStation, nextMode});
            }
        }
    }

    return res;
}

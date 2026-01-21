#include "project.hpp"

static bool isUnreachable(double timeValue) {
    return std::isinf(timeValue);
}

static bool pairBetter(double t1, int tr1, double t2, int tr2) {
    if (doubleLess(t1, t2)) return true;
    if (doubleEqual(t1, t2) && tr1 < tr2) return true;
    return false;
}

static void reverseVector(std::vector<int>& a) {
    if (a.size() < 2) return;
    size_t l = 0;
    size_t r = a.size() - 1;
    while (l < r) {
        swapValues(a[l], a[r]);
        ++l;
        --r;
    }
}

RouteResult buildRouteResult(const DijkstraResult& dres,
                             int startStation,
                             int targetStation,
                             double coef) {
    RouteResult res;
    res.target = targetStation;
    res.reachable = false;
    res.time = 0.0;
    res.transfers = 0;
    res.comfort = 0.0;
    res.stations.clear();
    res.modes.clear();

    if (targetStation == startStation) {
        res.reachable = true;
        res.stations.push_back(startStation);
        return res;
    }

    // choose best mode at target: min time, then min transfers
    double bestTime = std::numeric_limits<double>::infinity();
    int bestTransfers = std::numeric_limits<int>::max() / 4;
    int bestMode = -1;

    for (int mode = 0; mode < MODES; ++mode) {
        double t = dres.dist[targetStation][mode];
        int tr = dres.transfers[targetStation][mode];

        if (isUnreachable(t)) continue;

        if (bestMode < 0 || pairBetter(t, tr, bestTime, bestTransfers)) {
            bestTime = t;
            bestTransfers = tr;
            bestMode = mode;
        }
    }

    if (bestMode < 0 || isUnreachable(bestTime)) {
        return res; // unreachable
    }

    std::vector<int> stationsRev;
    std::vector<int> modesRev;

    int curStation = targetStation;
    int curMode = bestMode;

    stationsRev.push_back(curStation);

    while (true) {
        int pStation = dres.prevStation[curStation][curMode];
        int pMode = dres.prevMode[curStation][curMode];
        int pEdgeMode = dres.prevEdgeMode[curStation][curMode];

        if (pStation == 0) break;

        modesRev.push_back(pEdgeMode);
        stationsRev.push_back(pStation);

        curStation = pStation;
        curMode = pMode;

        if (curStation == startStation && curMode == START_MODE) break;
    }

    reverseVector(stationsRev);
    reverseVector(modesRev);

    res.reachable = true;
    res.time = bestTime;
    res.transfers = bestTransfers;
    res.comfort = bestTime + coef * static_cast<double>(bestTransfers);
    res.stations = stationsRev;
    res.modes = modesRev;

    return res;
}

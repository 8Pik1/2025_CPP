#include "project.hpp"

static bool routeLess(const RouteResult& a, const RouteResult& b) {
    // reachable first
    if (a.reachable != b.reachable) {
        return a.reachable && !b.reachable;
    }

    // both unreachable => by target id
    if (!a.reachable && !b.reachable) {
        return a.target < b.target;
    }

    // comfort, then time, then transfers, then target
    if (!doubleEqual(a.comfort, b.comfort)) return doubleLess(a.comfort, b.comfort);
    if (!doubleEqual(a.time, b.time)) return doubleLess(a.time, b.time);
    if (a.transfers != b.transfers) return a.transfers < b.transfers;
    return a.target < b.target;
}

static void quickSortRec(std::vector<RouteResult>& a, int left, int right) {
    int i = left;
    int j = right;
    RouteResult pivot = a[(left + right) / 2];

    while (i <= j) {
        while (routeLess(a[i], pivot)) ++i;
        while (routeLess(pivot, a[j])) --j;

        if (i <= j) {
            if (i != j) swapValues(a[i], a[j]);
            ++i;
            --j;
        }
    }

    if (left < j) quickSortRec(a, left, j);
    if (i < right) quickSortRec(a, i, right);
}

void quickSortRoutes(std::vector<RouteResult>& routes) {
    if (routes.empty()) return;
    quickSortRec(routes, 0, static_cast<int>(routes.size()) - 1);
}

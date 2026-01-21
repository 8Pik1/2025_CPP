#include "project.hpp"

int main() {
    Graph graph;
    std::vector<Query> queries;

    if (!readInput(graph, queries)) return 0;

    printZonesBlock("METRO", isolatedZones(graph, 0));
    printZonesBlock("BUS", isolatedZones(graph, 1));
    printZonesBlock("TRAIN", isolatedZones(graph, 2));
    printZonesBlock("ALL", isolatedZones(graph, START_MODE));

    for (size_t qi = 0; qi < queries.size(); ++qi) {
        const Query& q = queries[qi];

        DijkstraResult dres = runDijkstra(graph, q.start);

        std::vector<RouteResult> routes;
        routes.reserve(q.targets.size());

        for (size_t i = 0; i < q.targets.size(); ++i) {
            int target = q.targets[i];
            routes.push_back(buildRouteResult(dres, q.start, target, q.coef));
        }

        quickSortRoutes(routes);
        printQueryBlock(static_cast<int>(qi + 1), q, routes);
    }

    return 0;
}

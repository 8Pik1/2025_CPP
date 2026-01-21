#include "project.hpp"

static bool componentLess(const std::vector<int>& a,
                          const std::vector<int>& b) {
    // sort components: size desc, then lex asc
    if (a.size() != b.size()) return a.size() > b.size();
    return a < b;
}

std::vector<std::vector<int>> isolatedZones(const Graph& graph, int modeFilter) {
    int n = graph.stationCount();
    std::vector<int> used(n + 1, 0);

    std::vector<std::vector<int>> components;

    for (int start = 1; start <= n; ++start) {
        if (used[start]) continue;

        std::stack<int> st;
        st.push(start);
        used[start] = 1;

        std::vector<int> comp;

        while (!st.empty()) {
            int v = st.top();
            st.pop();

            comp.push_back(v);

            const std::vector<Edge>& list = graph.neighbors(v);
            for (size_t i = 0; i < list.size(); ++i) {
                const Edge& e = list[i];

                if (modeFilter != START_MODE && e.mode != modeFilter) {
                    continue;
                }

                int to = e.to;
                if (used[to]) continue;

                used[to] = 1;
                st.push(to);
            }
        }

        std::sort(comp.begin(), comp.end()); // inside component ascending
        components.push_back(comp);
    }

    std::sort(components.begin(), components.end(), componentLess);

    // largest component is components[0]
    std::vector<std::vector<int>> zones;
    for (size_t i = 1; i < components.size(); ++i) {
        zones.push_back(components[i]);
    }

    return zones;
}

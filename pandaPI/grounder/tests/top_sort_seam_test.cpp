#include "util.h"

#include <algorithm>
#include <iostream>
#include <vector>

namespace {

bool precedes(const std::vector<int>& order, int before, int after)
{
  const auto before_it = std::find(order.begin(), order.end(), before);
  const auto after_it = std::find(order.begin(), order.end(), after);
  return before_it != order.end() && after_it != order.end() && before_it < after_it;
}

} // namespace

int main()
{
  std::vector<std::vector<int>> graph(6);
  graph[0] = {1, 2};
  graph[1] = {3};
  graph[2] = {3};
  graph[3] = {4};

  std::vector<int> order;
  topsort(graph, order);

  if (order.size() != graph.size()) {
    std::cerr << "topological order has wrong size\n";
    return 1;
  }

  for (int node = 0; node < static_cast<int>(graph.size()); ++node) {
    if (std::find(order.begin(), order.end(), node) == order.end()) {
      std::cerr << "topological order omitted node " << node << '\n';
      return 1;
    }
  }

  if (!precedes(order, 0, 1) || !precedes(order, 0, 2) || !precedes(order, 1, 3) ||
      !precedes(order, 2, 3) || !precedes(order, 3, 4)) {
    std::cerr << "topological order violates dependency edge\n";
    return 1;
  }

  return 0;
}

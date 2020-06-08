#ifndef LION_GRAPH_TOPOLOGICAL_SORT_HPP
#define LION_GRAPH_TOPOLOGICAL_SORT_HPP

#include <vector>
#include <cstddef>
#include <functional>
#include <memory>
#include <utility>
#include <queue>
#include <deque>

namespace lion::graph
{
	template<typename Graph, typename OutputIterator, typename Allocator = typename Graph::allocator_type>
	inline bool topological_sort(const Graph& graph, OutputIterator out)
	{
		using vertex_type = typename Graph::vertex_type;

		std::unordered_map<
			vertex_type,
			std::size_t,
			std::hash<vertex_type>,
			std::equal_to<vertex_type>,
			typename std::allocator_traits<Allocator>::template rebind_alloc<std::pair<const vertex_type, std::size_t>>
		> indegrees;

		std::queue<vertex_type, std::deque<vertex_type, Allocator>> queue;

		for (const auto& vertex : graph.vertices())
		{
			indegrees[vertex] = 0;

			for (const auto& edge : graph.edges(vertex)) {
				++indegrees[edge];
			}
			if (indegrees[vertex] == 0) {
				queue.push(vertex);
			}
		}

		std::size_t count = 0;
		while (!queue.empty())
		{
			vertex_type vertex = queue.front();
			queue.pop();
			*out++ = vertex;

			for (const auto& edge : graph.edges(vertex))
			{
				if (--indegrees[edge] == 0) {
					queue.push(edge);
				}
			}
			++count;
		}

		return count == graph.vertex_count();
	}
}

#endif

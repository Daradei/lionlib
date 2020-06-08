#ifndef LION_GRAPH_DFS_BFS_HPP
#define LION_GRAPH_DFS_SBFS_HPP

#include <unordered_set>
#include <functional>
#include <stack>
#include <deque>
#include <utility>
#include <queue>

namespace lion::graph
{
	template<typename Graph, typename OutputIterator, typename Allocator = typename Graph::allocator_type>
	inline void dfs(const Graph& graph, const typename Graph::vertex_type& origin, OutputIterator out)
	{
		using vertex_type = typename Graph::vertex_type;

		std::unordered_set<vertex_type, std::hash<vertex_type>, std::equal_to<vertex_type>, Allocator> discovered;
		std::stack<vertex_type, std::deque<vertex_type, Allocator>> stack;

		stack.push(origin);

		while (!stack.empty())
		{
			vertex_type vertex = stack.top();
			stack.pop();

			const auto find = discovered.find(vertex);
			if (find == discovered.end())
			{
				*out++ = vertex;
				discovered.insert(std::move(vertex));

				for (const auto& edge : graph.edges(vertex)) {
					stack.push(edge);
				}
			}
		}
	}

	template<typename Graph, typename OutputIterator, typename Allocator = typename Graph::allocator_type>
	inline void bfs(const Graph& graph, const typename Graph::vertex_type& origin, OutputIterator out)
	{
		using vertex_type = typename Graph::vertex_type;

		std::unordered_set<vertex_type, std::hash<vertex_type>, std::equal_to<vertex_type>, Allocator> discovered;
		std::queue<vertex_type, std::deque<vertex_type, Allocator>> queue;

		discovered.insert(origin);
		queue.push(origin);

		while (!queue.empty())
		{
			vertex_type vertex = queue.front();
			*out++ = vertex;
			queue.pop();

			for (auto&& edge : graph.edges(vertex))
			{
				const auto find = discovered.find(edge);
				if (find == discovered.end())
				{
					discovered.insert(edge);
					queue.push(std::move(edge));
				}
			}
		}
	}
}

#endif

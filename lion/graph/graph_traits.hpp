#ifndef LION_GRAPH_GRAPH_TRAITS_HPP
#define LION_GRAPH_GRAPH_TRAITS_HPP

#include "digraph.hpp"
#include "wdigraph.hpp"

#include <type_traits>

namespace lion::graph
{
	template<typename Graph>
	struct graph_traits
	{
		static constexpr bool is_graph = false;
		static constexpr bool is_weighted = false;
		static constexpr bool is_directed = false;
	};

	template<
		template<typename, template<typename...> typename, typename> typename Graph,
		typename Vertex,
		template<typename...> typename Representation,
		typename Allocator
	>
	struct graph_traits<Graph<Vertex, Representation, Allocator>>
	{
		static constexpr bool is_graph = true;;
		static constexpr bool is_weighted = false;
		static constexpr bool is_directed = std::is_same_v<Graph<Vertex, Representation, Allocator>, 
													       digraph<Vertex, Representation, Allocator>>;
	};

	template<
		template<typename, typename, template<typename...> typename, typename> typename Graph,
		typename Vertex,
		typename Weight,
		template<typename...> typename Representation,
		typename Allocator
	>
	struct graph_traits<Graph<Vertex, Weight, Representation, Allocator>>
	{
		static constexpr bool is_graph = true;
		static constexpr bool is_weighted = true;
		static constexpr bool is_directed = std::is_same_v<Graph<Vertex, Weight, Representation, Allocator>,
														   wdigraph<Vertex, Weight, Representation, Allocator>>;
	};
}

#endif

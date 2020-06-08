#ifndef LION_GRAPH_GRAPH_HPP
#define LION_GRAPH_GRAPH_HPP

#include "../range.hpp"
#include "adjacency_list.hpp"

#include <memory>
#include <type_traits>
#include <initializer_list>
#include <utility>

namespace lion::graph
{
	template<
		typename Vertex,
		template<typename...> typename Representation = adjacency_list,
		typename Allocator = std::allocator<Vertex>
	> 
	class graph : public Representation<std::false_type, Vertex, void, Allocator>
	{
	public:
		// std::false_type for Unweighted representation
		using representation_type = Representation<std::false_type, Vertex, void, Allocator>;

		using vertex_type	  = typename representation_type::vertex_type;
		using edge_type		  = typename representation_type::edge_type;
		using allocator_type  = typename representation_type::allocator_type;

		using size_type       = typename representation_type::size_type;
		using difference_type = typename representation_type::difference_type;

		using vertex_iterator = typename representation_type::vertex_iterator;
		using edge_iterator   = typename representation_type::edge_iterator;

	public:
		explicit graph(const allocator_type& alloc = allocator_type{}) 
			: representation_type(alloc)
		{}

		graph(std::initializer_list<edge_type> edges, const allocator_type& alloc = allocator_type{})
			: graph(edges.begin(), edges.end(), alloc)
		{}

		template<typename ForwardIterator>
		graph(ForwardIterator first, ForwardIterator last, const allocator_type& alloc = allocator_type{})
			: representation_type(alloc)
		{
			for (; first != last; ++first) {
				add_edge(*first);
			}
		}

		void add_edge(const edge_type& edge) 
		{
			representation_type::add_edge(edge);
			representation_type::add_edge({ edge.second, edge.first });
		}

		void add_edge(edge_type&& edge) 
		{
			representation_type::add_edge({ edge.second, edge.first });
			representation_type::add_edge(std::move(edge));
		}

		void remove_edge(const edge_type& edge)
		{
			representation_type::remove_edge(edge);
			representation_type::remove_edge({ edge.second, edge.first });
		}

		range<vertex_iterator> vertices() const noexcept {
			return { this->begin(), this->end() };
		}

		range<edge_iterator> edges(const vertex_type& vert) const noexcept {
			return { this->begin(vert), this->end(vert) };
		}
	};
}

#endif

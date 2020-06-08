#ifndef LION_GRAPH_DIGRAPH_HPP
#define LION_GRAPH_DIGRAPH_HPP

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
	class digraph : public Representation<std::false_type, Vertex, void, Allocator>
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
		explicit digraph(const allocator_type& alloc = allocator_type{})
			: representation_type(alloc)
		{}

		digraph(std::initializer_list<edge_type> edges, const allocator_type& alloc = allocator_type{})
			: digraph(edges.begin(), edges.end(), alloc)
		{}

		template<typename ForwardIterator>
		digraph(ForwardIterator first, ForwardIterator last, const allocator_type& alloc = allocator_type{})
			: representation_type(alloc)
		{
			for (; first != last; ++first) {
				add_edge(*first);
			}
		}

		void add_edge(const edge_type& edge) 
		{
			representation_type::add_edge(edge);
			representation_type::add_vertex(edge.second);
		}

		void add_edge(edge_type&& edge) 
		{
			representation_type::add_edge({ std::move(edge.first), edge.second });
			representation_type::add_vertex(std::move(edge.second));
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

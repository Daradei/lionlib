#ifndef LION_GRAPH_WDIGRAPH_HPP
#define LION_GRAPH_WDIGRAPH_HPP

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
		typename Weight,
		template<typename...> typename Representation = adjacency_list,
		typename Allocator = std::allocator<Vertex>
	> 
	class wdigraph : public Representation<std::true_type, Vertex, Weight, Allocator>
	{
	public:
		// std::true_type for Weighted representation
		using representation_type = Representation<std::true_type, Vertex, Weight, Allocator>;

		using vertex_type	  = typename representation_type::vertex_type;
		using weight_type	  = typename representation_type::weight_type;
		using edge_type		  = typename representation_type::edge_type;
		using allocator_type  = typename representation_type::allocator_type;

		using size_type       = typename representation_type::size_type;
		using difference_type = typename representation_type::difference_type;

		using vertex_iterator = typename representation_type::vertex_iterator;
		using edge_iterator   = typename representation_type::edge_iterator;

	public:
		explicit wdigraph(const allocator_type& al = allocator_type{})
			: representation_type(al)
		{}

		wdigraph(std::initializer_list<edge_type> edges, const allocator_type& alloc = allocator_type{})
			: wdigraph(edges.begin(), edges.end(), alloc)
		{}

		template<typename ForwardIterator>
		wdigraph(ForwardIterator first, ForwardIterator last, const allocator_type& alloc = allocator_type{})
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
			representation_type::add_edge({ std::move(edge.first), edge.second, std::move(edge.weight) });
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

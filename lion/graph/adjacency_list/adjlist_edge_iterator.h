#ifndef LION_GRAPH_ADJLIST_EDGE_ITERATOR_HPP
#define LION_GRAPH_ADJLIST_EDGE_ITERATOR_HPP

#include "adjacency_list_unweighted.hpp"
#include "adjacency_list_weighted.hpp"

#include <type_traits>
#include <iterator>
#include <memory>

namespace lion::graph
{
	// unweighted
	template<typename Vertex, typename Weight, typename Allocator>
	class adjacency_list<std::false_type, Vertex, Weight, Allocator>::edge_iterator
	{
	private:
		template<typename Weighted, typename... Ts>
		friend class adjacency_list;

		using list = adjacency_list<std::false_type, Vertex, Weight, Allocator>;

		typename list::list_type::const_iterator current;

		explicit edge_iterator(typename list::list_type::const_iterator curr)
			: current(curr)
		{}

	public:
		using value_type		= typename list::list_type::const_iterator::value_type;
		using reference			= typename list::list_type::const_iterator::reference;
		using pointer			= typename list::list_type::const_iterator::pointer;
		using difference_type	= typename list::list_type::const_iterator::difference_type;
		using iterator_category = std::forward_iterator_tag;

		edge_iterator() = default;
		edge_iterator(const edge_iterator&) = default;
		edge_iterator& operator=(const edge_iterator&) = default;

		edge_iterator& operator++()
		{
			++current;
			return *this;
		}

		edge_iterator operator++(int)
		{
			edge_iterator temp(current);
			++current;
			return temp;
		}

		reference operator*() const { return *current; }
		pointer operator->()  const { return std::addressof(*current); }

		friend bool operator==(const edge_iterator& lhs, const edge_iterator& rhs) {
			return lhs.current == rhs.current;
		}
		friend bool operator!=(const edge_iterator& lhs, const edge_iterator& rhs) {
			return !(lhs == rhs);
		}
	};

	// weighted
	template<typename Vertex, typename Weight, typename Allocator>
	class adjacency_list<std::true_type, Vertex, Weight, Allocator>::edge_iterator
	{
	private:
		template<typename Weighted, typename... Ts>
		friend class adjacency_list;

		using list = adjacency_list<std::true_type, Vertex, Weight, Allocator>;

		typename list::list_type::const_iterator current;

		explicit edge_iterator(typename list::list_type::const_iterator curr)
			: current(curr)
		{}

	public:
		using value_type		= typename list::list_type::const_iterator::value_type;
		using reference			= typename list::list_type::const_iterator::reference;
		using pointer			= typename list::list_type::const_iterator::pointer;
		using difference_type	= typename list::list_type::const_iterator::difference_type;
		using iterator_category = std::forward_iterator_tag;

		edge_iterator() = default;
		edge_iterator(const edge_iterator&) = default;
		edge_iterator& operator=(const edge_iterator&) = default;

		edge_iterator& operator++()
		{
			++current;
			return *this;
		}

		edge_iterator operator++(int)
		{
			edge_iterator temp(current);
			++current;
			return temp;
		}

		reference operator*() const { return *current; }
		pointer operator->()  const { return std::addressof(*current); }

		friend bool operator==(const edge_iterator& lhs, const edge_iterator& rhs) {
			return lhs.current == rhs.current;
		}
		friend bool operator!=(const edge_iterator& lhs, const edge_iterator& rhs) {
			return !(lhs == rhs);
		}
	};
}

#endif

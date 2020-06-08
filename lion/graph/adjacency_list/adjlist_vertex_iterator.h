#ifndef LION_GRAPH_ADJLIST_VERTEX_ITERATOR_HPP
#define LION_GRAPH_ADJLIST_VERTEX_ITERATOR_HPP

#include "adjacency_list_unweighted.hpp"
#include "adjacency_list_weighted.hpp"

#include <type_traits>
#include <iterator>
#include <memory>

namespace lion::graph
{
	// unweighted
	template<typename Vertex, typename Weight, typename Allocator>
	class adjacency_list<std::false_type, Vertex, Weight, Allocator>::vertex_iterator
	{
	private:
		template<typename Weighted, typename... Ts>
		friend class adjacency_list;

		using list = adjacency_list<std::false_type, Vertex, Weight, Allocator>;

		typename list::map_iterator current;

		explicit vertex_iterator(typename list::map_iterator iter)
			: current(iter)
		{}

	public:
		using value_type		= typename list::vertex_type;
		using reference			= const value_type&;
		using pointer			= const value_type*;
		using difference_type	= typename list::map_iterator::difference_type;
		using iterator_category = std::forward_iterator_tag;

		vertex_iterator() = default;
		vertex_iterator(const vertex_iterator&) = default;
		vertex_iterator& operator=(const vertex_iterator&) = default;

		vertex_iterator& operator++()
		{
			++current;
			return *this;
		}

		vertex_iterator operator++(int)
		{
			vertex_iterator temp(current);
			++current;
			return temp;
		}

		reference operator*() const { return current->first; }
		pointer operator->()  const { return std::addressof(current->first); }

		friend bool operator==(const vertex_iterator& lhs, const vertex_iterator& rhs) {
			return lhs.current == rhs.current;
		}
		friend bool operator!=(const vertex_iterator& lhs, const vertex_iterator& rhs) {
			return !(lhs == rhs);
		}
	};

	// weighted
	template<typename Vertex, typename Weight, typename Allocator>
	class adjacency_list<std::true_type, Vertex, Weight, Allocator>::vertex_iterator
	{
	private:
		template<typename Weighted, typename... Ts>
		friend class adjacency_list;

		using list = adjacency_list<std::true_type, Vertex, Weight, Allocator>;

		typename list::map_iterator current;

		explicit vertex_iterator(typename list::map_iterator iter)
			: current(iter)
		{}

	public:
		using value_type		= typename list::vertex_type;
		using reference			= const value_type&;
		using pointer			= const value_type*;
		using difference_type	= typename list::map_iterator::difference_type;
		using iterator_category = std::forward_iterator_tag;

		vertex_iterator() = default;
		vertex_iterator(const vertex_iterator&) = default;
		vertex_iterator& operator=(const vertex_iterator&) = default;

		vertex_iterator& operator++() 
		{
			++current;
			return *this;
		}

		vertex_iterator operator++(int)
		{
			vertex_iterator temp(current);
			++current;
			return temp;
		}

		reference operator*() const { return current->first; }
		pointer operator->()  const { return std::addressof(current->first); }

		friend bool operator==(const vertex_iterator& lhs, const vertex_iterator& rhs) {
			return lhs.current == rhs.current;
		}
		friend bool operator!=(const vertex_iterator& lhs, const vertex_iterator& rhs) {
			return !(lhs == rhs);
		}
	};
}

#endif

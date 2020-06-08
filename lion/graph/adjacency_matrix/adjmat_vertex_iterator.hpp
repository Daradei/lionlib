#ifndef LION_GRAP_ADJMAT_VERTEX_ITERATOR
#define LION_GRAP_ADJMAT_VERTEX_ITERATOR

#include "adjacency_matrix_unweighted.hpp"
#include "adjacency_matrix_weighted.hpp"

#include <type_traits>
#include <iterator>
#include <memory>

namespace lion::graph
{
	// unweighted
	template<typename Vertex, typename Weight, typename Allocator>
	class adjacency_matrix<std::false_type, Vertex, Weight, Allocator>::vertex_iterator
	{
	private:
		template<typename Weighted, typename... Ts>
		friend class adjacency_matrix;

		using matrix = adjacency_matrix<std::false_type, Vertex, Weight, Allocator>;

		typename decltype(matrix::vertices)::const_iterator current;

		explicit vertex_iterator(decltype(current) iter)
			: current(iter)
		{}

	public:
		using value_type		= typename matrix::vertex_type;
		using reference			= const value_type&;
		using pointer			= const value_type*;
		using difference_type	= typename decltype(current)::difference_type;
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
	class adjacency_matrix<std::true_type, Vertex, Weight, Allocator>::vertex_iterator
	{
	private:
		template<typename Weighted, typename... Ts>
		friend class adjacency_matrix;

		using matrix = adjacency_matrix<std::true_type, Vertex, Weight, Allocator>;

		typename decltype(matrix::vertices)::const_iterator current;

		explicit vertex_iterator(decltype(current) iter)
			: current(iter)
		{}

	public:
		using value_type		= typename matrix::vertex_type;
		using reference			= const value_type&;
		using pointer		    = const value_type*;
		using difference_type	= typename decltype(current)::difference_type;
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

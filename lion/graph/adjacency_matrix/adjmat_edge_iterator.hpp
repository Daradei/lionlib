#ifndef LION_GRAPH_ADJMAT_EDGE_ITERATOR_HPP
#define LION_GRAPH_ADJMAT_EDGE_ITERATOR_HPP

#include "adjacency_matrix_unweighted.hpp"
#include "adjacency_matrix_weighted.hpp"

#include <type_traits>
#include <iterator>
#include <memory>

namespace lion::graph
{
	// unweighted
	template<typename Vertex, typename Weight, typename Allocator>
	class adjacency_matrix<std::false_type, Vertex, Weight, Allocator>::edge_iterator
	{
	private:
		template<typename Weighted, typename... Ts>
		friend class adjacency_matrix;

		using matrix = adjacency_matrix<std::false_type, Vertex, Weight, Allocator>;

		typename matrix::row_type::const_iterator current;
		typename matrix::vertices_list::const_iterator vertex;
		typename matrix::vertices_list::const_iterator end;

		edge_iterator(decltype(current) curr, decltype(vertex) vert, decltype(end) last)
			: current(curr), vertex(vert), end(last)
		{}

	public:
		using value_type		= typename matrix::vertex_type;
		using reference			= const value_type&;
		using pointer			= const value_type*;
		using difference_type	= typename  decltype(vertex)::difference_type;
		using iterator_category = std::forward_iterator_tag;

		edge_iterator() = default;
		edge_iterator(const edge_iterator&) = default;
		edge_iterator& operator=(const edge_iterator&) = default;

		edge_iterator& operator++()
		{
			++vertex;
			++current;
			if (vertex != end) 
			{
				if (!*current) {
					++*this;
				}
			}
			return *this;
		}

		edge_iterator operator++(int)
		{
			edge_iterator temp(current);
			++*this;
			return temp;
		}

		reference operator*() const { return vertex->first; }
		pointer operator->()  const { return std::addressof(vertex->first); }

		friend bool operator==(const edge_iterator& lhs, const edge_iterator& rhs) {
			return lhs.current == rhs.current && lhs.vertex == rhs.vertex;
		}
		friend bool operator!=(const edge_iterator& lhs, const edge_iterator& rhs) {
			return !(lhs == rhs);
		}
	};

	template<typename Vertex, typename Weight, typename Allocator>
	class adjacency_matrix<std::true_type, Vertex, Weight, Allocator>::edge_iterator
	{
	private:
		template<typename Weighted, typename... Ts>
		friend class adjacency_matrix;

		using matrix		  = adjacency_matrix<std::true_type, Vertex, Weight, Allocator>;
		using vert_iterator	  = typename matrix::vertices_list::const_iterator;
		using weight_iterator = typename matrix::row_type::const_iterator;

		vert_iterator end;

		edge_iterator(weight_iterator curr, vert_iterator vert, vert_iterator last)
			: end(last), value{ curr, vert }
		{}

		struct helper1
		{
		private:
			friend class matrix::edge_iterator;

			vert_iterator iter;
			helper1(vert_iterator it) : iter(it) {}

			bool operator==(const helper1& rhs) const { return iter == rhs.iter; }

		public:
			operator const typename matrix::vertex_type&() const { return iter->first; }
		};

		struct helper2
		{
		private:
			friend class matrix::edge_iterator;

			weight_iterator iter;
			helper2(weight_iterator it) : iter(it) {}

			bool operator==(const helper2& rhs) const { return iter == rhs.iter; }

		public:
			operator       typename matrix::weight_type&()       { return iter->weight; }
			operator const typename matrix::weight_type&() const { return iter->weight; }

			helper2& operator=(const typename matrix::weight_type& val)
			{
				iter->weight = val;
				return *this;
			}

			helper2& operator=(typename matrix::weight_type&& val)
			{
				iter->weight = std::move(val);
				return *this;
			}
		};

	public:
		struct value_type
		{
		private:
			friend class matrix::edge_iterator;

			bool operator==(const value_type& rhs) const {
				return second == rhs.second && weight == rhs.weight;
			}

			value_type(weight_iterator curr, vert_iterator vert)
				: second(vert), weight(curr)
			{}

		public:
			helper1 second;
			mutable helper2 weight;

			operator       typename matrix::vertex_type&()       { return second; }
			operator const typename matrix::vertex_type&() const { return second; }
		};

	private:
		value_type value;

	public:
		using reference			= const value_type&;
		using pointer			= const value_type*;
		using difference_type   = typename vert_iterator::difference_type;
		using iterator_category = std::forward_iterator_tag;

		edge_iterator() = default;
		edge_iterator(const edge_iterator&) = default;
		edge_iterator& operator=(const edge_iterator&) = default;

		edge_iterator& operator++()
		{
			++value.second.iter;
			++value.weight.iter;
			if (value.second.iter != end)
			{
				if (!value.weight.iter->exists) {
					++*this;
				}
			}
			return *this;
		}

		edge_iterator operator++(int)
		{
			edge_iterator temp(*this);
			++*this;
			return temp;
		}

		reference operator*() const { return value; }
		pointer operator->()  const { return std::addressof(operator*()); }

		bool operator==(const edge_iterator& rhs) const { return value == rhs.value; }
		bool operator!=(const edge_iterator& rhs) const { return !(*this == rhs); }
	};
}

#endif

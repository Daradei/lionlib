#ifndef LION_ADJACENCY_LIST_WEIGHTED_HPP
#define LION_ADJACENCY_LIST_WEIGHTED_HPP

#include <type_traits>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>
#include <utility>
#include <algorithm>

namespace lion::graph
{
	template<typename Weighted, typename... Ts>
	class adjacency_list;

	template<typename Vertex, typename Weight, typename Allocator>
	class adjacency_list<std::true_type, Vertex, Weight, Allocator>
	{
	protected:
		using vertex_type	 = Vertex;
		using weight_type	 = Weight;
		using allocator_type = Allocator;

	private:
		struct edge_type1
		{
			vertex_type second;
			mutable weight_type weight;

			operator const vertex_type&() const { return second; }
		};

		struct edge_type0 
		{
			vertex_type first;
		};

	protected:
		struct edge_type : edge_type0, edge_type1 {};

	private:
		using list_type = std::vector<
			edge_type1,
			typename std::allocator_traits<allocator_type>::template rebind_alloc<edge_type1>
		>;

		std::unordered_map<
			vertex_type,
			list_type,
			std::hash<vertex_type>,
			std::equal_to<>,
			typename std::allocator_traits<allocator_type>::template rebind_alloc<
				std::pair<const vertex_type, list_type>
			>
		> map;

		using map_iterator = typename decltype(map)::const_iterator;

	protected:
		using size_type		  = typename decltype(map)::size_type;
		using difference_type = typename decltype(map)::difference_type;

		class vertex_iterator;
		class edge_iterator;

		explicit adjacency_list(const allocator_type& al = allocator_type{})
			: map(al)
		{}

	public:
		void add_edge(const edge_type& edge) 
		{
			auto&&[iter, succ] = map.insert({ edge.first, list_type(get_allocator()) });
			iter->second.push_back({ edge.second, edge.weight });
		}

		void add_edge(edge_type&& edge) 
		{
			auto&&[iter, succ] = map.insert({ std::move(edge.first), list_type(get_allocator()) });
			iter->second.push_back({ std::move(edge.second), std::move(edge.weight) });
		}

		void add_vertex(const vertex_type& vertex) {
			map.insert({ vertex, list_type(get_allocator()) });
		}

		void add_vertex(vertex_type&& vertex) {
			map.insert({ std::move(vertex), list_type(get_allocator()) });
		}

		void remove_edge(const edge_type& edge)
		{
			auto& list = map.find(edge.first)->list;
			list.erase(std::remove(list.begin(), list.end(), edge.second), list.end());
		}

		void remove_vertex(const vertex_type& vertex)
		{
			map.erase(vertex);
			for (auto&[vert, list] : map) {
				list.erase(std::remove(list.begin(), list.end(), vertex), list.end());
			}
		}

		bool contains(const vertex_type& vertex) const {
			return map.find(vertex) != map.end();
		}

		vertex_iterator get(const vertex_type& vertex) const {
			return vertex_iterator(map.find(vertex));
		}

		size_type adjacent(const vertex_type& x, const vertex_type& y, weight_type* weight) const
		{
			size_type result = 0;

			const auto& list = map.find(x)->second;
			for (auto&& val : list)
			{
				if (val == y) 
				{
					++result;

					if (weight) {
						*weight = val.weight;
					}
				}
			}
			return result;
		}

		size_type vertex_count() const noexcept {
			return map.size();
		}

		size_type edge_count() const noexcept
		{
			size_type result = 0;
			for (auto&&[vert, list] : map) {
				result += list.size();
			}
			return result;
		}

		vertex_iterator begin() const noexcept { return vertex_iterator(map.begin()); }
		vertex_iterator end()   const noexcept { return vertex_iterator(map.end()); }

		edge_iterator begin(const vertex_type& vert) const noexcept {
			return edge_iterator(map.find(vert)->second.begin());
		}
		edge_iterator end(const vertex_type& vert) const noexcept {
			return edge_iterator(map.find(vert)->second.end());
		}

		friend bool operator==(const adjacency_list& lhs, const adjacency_list& rhs) {
			return lhs.map == rhs.map;
		}
		friend bool operator!=(const adjacency_list& lhs, const adjacency_list& rhs) {
			return !(lhs == rhs);
		}

		void swap(adjacency_list& rhs) noexcept(noexcept(std::swap(map, rhs.map))) {
			std::swap(map, rhs.map);
		}

		allocator_type get_allocator() const { return map.get_allocator(); }
	};
}

#endif

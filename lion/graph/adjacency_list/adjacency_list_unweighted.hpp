#ifndef LION_ADJACENCY_LIST_UNWEIGHTED_HPP
#define LION_ADJACENCY_LIST_UNWEIGHTED_HPP

#include <type_traits>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>
#include <utility>
#include <algorithm>

namespace lion::graph
{
	template<typename Weighted, typename... Ts>
	class adjacency_list;

	template<typename Vertex, typename Weight, typename Allocator>
	class adjacency_list<std::false_type, Vertex, Weight, Allocator> 
	{
	protected:
		using vertex_type	 = Vertex;
		using allocator_type = Allocator;

		struct edge_type
		{
			vertex_type first;
			vertex_type second;
		};

	private:
		using list_type = std::vector<vertex_type, allocator_type>;

		std::unordered_map<
			vertex_type,
			list_type,
			std::hash<vertex_type>,
			std::equal_to<vertex_type>,
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

		explicit adjacency_list(const allocator_type& alloc = allocator_type{})
			: map(alloc)
		{}

	public:
		void add_edge(const edge_type& edge) 
		{
			auto&&[iter, succ] = map.insert({ edge.first, list_type(get_allocator()) });
			iter->second.push_back(edge.second);
		}

		void add_edge(edge_type&& edge) 
		{
			auto&&[iter, succ] = map.insert({ std::move(edge.first), list_type(get_allocator()) });
			iter->second.push_back(std::move(edge.second));
		}

		void add_vertex(const vertex_type& vertex) { 
			map.insert({ vertex, list_type(get_allocator()) }); 
		}
		
		void add_vertex(vertex_type&& vertex) { 
			map.insert({ std::move(vertex), list_type(get_allocator()) }); 
		}

		void remove_edge(const edge_type& edge)
		{
			auto& list = map.find(edge.first)->second;
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

		size_type adjacent(const vertex_type& x, const vertex_type& y) const
		{
			auto& list = map.find(x)->second;	
			return std::count(list.begin(), list.end(), y);
		}

		size_type vertex_count() const noexcept { return map.size(); }

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

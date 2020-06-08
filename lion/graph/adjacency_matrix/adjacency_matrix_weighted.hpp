#ifndef LION_GRAPH_ADJACENCY_MATRIX_WEIGHTED_HPP
#define LION_GRAPH_ADJACENCY_MATRIX_WEIGHTED_HPP

#include <type_traits>
#include <unordered_map>
#include <cstddef>
#include <functional>
#include <memory>
#include <utility>
#include <vector>
#include <iterator>

namespace lion::graph
{
	template<typename Weighted, typename... Ts>
	class adjacency_matrix;

	template<typename Vertex, typename Weight, typename Allocator>
	class adjacency_matrix<std::true_type, Vertex, Weight, Allocator>
	{
	protected:
		using vertex_type	 = Vertex;
		using weight_type	 = Weight;
		using allocator_type = Allocator;

		struct edge_type
		{
			vertex_type first;
			vertex_type second;
			weight_type weight;
		};

	private:
		using vertices_list = std::unordered_map<
			vertex_type,
			std::size_t,
			std::hash<vertex_type>,
			std::equal_to<>,
			typename std::allocator_traits<allocator_type>::template rebind_alloc<
				std::pair<const vertex_type, std::size_t>
			>
		>;

		struct edge_type1
		{
			int exists;
			mutable weight_type weight;

			bool operator==(const edge_type1& rhs) const {
				return exists == rhs.exists && weight == rhs.weight;
			}
			bool operator!=(const edge_type1& rhs) const {
				return !(*this == rhs);
			}
		};

		using row_type = std::vector<
			edge_type1,
			typename std::allocator_traits<allocator_type>::template rebind_alloc<edge_type1>
		>;

		using matrix_type = std::vector<
			row_type,
			typename std::allocator_traits<allocator_type>::template rebind_alloc<row_type>
		>;

		std::size_t idx = 0;
		vertices_list vertices;
		matrix_type matrix;

	protected:
		using size_type		  = typename decltype(matrix)::size_type;
		using difference_type = typename decltype(matrix)::difference_type;

		class vertex_iterator;
		class edge_iterator;

		explicit adjacency_matrix(const allocator_type& al = allocator_type{})
			: vertices(al), matrix(al)
		{}

	private:
		size_type add_vertexi(const vertex_type& vertex)
		{
			if (auto&&[it, succ] = vertices.insert({ vertex, idx }); succ)
			{
				matrix.push_back(row_type(matrix.size()));
				for (auto& row : matrix) {
					row.push_back({ 0 });
				}
				return idx++;
			}
			else {
				return it->second;
			}
		}

		size_type add_vertexi(vertex_type&& vertex)
		{
			if (auto&&[it, succ] = vertices.insert({ std::move(vertex), idx }); succ)
			{
				matrix.push_back(row_type(matrix.size()));
				for (auto& row : matrix) {
					row.push_back({ 0 });
				}
				return idx++;
			}
			else {
				return it->second;
			}
		}

	public:
		void add_edge(const edge_type& edge)
		{
			const auto idx1 = add_vertexi(edge.first);
			const auto idx2 = add_vertexi(edge.second);

			matrix[idx1][idx2] = { 1, edge.weight };
		}

		void add_edge(edge_type&& edge)
		{
			const auto idx1 = add_vertexi(std::move(edge.first));
			const auto idx2 = add_vertexi(std::move(edge.second));

			matrix[idx1][idx2] = { 1, std::move(edge.weight) };
		}

		void add_vertex(const vertex_type& vertex) {
			add_vertexi(vertex);
		}

		void add_vertex(vertex_type&& vertex) {
			add_vertexi(std::move(vertex));
		}

		void remove_edge(const edge_type& edge)
		{
			const auto findx = vertices.find(edge.first);
			const auto findy = vertices.find(edge.second);
			
			matrix[findx->second][findy->second].exists = 0;
		}

		void remove_vertex(const vertex_type& vertex)
		{
			const auto find = vertices.find(vertex);
			const auto index = find->second;

			matrix.erase(std::next(matrix.begin(), index));

			for (auto& row : matrix) {
				row.erase(std::next(row.begin(), index));
			}
			for (auto it = std::next(find); it != vertices.end(); ++it) {
				--it->second;
			}
			vertices.erase(find);

			--idx;
		}

		bool has_vertex(const vertex_type& vertex) const
		{
			const auto find = vertices.find(vertex);
			return find != vertices.end();
		}

		size_type adjacent(const vertex_type& x, const vertex_type& y, weight_type* weight) const
		{
			const auto findx = vertices.find(x);
			const auto findy = vertices.find(y);

			const auto res = matrix[findx->second][findy->second];
			if (weight) {
				*weight = res.weight;
			}
			return res.exists;
		}

		size_type vertex_count() const noexcept {
			return vertices.size();
		}

		size_type edge_count() const noexcept
		{
			size_type result = 0;
			for (auto&& row : matrix)
			{
				for (auto&&[i, w] : row)
				{
					if (i) {
						++result;
					}
				}
			}
			return result;
		}

		vertex_iterator begin() const noexcept { return vertex_iterator(vertices.begin()); }
		vertex_iterator end()   const noexcept { return vertex_iterator(vertices.end()); }

		edge_iterator begin(const vertex_type& vert) const noexcept
		{
			const auto find = vertices.find(vert);
			const auto index = find->second;

			auto matit = matrix[index].begin();
			auto verit = vertices.begin();

			while (matit != matrix[index].end() && verit != vertices.end())
			{
				if (matit->exists) {
					break;
				}
				++matit;
				++verit;
			}

			return edge_iterator(matit, verit, vertices.end());
		}

		edge_iterator end(const vertex_type& vert) const noexcept
		{
			const auto find = vertices.find(vert);
			const auto index = find->second;

			return edge_iterator(matrix[index].end(), vertices.end(), vertices.end());
		}

		friend bool operator==(const adjacency_matrix& lhs, const adjacency_matrix& rhs) {
			return lhs.vertices == rhs.vertices && lhs.matrix == rhs.matrix;
		}
		friend bool operator!=(const adjacency_matrix& lhs, const adjacency_matrix& rhs) {
			return !(lhs == rhs);
		}

		void swap(adjacency_matrix& rhs) noexcept(
			noexcept(std::swap(idx, rhs.idx)) &&
			noexcept(std::swap(vertices, rhs.vertices)) &&
			noexcept(std::swap(matrix, rhs.matrix))) 
		{
			std::swap(idx, rhs.idx);
			std::swap(vertices, rhs.vertices);
			std::swap(matrix, rhs.matrix);
		}

		allocator_type get_allocator() const { return matrix.get_allocator(); }
	};
}

#endif

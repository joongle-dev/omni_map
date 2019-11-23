#pragma once
#include "mem_pool.h"

namespace util
{
	template <typename... T>
	class omni_map
	{
		static constexpr size_t num_element = sizeof...(T);

		using tuple_t = std::tuple<T...>;

		template <size_t I>
		using element_t = typename std::tuple_element_t<I, tuple_t>;

		struct node;

		struct branch {
			branch() : 
				left(nullptr), 
				right(nullptr) {}
			node* left;
			node* right;
		};

		struct node {
			node(T&&... t) :
				values(t...) {}
			tuple_t values;
			branch branches[num_element];
		};
		
	public:
		template <size_t I>
		class iterator {
			friend class omni_map<T...>;
		public:
			iterator(node* n = nullptr) :
				curr(n) {}
			const tuple_t& operator*() {
				return curr->values;
			}
			const tuple_t& operator->() {
				return curr->values;
			}
		private:
			node* curr;
		};

	public:
		omni_map() : 
			m_roots{ nullptr } {}

		void emplace(T&&... t) {
			node* n = m_nodepool.construct(std::forward<T>(t)...);
			insert_node<0>(n);
		}

		template <size_t I>
		iterator<I> find(const element_t<I>& value) {
			node* curr = m_roots[I];
			while (curr)
				if (value < std::get<I>(curr->values))
					curr = curr->branches[I].left;
				else if (value > std::get<I>(curr->values))
					curr = curr->branches[I].right;
				else
					return iterator<I>(curr);
			return iterator<I>();
		}

	private:
		template <size_t I>
		void insert_node(node* n) {
			node** curr = &m_roots[I];
			while (*curr)
				if (std::get<I>(n->values) <= std::get<I>((*curr)->values))
					curr = &(*curr)->branches[I].left;
				else
					curr = &(*curr)->branches[I].right;
			*curr = n;
			insert_node<I + 1>(n);
		}

		template <>
		void insert_node<num_element>(node* n) {}

	private:
		mem_pool<node> m_nodepool;
		node* m_roots[num_element];
	};
}
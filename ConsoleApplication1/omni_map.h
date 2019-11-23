#pragma once
#include "mem_pool.h"

namespace util
{
	template <typename... T>
	class omni_map
	{
		static constexpr size_t num_element = sizeof...(T);

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
			std::tuple<T...> values;
			branch branches[num_element];
		};
		
	public:
		template <size_t I>
		class iterator {
		public:
			iterator(node* n = nullptr) :
				curr(n) {}
			const std::tuple<T...>& operator*() {
				return curr->values;
			}
			const std::tuple<T...>& operator->() {
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
		template <size_t I, typename B>
		iterator<I> find(const B& b) {
			node* curr = m_roots[I];
			while (curr)
				if (b < std::get<I>(curr->values))
					curr = curr->branches[I].left;
				else if (b > std::get<I>(curr->values))
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
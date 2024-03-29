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
			template <typename... Args>
			node(Args&&... args) :
				values(std::forward<Args>(args)...) {}
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
			bool operator==(const iterator<I>& r) {
				return curr == r.curr;
			}
			bool operator!=(const iterator<I>& r) {
				return curr != r.curr;
			}
		private:
			node* curr;
		};

	public:
		omni_map() : 
			m_roots{ nullptr } {}

		template <typename... Args>
		void emplace(Args&&... args) {
			node* n = m_nodepool.construct(std::forward<Args>(args)...);
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

		template <size_t I>
		void erase(iterator<I> it) {
			remove_node<0>(it.curr);
			m_nodepool.destroy(it.curr);
		}

		template <size_t I>
		iterator<I> begin() {
			node* curr = m_roots[I];
			while (curr->branches[I].left)
				curr = curr->branches[I].left;
			return iterator<I>(curr);
		}

		template <size_t I>
		iterator<I> end() {
			return iterator<I>();
		}

	private:
		template <size_t I>
		void insert_node(node* n) {
			node** curr = &m_roots[I];
			while (*curr) {
				if (std::get<I>(n->values) <= std::get<I>((*curr)->values))
					curr = &(*curr)->branches[I].left;
				else
					curr = &(*curr)->branches[I].right;
			}
			*curr = n;
			insert_node<I + 1>(n);
		}

		template <>
		void insert_node<num_element>(node* n) {}

		template <size_t I>
		void remove_node(node* n) {
			node** curr = &m_roots[I];
			while (*curr != n)
				if (std::get<I>(n->values) <= std::get<I>((*curr)->values))
					curr = &(*curr)->branches[I].left;
				else
					curr = &(*curr)->branches[I].right;
			if (n->branches[I].left)
				if (n->branches[I].right) {
					node* left = n->branches[I].left;
					node* max_left = n->branches[I].left;
					while (max_left->branches[I].right)
						max_left = max_left->branches[I].right;
					*curr = max_left;
					while (max_left->branches[I].left)
						max_left = max_left->branches[I].left;
					max_left->branches[I].left = left;
				}
				else
					*curr = n->branches[I].left;
			else
				if (n->branches[I].right)
					*curr = n->branches[I].right;
				else
					*curr = nullptr;
			remove_node<I + 1>(n);
		}

		template <>
		void remove_node<num_element>(node* n) {}

	private:
		mem_pool<node> m_nodepool;
		node* m_roots[num_element];
	};
}
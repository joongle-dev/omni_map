#pragma once
#include <vector>
#include "alignment.h"

namespace util
{
	template <typename T, std::size_t chunk_size = 256U, std::size_t alignment = 64U>
	class mem_pool
	{
		using storage_t = std::aligned_storage_t<sizeof(T), alignof(T)>;

	public:
		mem_pool() : freelist(nullptr) {}

		T* allocate() {
			if (freelist == nullptr) {
				storage_t* chunk = chunks.emplace_back(align::make_aligned<storage_t[], alignment>(chunk_size)).get();
				void** p = freelist = reinterpret_cast<void**>(chunk);
				for (std::size_t i = 1; i < chunk_size - 1; i++) {
					*p = chunk + i;
					p = reinterpret_cast<void**>(*p);
				}
				*p = nullptr;
			}
			T* allocated = reinterpret_cast<T*>(freelist);
			freelist = reinterpret_cast<void**>(*freelist);
			return allocated;
		}

		void deallocate(T* object) {
			*reinterpret_cast<void**>(object) = freelist;
			freelist = reinterpret_cast<void**>(object);
		}

		template <typename... Args>
		T* construct(Args&&... args) {
			return new(allocate()) T(std::forward<Args>(args)...);
		}

		void destroy(T* object) {
			object->~T();
			deallocate(object);
		}

	private:
		void** freelist;
		std::vector<align::aligned_ptr<storage_t[]>> chunks;
	};
}
#pragma once
#include <memory>
#include <type_traits>

namespace align
{
	inline void* aligned_alloc(size_t size, size_t alignment) {
		size_t space = size + alignment - 1;
		void* allocated_mem = ::operator new(space + sizeof(void*));
		void* aligned_mem = static_cast<void*>(static_cast<char*>(allocated_mem) + sizeof(void*));
		std::align(alignment, size, aligned_mem, space);
		*(static_cast<void**>(aligned_mem) - 1) = allocated_mem;
		return aligned_mem;
	}

	inline void aligned_free(void* p) {
		::operator delete(*(static_cast<void**>(p) - 1));
	}

	namespace detail
	{
		static constexpr size_t default_alignment = 64U;

		template <typename T>
		struct aligned_delete_impl {
			void operator()(T* data) const {
				data->~T();
				::operator delete(*(reinterpret_cast<void**>(data) - 1));
			}
		};
		template <typename T>
		struct aligned_delete_impl<T[]> {
			void operator()(T* data) const {
				::operator delete(*(reinterpret_cast<void**>(data) - 1));
			}
		};
		template <typename T, size_t alignment, typename... Args>
		struct aligned_new_impl {
			static T* aligned_new(Args&&... args) {
				size_t space = sizeof(T) + alignment - 1;
				void* allocated_mem = ::operator new(space + sizeof(void*));
				void* aligned_mem = static_cast<char*>(allocated_mem) + sizeof(void*);
				std::align(alignment, sizeof(T), aligned_mem, space);
				*(reinterpret_cast<void**>(aligned_mem) - 1) = allocated_mem;
				return new(aligned_mem) T(std::forward<Args>(args)...);
			}
		};
		template <typename T, size_t alignment>
		struct aligned_new_impl<T[], alignment> {
			static T* aligned_new(size_t num) {
				size_t space = sizeof(T) * num + alignment - 1;
				void* allocated_mem = ::operator new(space + sizeof(void*));
				void* aligned_mem = static_cast<char*>(allocated_mem) + sizeof(void*);
				std::align(alignment, sizeof(T), aligned_mem, space);
				*(reinterpret_cast<void**>(aligned_mem) - 1) = allocated_mem;
				return new(aligned_mem) T[num];
			}
		};
		/*
		template <typename T, std::size_t alignment>
		struct aligned_new_impl<T[], alignment> {
			static T* aligned_new(std::size_t num) {
				std::size_t size = sizeof(T) * num;
				std::size_t space = size + alignment - 1;
				void* allocated_mem = ::operator new(space + sizeof(void*) + sizeof(std::size_t));
				void* aligned_mem = static_cast<char*>(allocated_mem) + sizeof(void*) + sizeof(std::size_t);
				std::align(alignment, size, aligned_mem, space);
				new(aligned_mem) T[num];
				*(reinterpret_cast<void**>(aligned_mem) - 1) = allocated_mem;
				*(reinterpret_cast<std::size_t*>(aligned_mem) - 2) = num;
				return reinterpret_cast<T*>(aligned_mem);
			}
		};
		template <typename T>
		struct aligned_delete_impl<T[]> {
			void operator()(T* data) const {
				for (std::size_t i = 0; i < *(reinterpret_cast<std::size_t*>(data) - 2); i++)
					data[i].~T();
				::operator delete(*(reinterpret_cast<void**>(data) - 1));
			}
		};
		*/
	}

	template <typename T>
	using aligned_ptr = std::unique_ptr<T, detail::aligned_delete_impl<T>>;

	template <typename T, std::size_t alignment = detail::default_alignment, typename... Args>
	aligned_ptr<T> make_aligned(Args&&... args) {
		return aligned_ptr<T>(detail::aligned_new_impl<T, alignment>::aligned_new(std::forward<Args>(args)...));
	}
}
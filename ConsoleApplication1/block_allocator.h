#pragma once
#include "omni_map.h"

namespace util
{
	class block_allocator
	{
	public:
		uint16_t allocate(uint16_t size) {

		}

		void free(uint16_t offset, uint16_t size) {
			
		}

	private:
		omni_map<uint16_t, uint16_t> m_freeblocks;
	};
}
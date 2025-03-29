#ifndef __NOSTD_POOL_ALLOCATOR_H__
#define __NOSTD_POOL_ALLOCATOR_H__

#include "allocator.h"
#include "stack_linked_list.h"
#include "vector.h"

namespace nostd {

	/**
	 * Pool allocator.
	 * Allocates memory blocks with constant size.
	 */
	class pool_allocator final
	: public allocator
	{

		using node_type = stack_linked_list::node_t;

	public:

		/**
		 * Constructor
		 *
		 * @param[in] num_chunks Number of chunks per single buffer
		 */
		pool_allocator(size_type num_chunks) noexcept;

		/**
		 * Copy constructor
		 *
		 * @param[in] other Other allocator.
		 */
		pool_allocator(const pool_allocator& other) noexcept;

		/**
		 * Move constructor
		 *
		 * @param[in] other Other allocator.
		 */
		pool_allocator(pool_allocator&& other) noexcept;

		/**
		 * Destructor
		 */
		~pool_allocator();

		/**
		 * Allocates block of memory
		 * Note that allocation size should be constant
		 *
		 * @param[in] size Size of memory block
		 */
		ptr_type allocate(size_type size) noexcept(false) final;

		/**
		 * Releases block of memory that was allocated previously
		 *
		 * @param[in] ptr Pointer to block of memory
		 */
		void free(ptr_type ptr) noexcept final;

		/**
		 * Returns number of chunks
		 */
		size_type num_chunks() const noexcept;

	private:

		/**
		 * Disallow default constructor
		 */
		pool_allocator() = delete;

		byte_type* _allocate_buffer() noexcept(false);
		
		size_type num_chunks_;
		size_type chunk_size_;
#ifdef NOSTD_MEMORY_DEBUG
		size_type total_size_;
		size_type used_;
#endif
		stack_linked_list free_list_;
		vector<byte_type*> buffers_;
	};

} // namespace nostd

#endif
#ifndef __NOSTD_TEST_ALLOCATOR_H__
#define __NOSTD_TEST_ALLOCATOR_H__

#include "allocator.h"

namespace nostd {

	/**
	 * Defines test allocator.
	 * Useful for testing allocation/deallocation balance during tests.
	 */
	class test_allocator final
	: public allocator
	{
	public:

		/**
		 * Constructor
		 */
		test_allocator() noexcept;

		/**
		 * Allocates block of memory
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
		 * Returns current number of allocated blocks.
		 */
		size_type count() const noexcept;

	private:

		size_type count_; //!< number of allocated blocks
	};

} // namespace nostd

#endif
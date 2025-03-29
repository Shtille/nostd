#ifndef __NOSTD_DEFAULT_ALLOCATOR_H__
#define __NOSTD_DEFAULT_ALLOCATOR_H__

#include "allocator.h"

namespace nostd {

	/**
	 * Defines default allocator.
	 */
	class default_allocator final
	: public allocator
	{
	public:

		/**
		 * @brief      Gets the thread-local instance.
		 *
		 * @return     The instance.
		 */
		static default_allocator * get_instance();

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
	};

} // namespace nostd

#endif
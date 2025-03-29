#ifndef __NOSTD_ALLOCATOR_H__
#define __NOSTD_ALLOCATOR_H__

namespace nostd {

	/**
	 * Allocator class interface
	 */
	class allocator
	{
	public:

		using size_type = unsigned int;
		using byte_type = unsigned char;
		using ptr_type = void*;

		/**
		 * Constructor
		 */
		allocator() = default;

		/**
		 * Destructor
		 */
		virtual ~allocator() = default;

		/**
		 * Allocates block of memory
		 *
		 * @param[in] size Size of memory block
		 */
		virtual ptr_type allocate(size_type size) noexcept(false) = 0;

		/**
		 * Releases block of memory that was allocated previously
		 *
		 * @param[in] ptr Pointer to block of memory
		 */
		virtual void free(ptr_type ptr) noexcept = 0;
	};

} // namespace nostd

#endif
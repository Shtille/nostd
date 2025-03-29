#include <nostd/default_allocator.h>

namespace nostd {

	default_allocator * default_allocator::get_instance()
	{
		static thread_local default_allocator instance;
		return &instance;
	}
	allocator::ptr_type default_allocator::allocate(size_type size) noexcept(false)
	{
		return reinterpret_cast<ptr_type>(new byte_type[size]);
	}
	void default_allocator::free(ptr_type ptr) noexcept
	{
		delete[] reinterpret_cast<byte_type*>(ptr);
	}

} // namespace nostd
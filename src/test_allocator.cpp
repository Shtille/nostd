#include <nostd/test_allocator.h>

namespace nostd {

	test_allocator::test_allocator() noexcept
	: count_(0)
	{
	}
	allocator::ptr_type test_allocator::allocate(size_type size) noexcept(false)
	{
		++count_;
		return reinterpret_cast<ptr_type>(new byte_type[size]);
	}
	void test_allocator::free(ptr_type ptr) noexcept
	{
		--count_;
		delete[] reinterpret_cast<byte_type*>(ptr);
	}
	allocator::size_type test_allocator::count() const noexcept
	{
		return count_;
	}

} // namespace nostd
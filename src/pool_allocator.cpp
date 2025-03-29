#include <nostd/pool_allocator.h>

#include <cassert>
#include <stdexcept>

namespace nostd {

	pool_allocator::pool_allocator(size_type num_chunks) noexcept
	: num_chunks_(num_chunks)
	, chunk_size_(0)
#ifdef NOSTD_MEMORY_DEBUG
	, total_size_(0)
	, used_(0)
#endif
	, free_list_()
	, buffers_()
	{
	}
	pool_allocator::pool_allocator(const pool_allocator& other) noexcept
	: num_chunks_(other.num_chunks_)
	, chunk_size_(0)
#ifdef NOSTD_MEMORY_DEBUG
	, total_size_(0)
	, used_(0)
#endif
	, free_list_()
	, buffers_()
	{
	}
	pool_allocator::pool_allocator(pool_allocator&& other) noexcept
	: num_chunks_(other.num_chunks_)
	, chunk_size_(other.chunk_size_)
#ifdef NOSTD_MEMORY_DEBUG
	, total_size_(other.total_size_)
	, used_(other.used_)
#endif
	, free_list_(utility::move(other.free_list_))
	, buffers_(utility::move(other.buffers_))
	{
		other.chunk_size_ = 0;
#ifdef NOSTD_MEMORY_DEBUG
		other.total_size_ = 0;
		other.used_ = 0;
#endif
	}
	pool_allocator::~pool_allocator()
	{
		for (auto buffer : buffers_)
			delete[] buffer;
	}
	allocator::ptr_type pool_allocator::allocate(size_type size) noexcept(false)
	{
		assert((buffers_.empty() || sizeof(node_type) + size == chunk_size_) && "Allocated size should be constant");
		node_type * free_node = free_list_.pop();
		if (free_node == nullptr)
		{
			if (buffers_.empty())
			{
				// First time buffer allocation
				chunk_size_ = sizeof(node_type) + size;
			}
#ifdef NOSTD_MEMORY_DEBUG
			total_size_ += num_chunks_ * chunk_size_;
#endif
			// The pool allocator is full
			// Add a new pool
			byte_type* buffer = _allocate_buffer();
			// Create a linked-list with all free positions
			for (size_type i = 0; i < num_chunks_; ++i)
			{
				byte_type* node_ptr = buffer + i * chunk_size_;
				free_list_.push(reinterpret_cast<node_type*>(node_ptr));
			}
			free_node = free_list_.pop();
		}
#ifdef NOSTD_MEMORY_DEBUG
		used_ += chunk_size_;
#endif
		byte_type* address = reinterpret_cast<byte_type*>(free_node) + sizeof(node_type);
		return reinterpret_cast<ptr_type>(address);
	}
	void pool_allocator::free(ptr_type ptr) noexcept
	{
#ifdef NOSTD_MEMORY_DEBUG
		used_ -= chunk_size_;
#endif
		byte_type* node_ptr = reinterpret_cast<byte_type*>(ptr) - sizeof(node_type);
		free_list_.push(reinterpret_cast<node_type*>(node_ptr));
	}
	allocator::byte_type* pool_allocator::_allocate_buffer() noexcept(false)
	{
		size_type size_to_allocate = num_chunks_ * chunk_size_;
		byte_type* buffer = new byte_type[size_to_allocate];
		if (buffer == nullptr)
			throw std::bad_alloc();
		buffers_.push_back(buffer);
		return buffer;
	}
	allocator::size_type pool_allocator::num_chunks() const noexcept
	{
		return num_chunks_;
	}

} // namespace nostd
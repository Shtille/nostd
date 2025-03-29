#ifndef __NOSTD_VECTOR_H__
#define __NOSTD_VECTOR_H__

#include "default_allocator.h"
#include "utility.h"

#include <new>
#include <stdexcept>

namespace nostd {

	/**
	 * Defines array container (analog of std::vector).
	 * Move semantics should be defined for used type.
	 * If no allocator is provided, default new/delete allocation/deallocation routine is used.
	 */
	template <typename T>
	class vector {
	public:

		using size_type = allocator::size_type;

		/**
		 * Defines iterator class.
		 */
		class iterator {
			friend class vector;

			iterator() noexcept
			: ptr_(nullptr)
			{
			}
			iterator(T * ptr) noexcept
			: ptr_(ptr)
			{
			}
		public:
			iterator(const iterator& other) noexcept
			: ptr_(other.ptr_)
			{
			}
			iterator& operator =(const iterator& other) noexcept
			{
				ptr_ = other.ptr_;
				return *this;
			}
			bool operator ==(const iterator& other) noexcept
			{
				return ptr_ == other.ptr_;
			}
			bool operator !=(const iterator& other) noexcept
			{
				return ptr_ != other.ptr_;
			}
			iterator& operator ++() noexcept(false) // prefix increment
			{
				++ptr_;
				return *this;
			}
			iterator operator ++(int) noexcept(false) // postfix increment
			{
				iterator it(ptr_);
				++ptr_;
				return it;
			}
			iterator& operator --() noexcept(false) // prefix decrement
			{
				--ptr_;
				return *this;
			}
			iterator operator --(int) noexcept(false) // postfix decrement
			{
				iterator it(ptr_);
				--ptr_;
				return it;
			}
			T& operator *() noexcept(false)
			{
				return *ptr_;
			}
			const T& operator *() const noexcept(false)
			{
				return *ptr_;
			}
			T* operator ->() noexcept(false)
			{
				return ptr_;
			}
			const T* operator ->() const noexcept(false)
			{
				return ptr_;
			}
		private:
			T * ptr_;
		};

	public:

		/**
		 * Default constructor.
		 */
		vector() noexcept
		: buffer_(nullptr)
		, allocator_(default_allocator::get_instance())
		, buffer_size_(0U)
		, size_(0U)
		{
		}

		/**
		 * Constructor with allocator.
		 * 
		 * @param[in] alloc The allocator to be used to allocate nodes.
		 */
		vector(allocator * alloc) noexcept
		: buffer_(nullptr)
		, allocator_(alloc)
		, buffer_size_(0U)
		, size_(0U)
		{
		}

		/**
		 * Copy constructor.
		 * 
		 * @param[in] other The other array.
		 */
		vector(const vector& other) noexcept(false)
		: buffer_(nullptr)
		, allocator_(nullptr)
		, buffer_size_(0U)
		, size_(0U)
		{
			_set_by_copy(other);
		}

		/**
		 * Move constructor.
		 * 
		 * @param[in] other The other array.
		 */
		vector(vector && other) noexcept
		: buffer_(nullptr)
		, allocator_(nullptr)
		, buffer_size_(0U)
		, size_(0U)
		{
			_set_by_move(utility::forward<vector>(other));
		}

		/**
		 * Destructor.
		 */
		~vector()
		{
			_clean();
		}

		/**
		 * Copy assignment.
		 * 
		 * @param[in] other The other array.
		 */
		vector& operator =(const vector& other) noexcept(false)
		{
			_set_by_copy(other);
			return *this;
		}

		/**
		 * Move assignment.
		 * 
		 * @param[in] other The other array.
		 */
		vector& operator =(vector && other) noexcept
		{
			_set_by_move(utility::forward<vector>(other));
			return *this;
		}

		/**
		 * Accesses element at index.
		 * 
		 * @param[in] index  The index in the array.
		 * 
		 * @return Returns element at index.
		 */
		T& operator [](size_type index) noexcept
		{
			return buffer_[index];
		}

		/**
		 * Accesses element at index. Checks bounds.
		 * 
		 * @param[in] index  The index in the array.
		 * 
		 * @return Returns element at index.
		 */
		T& at(size_type index) noexcept(false)
		{
			if (!(0U <= index && index < size_))
				throw std::range_error("index out of range");
			return buffer_[index];
		}

		/**
		 * Checks if container is empty.
		 * 
		 * @return Returns true if empty and false otherwise.
		 */
		bool empty() const noexcept
		{
			return size_ == 0U;
		}

		/**
		 * Gets size of container.
		 * 
		 * @return Returns size of container.
		 */
		size_type size() const noexcept
		{
			return size_;
		}

		/**
		 * Gets capacity of container.
		 * 
		 * @return Returns capacity of container.
		 */
		size_type capacity() const noexcept
		{
			return buffer_size_;
		}

		/**
		 * Gets pointer to buffer.
		 * 
		 * @return Returns pointer to buffer.
		 */
		T* data() noexcept
		{
			return buffer_;
		}

		/**
		 * Gets pointer to buffer.
		 * 
		 * @return Returns pointer to buffer.
		 */
		const T* data() const noexcept
		{
			return buffer_;
		}

		/**
		 * Gets the first element in the array.
		 * 
		 * @return Returns the first element.
		 */
		T& front() noexcept(false)
		{
			if (0U == size_)
				throw std::range_error("container is empty");
			return buffer_[0U];
		}

		/**
		 * Gets the last element in the array.
		 * 
		 * @return Returns the last element.
		 */
		T& back() noexcept(false)
		{
			if (0U == size_)
				throw std::range_error("container is empty");
			return buffer_[size_ - 1];
		}

		/**
		 * Returns iterator to the first element.
		 * 
		 * @return Returns iterator to the first element.
		 */
		iterator begin() noexcept
		{
			return iterator(buffer_);
		}

		/**
		 * Returns iterator to the end.
		 * 
		 * @return Returns iterator to the end.
		 */
		iterator end() noexcept
		{
			return iterator(buffer_ + size_);
		}

		/**
		 * Clears all elements in the list
		 */
		void clear() noexcept
		{
			for (size_type i = 0U; i < size_; ++i)
				(buffer_ + i)->~T();
			size_ = 0U;
		}

		/**
		 * Reserves buffer for number of elements.
		 * 
		 * @param[in] size  The size to reserve.
		 */
		void reserve(size_type size) noexcept(false)
		{
			if (size == 0U)
			{
				if (buffer_ != nullptr)
				{
					_free_buffer(buffer_);
					buffer_ = nullptr;
				}
			}
			else if (buffer_size_ < size)
			{
				// Allocate a new buffer
				T * new_buffer = _allocate_buffer(size);
				if (new_buffer == nullptr)
					throw std::bad_alloc();
				if (buffer_ != nullptr)
				{
					// Call move constructor on old elements
					for (size_type i = 0U; i < size_; ++i)
					{
						new (new_buffer + i) T(utility::move(buffer_[i]));
					}
					// Free old buffer
					_free_buffer(buffer_);	
				}
				// Replace buffers
				buffer_ = new_buffer;
			}
			buffer_size_ = size;
		}

		/**
		 * Resizes array to the new size.
		 * 
		 * @param[in] new_size  The new size.
		 */
		void resize(size_type new_size) noexcept(false)
		{
			size_type old_size = size_;
			size_ = new_size;
			if (new_size <= old_size)
			{
				// Destruct old elements
				for (size_type i = new_size; i < old_size; ++i)
					(buffer_ + i)->~T();
			}
			else
			{
				// New size is greater than old one
				reserve(new_size + (new_size >> 2));
				// Copy default T into new elements.
				for (size_type i = old_size; i < new_size; ++i)
				{
					new (buffer_ + i) T();
				}
			}
		}

		/**
		 * Pushes data to the end of the array.
		 * Version that copies data.
		 * 
		 * @param[in] value The data.
		 */
		void push_back(const T& value) noexcept(false)
		{
			resize(size_ + 1);
			buffer_[size_ - 1] = value;
		}

		/**
		 * Pushes data to the end of the array.
		 * Version that moves data.
		 * 
		 * @param[in] value The data.
		 */
		void push_back(T && value) noexcept(false)
		{
			resize(size_ + 1);
			buffer_[size_ - 1] = utility::move(value);
		}

		/**
		 * Removes element from the end of the array.
		 * 
		 * @return True on success and false otherwise.
		 */
		void pop_back() noexcept(false)
		{
			if (size_ != 0U)
				resize(size_ - 1);
			else
				throw std::range_error("pop_back on empty container");
		}

		/**
		 * Swaps content with the other container.
		 * 
		 * @param[in] other  The other container.
		 */
		void swap(vector& other) noexcept
		{
			utility::swap(buffer_, other.buffer_);
			utility::swap(allocator_, other.allocator_);
			utility::swap(buffer_size_, other.buffer_size_);
			utility::swap(size_, other.size_);
		}

	private:

		T * _allocate_buffer(size_type size) noexcept(false)
		{
			return reinterpret_cast<T*>(allocator_->allocate(sizeof(T) * size));
		}
		void _free_buffer(T * buffer) noexcept
		{
			allocator_->free(reinterpret_cast<allocator::ptr_type>(buffer));
		}
		void _clean() noexcept
		{
			clear();
			if (buffer_ != nullptr)
			{
				_free_buffer(buffer_);
				buffer_ = nullptr;
			}
			buffer_size_ = 0U;
		}
		void _set_by_copy(const vector& other) noexcept(false)
		{
			_clean();
			allocator_ = other.allocator_;
			buffer_size_ = other.buffer_size_;
			if (other.buffer_size_ > 0U)
			{
				buffer_ = _allocate_buffer(other.buffer_size_);
				if (buffer_ == nullptr)
					throw std::bad_alloc();
			}
			else
				buffer_ = nullptr;
			size_ = other.size_;
			// Copy data
			for (size_type i = 0U; i < size_; ++i)
			{
				new (buffer_ + i) T(other.buffer_[i]);
			}
		}
		void _set_by_move(vector && other) noexcept
		{
			_clean();
			// Copy other fields
			buffer_ = other.buffer_;
			allocator_ = other.allocator_;
			buffer_size_ = other.buffer_size_;
			size_ = other.size_;
			// Nullify other
			other.buffer_ = nullptr;
			other.allocator_ = nullptr;
			other.buffer_size_ = 0U;
			other.size_ = 0U;
		}

		T * buffer_;
		allocator * allocator_;
		size_type buffer_size_;
		size_type size_;
	};

} // namespace nostd

#endif
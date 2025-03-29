#ifndef __NOSTD_STACK_H__
#define __NOSTD_STACK_H__

#include "default_allocator.h"
#include "utility.h"

#include <stdexcept>

namespace nostd {

	/**
	 * Defines stack container. Implemented as single linked list.
	 * Move semantics should be defined for used type.
	 * If no allocator is provided, default allocator's new/delete allocation/deallocation routine is used.
	 * Note: copy operations require additional allocations.
	 */
	template <typename T>
	class stack {

		/**
		 * Defines single list node that holds data.
		 */
		struct node_t {
			node_t * next;
			T data;
		};

	public:

		using size_type = allocator::size_type;

		/**
		 * Default constructor.
		 */
		stack() noexcept
		: head_(nullptr)
		, allocator_(default_allocator::get_instance())
		, size_(0U)
		{
		}

		/**
		 * Constructor with allocator.
		 * 
		 * @param[in] alloc The allocator to be used to allocate nodes.
		 */
		stack(allocator * alloc) noexcept
		: head_(nullptr)
		, allocator_(alloc)
		, size_(0U)
		{
		}

		/**
		 * Copy constructor.
		 * 
		 * @param[in] other The other stack.
		 */
		stack(const stack& other) noexcept(false)
		: head_(nullptr)
		, allocator_(nullptr)
		, size_(0U)
		{
			_set_by_copy(other);
		}

		/**
		 * Move constructor.
		 * 
		 * @param[in] other The other stack.
		 */
		stack(stack && other) noexcept
		: head_(nullptr)
		, allocator_(nullptr)
		, size_(0U)
		{
			_set_by_move(utility::forward(other));
		}

		/**
		 * Destructor.
		 */
		~stack()
		{
			_clean();
		}

		/**
		 * Copy assignment.
		 * 
		 * @param[in] other The other stack.
		 */
		stack& operator =(const stack& other) noexcept(false)
		{
			_set_by_copy(other);
			return *this;
		}

		/**
		 * Move assignment.
		 * 
		 * @param[in] other The other stack.
		 */
		stack& operator =(stack && other) noexcept
		{
			_set_by_move(utility::forward(other));
			return *this;
		}

		/**
		 * Gets the top element.
		 * 
		 * @return Returns reference to the top element.
		 */
		T& top() const noexcept(false)
		{
			if (head_ == nullptr)
				throw std::range_error("Calling top() on an empty container.");
			return head_->data;
		}

		/**
		 * Checks if stack is empty.
		 * 
		 * @return Returns true if empty and false otherwise.
		 */
		bool empty() const noexcept
		{
			return size_ == 0U;
		}

		/**
		 * Returns number of stack elements.
		 * 
		 * @return Returns stack size.
		 */
		size_type size() const noexcept
		{
			return size_;
		}

		/**
		 * Clears stack.
		 * Data move constructor is used for data destruction.
		 */
		void clear() noexcept
		{
			while (size_ != 0U)
			{
				T data(utility::move(top()));
				pop();
			}
		}

		/**
		 * Pushes data to the top of the stack.
		 * Version that copies data.
		 * 
		 * @param[in] data The data.
		 */
		void push(const T& data) noexcept(false)
		{
			node_t * node = _allocate_node();
			if (node == nullptr)
				throw std::bad_alloc();
			node->data = data;
			node->next = head_;
			head_ = node;
			++size_;
		}

		/**
		 * Pushes data to the top of the stack.
		 * Version that moves data.
		 * 
		 * @param[in] data The data.
		 */
		void push(T&& data) noexcept(false)
		{
			node_t * node = _allocate_node();
			if (node == nullptr)
				throw std::bad_alloc();
			node->data = utility::move(data);
			node->next = head_;
			head_ = node;
			++size_;
		}

		/**
		 * Removes element from the top of the stack.
		 */
		void pop() noexcept
		{
			node_t * node = head_;
			if (node)
			{
				head_ = node->next;
				--size_;
				_free_node(node);
			}
		}

		/**
		 * Swaps stack with other one.
		 * 
		 * @param[in] other The other stack.
		 */
		void swap(stack & other) noexcept
		{
			utility::swap(head_, other.head_);
			utility::swap(allocator_, other.allocator_);
			utility::swap(size_, other.size_);
		}

	private:

		node_t * _allocate_node() noexcept(false)
		{
			return reinterpret_cast<node_t*>(allocator_->allocate(sizeof(node_t)));
		}
		void _free_node(node_t * node) noexcept
		{
			allocator_->free(reinterpret_cast<allocator::ptr_type>(node));
		}
		void _clean() noexcept
		{
			clear();
		}
		void _set_by_copy(const stack& other) noexcept(false)
		{
			_clean();
			head_ = nullptr;
			allocator_ = other.allocator_;
			size_ = 0U;

			size_type other_size = other.size();
			if (other_size > 0U)
			{
				// Allocate storage for nodes
				node_t ** nodes = new node_t*[other_size];
				if (nodes == nullptr)
					throw std::bad_alloc();

				// Copy other stack nodes
				int i = 0;
				node_t * other_node = other.head_;
				while (other_node != nullptr)
				{
					nodes[i++] = other_node;
					other_node = other_node->next;
				}
				// Push data in reverse order to keep the same order
				for (i = other_size - 1; i >= 0; --i)
				{
					push_front(nodes[i]->data);
				}

				// Finally
				delete[] nodes;
			}
		}
		void _set_by_move(stack && other) noexcept
		{
			_clean();
			// Copy other fields
			head_ = other.head_;
			allocator_ = other.allocator_;
			size_ = other.size_;
			// Nullify other
			other.head_ = nullptr;
			other.allocator_ = nullptr;
			other.size_ = 0U;
		}

		node_t * head_;
		allocator * allocator_;
		size_type size_;
	};

} // namespace nostd

#endif
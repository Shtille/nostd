#ifndef __NOSTD_LIST_H__
#define __NOSTD_LIST_H__

#include "default_allocator.h"
#include "utility.h"

#include <stdexcept>

namespace nostd {

	/**
	 * Defines list container. Implemented as double linked list.
	 * Move semantics should be defined for used type.
	 * If no allocator is provided, default new/delete allocation/deallocation routine is used.
	 */
	template <typename T>
	class list {

		/**
		 * Defines single list node that holds data.
		 */
		struct node_t {
			node_t * prev;
			node_t * next;
			T data;
		};

		/**
		 * Defines predicate for matching data
		 */
		struct data_match_predicate {
			data_match_predicate(const T& data) noexcept
			: data_(data)
			{
			}
			bool operator()(const T& data) noexcept
			{
				return data_ == data;
			}
			const T& data_; // hold data reference
		};

	public:

		/**
		 * Defines iterator class.
		 */
		class iterator {
			friend class list;

			iterator() noexcept
			: node_(nullptr)
			{
			}
			iterator(node_t * node) noexcept
			: node_(node)
			{
			}
			void _check_node() noexcept(false)
			{
				if (node_ == nullptr)
					throw std::runtime_error("invalid iterator operation");
			}
		public:
			iterator(const iterator& other) noexcept
			: node_(other.node_)
			{
			}
			iterator& operator =(const iterator& other) noexcept
			{
				node_ = other.node_;
				return *this;
			}
			bool operator ==(const iterator& other) noexcept
			{
				return node_ == other.node_;
			}
			bool operator !=(const iterator& other) noexcept
			{
				return node_ != other.node_;
			}
			iterator& operator ++() noexcept(false) // prefix increment
			{
				_check_node();
				node_ = node_->next;
				return *this;
			}
			void operator ++(int) noexcept(false) // postfix increment
			{
				_check_node();
				node_ = node_->next;
			}
			iterator& operator --() noexcept(false) // prefix decrement
			{
				_check_node();
				node_ = node_->prev;
				return *this;
			}
			void operator --(int) noexcept(false) // postfix decrement
			{
				_check_node();
				node_ = node_->prev;
			}
			T& operator *() noexcept(false)
			{
				_check_node();
				return node_->data;
			}
			const T& operator *() const noexcept(false)
			{
				_check_node();
				return node_->data;
			}
			T& operator ->() noexcept(false)
			{
				_check_node();
				return node_->data;
			}
			const T& operator ->() const noexcept(false)
			{
				_check_node();
				return node_->data;
			}
		private:
			node_t * node_;
		};

	public:

		using size_type = allocator::size_type;

		/**
		 * Default constructor.
		 */
		list() noexcept
		: head_(nullptr)
		, tail_(nullptr)
		, allocator_(default_allocator::get_instance())
		, size_(0U)
		{
		}

		/**
		 * Constructor with allocator.
		 * 
		 * @param[in] alloc The allocator to be used to allocate nodes.
		 */
		list(allocator * alloc) noexcept
		: head_(nullptr)
		, tail_(nullptr)
		, allocator_(alloc)
		, size_(0U)
		{
		}

		/**
		 * Copy constructor.
		 * 
		 * @param[in] other The other list.
		 */
		list(const list& other) noexcept(false)
		: head_(nullptr)
		, tail_(nullptr)
		, allocator_(nullptr)
		, size_(0U)
		{
			_set_by_copy(other);
		}

		/**
		 * Move constructor.
		 * 
		 * @param[in] other The other list.
		 */
		list(list && other) noexcept
		: head_(nullptr)
		, tail_(nullptr)
		, allocator_(nullptr)
		, size_(0U)
		{
			_set_by_move(utility::forward(other));
		}

		/**
		 * Destructor.
		 */
		~list()
		{
			_clean();
		}

		/**
		 * Copy assignment.
		 * 
		 * @param[in] other The other list.
		 */
		list& operator =(const list& other) noexcept(false)
		{
			_set_by_copy(other);
			return *this;
		}

		/**
		 * Move assignment.
		 * 
		 * @param[in] other The other list.
		 */
		list& operator =(list && other) noexcept
		{
			_set_by_move(utility::forward(other));
			return *this;
		}

		/**
		 * Returns iterator to the first element.
		 * 
		 * @return Returns iterator to the first element.
		 */
		iterator begin() noexcept
		{
			return iterator(head_);
		}

		/**
		 * Returns iterator to the end.
		 * 
		 * @return Returns iterator to the end.
		 */
		static iterator end() noexcept
		{
			return iterator(nullptr);
		}

		/**
		 * Gets the first element.
		 * 
		 * @return Returns reference to the first element.
		 */
		T& front() const noexcept(false)
		{
			if (head_ == nullptr)
				throw std::range_error("Calling front() on an empty container.");
			return head_->data;
		}

		/**
		 * Gets the last element.
		 * 
		 * @return Returns reference to the last element.
		 */
		T& back() const noexcept(false)
		{
			if (tail_ == nullptr)
				throw std::range_error("Calling back() on an empty container.");
			return tail_->data;
		}

		/**
		 * Checks if list is empty.
		 * 
		 * @return Returns true if empty and false otherwise.
		 */
		bool empty() const noexcept
		{
			return size_ == 0U;
		}

		/**
		 * Returns number of list elements.
		 * 
		 * @return Returns list size.
		 */
		size_type size() const noexcept
		{
			return size_;
		}

		/**
		 * Returns number of list elements.
		 * 
		 * @return Returns list size.
		 */
		size_type length() const noexcept
		{
			return size_;
		}

		/**
		 * Clears list.
		 * Data move constructor is used for data destruction.
		 */
		void clear() noexcept
		{
			while (size_ != 0U)
			{
				T data(utility::move(front()));
				pop_front();
			}
		}

		/**
		 * Pushes data to the beginning of the list.
		 * Version that copies data.
		 * 
		 * @param[in] data The data.
		 */
		void push_front(const T& data) noexcept(false)
		{
			node_t * node = _allocate_node();
			if (node == nullptr)
				throw std::bad_alloc();
			node->data = data;
			node->prev = nullptr;
			node->next = head_;
			if (node->next)
				node->next->prev = node;
			head_ = node;
			if (tail_ == nullptr)
				tail_ = node;
			++size_;
		}

		/**
		 * Pushes data to the beginning of the list.
		 * Version that moves data.
		 * 
		 * @param[in] data The data.
		 */
		void push_front(T&& data) noexcept(false)
		{
			node_t * node = _allocate_node();
			if (node == nullptr)
				throw std::bad_alloc();
			node->data = utility::move(data);
			node->prev = nullptr;
			node->next = head_;
			if (node->next)
				node->next->prev = node;
			head_ = node;
			if (tail_ == nullptr)
				tail_ = node;
			++size_;
		}

		/**
		 * Pushes data to the end of the list.
		 * Version that copies data.
		 * 
		 * @param[in] data The data.
		 */
		void push_back(const T& data) noexcept(false)
		{
			node_t * node = _allocate_node();
			if (node == nullptr)
				throw std::bad_alloc();
			node->data = data;
			node->prev = tail_;
			node->next = nullptr;
			if (node->prev)
				node->prev->next = node;
			if (head_ == nullptr)
				head_ = node;
			tail_ = node;
			++size_;
		}

		/**
		 * Pushes data to the end of the list.
		 * Version that moves data.
		 * 
		 * @param[in] data The data.
		 */
		void push_back(T&& data) noexcept(false)
		{
			node_t * node = _allocate_node();
			if (node == nullptr)
				throw std::bad_alloc();
			node->data = utility::move(data);
			node->prev = tail_;
			node->next = nullptr;
			if (node->prev)
				node->prev->next = node;
			if (head_ == nullptr)
				head_ = node;
			tail_ = node;
			++size_;
		}

		/**
		 * Removes element from the beginning of the list.
		 */
		void pop_front() noexcept
		{
			node_t * node = head_;
			if (node)
			{
				head_ = node->next;
				if (head_)
					head_->prev = nullptr;
				if (tail_ == node)
					tail_ = nullptr;
				--size_;
				_free_node(node);
			}
		}

		/**
		 * Removes element from the end of the list.
		 */
		void pop_back() noexcept
		{
			node_t * node = tail_;
			if (node)
			{
				tail_ = node->prev;
				if (tail_)
					tail_->next = nullptr;
				if (head_ == node)
					head_ = nullptr;
				--size_;
				_free_node(node);
			}
		}

		/**
		 * Swaps list with other one.
		 * 
		 * @param[in] other The other list.
		 */
		void swap(list & other) noexcept
		{
			utility::swap(head_, other.head_);
			utility::swap(tail_, other.tail_);
			utility::swap(size_, other.size_);
			utility::swap(allocator_, other.allocator_);
		}

		/**
		 * Inserts element before the selected position.
		 * 
		 * @param[in] pos  The position to insert before.
		 * @param[in] data The data to insert.
		 */
		void insert(iterator pos, const T& data) noexcept(false)
		{
			if (pos.node_ == nullptr)
			{
				push_back(data);
				return;
			}

			node_t * node = _allocate_node();
			if (node == nullptr)
				throw std::bad_alloc();
			node->data = data;
			node->prev = pos.node_->prev;
			node->next = pos.node_;
			if (pos.node_->prev != nullptr)
				pos.node_->prev->next = node;
			pos.node_->prev = node;
			if (pos.node_ == head_)
				head_ = node;
			++size_;
		}

		/**
		 * Erases element at selected position.
		 * 
		 * @param[in] pos  The position to erase.
		 * 
		 * @return Returns iterator to the next element.
		 */
		iterator erase(iterator pos) noexcept
		{
			node_t * node = pos.node_;
			if (node == nullptr)
				return iterator(nullptr);

			iterator next_iterator(node->next);
			if (node->prev)
				node->prev->next = node->next;
			if (node->next)
				node->next->prev = node->prev;
			if (head_ == node)
				head_ = node->next;
			if (tail_ == node)
				tail_ = node->prev;
			_free_node(node);
			--size_;
			return next_iterator;
		}

		/**
		 * Removes all elements matching passed one.
		 * 
		 * @param[in] data The data to match with.
		 */
		void remove(const T& data) noexcept
		{
			data_match_predicate predicate(data);
			remove_if(predicate);
		}

		/**
		 * Removes all elements matching predicate.
		 * 
		 * @param[in] predicate The predicate to be matched with.
		 */
		template <class UnaryPredicate>
		void remove_if(UnaryPredicate predicate) noexcept
		{
			node_t * node = head_;
			while (node != nullptr)
			{
				if (predicate(node->data))
				{
					// Remove node
					iterator it = erase(iterator(node));
					node = it.node_;
				}
				else
					node = node->next;
			}
		}

		/**
		 * Finds first element in the list mathing data.
		 * 
		 * @param[in] data The data to match with.
		 * 
		 * @return Returns iterator to found element or iterator to the end otherwise.
		 */
		iterator find(const T& data) noexcept
		{
			data_match_predicate predicate(data);
			node_t * node = head_;
			while (node != nullptr)
			{
				if (predicate(node->data))
				{
					return iterator(node);
				}
				node = node->next;
			}
			return iterator(nullptr);
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
		void _set_by_copy(const list& other) noexcept(false)
		{
			_clean();
			head_ = nullptr;
			tail_ = nullptr;
			allocator_ = other.allocator_;
			size_ = 0U;

			// Copy other list nodes
			node_t * other_node = other.head_;
			while (other_node != nullptr)
			{
				push_back(other_node->data);
				other_node = other_node->next;
			}
		}
		void _set_by_move(list && other) noexcept
		{
			_clean();
			// Copy other fields
			head_ = other.head_;
			tail_ = other.tail_;
			allocator_ = other.allocator_;
			size_ = other.size_;
			// Nullify other
			other.head_ = nullptr;
			other.tail_ = nullptr;
			other.allocator_ = nullptr;
			other.size_ = 0U;
		}

		node_t * head_;
		node_t * tail_;
		allocator * allocator_;
		size_type size_;
	};

} // namespace nostd

#endif
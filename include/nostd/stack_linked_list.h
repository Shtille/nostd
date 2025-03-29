#ifndef __NOSTD_STACK_LINKED_LIST_H__
#define __NOSTD_STACK_LINKED_LIST_H__

namespace nostd {

	/**
	 * Utilitary class for allocators.
	 * Represented as single linked list.
	 */
	class stack_linked_list
	{
	public:

		/**
		 * Structure that represents single node of list
		 */
		struct node_t {
			node_t * next;
		};

		/**
		 * Constructor
		 */
		stack_linked_list() noexcept
		: head_(nullptr)
		{
		}

		/**
		 * Move constructor
		 */
		stack_linked_list(stack_linked_list&& other) noexcept
		: head_(other.head_)
		{
			other.head_ = nullptr;
		}

		/**
		 * Pushes new node to top of the list
		 *
		 * @param[in] new_node A new node
		 */
		void push(node_t * new_node) noexcept
		{
			new_node->next = head_;
			head_ = new_node;
		}

		/**
		 * Pops top element from the list
		 *
		 * @return A node at top
		 */
		node_t * pop() noexcept
		{
			node_t * top = head_;
			if (head_)
				head_ = head_->next;
			return top;
		}

	private:

		/**
		 * Disallow copy
		 */
		stack_linked_list(const stack_linked_list&) = delete;

		node_t * head_;
	};

} // namespace nostd

#endif
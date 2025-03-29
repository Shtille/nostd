#ifndef __NOSTD_MAP_H__
#define __NOSTD_MAP_H__

#include "default_allocator.h"
#include "utility.h"

#include <stdexcept>

namespace nostd {

	/**
	 * Defines map container. Implemented as red-black tree.
	 * Move semantics should be defined for used type.
	 * If no allocator is provided, default new/delete allocation/deallocation routine is used.
	 * PoolAllocator is the best solution for custom allocator.
	 * @see PoolAllocator
	 */
	template <typename Key, typename T>
	class map {
	public:

		using pair_type = utility::pair<Key, T>;
		using size_type = allocator::size_type;

	private:
		/**
		 * Defines single tree node that holds data.
		 */
		struct node_t {
			node_t * parent;
			node_t * left;
			node_t * right;
			bool red; // if red = false then the node is black
			pair_type data; // key value pair

			node_t(node_t * node) noexcept
			: parent(node)
			, left(node)
			, right(node)
			, red(false)
			, data()
			{
			}
		};

	public:

		/**
		 * Defines iterator class.
		 */
		class iterator {
			friend class map;

			iterator(map const * map, node_t * node) noexcept
			: map_(map)
			, node_(node)
			{
			}
			node_t * _next(node_t * prev) noexcept
			{
				node_t * x = prev;
				node_t * y;

				if (map_->nil_ != (y = x->right)) { /* assignment to y is intentional */
					while (y->left != map_->nil_) { /* returns the minium of the right subtree of x */
						y = y->left;
					}
					return y;
				} else {
					y = x->parent;
					while (y != map_->nil_ && x == y->right) { /* sentinel used instead of checking for nil */
						x = y;
						y = y->parent;
					}
					if (y == map_->root_)
						return map_->nil_;
					return y;
				}
			}
			void _check_node() noexcept(false)
			{
				if (node_ == nullptr)
					throw std::runtime_error("invalid iterator operation");
			}
		public:
			iterator(const iterator& other) noexcept
			: map_(other.map_)
			, node_(other.node_)
			{
			}
			iterator& operator =(const iterator& other) noexcept
			{
				map_ = other.map_;
				node_ = other.node_;
				return *this;
			}
			bool operator ==(const iterator& other) const noexcept
			{
				return map_ == other.map_ && node_ == other.node_;
			}
			bool operator !=(const iterator& other) const noexcept
			{
				return map_ != other.map_ || node_ != other.node_;
			}
			iterator& operator ++() noexcept(false) // prefix increment
			{
				_check_node();
				node_ = _next(node_);
				return *this;
			}
			iterator operator ++(int) noexcept(false) // postfix increment
			{
				_check_node();
				iterator it(*this);
				node_ = _next(node_);
				return it;
			}
			pair_type& operator *() noexcept(false)
			{
				_check_node();
				return node_->data;
			}
			const pair_type& operator *() const noexcept(false)
			{
				_check_node();
				return node_->data;
			}
			pair_type& operator ->() noexcept(false)
			{
				_check_node();
				return node_->data;
			}
			const pair_type& operator ->() const noexcept(false)
			{
				_check_node();
				return node_->data;
			}
		private:
			map const * map_;
			node_t * node_;
		};

	public:

		/**
		 * Default constructor.
		 */
		map() noexcept(false)
		: nil_(nullptr)
		, root_(nullptr)
		, allocator_(default_allocator::get_instance())
		, size_(0U)
		{
			nil_ = _make_nil_node();
			root_ = _make_root_node();
		}

		/**
		 * Constructor with allocator.
		 * 
		 * @param[in] alloc The allocator to be used to allocate nodes.
		 */
		map(allocator * alloc) noexcept(false)
		: nil_(nullptr)
		, root_(nullptr)
		, allocator_(alloc)
		, size_(0U)
		{
			nil_ = _make_nil_node();
			root_ = _make_root_node();
		}

		/**
		 * Copy constructor.
		 * If other does not own allocator both will share it.
		 * 
		 * @param[in] other The other map.
		 */
		map(const map& other) noexcept(false)
		: nil_(nullptr)
		, root_(nullptr)
		, allocator_(nullptr)
		, size_(0U)
		{
			_set_by_copy(other);
		}

		/**
		 * Move constructor.
		 * 
		 * @param[in] other The other map.
		 */
		map(map && other) noexcept
		: nil_(nullptr)
		, root_(nullptr)
		, allocator_(nullptr)
		, size_(0U)
		{
			_set_by_move(utility::forward(other));
		}

		/**
		 * Destructor.
		 */
		~map()
		{
			_clean();
		}

		/**
		 * Copy assignment.
		 * 
		 * @param[in] other The other map.
		 */
		map& operator =(const map& other) noexcept(false)
		{
			_set_by_copy(other);
			return *this;
		}

		/**
		 * Move assignment.
		 * 
		 * @param[in] other The other map.
		 */
		map& operator =(map && other) noexcept
		{
			_set_by_move(utility::forward(other));
			return *this;
		}

		/**
		 * Value access by key.
		 * 
		 * @param[in] key  The key.
		 * 
		 * @return Returns reference to found value.
		 */
		T& operator [](const Key& key) noexcept(false)
		{
			node_t * existing = _search(key);
			if (existing != nil_)
				return existing->data.second;

			node_t * x;
			node_t * new_node;

			x = _allocate_node();
			x->data = pair_type(key, T());

			new_node = _insert(x);
			return new_node->data.second;
		}

		/**
		 * Checks if map is empty.
		 * 
		 * @return Returns true if empty and false otherwise.
		 */
		bool empty() const noexcept
		{
			return size_ == 0U;
		}

		/**
		 * Returns number of map elements.
		 * 
		 * @return Returns map size.
		 */
		size_type size() const noexcept
		{
			return size_;
		}

		/**
		 * Clears map.
		 * Data move constructor is used for data destruction.
		 */
		void clear() noexcept
		{
			_destroy_helper(root_->left);
			root_->left = nil_;
			size_ = 0U;
		}

		/**
		 * Returns iterator to the first element.
		 * 
		 * @return Returns iterator to the first element.
		 */
		iterator begin() noexcept
		{
			node_t * x = root_->left;
			if (x != nil_)
			{
				while (x->left != nil_)
					x = x->left;
				return iterator(this, x);
			}
			else
				return end();
		}

		/**
		 * Returns iterator to the end.
		 * 
		 * @return Returns iterator to the end.
		 */
		iterator end() noexcept
		{
			return iterator(this, nil_);
		}

		/**
		 * Inserts element in the map.
		 * Version that copies data.
		 * If find call precedes insert call then you should prefer
		 * trusted_insert over regular insert call for performance reason.
		 * @see trusted_insert
		 * 
		 * @param[in] value The value.
		 * 
		 * @return Returns a pair consisting of an iterator to the inserted element 
		 *         (or to the element that prevented the insertion) and 
		 *         a bool value map to true if the insertion took place.
		 */
		utility::pair<iterator, bool> insert(const pair_type& value) noexcept(false)
		{
			node_t * existing = _search(value.first);
			if (existing != nil_)
				return utility::pair<iterator, bool>(iterator(this, existing), false);

			node_t * x;
			node_t * new_node;

			x = _allocate_node();
			x->data = value;

			new_node = _insert(x);
			return utility::pair<iterator, bool>(iterator(this, new_node), true);
		}

		/**
		 * Inserts element in the map.
		 * Version that moves data.
		 * If find call precedes insert call then you should prefer
		 * trusted_insert over regular insert call for performance reason.
		 * @see trusted_insert
		 * 
		 * @param[in] value The value.
		 * 
		 * @return Returns a pair consisting of an iterator to the inserted element 
		 *         (or to the element that prevented the insertion) and 
		 *         a bool value map to true if the insertion took place.
		 */
		utility::pair<iterator, bool> insert(pair_type && value) noexcept(false)
		{
			node_t * existing = _search(value.first);
			if (existing != nil_)
				return utility::pair<iterator, bool>(iterator(this, existing), false);

			node_t * x;
			node_t * new_node;

			x = _allocate_node();
			x->data = utility::move(value);

			new_node = _insert(x);
			return utility::pair<iterator, bool>(iterator(this, new_node), true);
		}

		/**
		 * Inserts element in the map.
		 * Version that copies data.
		 * It's guaranteed that element is not in the map (mostly had find call previously).
		 * 
		 * @param[in] value The value.
		 * 
		 * @return Returns an iterator to the inserted element.
		 */
		iterator trusted_insert(const pair_type& value) noexcept(false)
		{
			node_t * x;
			node_t * new_node;

			x = _allocate_node();
			x->data = value;

			new_node = _insert(x);
			return iterator(this, new_node);
		}

		/**
		 * Inserts element in the map.
		 * Version that moves data.
		 * It's guaranteed that element is not in the map (mostly had find call previously).
		 * 
		 * @param[in] value The value.
		 * 
		 * @return Returns an iterator to the inserted element.
		 */
		iterator trusted_insert(pair_type && value) noexcept(false)
		{
			node_t * x;
			node_t * new_node;

			x = _allocate_node();
			x->data = utility::move(value);

			new_node = _insert(x);
			return iterator(this, new_node);
		}

		/**
		 * Finds element in the map.
		 * 
		 * @param[in] key The key.
		 * 
		 * @return Returns an iterator to the found element or end iterator.
		 */
		iterator find(const Key& key) noexcept
		{
			node_t * node = _search(key);
			if (node != nil_)
				return iterator(this, node);
			else
				return end();
		}

		/**
		 * Removes element from the map.
		 * 
		 * @param[in] pos The iterator to the element to remove.
		 */
		void erase(iterator pos) noexcept(false)
		{
			if (pos.node_ == nil_)
				throw std::runtime_error("trying to erase nil node");
			_delete(pos.node_);
		}

		/**
		 * Removes all elements with key from the map.
		 * 
		 * @param[in] key The key to match removable elements.
		 * 
		 * @return Number of elements removed (0 or 1).
		 */
		size_type erase(const Key& key) noexcept
		{
			size_type count = 0U;
			node_t * node;
			for (iterator it = begin(); it != end(); ++it)
			{
				node = it.node_;
				if (node->data.first == key)
				{
					_delete(node);
					++count;
				}
			}
			return count;
		}

		/**
		 * Swaps map with other one.
		 * 
		 * @param[in] other The other map.
		 */
		void swap(map & other) noexcept
		{
			utility::swap(nil_, other.nil_);
			utility::swap(root_, other.root_);
			utility::swap(size_, other.size_);
			utility::swap(allocator_, other.allocator_);
		}

	private: // Helpers

		void _left_rotate(node_t * x) noexcept
		{
			node_t * y;

			y = x->right;
			x->right = y->left;

			if (y->left != nil_) y->left->parent = x; /* used to use sentinel here */
			/* and do an unconditional assignment instead of testing for nil */

			y->parent = x->parent;

			/* instead of checking if x->parent is the root as in the book, we */
			/* count on the root sentinel to implicitly take care of this case */
			if (x == x->parent->left) {
				x->parent->left = y;
			} else {
				x->parent->right = y;
			}
			y->left = x;
			x->parent = y;
		}

		void _right_rotate(node_t * y) noexcept
		{
			node_t * x;

			x = y->left;
			y->left = x->right;

			if (nil_ != x->right) x->right->parent = y; /*used to use sentinel here */
			/* and do an unconditional assignment instead of testing for nil */

			/* instead of checking if x->parent is the root as in the book, we */
			/* count on the root sentinel to implicitly take care of this case */
			x->parent = y->parent;
			if (y == y->parent->left) {
				y->parent->left = x;
			} else {
				y->parent->right = x;
			}
			x->right = y;
			y->parent = x;
		}

		node_t * _search(const Key& key) noexcept
		{
			node_t * x = root_->left;
			if (x == nil_)
				return nil_;
			while (!(key == x->data.first)) {
				if (key < x->data.first)
					x = x->left;
				else
					x = x->right;
				if (x == nil_)
					return nil_;
			}
			return x;
		}

		void _insert_help(node_t * z) noexcept
		{
			node_t * x;
			node_t * y;

			z->left = z->right = nil_;
			y = root_;
			x = root_->left;
			while (x != nil_) {
				y = x;
				if (z->data.first < x->data.first) { /* x.data > z.data */
					x = x->left;
				} else { /* x.data <= z.data */
					x = x->right;
				}
			}
			z->parent = y;
			if ((y == root_) ||
				(z->data.first < y->data.first)) { /* y.data > z.data */
				y->left = z;
			} else {
				y->right = z;
			}
		}

		node_t * _insert(node_t * x) noexcept
		{
			node_t * y;
			node_t * new_node;

			_insert_help(x);
			new_node = x;
			x->red = true;
			while (x->parent->red)
			{ /* use sentinel instead of checking for root */
				if (x->parent == x->parent->parent->left)
				{
					y = x->parent->parent->right;
					if (y->red)
					{
						x->parent->red = false;
						y->red = false;
						x->parent->parent->red = true;
						x = x->parent->parent;
					}
					else
					{
						if (x == x->parent->right)
						{
							x = x->parent;
							_left_rotate(x);
						}
						x->parent->red = false;
						x->parent->parent->red = true;
						_right_rotate(x->parent->parent);
					}
				}
				else
				{ /* case for x->parent == x->parent->parent->right */
					y = x->parent->parent->left;
					if (y->red)
					{
						x->parent->red = false;
						y->red = false;
						x->parent->parent->red = true;
						x = x->parent->parent;
					}
					else
					{
						if (x == x->parent->left)
						{
							x = x->parent;
							_right_rotate(x);
						}
						x->parent->red = false;
						x->parent->parent->red = true;
						_left_rotate(x->parent->parent);
					}
				}
			}
			root_->left->red = false;
			++size_;
			return new_node;
		}

		void _destroy_helper(node_t * x) noexcept
		{
			if (x != nil_) {
				_destroy_helper(x->left);
				_destroy_helper(x->right);
				// Destroy data
				x->data.~pair_type(); // or: T data(utility::move(x->data));
				_free_node(x);
			}
		}

		/* Performs rotations and changes colors to restore red-black properties after a node is deleted */
		void _delete_fix_up(node_t * x) noexcept
		{
			node_t * root = root_->left;
			node_t * w;

			while ((!x->red) && (root != x))
			{
				if (x == x->parent->left)
				{
					w = x->parent->right;
					if (w->red)
					{
						w->red = false;
						x->parent->red = true;
						_left_rotate(x->parent);
						w = x->parent->right;
					}
					if ((!w->right->red) && (!w->left->red) )
					{
						w->red = true;
						x = x->parent;
					} else {
						if (!w->right->red) {
							w->left->red = false;
							w->red = true;
							_right_rotate(w);
							w = x->parent->right;
						}
						w->red = x->parent->red;
						x->parent->red = false;
						w->right->red = false;
						_left_rotate(x->parent);
						x = root; /* this is to exit while loop */
					}
				} else { /* the code below is has left and right switched from above */
					w = x->parent->left;
					if (w->red) {
						w->red = false;
						x->parent->red = true;
						_right_rotate(x->parent);
						w = x->parent->left;
					}
					if ( (!w->right->red) && (!w->left->red) ) { 
						w->red = true;
						x = x->parent;
					} else {
						if (!w->left->red) {
							w->right->red = false;
							w->red = true;
							_left_rotate(w);
							w = x->parent->left;
						}
						w->red = x->parent->red;
						x->parent->red = false;
						w->left->red = false;
						_right_rotate(x->parent);
						x = root; /* this is to exit while loop */
					}
				}
			}
			x->red = false;
		}

		node_t * _tree_successor(node_t * x) noexcept
		{
			node_t * y;

			if (nil_ != (y = x->right)) { /* assignment to y is intentional */
				while (y->left != nil_) { /* returns the minium of the right subtree of x */
					y = y->left;
				}
				return y;
			} else {
				y = x->parent;
				while (x == y->right) { /* sentinel used instead of checking for nil */
					x = y;
					y = y->parent;
				}
				if (y == root_)
					return nil_;
				return y;
			}
		}

		void _delete(node_t * z) noexcept
		{
			node_t * y;
			node_t * x;
			node_t * root = root_;

			y = ((z->left == nil_) || (z->right == nil_)) ? z : _tree_successor(z);
			x = (y->left == nil_) ? y->right : y->left;
			if (root == (x->parent = y->parent)) { /* assignment of y->p to x->p is intentional */
				root->left = x;
			} else {
				if (y == y->parent->left) {
					y->parent->left = x;
				} else {
					y->parent->right = x;
				}
			}
			if (y != z) { /* y should not be nil in this case */

				/* y is the node to splice out and x is its child */
				if (!(y->red))
					_delete_fix_up(x);

				z->data.~pair_type(); // call destructor
				y->left = z->left;
				y->right = z->right;
				y->parent = z->parent;
				y->red = z->red;
				z->left->parent = z->right->parent = y;
				if (z == z->parent->left) {
					z->parent->left = y; 
				} else {
					z->parent->right = y;
				}
				_free_node(z);
			} else {
				y->data.~pair_type(); // call destructor
				if (!(y->red))
					_delete_fix_up(x);
				_free_node(y);
			}
			--size_;
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
		node_t * _make_nil_node() noexcept(false)
		{
			node_t * node = _allocate_node();
			new (node) node_t(node);
			return node;
		}
		node_t * _make_root_node() noexcept(false)
		{
			// NIL node should be initialized first
			node_t * node = _allocate_node();
			new (node) node_t(nil_);
			return node;
		}
		void _clean() noexcept
		{
			if (root_ != nullptr)
			{
				_destroy_helper(root_->left);
				_free_node(root_);
				root_ = nullptr;
			}
			if (nil_ != nullptr)
			{
				_free_node(nil_);
				nil_ = nullptr;
			}
		}
		void _set_by_copy(const map& other) noexcept(false)
		{
			// Clean old data
			_clean();

			// map values
			nil_ = _make_nil_node();
			root_ = _make_root_node();
			allocator_ = other.allocator_;
			size_ = 0U;

			// Copy other map nodes
			node_t * x;
			for (iterator it = other.begin(); it != other.end(); ++it)
			{
				x = _allocate_node();
				x->data = it.node_->data;
				(void)_insert(x);
			}
		}
		void _set_by_move(map && other) noexcept
		{
			// Clean old data
			_clean();
			// Copy other fields
			nil_ = other.nil_;
			root_ = other.root_;
			allocator_ = other.allocator_;
			size_ = other.size_;
			// Nullify other
			other.nil_ = nullptr;
			other.root_ = nullptr;
			other.allocator_ = nullptr;
			other.size_ = 0U;
		}

		node_t * nil_;
		node_t * root_;
		allocator * allocator_;
		size_type size_;
	};

} // namespace nostd

#endif
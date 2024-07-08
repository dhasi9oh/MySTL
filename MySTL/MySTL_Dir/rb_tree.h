#ifndef MYSTL_RB_TREE_H
#define MYSTL_RB_TREE_H

#include <initializer_list>
#include <cassert>

#include "functional.h"
#include "iterator.h"
#include "memory.h"
#include "type_traits.h"
#include "exceptdef.h"

namespace mystl {

	using rb_tree_color_type = bool;

	static constexpr rb_tree_color_type rb_tree_red = false;
	static constexpr rb_tree_color_type rb_tree_black = true;

	template <class T> struct rb_tree_node_base;
	template <class T> struct rb_tree_node;

	template <class T> struct rb_tree_iterator;
	template <class T> struct rb_tree_const_iterator;

	template <class T, bool>
	struct rb_tree_value_traits_imp
	{
		using key_type = T;
		using value_type = T;
		using mapped_type = T;

		// 这里的 static 是指这个函数是属于类的，不是修饰返回类型的
		template <class Tp>
		static const key_type& get_key(const Tp& value)
		{
			return value;
		}

		template <class Tp>
		static const value_type& get_value(const Tp& value)
		{
			return value;
		}

	};

	template <class T>
	struct rb_tree_value_traits_imp<T, true>
	{
		using key_type = typename std::remove_cv<typename T::first_type>::type;
		using mapped_type = typename T::second_type;
		using value_type = T;

		// 这里的 static 是指这个函数是属于类的，不是修饰返回类型的
		template <class Tp>
		static const key_type& get_key(const Tp& value)
		{
			return value;
		}

		template <class Tp>
		static const value_type& get_value(const Tp& value)
		{
			return value;
		}

	};

	template <class T>
	struct rb_tree_value_traits
	{
		static constexpr bool is_map = mystl::is_pair<T>::value;
		using value_traits_type = rb_tree_value_traits_imp<T, is_map>;

		using key_type = typename value_traits_type::key_type;
		using value_type = typename value_traits_type::value_type;
		using mapped_type = typename value_traits_type::mapped_type;

		template <class Tp>
		static const key_type& get_key(const Tp& value)
		{
			return value_traits_type::get_key(value);
		}

		template <class Tp>
		static const value_type& get_value(const Tp& value)
		{
			return value_traits_type::get_value(value);
		}
	};

	template <class T>
	struct rb_tree_node_traits
	{
		using color_type = rb_tree_color_type;
		using value_traits = rb_tree_value_traits<T>;
		using key_type = typename value_traits::key_type;
		using value_type = typename value_traits::value_type;
		using mapped_type = typename value_traits::mapped_type;

		using base_ptr = rb_tree_node_base<T>*;
		using node_ptr = rb_tree_node<T>*;
	};

	template <class T>
	struct rb_tree_node_base
	{
		using color_type = rb_tree_color_type;
		using base_ptr = rb_tree_node_base<T>*;
		using node_ptr = rb_tree_node<T>*;

		base_ptr parent, left, right;
		color_type color;

		base_ptr get_base_ptr()
		{
			return &(*this);
		}

		node_ptr get_node_ptr()
		{
			return static_cast<node_ptr>(&(*this));
		}

		node_ptr& get_node_ref()
		{
			return static_cast<node_ptr>(&(*this));
		}
	};

	template <class T>
	struct rb_tree_node : public rb_tree_node_base<T>
	{
		using base_ptr = rb_tree_node_base<T>*;
		using node_ptr = rb_tree_node<T>*;

		T value;

		base_ptr get_base_ptr()
		{
			return static_cast<base_ptr>(&(*this));
		}

		node_ptr get_node_ptr()
		{
			return &(*this);
		}

	};

	template <class T>
	struct rb_tree_traits
	{
		using value_traits = rb_tree_value_traits<T>;
		using key_type = typename value_traits::key_type;
		using mapped_type = typename value_traits::mapped_type;
		using value_type = typename value_traits::value_type;

		using pointer = value_type*;
		using reference = value_type&;
		using const_pointer = const value_type*;
		using const_reference = const value_type&;

		using base_type = rb_tree_node_base<T>;
		using node_type = rb_tree_node<T>;

		using base_ptr = rb_tree_node_base<T>*;
		using node_ptr = rb_tree_node<T>*;
	};

	template <class T>
	struct rb_tree_iterator_base : public mystl::iterator<mystl::bidirectional_iterator_tag, T>
	{
		using base_ptr = typename rb_tree_traits<T>::base_ptr;

		base_ptr node;

		rb_tree_iterator_base() : node(nullptr) {}

		void inc()
		{
			if (node->right != nullptr) {
				node = rb_tree_min(node->right);
			}
			else {
				base_ptr parent = node->parent;
				while (parent->right == node) {
					node = parent;
					parent = node->parent;
				}
				if (parent->right != node) {
					node = parent;
				}
			}
		}

		void dec()
		{
			if (node->parent->parent == node && rb_tree_is_red(node)) {
				node = node->right;
			}
			else if (node->left != nullptr) {
				node = rb_tree_max(node->left);
			}
			else {
				base_ptr parent = node->parent;
				while (parent->left == node) {
					node = parent;
					parent = node->parent;
				}
				if (parent->left != node) {
					node = parent;
				}
			}
		}
	};

	template <class T>
	struct rb_tree_iterator : public rb_tree_iterator_base<T>
	{
		using tree_traits = rb_tree_traits<T>;

		using value_type = typename tree_traits::value_type;
		using pointer = typename tree_traits::pointer;
		using reference = typename tree_traits::reference;
		using base_ptr = typename tree_traits::base_ptr;
		using node_ptr = typename tree_traits::node_ptr;

		using iterator = rb_tree_iterator<T>;
		using const_iterator = rb_tree_const_iterator<T>;
		using self = iterator;

		using rb_tree_iterator_base<T>::node;

		rb_tree_iterator() { node = nullptr; }
		rb_tree_iterator(base_ptr x) { node = x; }
		rb_tree_iterator(node_ptr x) { node = x->get_base_ptr(); }
		rb_tree_iterator(const iterator& x) { node = x.node; }
		rb_tree_iterator(const const_iterator& x) { node = x.node; }

		reference operator*()
		{
			return node->get_node_ptr()->value;
		}

		pointer operator->()
		{
			return &operator*();
		}

		self& operator++()
		{
			inc();
			return *this;
		}

		self operator++(int)
		{
			iterator tmp(*this);
			++(*this);
			return tmp;
		}

		self& operator--()
		{
			dec();
			return *this;
		}

		self operator--(int)
		{
			iterator tmp(*this);
			--(*this);
			return tmp;
		}

	};

	template <class T>
	struct rb_tree_const_iterator : public rb_tree_iterator_base<T>
	{
		using tree_traits = rb_tree_traits<T>;

		using value_type = typename tree_traits::value_type;
		using pointer = typename tree_traits::const_pointer;
		using reference = typename tree_traits::const_reference;
		using base_ptr = typename tree_traits::base_ptr;
		using node_ptr = typename tree_traits::node_ptr;

		using iterator = rb_tree_iterator<T>;
		using const_iterator = rb_tree_const_iterator<T>;
		using self = iterator;

		using rb_tree_iterator_base<T>::node;

		rb_tree_const_iterator() { node = nullptr; }
		rb_tree_const_iterator(base_ptr x) { node = x; }
		rb_tree_const_iterator(node_ptr x) { node = x->get_base_ptr(); }
		rb_tree_const_iterator(const iterator& x) { node = x.node; }
		rb_tree_const_iterator(const const_iterator& x) { node = x.node; }

		reference operator*() const
		{
			return node->get_node_ptr()->value;
		}

		pointer operator->() const
		{
			return &operator*();
		}

		self& operator++()
		{
			inc();
			return *this;
		}

		self operator++(int)
		{
			iterator tmp(*this);
			++(*this);
			return tmp;
		}

		self& operator--()
		{
			dec();
			return *this;
		}

		self operator--(int)
		{
			iterator tmp(*this);
			--(*this);
			return tmp;
		}

	};

	template <class NodePtr>
	NodePtr rb_tree_min(NodePtr node) noexcept
	{
		while (node->left != nullptr) {
			node = node->left;
		}
		return node;
	}

	template <class NodePtr>
	NodePtr rb_tree_max(NodePtr node) noexcept
	{
		while (node->right != nullptr) {
			node = node->right;
		}
		return node;
	}

	template <class NodePtr>
	bool rb_tree_is_lchild(NodePtr node) noexcept
	{
		return node == node->parent->left;
	}

	template <class NodePtr>
	bool rb_tree_is_rchild(NodePtr node) noexcept
	{
		return node == node->parent->right;
	}

	template <class NodePtr>
	bool rb_tree_is_red(NodePtr node) noexcept
	{
		return rb_tree_red == node->color;
	}

	template <class NodePtr>
	bool rb_tree_is_black(NodePtr node) noexcept
	{
		return rb_tree_black == node->color;
	}

	template <class NodePtr>
	void rb_tree_set_red(NodePtr node) noexcept
	{
		node->color = rb_tree_red;
	}

	template <class NodePtr>
	void rb_tree_set_black(NodePtr node) noexcept
	{
		node->color = rb_tree_black;
	}

	template <class NodePtr>
	NodePtr rb_tree_next(NodePtr node) noexcept
	{
		if (node->right != nullptr) {
			return rb_tree_min(node->right);
		}
		NodePtr parent = node->parent;
		while (parent->right == node) {
			node = parent;
			parent = node->parent;
		}
		return parent;
	}

	/*---------------------------------------*\
	|       p                         p       |
	|      / \                       / \      |
	|     x   d    rotate left      y   d     |
	|    / \       ===========>    / \        |
	|   a   y                     x   c       |
	|      / \                   / \          |
	|     b   c                 a   b         |
	\*---------------------------------------*/

	template <class NodePtr>
	void rb_tree_rotate_left(NodePtr x, NodePtr& root) noexcept
	{
		NodePtr y = x->right;
		x->right = y->left;
		if (y->left != nullptr) {
			y->left->parent = x;
		}
		y->parent = x->parent;

		if (x == root) {
			root = y;
		}
		else if (rb_tree_is_lchild(x)) {
			x->parent->left = y;
		}
		else {
			x->parent->right = y;
		}

		y->left = x;
		x->parent = y;
	}


	/*----------------------------------------*\
	|     p                         p          |
	|    / \                       / \         |
	|   d   x      rotate right   d   y        |
	|      / \     ===========>      / \       |
	|     y   a                     b   x      |
	|    / \                           / \     |
	|   b   c                         c   a    |
	\*----------------------------------------*/

	template <class NodePtr>
	void rb_tree_rotate_right(NodePtr x, NodePtr& root) noexcept
	{
		NodePtr y = x->left;
		x->left = y->right;
		if (y->right != nullptr) {
			y->right->parent = x;
		}
		y->parent = x->parent;

		if (x == root) {
			root = x;
		}
		else if (rb_tree_is_lchild(x)) {
			x->parent->left = y;
		}
		else {
			x->parent->right = y;
		}

		y->right = x;
		x->parent = y;
	}

	/*-------------------*\
	|           y         |
	|          / \        |
	|         x   c       |
	|        / \          |
	|       a   b         |
	\*------------------ */

	// 1. 黑插入红，直接插入
	// 2. 红插入红，且 uncle 为红，x 和 c 变黑， y变红
	// 3. 红插入红，且 uncle 为黑，y 变成红， x变成黑，再右旋
	// 4. 红插入红，且 uncle 为 nullptr，y 变红，x 变黑，再右旋

	template <class NodePtr>
	void rb_tree_insert_rebalance(NodePtr x, NodePtr& root) noexcept
	{
		rb_tree_set_red(x);
		while (x != root && rb_tree_is_red(x->parent)) {
			if (rb_tree_is_lchild(x->parent)) {
				NodePtr uncle = x->parent->parent->right;
				if (uncle != nullptr && rb_tree_is_red(uncle)) {
					rb_tree_set_black(x->parent);
					rb_tree_set_black(uncle);
					x = x->parent->parent;
					rb_tree_set_red(x);
				}
				else {
					if (rb_tree_is_lchild(x) == false) {
						x = x->parent;
						rb_tree_rotate_left(x, root);
					}
					rb_tree_set_black(x->parent);
					rb_tree_set_red(x->parent->parent);
					rb_tree_rotate_right(x->parent->parent, root);
					break;
				}
			}
			else {
				NodePtr uncle = x->parent->parent->left;
				if (uncle != nullptr && rb_tree_is_red(uncle)) {
					rb_tree_set_black(uncle);
					rb_tree_set_black(x->parent);
					x = x->parent->parent;
					rb_tree_set_red(x);
				}
				else {
					if (rb_tree_is_rchild(x) == false) {
						x = x->parent;
						rb_tree_rotate_right(x, root);
					}
					rb_tree_set_black(x->parent);
					rb_tree_set_red(x->parent->parent);
					rb_tree_rotate_left(x->parent->parent, root);
					break;
				}
			}
		}
	}

	template <class NodePtr>
	NodePtr rb_tree_erase_rebalance(NodePtr z, NodePtr& root, NodePtr& leftmost, NodePtr& rightmost) noexcept
	{
		// 这里的 y 只可能是 z 右子树的最左节点或者 z
		// 因为是红黑树，所以当一个节点为空的时候另一个节点必为红色
		NodePtr y = (z->left == nullptr || z->right == nullptr) ? z : rb_tree_next(z);
		// 如果 y 是右子树最左节点则 x 是 y 的右节点，否则是 z 的左节点
		NodePtr x = (y->left != nullptr) ? y->left : y->right;
		// xp 是用来指向要 x 的父节点
		NodePtr xp = nullptr;
		// 如果 y 不是 z
		if (y != z) {
			// z 和左子节点的连接变成 y 和 z的左子节点的连接
			z->left->parent = y;
			y->left = z->left;
			// 如果 y 的右子树的最左节点是它自己
			if (y != z->right) {
				xp = y->parent;
				// x 可能是空的情况
				if (x != nullptr) {
					x->parent = y->parent;
				}
				y->parent->left = x;
				y->right = z->right;
				z->right->parent = y;
			}
			else {
				// 如果 y == z->right 则说明 y 的左边是空的
				// 我们只需要让 y 的左节点指向 z 的左节点就可以了
				xp = y;
			}
			// z 和父节点的连接变成 z 的父节点和 y 的连接
			if (root == z) {
				root = y;
			}
			else if (rb_tree_is_lchild(z)) {
				z->parent->left = y;
			}
			else {
				z->parent->right = y;
			}
			y->parent = z->parent;
			mystl::swap(y->color, z->color);
			// y 指向 z
			y = z;
		}
		else {
			xp = y->parent;
			if (x != nullptr) {
				x->parent = y->parent;
			}

			if (root == z) {
				root = x;
			}
			else if (rb_tree_is_lchild(z)) {
				z->parent->left = x;
			}
			else {
				z->parent->right = x;
			}

			if (leftmost == z) {
				leftmost = (x == nullptr) ? xp : rb_tree_min(x);
			}
			if (rightmost == z) {
				rightmost = (x == nullptr) ? xp : rb_tree_max(x);
			}
		}

		// 删除分六种情况
		// https://blog.csdn.net/qq_40843865/article/details/102498310
		// 其中循环外解决情况 2 以为的所有情况
		// 循环内解决情况 2 中的四种情况

		if (rb_tree_is_black(y)) {
			// x 是会不断的循环，可能 x 会循环到 root 节点
			// 我们需要调整 x 节点

			// 一开始进入循环是因为 x == nullptr ,因为删除的是黑色没有孩子的节点
			while (x != root && (x == nullptr || rb_tree_is_black(x)))
			{
				if (x == xp->left) {
					NodePtr brother = xp->right;
					if (rb_tree_is_red(brother)) {
						// 情况四
						rb_tree_set_red(xp);
						rb_tree_set_black(brother);
						rb_tree_rotate_left(xp, root);
						brother = xp->right;
					}
					// 情况三和情况一、情况二的区别是 brother 下的节点
					if ((brother->left == nullptr || rb_tree_is_black(brother->left)) &&
						(brother->right == nullptr || rb_tree_is_black(brother->right))) {
						// 情况三
						rb_tree_set_red(brother);
						x = xp;
						xp = xp->parent;
					}
					else {
						// 情况二转情况一
						if (brother->right == nullptr || rb_tree_is_black(brother->right)) {
							// 情况二
							if (brother->left == nullptr) {
								rb_tree_set_black(brother->left);
							}
							rb_tree_set_red(brother);
							rb_tree_rotate_right(brother, root);
							brother = xp->right;
						}
						// 情况一
						brother->color = xp->color;
						rb_tree_set_black(xp);
						if (brother->right != nullptr) {
							rb_tree_set_black(brother->right);
						}
						rb_tree_rotate_left(xp, root);
						break;
					}
				}
				else {
					NodePtr brother = xp->left;
					if (rb_tree_is_red(brother)) {
						rb_tree_set_red(xp);
						rb_tree_set_black(brother);
						rb_tree_rotate_right(xp, root);
						brother = xp->left;
					}

					if ((brother->left == nullptr || rb_tree_is_black(brother->left)) &&
						(brother->right == nullptr || rb_tree_is_black(brother->right))) {
						rb_tree_set_red(brother);
						x = xp;
						xp = xp->parent;
					}
					else {
						if (brother->left == nullptr || rb_tree_is_black(brother->left)) {
							if (brother->right != nullptr) {
								rb_tree_set_black(brother->right);
							}
							rb_tree_set_red(brother);
							rb_tree_rotate_left(brother, root);
							brother = xp->left;
						}
						brother->color = xp->color;
						rb_tree_set_black(xp);
						if (brother->left != nullptr) {
							rb_tree_set_black(brother->left);
						}
						rb_tree_rotate_right(xp, root);
						break;
					}
				}
			}
			if (x != nullptr) {
				rb_tree_set_black(x);
			}
		}
		return y;
	}


	template <class T, class Compare>
	class rb_tree
	{
	public:

		using tree_traits = rb_tree_traits<T>;
		using value_traits = rb_tree_value_traits<T>;

		using base_type = typename tree_traits::base_type;
		using base_ptr = typename tree_traits::base_ptr;
		using node_type = typename tree_traits::node_type;
		using node_ptr = typename tree_traits::node_ptr;
		using key_type = typename tree_traits::key_type;
		using mapped_type = typename tree_traits::mapped_type;
		using value_type = typename tree_traits::value_type;
		using key_compare = Compare;

		using allocator_type = allocator<T>;
		using data_allocator = allocator<T>;
		using base_allocator = allocator<base_type>;
		using node_allocator = allocator<node_type>;

		using pointer = typename allocator_type::pointer;
		using const_pointer = typename allocator_type::const_pointer;
		using reference = typename allocator_type::reference;
		using const_reference = typename allocator_type::const_reference;
		using size_type = typename allocator_type::size_type;
		using difference_type = typename allocator_type::difference_type;

		using iterator = rb_tree_iterator<T>;
		using const_iterator = rb_tree_const_iterator<T>;
		using reverse_iterator = mystl::reverse_iterator<iterator>;
		using const_reverse_iterator = mystl::reverse_iterator<const_iterator>;

		allocator_type get_allocate() const { return allocator_type(); }
		key_compare key_comp() const { return key_compare(); }

	private:

		base_ptr mHeader;
		size_type mNodeCount;
		// mKeyComp 可以看作 <
		key_compare mKeyComp;

	private:

		base_ptr& root() const { return mHeader->parent; }
		base_ptr& leftmost() const { return mHeader->left; }
		base_ptr& rightmost() const { return mHeader->right; }

	public:

		rb_tree()
		{
			rb_tree_init();
		}

		rb_tree(const rb_tree& rhs)
		{
			rb_tree_init();
			if (rhs.mNodeCount != 0) {
				root() = copy_from(rhs.root(), mHeader);
				leftmost() = rb_tree_min(root());
				rightmost() = rb_tree_max(root());
			}
			mNodeCount = rhs.mNodeCount;
			mKeyComp = rhs.mKeyComp;
		}

		rb_tree(rb_tree&& rhs)
			: mHeader(mystl::move(rhs.mHeader))
			, mNodeCount(rhs.mNodeCount)
			, mKeyComp(rhs.mKeyComp)
		{
			rhs.reset();
		}

		rb_tree& operator=(const rb_tree& rhs)
		{
			if (&rhs != this) {
				clear();
				if (rhs.mNodeCount != 0) {
					root() = copy_from(rhs.root(), mHeader);
					leftmost() = rb_tree_min(root());
					rightmost() = rb_tree_max(root());
				}

				mNodeCount = rhs.mNodeCount;
				mKeyComp = rhs.mKeyComp;
			}
			return *this;
		}

		rb_tree& operator=(rb_tree&& rhs)
		{
			clear();
			mHeader = mystl::move(rhs.mHeader);
			mNodeCount = rhs.mNodeCount;
			mKeyComp = rhs.mKeyComp;
			rhs.reset();
			return *this;
		}

		~rb_tree()
		{
			clear();
		}

	public:

		iterator begin() noexcept
		{
			return iterator(leftmost());
		}

		const_iterator begin() const noexcept
		{
			return const_iterator(leftmost());
		}

		iterator end() noexcept
		{
			return iterator(mHeader);
		}

		const_iterator end() const noexcept
		{
			return const_iterator(mHeader);
		}

		reverse_iterator rbegin() noexcept
		{
			return reverse_iterator(end());
		}

		const_reverse_iterator rbegin() const noexcept
		{
			return const_reverse_iterator(end());
		}

		reverse_iterator rend() noexcept
		{
			return reverse_iterator(begin());
		}

		const_reverse_iterator rend() const noexcept
		{
			return reverse_iterator(begin());
		}

		const_iterator cbegin() const noexcept
		{
			return const_iterator(mHeader->left);
		}

		const_iterator cend() const noexcept
		{
			return const_iterator(mHeader);
		}

		const_reverse_iterator crbegin() const noexcept
		{
			return const_reverse_iterator(rbegin());
		}

		const_reverse_iterator crend() const noexcept
		{
			return const_reverse_iterator(rend());
		}

		bool empty() const noexcept
		{
			return mNodeCount == 0;
		}

		size_type size() const noexcept
		{
			return mNodeCount;
		}

		size_type max_size() const noexcept
		{
			return static_cast<size_type>(-1);
		}

	public:

		template <class ...Args>
		iterator emplace_multi(Args&& ...args)
		{
			THROW_LENGTH_ERROR_IF(mNodeCount > max_size() - 1, "");
			node_ptr np = create_node(mystl::move(args));
			mystl::pair<iterator, bool> p = get_insert_multi_pos(value_traits::get_key(np->value));
			return insert_node_at(p.first, np, p.second);
		}

		template <class ...Args>
		iterator emplace_multi_use_hint(iterator pos, Args&& ...args)
		{
			THROW_LENGTH_ERROR_IF(mNodeCount > max_size() - 1, " ");
			node_ptr np = create_node(mystl::move(args));
			if (mNodeCount == 0) {
				return insert_node_at(mHeader, np, true);
			}
			key_type key = value_traits::get_key(np->value);
			if (pos == begin()) {
				if (mKeyComp(key, value_traits::get_key(*pos))) {
					return insert_node_at(pos.node, np, true);
				}
				else {
					mystl::pair<iterator, bool> iter = get_insert_multi_pos(key);
					return insert_node_at(iter.first.node, np, iter.second);
				}
			}
			else if (pos == end()) {
				if (mKeyComp(key, value_traits::get_key(rightmost()->get_node_ptr()->value)) == false) {
					return insert_node_at(rightmost(), np, false);
				}
				else {
					mystl::pair<iterator, bool> iter = get_insert_multi_pos(key);
					return insert_node_at(iter.first.node, np, iter.second);
				}
			}
			return inset_multi_use_hint(pos, key, np);
		}

		iterator insert_multi(const value_type& value)
		{
			THROW_LENGTH_ERROR_IF(mNodeCount > max_size() - 1, " ");
			mystl::pair<iterator, bool> p = get_insert_multi_pos(value_traits::get_key(value));
			return insert_value_at(p.first.node, value, p.second);
		}

		iterator insert_multi(value_type&& value)
		{
			return emplace_multi(mystl::move(value));
		}

		iterator insert_multi(iterator hint, const value_type& value)
		{
			return emplace_multi_use_hint(hint, value);
		}

		iterator insert_multi(iterator hint, value_type&& value)
		{
			return emplace_multi_use_hint(hint, mystl::move(value));
		}



		template <class InputIter>
		void insert_multi(InputIter first, InputIter last)
		{
			size_type n = mystl::distance(first, last);
			THROW_LENGTH_ERROR_IF(max_size() - n < mNodeCount, "");
			for (; n > 0; n--, first++) {
				insert_multi(end(), *first);
			}
		}


		template <class ...Args>
		mystl::pair<iterator, bool> emplace_unique(Args&& ...args)
		{
			THROW_LENGTH_ERROR_IF(mNodeCount > max_size() - 1, "");
			node_ptr np = create_node(mystl::move(args));
			mystl::pair<mystl::pair<iterator, bool>, bool> p = get_insert_unique_pos(value_traits::get_key(np->value));
			if (p.second) {
				return make_pair(insert_node_at(p.first.first, np, p.first.second), true);
			}
			destroy_node(np);
			return make_pair(iterator(p.first.first), false);
		}

		template <class ...Args>
		iterator emplace_unique_use_hint(iterator pos, Args&& ...args)
		{
			THROW_LENGTH_ERROR_IF(mNodeCount > max_size() - 1, " ");
			node_ptr np = create_node(mystl::move(args));
			if (mNodeCount == 0) {
				return insert_node_at(mHeader, np, true);
			}
			key_type key = value_traits::get_key(np->value);
			if (pos == begin()) {
				if (mKeyComp(key, value_traits::get_key(*pos))) {
					return insert_node_at(pos.node, np, true);
				}
				else {
					mystl::pair<mystl::pair<iterator, bool>, bool> iter = get_insert_unique_pos(key);
					if (iter.second == false) {
						destroy_node(np);
						return iter.first.first;
					}
					return insert_node_at(iter.first.node, np, iter.second);
				}
			}
			else if (pos == end()) {
				if (mKeyComp(key, value_traits::get_key(rightmost()->get_node_ptr()->value)) == false) {
					return insert_node_at(rightmost(), np, false);
				}
				else {
					mystl::pair<mystl::pair<iterator, bool>, bool> iter = get_insert_unique_pos(key);
					if (iter.second == false) {
						destroy_node(np);
						return iter.first.first;
					}
					return insert_node_at(iter.first.node, np, iter.second);
				}
			}
			return inset_unique_use_hint(pos, key, np);
		}

		mystl::pair<iterator, bool> insert_unique(const value_type& value)
		{
			THROW_LENGTH_ERROR_IF(mNodeCount > max_size() - 1, " ");
			mystl::pair<pair<iterator, bool>, bool> p = get_insert_unique_pos(value_traits::get_key(value));
			if (p.second) {
				return make_pair(insert_value_at(p.first.first.node, value, p.first.second), true);
			}
			return make_pair(p.first.first, false);
		}

		mystl::pair<iterator, bool> insert_unique(value_type&& value)
		{
			return emplace_unique(mystl::move(value));
		}

		mystl::pair<iterator, bool> insert_unique(iterator hint, const value_type& value)
		{
			return emplace_unique_use_hint(hint, value);
		}

		mystl::pair<iterator, bool> insert_unique(iterator hint, value_type&& value)
		{
			return emplace_unique_use_hint(hint, mystl::move(value));
		}

		template <class InputIter>
		void insert_unique(InputIter first, InputIter last)
		{
			size_type n = mystl::distance(first, last);
			THROW_LENGTH_ERROR_IF(max_size() - n < mNodeCount, "");
			for (; n > 0; n--, first++) {
				insert_unique(end(), *first);
			}
		}

		iterator erase(iterator hint)
		{
			node_ptr np = hint.node->get_node_ptr();
			iterator next = hint;
			++next;
			rb_tree_erase_rebalance(np, root(), leftmost(), rightmost());
			--mNodeCount;
			return next;
		}

		size_type erase_multi(const key_type& key)
		{
			mystl::pair<iterator, iterator> range = equal_range_multi(key);
			size_type n = mystl::distance(range.first, range.second);
			erase(range.first, range.second);
			return n;
		}

		size_type erase_unique(const key_type& key)
		{
			iterator it = find(key);
			if (it != end()) {
				erase(key);
				return 1;
			}
			return 0;
		}

		void erase(iterator first, iterator last)
		{
			if (first == begin() && last == end()) {
				clear();
			}
			else {
				while (first != last) {
					erase(first++);
				}
			}
		}

		void clear()
		{
			if (mNodeCount != 0) {
				erase_since(root());
				leftmost() = rightmost() = mHeader;
				root() == nullptr;
				mNodeCount = 0;
			}
		}

		iterator find(const key_type& key)
		{
			base_ptr y = mHeader;
			base_ptr x = root();
			while (x != nullptr) {
				if (mKeyComp(value_traits::get_key(x->get_node_value()), key) == false) {
					y = x;
					x = x->left;
				}
				else {
					x = x->right;
				}
			}
			iterator iter = iterator(y);
			return (iter == end() || mKeyComp(key, value_traits::get_key(*iter))) ? end() : iter;
		}

		const_iterator find(const key_type& key) const
		{
			base_ptr y = mHeader;
			base_ptr x = root();
			while (x != nullptr) {
				if (mKeyComp(value_traits::get_key(x->get_node_value()), key) == false) {
					y = x;
					x = x->left;
				}
				else {
					x = x->right;
				}
			}
			const_iterator iter = iterator(y);
			return (iter == end() || mKeyComp(key, value_traits::get_key(*iter))) ? end() : iter;
		}

		size_type count_multi(const key_type& key) const
		{
			mystl::pair<iterator, iterator> p = equal_range_multi(key);
			return static_cast<size_type>(mystl::distance(p.first, p.second));
		}

		size_type count_unique(const key_type& key) const
		{
			return find(key) == end() ? 1 : 0;
		}

		iterator lower_bound(const key_type& key)
		{
			base_ptr y = mHeader;
			base_ptr x = root();
			while (x != nullptr) {
				if (mKeyComp(value_traits::get_key(x->get_node_value()), key) == false) {
					y = x;
					x = x->left;
				}
				else {
					x = x->right;
				}
			}
			iterator iter = iterator(y);
			return iter;
		}

		const_iterator lower_bound(const key_type& key) const
		{
			base_ptr y = mHeader;
			base_ptr x = root();
			while (x != nullptr) {
				if (mKeyComp(value_traits::get_key(x->get_node_value()), key) == false) {
					y = x;
					x = x->left;
				}
				else {
					x = x->right;
				}
			}
			const_iterator iter = iterator(y);
			return iter;
		}

		iterator upper_bound(const key_type& key)
		{
			base_ptr y = mHeader;
			base_ptr x = root();
			while (x != nullptr) {
				if (mKeyComp(key, value_traits::get_key(x->get_node_value()))) {
					y = x;
					x = x->left;
				}
				else {
					x = x->right;
				}
			}
			iterator iter = iterator(y);
			return iter;
		}

		const_iterator upper_bound(const key_type& key) const
		{
			base_ptr y = mHeader;
			base_ptr x = root();
			while (x != nullptr) {
				if (mKeyComp(key, value_traits::get_key(x->get_node_value()))) {
					y = x;
					x = x->left;
				}
				else {
					x = x->right;
				}
			}
			const_iterator iter = iterator(y);
			return iter;
		}

		mystl::pair<iterator, iterator>
			equal_range_multi(const key_type& key)
		{
			return make_pair(lower_bound(key), upper_bound(key));
		}

		mystl::pair<const_iterator, const_iterator>
			equal_range_multi(const key_type& key) const
		{
			return make_pair(lower_bound(key), upper_bound(key));
		}

		mystl::pair<iterator, iterator>
			equal_range_unique(const key_type& key)
		{
			iterator iter = find(key);
			iterator next = iter;
			++next;
			return iter == end() ? make_pair(iter, iter) : make_pair(iter, next);
		}

		mystl::pair<const_iterator, const_iterator>
			equal_range_unique(const key_type& key) const
		{
			const_iterator iter = find(key);
			const_iterator next = iter;
			++next;
			return iter == end() ? make_pair(iter, iter) : make_pair(iter, next);
		}

		void swap(rb_tree& rhs)
		{
			mystl::swap(rhs.mNodeCount, mNodeCount);
			mystl::swap(rhs.mKeyComp, mKeyComp);
			mystl::swap(rhs.mHeader, mHeader);
		}

	private:

		template <class ...Args>
		node_ptr create_node(Args&& ...args)
		{
			node_ptr tmp = node_allocator::allocate(1);
			try {
				data_allocator::construct(mystl::address_of(tmp->value), mystl::move(args)...);
				tmp->left = tmp->right = tmp->parent = nullptr;
			}
			catch (...) {
				node_allocator::deallocate(tmp, 1);
				throw;
			}
			return tmp;
		}

		node_ptr clone_node(base_ptr x)
		{
			node_ptr tmp = create_node(x->get_node_ptr()->value);
			tmp->color = x->color;
			tmp->left = nullptr;
			tmp->right = nullptr;
			return tmp;
		}

		void destroy_node(node_ptr p)
		{
			data_allocator::destroy(&(p->value));
			node_allocator::deallocate(p);
		}

		void rb_tree_init()
		{
			mHeader = base_allocator::allocate(1);
			mHeader->color = rb_tree_red;
			root() = nullptr;
			leftmost() = rightmost() = mHeader;
			mNodeCount = 0;
		}

		void reset()
		{
			mHeader = nullptr;
			mNodeCount = 0;
		}

		mystl::pair<iterator, bool>
			get_insert_multi_pos(const key_type& key)
		{
			base_ptr x = root();
			base_ptr y = mHeader;
			bool add_to_left = true;
			while (x != nullptr) {
				y = x;
				add_to_left = mKeyComp(key, value_traits::get_key(x->get_node_ptr()->value));
				x = add_to_left ? x->left : x->right;
			}
			return  make_pair(iterator(y), add_to_left);
		}

		mystl::pair<mystl::pair<base_ptr, bool>, bool>
			get_insert_unique_pos(const key_type& key)
		{
			base_ptr x = root();
			base_ptr y = mHeader;
			bool add_to_left = true;
			while (x != nullptr) {
				y = x;
				add_to_left = mKeyComp(key, value_traits::get_key(x->get_node_ptr()->value));
				x = add_to_left ? x->left : x->right;
			}
			iterator it = iterator(y);
			if (add_to_left) {
				if (y == begin() || y == mHeader) {
					return make_pair(make_pair(it, true), true);
				}
				else {
					--it;
				}
			}

			if (mKeyComp(value_traits::get_key(*it), key)) {
				return make_pair(make_pair(y, add_to_left), true);
			}
			return make_pair(make_pair(y, add_to_left), false);
		}

		iterator insert_value_at(base_ptr x, const value_type& value, bool add_to_left)
		{
			node_ptr np = create_node(value);
			np->parent = x;
			base_ptr base_np = np->get_base_ptr();
			if (x == mHeader) {
				mHeader->left = mHeader->right = x;
			}
			else if (add_to_left) {
				x->left = base_np;
				if (leftmost() == x) {
					leftmost() = base_np;
				}
			}
			else {
				x->right = base_np;
				if (rightmost() == x) {
					rightmost() == base_np;
				}
			}
			++mNodeCount;
			rb_tree_insert_rebalance(np, root);
			return iterator(np);
		}

		iterator insert_node_at(base_ptr x, node_ptr np, bool add_to_left)
		{
			np->parent = x;
			base_ptr base_np = np->get_base_ptr();
			if (x == mHeader) {
				mHeader->left = mHeader->right = x;
			}
			else if (add_to_left) {
				x->left = base_np;
				if (leftmost() == x) {
					leftmost() = base_np;
				}
			}
			else {
				x->right = base_np;
				if (rightmost() == x) {
					rightmost() == base_np;
				}
			}
			++mNodeCount;
			rb_tree_insert_rebalance(np, root);
			return iterator(np);
		}

		iterator inset_multi_use_hint(iterator hint, key_type key, node_ptr node)
		{
			base_ptr np = hint.node;
			iterator before = hint;
			--before;
			base_ptr bnp = before.node;
			if (!(mKeyComp(key, value_traits::get_key(*before))) &&
				!(mKeyComp(value_traits::get_key(*hint), key)))
			{
				// before < key < hint
				if (bnp->right == nullptr) {
					return insert_node_at(bnp, np, false);
				}
				else if (bnp->left == nullptr) {
					return insert_node_at(bup, np, true);
				}
			}
			mystl::pair<iterator, bool> pos = get_insert_multi_pos(key);
			return insert_node_at(pos.first.node, np, pos.second);
		}

		iterator inset_unique_use_hint(iterator hint, key_type key, node_ptr node)
		{
			base_ptr np = hint.node;
			iterator before = hint;
			--before;
			base_ptr bnp = before.node;
			if (!(mKeyComp(key, value_traits::get_key(*before))) &&
				!(mKeyComp(value_traits::get_key(*hint), key)))
			{
				// before < key < hint
				if (bnp->right == nullptr) {
					return insert_node_at(bnp, np, false);
				}
				else if (bnp->left == nullptr) {
					return insert_node_at(bup, np, true);
				}
			}
			mystl::pair<mystl::pair<iterator, bool>, bool> pos = get_insert_unique_pos(key);
			if (pos.second == false) {
				destroy_node(np);
				return pos.first.first;
			}
			return insert_node_at(pos.first.node, np, pos.second);
		}

		base_ptr copy_from(base_ptr x, base_ptr p)
		{
			node_ptr np = clone_node(x);
			np->parent = p;
			try {
				if (x->right != nullptr) {
					copy_from(x->right, np);
				}
				p = np;
				x = x->left;
				while (p != nullptr) {
					node_ptr y = clone_node(x);
					y->parent = p;
					p->left = y;
					if (x->right != nullptr) {
						y->right = copy_from(x->right, y);
					}
					p = y;
					x = x->left;
				}
			}
			catch (...) {
				destroy_node(np);
				throw;
			}
			return np;
		}

		void erase_since(base_ptr x)
		{
			while (x != nullptr) {
				erase_since(x->right);
				base_ptr y = x->left;
				destroy_node(x->get_node_ptr());
				x = y;
			}
		}
	};


	template <class T, class Compare>
	bool operator==(const rb_tree<T, Compare>& lhs, const rb_tree<T, Compare>& rhs)
	{
		return lhs.size() == rhs.size() && (mystl::equal(lhs.begin(), lhs.end(), rhs.begin()));
	}

	template <class T, class Compare>
	bool operator!=(const rb_tree<T, Compare>& lhs, const rb_tree<T, Compare>& rhs)
	{
		return !(lhs == rhs);
	}

	template <class T, class Compare>
	bool operator<(const rb_tree<T, Compare>& lhs, const rb_tree<T, Compare>& rhs)
	{
		return mystl::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
	}

	template <class T, class Compare>
	bool operator>(const rb_tree<T, Compare>& lhs, const rb_tree<T, Compare>& rhs)
	{
		return rhs < lhs;
	}

	template <class T, class Compare>
	bool operator<=(const rb_tree<T, Compare>& lhs, const rb_tree<T, Compare>& rhs)
	{
		return !(lhs > rhs);
	}

	template <class T, class Compare>
	bool operator>=(const rb_tree<T, Compare>& lhs, const rb_tree<T, Compare>& rhs)
	{
		return !(lhs < rhs);
	}

	template <class T, class Compare>
	void swap(rb_tree<T, Compare>& lhs, rb_tree<T, Compare>& rhs) noexcept
	{
		lhs.swap(rhs);
	}

}


#endif // !MYSTL_RB_TREE_H

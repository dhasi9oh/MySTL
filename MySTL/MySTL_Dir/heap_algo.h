#ifndef MYSTL_HELP_ALGO_H
#define MYSTL_HELP_ALGO_H

#include "iterator.h"


namespace mystl {

	// 使用数组做的堆插入
	// 插入一个值，不断往上查，插入到亲节点大于他的子节点处
	template <class RandomIter, class Distance, class T>
	void push_heap_aux(RandomIter Iter, Distance holeIndex, Distance topIndex, T value)
	{
		auto parent = (holeIndex - 1) / 2;

		while (holeIndex > topIndex && *(parent + Iter) < value) {
			*(Iter + holeIndex) = *(Iter + parent);
			holeIndex = parent;
			parent = (holeIndex - 1) / 2;
		}
		
		*(Iter + holeIndex) = value;
	}

	// 从二叉树的尾部往上搜，将插入尾部的元素插入到它适合的位置
	template <class RandomIter, class Distance>
	void push_heap_d(RandomIter first, RandomIter last, Distance *)
	{
		push_heap_aux(first, (last - first) - 1, static_cast<Distance>(0), *(last - 1));
	}


	template <class RandomIter>
	void push_heap(RandomIter first, RandomIter last)
	{
		mystl::push_heap_aux(first, last, mystl::distance_type(first));
	}


	template <class RandomIter, class Distance, class T, class Compared>
	void push_heap_aux(RandomIter Iter, Distance holeIndex, Distance topIndex, T value, Compared cmp)
	{
		auto parent = (holeIndex - 1) / 2;

		while (holeIndex > topIndex && cmp(*(parent + Iter), value)) {
			*(Iter + holeIndex) = *(Iter + parent);
			holeIndex = parent;
			parent = (holeIndex - 1) / 2;
		}

		*(Iter + holeIndex) = value;
	}


	template <class RandomIter, class Compared, class Distance>
	void push_heap_d(RandomIter first, RandomIter last, Distance *, Compared cmp)
	{
		mystl::push_heap_aux(first, (last - first) - 1, static_cast<Distance>(0), *(last - 1), cmp);
	}


	template <class RandomIter, class Compared>
	void push_heap(RandomIter first, RandomIter last, Compared cmp)
	{
		mystl::push_heap_d(first, last, mystl::distance_type(first), cmp);
	}


	// 找到最大的一条路径，在这条路径上节点通通往上挪一位
	template <class RandomIter, class Distance, class T>
	void adjust_heap(RandomIter first, Distance holeIndex, Distance len, T value)
	{
		auto topIndex = holeIndex;
		auto rchild = 2 * (topIndex + 1);

		while (rchild < len) {
			if (*(first + rchild) < *(first + (rchild - 1))) {
				rchild--;
			}
			*(first + holeIndex) = *(first + rchild);
			holeIndex = rchild;
			rchild = (rchild + 1) * 2;
		}

		if (rchild == len) {
			*(first + holeIndex) = *(first + (rchild - 1));
			holeIndex = rchild - 1;
		}

		mystl::push_heap_aux(first, holeIndex, topIndex, value);
	}


	template <class RandomIter, class T, class Distance>
	void pop_heap_aux(RandomIter first, RandomIter last, RandomIter result, T value, Distance*)
	{
		*result = *first;
		mystl::adjust_heap(first, static_cast<Distance> (0), last - first, value);
	}


	template <class RandomIter>
	void pop_heap(RandomIter first, RandomIter last)
	{
		mystl::pop_heap_aux(first, last - 1, last - 1, *(last - 1), last - first);
	}


	template <class RandomIter, class Distance, class T, class Compared>
	void adjust_heap(RandomIter first, Distance holeIndex, Distance len, T value, Compared cmp)
	{
		auto topIndex = holeIndex;
		auto rchild = 2 * (topIndex + 1);

		while (rchild < len) {
			if (cmp(*(first + rchild), *(first + (rchild - 1)))) {
				rchild--;
			}
			*(first + holeIndex) = *(first + rchild);
			holeIndex = rchild;
			rchild = (rchild + 1) * 2;
		}

		if (rchild == len) {
			*(first + holeIndex) = *(first + (rchild - 1));
			holeIndex = rchild - 1;
		}

		mystl::push_heap_aux(first, holeIndex, topIndex, value, cmp);
	}


	template <class RandomIter, class T, class Distance, class Compared>
	void pop_heap_aux(RandomIter first, RandomIter last, RandomIter result,
		T value, Distance*, Compared cmp)
	{
		*result = *first;
		mystl::adjust_heap(first, static_cast<Distance>(0), last - first, value, cmp);
	}


	template <class RandomIter, class T, class Distance, class Compared>
	void pop_heap(RandomIter first, RandomIter last, Compared cmp)
	{
		mystl::pop_heap_aux(first, last - 1, last - 1, *(last - 1), distance_type(first), cmp);
	}



	template <class RandomIter>
	void sort_heap(RandomIter first, RandomIter last)
	{
		while (last - first > 1) {
			mystl::pop_heap(first, last--);
		}
	}

	
	template <class RandomIter, class Compared>
	void sort_heap(RandomIter first, RandomIter last, Compared cmp)
	{
		while (last - first > 1) {
			mystl::pop_heap(first, last--, cmp);
		}
	}


	template <class RandomIter, class Distance>
	void make_heap_aux(RandomIter first, RandomIter last, Distance*)
	{
		if (last - first > 1) {
			return;
		}

		auto len = last - first;
		auto holeIndex = (len - 2) / 2;

		while (true) {
			mystl::adjust_heap(first, holeIndex, len, *(first + holeIndex));
			if (holeIndex == 0) {
				return 0;
			}
			holeIndex--;
		}
	}

	template <class RandomIter>
	void make_heap(RandomIter first, RandomIter last)
	{
		mystl::make_heap_aux(first, last, mystl::distance_type(first));
	}
	

	template <class RandomIter, class Distance, class Compared>
	void make_heap_aux(RandomIter first, RandomIter last, Distance*, Compared cmp)
	{
		if (last - first > 1) {
			return;
		}

		auto len = last - first;
		auto holeIndex = (len - 2) / 2;

		while (true) {
			mystl::adjust_heap(first, holeIndex, len, *(first + holeIndex), cmp);
			if (holeIndex == 0) {
				return 0;
			}
			holeIndex--;
		}
	}

	
	template <class RandomIter, class Compared>
	void make_heap(RandomIter first, RandomIter last, Compared cmp)
	{
		mystl::make_heap_aux(first, last, mystl::distance_type(first), cmp);
	}

}





#endif // !MYSTL_HELP_ALGO_H

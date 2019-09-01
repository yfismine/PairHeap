#ifndef _PAIRINGHEAPH_
#define _PAIRINGHEAPH_
#pragma once
#include<functional>
#include<stdexcept>
#include<vector>
namespace PAIRING
{
	template <typename Element, typename CalObj = std::less<Element>>
	class PairingHeap
	{
	private:
		struct PairNode
		{
			Element data;
			PairNode *prev;
			PairNode *leftChild;
			PairNode *nextSibling;
			PairNode(const Element et = Element(), PairNode *pr = nullptr, PairNode *ltc = nullptr, PairNode *nts = nullptr)
				:data(et), prev(pr), leftChild(ltc), nextSibling(nts) {};
		};
		CalObj function;
		PairNode *heap;
	public:
		using Position = const PairNode * ;
		PairingHeap();
		PairingHeap(const CalObj obj);
		const Element& top();
		void pop();
		Position push(const Element &e);
		bool Empty();
		void clear();
		void changeKey(Position p, const Element &newE);
		~PairingHeap();
	private:
		void compareAndLink(PairNode *&first, PairNode *second);
		PairNode *combineSibings(PairNode *firstSibling);
	};

	template<typename Element, typename CalObj>
	inline PairingHeap<Element, CalObj>::PairingHeap() :function(CalObj()), heap(nullptr)
	{
	}

	template<typename Element, typename CalObj>
	inline PairingHeap<Element, CalObj>::PairingHeap(const CalObj obj) : function(obj), heap(nullptr)
	{
	}

	template<typename Element, typename CalObj>
	inline const Element& PairingHeap<Element, CalObj>::top()
	{
		if (Empty())
			throw std::underflow_error("Heap is empty!,We can get the mininum node");
		else
			return heap->data;
	}

	template<typename Element, typename CalObj>
	inline void PairingHeap<Element, CalObj>::pop()
	{
		if (Empty())
			throw std::underflow_error("Heap is empty! We can not delete the mininum node");
		PairNode *oldHeap = heap;
		if (heap->leftChild == nullptr)
			heap = nullptr;
		else
			heap = combineSibings(heap->leftChild);
		delete oldHeap;
	}

	template<typename Element, typename CalObj>
	inline typename PairingHeap<Element,CalObj>::Position PairingHeap<Element, CalObj>::push(const Element & e)
	{
		PairNode *newNode = new PairNode(e);
		if (heap == nullptr)
			heap = newNode;
		else
			compareAndLink(heap, newNode);
		return newNode;
	}

	template<typename Element, typename CalObj>
	inline bool PairingHeap<Element, CalObj>::Empty()
	{
		if (heap == nullptr)
			return true;
		else
			return false;
	}

	template<typename Element, typename CalObj>
	inline void PairingHeap<Element, CalObj>::clear()
	{
		PairNode *t1=heap, *t2=nullptr;
		while (t1->leftChild!=nullptr)
		{
			t2 = t1;
			t1 = t1->leftChild;
			while (t2->nextSibling!=nullptr)
			{
				t2 = t2->nextSibling;
				delete t2->prev;
			}
			delete t2;
		}
		delete t1;
		heap = nullptr;
	}

	template<typename Element, typename CalObj>
	inline void PairingHeap<Element, CalObj>::changeKey(Position p, const Element & newE)
	{
		if (function(p->data, newE))  //只能向优先级高的方向修改值
			throw std::invalid_argument("We can only modify the node's priority to higher");
		auto pos = const_cast<PairNode *>(p);
		pos->data = newE;
		if (pos != heap)  //采用裂堆合并解决changeKey
		{
			if (pos->nextSibling != nullptr)
				pos->nextSibling->prev = pos->prev;
			if (pos->prev->leftChild == pos)  //是否为最左结点
				pos->prev->leftChild = pos->nextSibling;
			else
				pos->prev->nextSibling = pos->nextSibling;
			pos->nextSibling = nullptr;
			compareAndLink(heap, pos);
		}
	}

	template<typename Element, typename CalObj>
	inline PairingHeap<Element, CalObj>::~PairingHeap()
	{
		if(heap!=nullptr)
			clear();
	}

	template<typename Element, typename CalObj>
	inline void PairingHeap<Element, CalObj>::compareAndLink(PairNode *& first, PairNode * second)
	{
		if (second == nullptr)
			return;
		if (function(second->data, first->data))
		{
			//将first链接到second上
			second->prev = first->prev;
			first->prev = second;
			first->nextSibling = second->leftChild;
			if (first->nextSibling != nullptr)
				first->nextSibling->prev = first;
			second->leftChild = first;
			first = second;
		}
		else
		{
			//将second链接到first上
			second->prev = first;
			first->nextSibling = second->nextSibling;
			if (first->nextSibling != nullptr)
				first->nextSibling->prev = first;
			second->nextSibling = first->leftChild;
			if (second->nextSibling != nullptr)
				second->nextSibling->prev = second;
			first->leftChild = second;
		}
	}

	/*整个程序中最巧妙的部分，采用两趟合并法解决pop*/
	template<typename Element, typename CalObj>
	inline typename PairingHeap<Element,CalObj>::PairNode * PairingHeap<Element, CalObj>::combineSibings(PairNode * firstSibling)
	{
		if (firstSibling->nextSibling == nullptr)
			return firstSibling;
		std::vector<PairNode *> treeArray(5);
		int numSibling = 0;   //结点个数
		for (; firstSibling != nullptr; numSibling++)
		{
			if (numSibling == treeArray.size())
				treeArray.resize(numSibling * 2);
			treeArray[numSibling] = firstSibling;
			firstSibling->prev->nextSibling = nullptr;
			firstSibling = firstSibling->nextSibling;
		}
		int i = 0;
		for (; i + 1 < numSibling; i += 2)  //第一趟合并，采用两两合并，奇数项则少合并一次
			compareAndLink(treeArray[i], treeArray[i + 1]);
		int j = i - 2;
		if (j == numSibling - 3)  //当为奇数项时执行该合并，目的在于排除非空与空结点的合并
			compareAndLink(treeArray[j], treeArray[j + 2]);
		for (; j >= 2; j -= 2) //第二趟合并，采用逆序邻项合并
			compareAndLink(treeArray[j - 2], treeArray[j]);
		return treeArray[0];
	}
}
#endif

/*===============================================================================================

	This stack works slowly, than one from STL, but it doesn't allocates 4kB of memory and
	behaves stable enough.

===============================================================================================*/

#ifndef _MYSTACK_H_
#define _MYSTACK_H_

template < class T > class MyStack
{
	struct node
	{
		node ( const T& d, node *n ) : data (d), next (n) {};

		T data;
		node *next;
	};

	node *tos;

public:

	MyStack () : tos (0) {};

	~MyStack ()
	{
		while (!empty())
			pop();
	};

	void push ( const T& element )
	{
		tos = new node (element, tos);
	};

	T& top ()
	{
		return tos->data;
	};

	void pop ()
	{
		node *tmp = tos->next;
		delete tos;
		tos = tmp;
	};

	bool empty ()
	{
		return tos == 0;
	};
};

#endif
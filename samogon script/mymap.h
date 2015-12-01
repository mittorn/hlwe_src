#ifndef _MYMAP_H_
#define _MYMAP_H_

template < class T > class MyMap
{
	struct element
	{
		element (char *k, T v, element *n) : value (v), next (n)
		{
			int l = strlen(k);

			key = new char[l + 1];
			strcpy (key, k);
			key[l] = '\0';
		};

		~element ()
		{
			delete [] key;
		};

		char *key;
		T value;
		element *next;
	};

	element *head;

public:

	MyMap () : head (0) {};

	~MyMap()
	{
		clear ();
	};

	T& operator[] ( char* k )
	{
		for (element *i = head; i != 0; i = i->next)
		{
			if (!strcmp(k, i->key))
			{
				return i->value;
			}
		}

		head = new element (k, T(), head);

		return head->value;
	};

	void clear ()
	{
		element *i = head;

		while (i)
		{
			element *tmp = i;
			i = i->next;
			delete tmp;
		}

		head = 0;
	};
};

#endif
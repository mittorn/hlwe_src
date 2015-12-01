/*===============================================================================================================================

	HASH MAP (Implemented by LLAPb, 2010). Has the same interface as usual ss_map, but works faster - O(const).
	Uses CRC32 to convolute the key. Has an ability to reserve some memory for new elements,
	and this ability must be used for higher performance instead of slow reallocation routine.

===============================================================================================================================*/

#ifndef _SS_HASHMAP_H_
#define _SS_HASHMAP_H_

#include "extdll.h"
#include "enginecallback.h"//CRC32 callbacks

//Shit
typedef unsigned int offset_t;

template < class T > class HashMap
{
	template < class E > struct ListNode
	{
		ListNode<E> *next;
		E element;
		char *key;

		ListNode (E e, char *k, ListNode<E> *n) : element(e), next(n)
		{
			key = new char[strlen (k) + 1];
			strcpy (key, k);
		};

		~ListNode ()
		{
			delete [] key;
		};

		static void DeleteList (ListNode<E> *list)
		{
			while (list != 0)
			{
				ListNode<E> *tmp = list->next;

				delete list;

				list = tmp;
			}
		};

		static void AppendNode (ListNode<E> **list, E new_element, char *new_key)
		{
			ListNode<E> *new_head = new ListNode (new_element, new_key, *list);
			*list = new_head;
		};
	};

	typedef ListNode<T> *List;

	List	*table;
	size_t	tabsz, listsc;

public:

	bool	enable_resize;

	HashMap (size_t N) : listsc(0), enable_resize(true)//Allocate T[N] memory apriory...
	{
		table = new List[N];
		//Exception may be raised here... Maybe try to handle it?

		if (table)
		{
			tabsz = N;

			for (offset_t i = 0; i < N; ++i)
				table[i] = 0;
		}
	};

	~HashMap ()
	{
		clear ();
	};

	T& operator[] (char *key)
	{
		if (enable_resize && listsc > 0.75 * tabsz)//If hash table is filled by 75%, it starts work slowly
		{
			resize (ceil (tabsz * 1.5));
		}

		return GetElement (table, tabsz, key, enable_resize, listsc);
	};

	void clear ()
	{
		//clear all the lists:
		for (offset_t i = 0; i < tabsz; ++i)
		{
			ListNode<T>::DeleteList (table[i]);
		}

		//free table memory
		delete [] table;

		tabsz = 0;
		listsc = 0;
	};

private:

	unsigned long int hash (char *key, size_t keylen, size_t modulo)//This function is tablesize-dependant
	{
		CRC32_t ulCrc;
		CRC32_INIT(&ulCrc);
		CRC32_PROCESS_BUFFER(&ulCrc, key, keylen);

		return CRC32_FINAL(ulCrc) % modulo;
	};

	T& GetElement (List *tabptr, size_t tabsize, char *key, bool resizable, unsigned int& counter)
	{
		size_t keylen = strlen (key);
		offset_t index = hash (key, keylen, tabsize);

		if (tabptr[index] == 0)//Need to allocate new list here
		{
			if (resizable)
			{
				counter++;
				tabptr[index] = new ListNode<T> (T(), key, 0);

				//go to end of function
			}
			else
			{
				static T null = T(0);
				return null;
				//Cheaty shit... But it can increase performance.
				//Warning!!! ASSERT, that we will never ASSIGN to this null.
			}
		}
		else//Check already existing list
		{
			List tmp = tabptr[index];

			while (tmp != 0)
			{
				if (!strcmp (key, tmp->key))
				{
					return tmp->element;
				}

				tmp = tmp->next;
			}

			//We get here if we hadn't find anything
			if (resizable)
			{
				ListNode<T>::AppendNode (&tabptr[index], T(), key);

				//now go to the end of function
			}
			else
			{
				static T null = T(0);
				return null;
			}
		}

		return tabptr[index]->element;
	}

	void resize (size_t newsz)//This is very expensive operation. Use flag "enable_resize" to disable it.
	{
		List *new_table = new List[newsz];
		
		for (offset_t j = 0; j < newsz; ++j)
			new_table[j] = 0;

		size_t new_lists_count = 0;

		//Go through old table and rehash all it's shit
		for (offset_t i = 0; i < tabsz; ++i)
		{
			List cell = table[i];

			while (cell != 0)//Go through the list in single cell
			{
				GetElement (new_table, newsz, cell->key, true, new_lists_count) = cell->element;//And insert nodes to new table

				cell = cell->next;
			}
		}

		//Destroy old table
		clear();

		//Re-initialize member variables
		table = new_table;
		tabsz = newsz;
		listsc = new_lists_count;
	};
};

#endif
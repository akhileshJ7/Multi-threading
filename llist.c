// DER (c) 2019
#include <stdlib.h>
#include <string.h>
#include "llist.h"
 
/* statics */
static node *makeNode(char *toStore);
static void killNode(node *toKill);

/* llInit()
 *     Initialize a list structure.  An empty list will
 *     be characterized by head and tail pointer both being NULL.
 *         Parameters:  myList - a pointer to the structure to be init
 *         Returns: void
 */ 
void llInit(list *myList)
{
	myList->head = NULL;
	myList->tail = NULL;
	return;
}

/* llSize()
 *     Reports the current size of the list.  Will need to iterate
 *     the list to get this data size there is no size property, nor
 *     can there really be one given that users and access nodes.
 *         Parameters:  myList - the list
 *         Returns: int, size of list
 */ 
int llSize(list *myList)
{
	int size = 0;
	node *iterator = myList->head;
	while(iterator)
	{
		size++;
		iterator = iterator->next;
	}
	return size;
}

/* llPushFront()
 *     Add a new node with provided data and place node at
 *     front of list.  The new node will replace the head node.
 *     This method should check to make sure the provided char * is
 *     not NULL.  If it is NULL, this method should do nothing and
 *     make no changes to the list.  If it is not NULL, it can be
 *     assumed that it is a valid null-terminated string.
 *         Parameters:  myList - the list
 *                      toStore - the char array to store
 *         Returns: int - 0 if no push (toStore was NULL) or non-zero
 *                        if push successful
 */ 
int llPushFront(list *myList,char *toStore)
{
	if(toStore)
	{
		node *newNode = makeNode(toStore);
		
		/* is it first node in list? */
		if (myList->head == NULL)
		{
			myList->head = myList->tail = newNode;
		}
		else /* not first node */
		{
			newNode->next = myList->head;
			myList->head->prev = newNode;
			myList->head = newNode;
		}
		return 1; /* success */
	}
	return 0;
}

/* llPopFront()
 *     Removes first item in list.  Note, this does not return
 *     anything from the list.  If the data in the node is needed
 *     it should be accessed prior to the pop (list.head->string).
 *         Parameters:  myList - the list
 *         Returns: int - 0 if no pop (list was empty) or non-zero
 *                        if pop successful
 */ 
int llPopFront(list *myList)
{
	int toReturn = 0;
	
	if(myList->head) /* not NULL */
	{
		node *temp = myList->head;
		myList->head = temp->next;
		if(myList->head) /* make sure not last node */
		{
			myList->head->prev = NULL;
		}
		else /* fix tail pointer if list now empty */
		{
			myList->tail = myList->head;
		}
		killNode(temp);
		toReturn = 1;
	}

	return toReturn;
}

/* llPushBack()
 *     Add a new node with provided data and place node at
 *     end of list.  This new node will be the new tail node.  
 *     This method should check to make sure the provided char * is
 *     not NULL.  If it is NULL, this method should do nothing and
 *     make no changes to the list.  If it is not NULL, it can be
 *     assumed that it is a valid null-terminated string.
 *         Parameters:  myList - the list
 *                      toStore - the char array to store
 *         Returns: int - 0 if no push (toStore was NULL) or non-zero
 *                        if push successful
 */ 
int llPushBack(list *myList, char *toStore)
{
	if (toStore)
	{
		node *newNode = makeNode(toStore);
		
		/* is it first node in list? */
		if (myList->tail == NULL)
		{
			myList->tail = myList->head = newNode;
		}
		else /* not first node */
		{
			newNode->next = NULL;
			myList->tail->next = newNode;
			newNode->prev = myList->tail;
			myList->tail = newNode;
		}
		return 1; /* success */
	}
	return 0; /* fail */
}

/* llPopBack()
 *     Removes last item in list.  Note, this does not return
 *     anything from the list.  If the data in the node is needed
 *     it should be accessed prior to the pop (list.head->string).
 *         Parameters:  myList - the list
 *         Returns: int - 0 if no pop (list was empty) or non-zero
 *                        if pop successful
 */ 
int llPopBack(list *myList)
{
	int toReturn = 0;
	
	if(myList->tail) /* not NULL */
	{
		node *temp = myList->tail;
		myList->tail = temp->prev;
		if(myList->tail) /* make sure not last node */
		{
			myList->tail->next = NULL;
		}
		else /* fix head pointer if list now empty */
		{
			myList->head = myList->tail;
		}
		killNode(temp);
		toReturn = 1;
	}

	return toReturn;
}

/* llInsertAfter()
 *     Add a new node with provided data and place node after
 *     provided node reference.
 *     This method should check to make sure the provided char * is
 *     not NULL.  If it is NULL, this method should do nothing and
 *     make no changes to the list.  If it is not NULL, it can be
 *     assumed that it is a valid null-terminated string.
 *     If this method is called on the tail node, a change to the
 *     list structure will need to be made.
 *         Parameters:  myList - theList
 *                      insNode - the node after which item is added
 *                      toStore - the char array to store
 *         Returns: int - 0 if no insert (toStore was NULL or insNode
 *                                        is NULL) 
 *                        non-zero if insert successful
 */ 
int llInsertAfter(list *myList, node *insNode, char *toStore)
{
	int toReturn = 0;
	
	if (toStore && insNode)
	{
		if (insNode == myList->tail)
		{
			toReturn = llPushBack(myList,toStore);
		}
		else
		{
			node *newNode = makeNode(toStore);
			
			insNode->next->prev = newNode;
			newNode->next = insNode->next;
			newNode->prev = insNode;
			insNode->next = newNode;
			toReturn = 1;
		}
		return toReturn; /* success */
	}
	return toReturn; /* fail */
}

/* llInsertBefore()
 *     Add a new node with provided data and place node before
 *     provided node reference.
 *     This method should check to make sure the provided char * is
 *     not NULL.  If it is NULL, this method should do nothing and
 *     make no changes to the list.  If it is not NULL, it can be
 *     assumed that it is a valid null-terminated string.
 *     If this method is called on the head node, a change to the
 *     list structure will need to be made.
 *         Parameters:  myList - theList
 *                      insNode - the node before which item is added
 *                      toStore - the char array to store
 *         Returns: int - 0 if no insert (toStore was NULL or insNode
 *                                        is NULL) 
 *                        non-zero if insert successful
 */ 
int llInsertBefore(list *myList, node *insNode, char *toStore)
{
	int toReturn = 0;
	
	if (toStore && insNode)
	{
		if(insNode == myList->head)
		{
			toReturn = llPushFront(myList,toStore);
		}
		else
		{
			node *newNode = makeNode(toStore);
			
			insNode->prev->next = newNode;
			newNode->next = insNode;
			newNode->prev = insNode->prev;
			insNode->prev = newNode;
			toReturn = 1;
		}
		return toReturn; /* success */
	}
	return toReturn; /* fail */
}

/* llRemove()
 *     Removes the node referenced.  Releases
 *     all associated dynamic memory.
 *     If this method is called the current head or tail node, changes
 *     to the list structure may need to be made.
 *         Parameters:  myList - the list
 *                      rmvNode - the node prior to the node to be
 *                                removed.
 *         Returns: 0 if fail (rmvNode is NULL), non-zero for success
 */ 
int llRemove(list *myList, node *rmvNode)
{
	if(rmvNode) /* not NULL */
	{
		if (rmvNode->next) /* not tail node? */
		{
			rmvNode->next->prev = rmvNode->prev;
		}
		else /* was tail node */
		{
			myList->tail = rmvNode->prev;
		}
		if (rmvNode->prev) /* not head node */
		{
			rmvNode->prev->next = rmvNode->next;
		}
		else
		{
			myList->head = rmvNode->next;
		}
		killNode(rmvNode);
		return 1; /* success */
	}
	
	return 0; /* fail */
}


/* llClear()
 *     Clears all nodes and releases all dynamic memory.  
 *         Parameters:  myList - the list
 *         Returns: nothing
 */ 
void llClear(list *myList)
{
	while(llPopFront(myList))
	{
	}
	return;
}

/* static helpers */
static node *makeNode(char* toStore)
{
	node *newNode = malloc(sizeof(node));
	/* remember to make room for the terminator */
	newNode->string = malloc(strlen(toStore)+1);
	strcpy(newNode->string,toStore);
	newNode->next = NULL;
	newNode->prev = NULL;
	return newNode;
}

static void killNode(node *toKill)
{
	free(toKill->string);
	free(toKill);
	return;
}

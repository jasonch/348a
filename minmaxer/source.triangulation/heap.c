/* Heap Sources */
/*---------------------------------------------------------------------------*/

#include <stdio.h>
#include <malloc.h>
#include "road.h"
#include "heap.h"

/*---------------------------------------------------------------------------*/

#define HPMOVE(hp, e, p) {(hp)->entry[p] = (e); \
			  (hp)->indexOf[e] = (p);}

/*---------------------------------------------------------------------------*/

void
hpPrint (hp)

     queuesType *hp;

{
  int pointer;

  pointer = 1;
  while (pointer < hp->free) {
    printf ("index %d, ", pointer);
    printf ("entry %d, ", hp->entry[pointer]);
    printf ("indexof(entry) %d", hp->indexOf[hp->entry[pointer]]);
    if ( hp->indexOf[hp->entry[pointer]] != pointer)
      printf ("  <== ERROR ");
    printf ("\n");
    pointer++;
  }
  printf ("\n");
}

/*---------------------------------------------------------------------------*/

void
hpExc (hp, ind1, ind2)

     queuesType *hp;
     int      ind1, ind2;

{
  indexType dummy;

  dummy = hp->entry[ind1];
  HPMOVE (hp, hp->entry[ind2], ind1);
  HPMOVE (hp, dummy, ind2);
}

/*---------------------------------------------------------------------------*/

void
hpConsistCheck (hp, father,son)

     queuesType *hp;
     int      father, son;
{
  if ((son < hp->free) &&
      (*(hp->GT)) (hp->data, hp->entry[son], hp->entry[father])) 
    printf ("ERROR in heap, Data inconsistent.\n");
}

/*---------------------------------------------------------------------------*/

void
hpReset (hp)

     queuesType *hp;

{
  hp->free = 1;
}

/*---------------------------------------------------------------------------*/

void
hpMoveDownEntry (hp, index)

     queuesType *hp;
     int      index;

{
  while ((index * 2 + 1) < hp->free) {
    if ((*(hp->GT)) 
	(hp->data, hp->entry[index * 2], hp->entry[index]))
      if ((*(hp->GT)) 
	  (hp->data, hp->entry[index * 2 + 1], hp->entry[index]))
	if ((*(hp->GT)) 
	    (hp->data, hp->entry[index * 2], hp->entry[index * 2 + 1])) {
	  hpExc (hp, index*2, index);
	  hpConsistCheck (hp, index,index*2+1);
	  index = index*2;
	}
	else {
	  hpExc (hp, index*2+1, index);
	  hpConsistCheck (hp, index,index*2);
	  index = index * 2 + 1;
	}
      else {
	hpExc (hp, index*2, index);
	hpConsistCheck (hp, index,index*2+1);
	index = index * 2;
      }
    else
      if ((*(hp->GT)) 
	  (hp->data, hp->entry[index * 2 + 1], hp->entry[index])) {
	hpExc (hp, index*2+1, index);
	hpConsistCheck (hp, index,index*2);
	index = index * 2 + 1;
      } else
	return;
  }
  if ((index * 2) < hp->free) 
    if ((*(hp->GT)) 
	(hp->data, hp->entry[index * 2], hp->entry[index]))
      hpExc (hp, index*2, index);
}

/*---------------------------------------------------------------------------*/

void
hpMoveUpEntry (hp, index)

     queuesType *hp;
     int      index;

{
  while ((index) >= 2) 
    if ((*(hp->GT)) 
	(hp->data, hp->entry[index], hp->entry[index / 2])) {
      hpExc (hp, index, index / 2);
      hpConsistCheck (hp, index / 2,index);
      index = index / 2;
    } else
      return;
}

/*---------------------------------------------------------------------------*/

queuesType *hpNew (n, array, GTfunction)

     int n;
     char *array;
     bool (*GTfunction) ();

{
  queuesType *hp;
  char *fail = { "hpNew: fail on [mc]alloc.\n" };
  int i;

  if ((hp = (queuesType *) malloc (sizeof (queuesType))) == NULL) {
    (void) fprintf (stdout, fail);
    exit (1);
  }

  hp->entry = (indexType *) calloc ((unsigned int) n+1, sizeof (indexType));
  if (hp->entry == NULL) {
    (void) fprintf (stdout, fail);
    exit (1);
  }

  hp->indexOf = (indexType *) calloc ((unsigned int) n+1, sizeof (indexType));
  if (hp->entry == NULL) {
    (void) fprintf (stdout, fail);
    exit (1);
  }

  for (i = 0; i <= n; i++) hp->indexOf[i] = 0;

  hp->maxn = n;
  hpReset (hp);
  hp->GT = GTfunction;
  hp->data = array;

  return (hp);
} 

/*---------------------------------------------------------------------------*/

void 
hpDispose (hp)

     queuesType *hp;

{
  free ((char *) hp->entry);
  free ((char *) hp);
}

/*---------------------------------------------------------------------------*/

void
hpInsertNode (hp, newEntry)

     queuesType  *hp;
     indexType newEntry;

{
/*  debugPrintQE2 (hp->data, "hpInsert", newEntry, 0);*/
  
  if ( hp->indexOf[newEntry] != 0) {
    printf ("hpInsert: ERROR, Entry %d already in Heap!\n", newEntry);
    exit (1);
  }
  
  if (newEntry > hp->maxn)
    printf ("hpInsert: ERROR, Entry %d out of Bounds!\n", newEntry);
  else {
    if (hp->free >= hp->maxn) {
      printf ("hpInsert: SORRY, I'M FULL.");
      exit (1);
    } else {
      HPMOVE (hp, newEntry, hp->free);
      hpMoveUpEntry (hp, hp->free);
      hp->free++;
    }
  }
}

/*---------------------------------------------------------------------------*/

void
hpDeleteIndex (hp, index)

     queuesType *hp;
     indexType index;

{
  if (index > hp->maxn)
    printf ("hpDelete: ERROR, Index %d out of Bounds!\n", index);
  else {
    if ((index >= hp->free) || (hp->free == 1))
      printf("hpDelete: SORRY, OUT OF BOUNDS.");
    else {
      hp->indexOf[hp->entry[index]] = 0;
      hp->free--;
      if (index != hp->free)
	if ((*(hp->GT)) 
	    (hp->data, hp->entry[hp->free], hp->entry[index])) {
	  HPMOVE (hp, hp->entry[hp->free], index);
	  hpMoveUpEntry (hp, index);
	} else {
	  HPMOVE (hp, hp->entry[hp->free], index);
	  hpMoveDownEntry (hp, index);
	}
    }
  }
}

/*---------------------------------------------------------------------------*/

void
hpDeleteNode (hp, oldEntry)

     queuesType *hp;
     indexType oldEntry;

{
  int index;

/*  debugPrintQE2 (hp->data, "hpDelete", oldEntry, 0);*/

  if (oldEntry > hp->maxn)
    printf ("hpDelete: ERROR, Entry %d out of Bounds!\n", oldEntry);
  else {
    index = hp->indexOf[oldEntry];

    if (index == 0) {
      printf ("hpDelete: ERROR, Entry %d not in Heap!\n", oldEntry);
      exit (1);
    }
    else 
      hpDeleteIndex (hp, index);
  }
}
/*---------------------------------------------------------------------------*/

bool
hpContainsNode (hp, oldEntry)

     queuesType *hp;
     indexType oldEntry;

{
  if (oldEntry > hp->maxn) {
    printf ("hpDelete: ERROR, Entry %d out of Bounds!\n", oldEntry);
    return 0;
  } else
    return hp->indexOf[oldEntry] != 0;
}

/*---------------------------------------------------------------------------*/

bool
hpGetMax (hp, maxEntry)

     queuesType  *hp;
     indexType *maxEntry;
  
{
  if (hp->free == 1) 
    return 0;
  else {
    *maxEntry = hp->entry[1];
    return 1;
  }
}

/*---------------------------------------------------------------------------*/

poolType * 
heap ()

{
  poolType *tmp;

  if ((tmp = (poolType *) malloc (sizeof (poolType))) == NULL) {
    (void) fprintf (stdout, "queue: fail on [mc]alloc.\n");
    exit (1);
  }

  tmp->create = hpNew;
  tmp->top = hpGetMax;
  tmp->insert = hpInsertNode;
  tmp->delete = hpDeleteNode;
  tmp->doesContain = hpContainsNode;
  tmp->dispose = hpDispose;

  return tmp;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

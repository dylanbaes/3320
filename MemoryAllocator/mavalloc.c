// The MIT License (MIT)
// 
// Copyright (c) 2022 Trevor Bakker 
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include "mavalloc.h"

enum TYPE
{
    FREE = 0,
    USED
};

struct Node {
  size_t size;
  enum TYPE type;
  void * arena;
  struct Node * next;
  struct Node * prev;
};

struct Node *alloc_list;
struct Node *previous_node;

void * arena;

enum ALGORITHM allocation_algorithm = FIRST_FIT;

// allocates a pool of memory with an assigned algorithm
int mavalloc_init( size_t size, enum ALGORITHM algorithm )
{
  arena = malloc( ALIGN4( size ) );
  
  allocation_algorithm = algorithm;

  alloc_list = ( struct Node * )malloc( sizeof( struct Node ));

  alloc_list -> arena = arena;
  alloc_list -> size  = ALIGN4(size);
  alloc_list -> type  = FREE;
  alloc_list -> next  = NULL;
  alloc_list -> prev  = NULL;

  previous_node  = alloc_list;

  return 0;
}

void mavalloc_destroy( )
{
  // iterate over the linked list and free up all the memory
  free(arena);
  free(alloc_list);
  alloc_list = NULL;
  // struct Node * node = alloc_list;
  // struct Node * next = node->next;
  // while ( next )
  // {
  //   free(node);
  //   node = next;
  //   next = node->next;
  // }
  return;
}

void * mavalloc_alloc( size_t size )
{
  struct Node * node;
  size_t aligned_size = ALIGN4( size );

  if ( allocation_algorithm == BEST_FIT )
  {
    // best fit, iterate through the list, find the node with the smallest leftover size and allocate the memory there
    // if the size of a free block == requested size, allocate memory there
    // while you iterate through the list, keep track of the arena with the smallest leftover size so far. then after iterating through the list, allocate to that pointer
    node = alloc_list;
    struct Node * smallest_size;
    int leftover_size = 0;
    while( node )
    {
      if( node -> size >= aligned_size  && node -> type == FREE )
      {
        leftover_size = node->size - aligned_size;
        int minimum_size = 999999;
        if (node -> size == aligned_size && leftover_size == 0) 
        {
          node -> type = USED;
          //previous_node = node;
          return ( void * ) node -> arena;
        }
        else
        {
          if (leftover_size < minimum_size) 
          {
            smallest_size = node;
          }
        }
      }
      node = node -> next;
    }
    smallest_size -> type = USED;
    leftover_size = smallest_size -> size - aligned_size;
    smallest_size->size = aligned_size;
    if (leftover_size > 0)
    {
      struct Node * previous_next = smallest_size -> next;
      struct Node * leftover_node = ( struct Node * ) malloc ( sizeof( struct Node ));

      leftover_node -> arena = smallest_size -> arena + aligned_size;
      leftover_node -> type  = FREE;
      leftover_node -> size  = leftover_size;
      leftover_node -> next  = previous_next;
      leftover_node -> prev = smallest_size;
      smallest_size -> next = leftover_node;
    }
    return ( void * ) smallest_size -> arena;
  }

  else if( allocation_algorithm == WORST_FIT )
  { 
    node = alloc_list;
    while( node )
    {
      if( node -> size >= aligned_size  && node -> type == FREE )
      {
        int leftover_size = 0;
  
        node -> type  = USED;
        leftover_size = node -> size - aligned_size;
        node -> size =  aligned_size;

        if( leftover_size > 0 )
        {
          struct Node * previous_next = node -> next;
          struct Node * leftover_node = ( struct Node * ) malloc ( sizeof( struct Node ));
  
          leftover_node -> arena = node -> arena + aligned_size;
          leftover_node -> type  = FREE;
          leftover_node -> size  = leftover_size;
          leftover_node -> next  = previous_next;
          leftover_node -> prev = node;
          node -> next = leftover_node;
        }
        previous_node = node;
        return ( void * ) node -> arena;
      }
      node = node -> next;
    }
  }
  else if ( allocation_algorithm == NEXT_FIT )
  {
    
    node = previous_node; // node that points to the starting block where the last allocated node ends
    while ( node )
    {
      if( node -> size >= aligned_size  && node -> type == FREE )
      {
        int leftover_size = 0;
  
        node -> type  = USED;
        leftover_size = node -> size - aligned_size;
        node -> size =  aligned_size;
  
        if( leftover_size > 0 )
        {
          struct Node * previous_next = node -> next;
          struct Node * leftover_node = ( struct Node * ) malloc ( sizeof( struct Node ));
  
          leftover_node -> arena = node -> arena + aligned_size;
          leftover_node -> type  = FREE;
          leftover_node -> size  = leftover_size;
          leftover_node -> next  = previous_next;
          leftover_node -> prev = node;

          node -> next = leftover_node;
        }
        previous_node = node->next;
        return ( void * ) node -> arena;
        
      }
      node = node -> next;
    }
    
  }

  else if( allocation_algorithm == FIRST_FIT )
  {
    node = alloc_list;

    while( node )
    {
      if( node -> size >= aligned_size  && node -> type == FREE )
      {
        int leftover_size = 0;
  
        node -> type  = USED;
        leftover_size = node -> size - aligned_size;
        node -> size =  aligned_size;
  
        if( leftover_size > 0 )
        {
          struct Node * previous_next = node -> next;
          struct Node * leftover_node = ( struct Node * ) malloc ( sizeof( struct Node ));
  
          leftover_node -> arena = node -> arena + aligned_size;
          leftover_node -> type  = FREE;
          leftover_node -> size  = leftover_size;
          leftover_node -> next  = previous_next;
          leftover_node -> prev = node;
  
          node -> next = leftover_node;
        }
        previous_node = node;
        return ( void * ) node -> arena;
      }
      node = node -> next;
    }
  }

  // Implement Next Fit
  // Implement Worst Fit
  // Implement Best Fit

  return NULL;
}
void mavalloc_free( void * ptr )
{
  void * arena = ptr;
  struct Node * node;
  node = alloc_list;

  while ( node )
  {
    if (node->arena == arena) 
    {
      node->type = FREE;
      if (node->next != NULL && node->next->type == FREE) // if the next node is also free. then join both nodes together, with the primary node being this node
      {
        node->size = node->size + node->next->size;
        node->next = node->next->next;
      }
      if (node->prev != NULL && node->prev->type == FREE) // if the previous node is also free, then join both nodes together, with the primary node being the previous node
      {
        node->prev->size = node->prev->size + node->size;
        node->prev->next = node->next;
      }
      previous_node = alloc_list;
    }
    node = node->next;
  }
  return;
}

int mavalloc_size( )
{
  int number_of_nodes = 0;
  struct Node * ptr = alloc_list;

  while( ptr )
  {
    number_of_nodes ++;
    ptr = ptr -> next; 
  }
  //printf("There are %d nodes\n", number_of_nodes);
  return number_of_nodes;
}

#ifndef JSW_ATREE_H
#define JSW_ATREE_H

/*
  Andersson tree library

    > Created (Julienne Walker): September 10, 2005

  This code is in the public domain. Anyone may
  use it or change it in any way that they see
  fit. The author assumes no responsibility for 
  damages incurred through use of the original
  code or any variations thereof.

  It is requested, but not required, that due
  credit is given to the original author and
  anyone who has modified the code through
  a header comment, such as this one.
*/
#ifdef __cplusplus
#include <cstddef>

using std::size_t;

extern "C" {
#else
#include <stddef.h>
#endif

/* Opaque types */
typedef struct jsw_atree jsw_atree_t;
typedef struct jsw_atrav jsw_atrav_t;

/* User-defined item handling */
typedef int   (*cmp_f) ( const void *p1, const void *p2 );
typedef void *(*dup_f) ( void *p );
typedef void  (*rel_f) ( void *p );

/* Andersson tree functions */
jsw_atree_t *jsw_anew ( cmp_f cmp, dup_f dup, rel_f rel );
void         jsw_adelete ( jsw_atree_t *tree );
void        *jsw_afind ( jsw_atree_t *tree, void *data );
int          jsw_ainsert ( jsw_atree_t *tree, void *data );
int          jsw_aerase ( jsw_atree_t *tree, void *data );
size_t       jsw_asize ( jsw_atree_t *tree );

/* Traversal functions */
jsw_atrav_t *jsw_atnew ( void );
void         jsw_atdelete ( jsw_atrav_t *trav );
void        *jsw_atfirst ( jsw_atrav_t *trav, jsw_atree_t *tree );
void        *jsw_atlast ( jsw_atrav_t *trav, jsw_atree_t *tree );
void        *jsw_atnext ( jsw_atrav_t *trav );
void        *jsw_atprev ( jsw_atrav_t *trav );

#ifdef __cplusplus
}
#endif

#endif

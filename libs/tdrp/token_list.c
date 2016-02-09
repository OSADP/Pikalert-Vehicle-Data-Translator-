/* *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* */
/* ** Copyright UCAR (c) 1992 - 2015 */
/* ** University Corporation for Atmospheric Research(UCAR) */
/* ** National Center for Atmospheric Research(NCAR) */
/* ** Research Applications Laboratory(RAL) */
/* ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA */
/* ** See LICENCE.TXT if applicable for licence details */
/* ** 2015/02/02 20:35:09 */
/* *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* */

/*****************************************************
 * token_list.c
 *
 * Token list manipulation routines for tdrp.
 *
 * The token list is a list of tdrpToken_t * pointers.
 *
 */

#include "tdrp.h"
#include <assert.h>
#include <string.h>

/************************
 * tdrpTokenListInit()
 *
 * Initialize the token list.
 */

void tdrpTokenListInit(token_handle_t *handle)

{
  handle->list_buf = tdrpBufCreate();
}

/*************************
 * tdrpTokenListReset()
 *
 * Reset the token list.
 */

void tdrpTokenListReset(token_handle_t *handle)

{
  int nlist = tdrpNTokenList(handle);
  int i;
  tdrpToken_t *tok = tdrpTokenListPtr(handle);
  for (i = 0; i < nlist; i++, tok++) {
    tdrpFree(tok->tok);
  }
  tdrpBufReset(handle->list_buf);
}

/************************
 *  tdrpTokenListAdd()
 *
 *  Adds entry to token list.
 *
 */

void tdrpTokenListAdd(token_handle_t *handle, const tdrpToken_t *token)

{

  tdrpToken_t tmp_tok = *token;
  tmp_tok.tok = (char*) tdrpMalloc(strlen(token->tok) + 1);
  strcpy(tmp_tok.tok, token->tok);
  tdrpBufAdd(handle->list_buf, &tmp_tok, sizeof(tdrpToken_t));

}

/*********************
 * tdrpNTokenList()
 *
 * return number of tokens in list
 */

int tdrpNTokenList(token_handle_t *handle)

{
  int nlist;
  assert(handle->list_buf != NULL);
  nlist = tdrpBufLen(handle->list_buf) / sizeof(tdrpToken_t);
  return (nlist);
}

/************************
 *  tdrpTokenListPtr()
 *
 *  Returns pointer to list.
 *
 */

tdrpToken_t *tdrpTokenListPtr(token_handle_t *handle)

{
  tdrpToken_t *tokens;
  assert(handle->list_buf != NULL);
  tokens = (tdrpToken_t *) tdrpBufPtr(handle->list_buf);
  return (tokens);
}

/************************
 * tdrpTokenListFree()
 *
 * Free the token list.
 */

void tdrpTokenListFree(token_handle_t *handle)

{
  assert(handle->list_buf != NULL);
  tdrpTokenListReset(handle);
  tdrpBufDelete(handle->list_buf);
}


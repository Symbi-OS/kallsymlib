/******************************************************************************
* Copyright (C) 2021 by Jonathan Appavoo, Boston University
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*****************************************************************************/
#include <stdio.h>
#include <uthash.h>
#include <strings.h>

#include "kallsymlib.h"

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

//#define ENABLE_VERBOSE

#ifdef ENABLE_VERBOSE
#define VPRINTF(fmt, ...) fprintf(stderr, fmt, __VA_ARGS__)
#define VPRINT(fmt) fprintf(stderr, fmt)
#else
#define VPRINTF(...)
#define VPRINT(...)
#endif

struct HASH_NODE {
  struct kallsymlib_info info;
  UT_hash_handle hh;
  int count;
};

static struct HASH_NODE  *HashTable = NULL;

static int kallsymlib_status = KALLSYMLIB_NOT_INITIALIZED;

int 
kallsymlib_init(char *path) 
{
  FILE *fp;
  int n;
  char c;
  struct kallsymlib_info info;
  struct HASH_NODE *node;
  
  if ((fp=fopen(path, "r")) == NULL) {
    perror(path);
    return 0;
  }

  // The use of %m below causes fscanf to allocate memory for the strings
  while ((n = fscanf(fp, "%llx %c %ms", 
		     &info.addr, &info.type, &info.symbol, &info.extra)
	  ) != EOF) {
    info.next = NULL;    // set unread fields to defaults
    info.extra = NULL;

    VPRINTF("%016llx %c (%p)%s",
	   info.addr, info.type, 
	    info.symbol, info.symbol);

    if ((c=fgetc(fp)) != '\n') {
      // read extra text if it exists
      ungetc(c,fp); 
      n=fscanf(fp, "%ms", &info.extra);
      if (n==EOF) break;
      else {
	VPRINTF(" (%p)%s", info.extra, info.extra); 
      }
    }

    // Add to hash table
    HASH_FIND_STR(HashTable, info.symbol, node);
    if (node==NULL) {
      // first defintion for symbol add symbol to hash table
      node = (struct HASH_NODE *)malloc(sizeof(struct HASH_NODE));
      node->info = info;
      node->count = 1;
      HASH_ADD_KEYPTR(hh, HashTable, 
		      node->info.symbol, strlen(node->info.symbol), node);
    } else {
      // second definition add to existing node for symbol
      info.next = node->info.next;
      node->info.next = malloc(sizeof(struct kallsymlib_info));
      *(node->info.next) = info;
      node->count++;
    }
    VPRINTF(" %d\n", node->count);
  }

  kallsymlib_status = KALLSYMLIB_INITIALIZED;

  return 1;
}

// lookup symbol
//  args: name - C string of symbol name to lookup
//        info_ptr - pointer to info pointer
//  return: count of definitions found
// Returns number of definitions found for the symbol name and pointer to
// the list of definitions is returned.
// 
// if no defintions zero and NULL are returned
int 
kallsymlib_lookup(char *name, struct kallsymlib_info **info_ptr) 
{ 
  struct HASH_NODE *node;
  struct kallsymlib_info *info = NULL;

  if (kallsymlib_status != KALLSYMLIB_INITIALIZED) {
    if (!kallsymlib_init(KALLSYMLIB_DEFAULT_PATH)) {
      // hard coding exit here might want to remove
      fprintf(stderr, "ERROR: kallsymlib failed to initialize\n");
      exit(-1);
      return 0;
    }
  }

  HASH_FIND_STR(HashTable, name, node);

  if (node == NULL) { 
    *info_ptr = NULL;
    return 0;
  }
  *info_ptr = &(node->info);

  return node->count;
}

void
kallsymlib_print_info(FILE *fp, struct kallsymlib_info *info)
{
  int count=0;

  if (info) fprintf(fp, "%s :", info->symbol);
  while (info) {
    if (count) fprintf(fp, ",");
    if (info->extra) {
      fprintf(fp, " %016llx %c %s", info->addr, info->type, info->extra);
    } else {
      fprintf(fp, " %016llx %c", info->addr, info->type);
    }
    count++;
    info = info->next;
  }
  fprintf(fp, "\n");
}

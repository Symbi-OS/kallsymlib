#ifndef __KALLSYMLIB_H__
#define __KALLSYMLIB_H__
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

#define KALLSYMLIB_DEFAULT_PATH "/proc/kallsyms"
#define KALLSYMSLIB_MAX_STR_LEN 80

enum KALLSYSLIB_STATUS { 
  KALLSYMLIB_NOT_INITIALIZED=0, 
  KALLSYMLIB_INITIALIZED=1 
};

struct kallsymlib_info {
  unsigned long long addr;
  char type;
  char *symbol;
  char *extra;
  struct kallsymlib_info *next;
};

extern int kallsymlib_lookup(char *name, struct kallsymlib_info **info_ptr);
extern void kallsymlib_print_info(FILE *file, struct kallsymlib_info *);

extern int kallsymlib_init(char *path);
extern int kallsymlib_cleanup();

#endif

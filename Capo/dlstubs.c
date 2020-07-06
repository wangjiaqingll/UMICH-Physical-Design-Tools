#include <sys/types.h>
#include <dlfcn.h>

	/* dl*() stub routines for static compilation.  Prepared from
	   /usr/include/dlfcn.h by Hal Pomeranz <<EMAIL: PROTECTED>> */
void *_dlopen(const char *str, int x) {}
void *_dlsym(void *ptr, const char *str) {}
int _dlclose(void *ptr) {}

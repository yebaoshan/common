#include "other.h"

#define sfree(ptr)                                                             \
  do {                                                                         \
    free(ptr);                                                                 \
    (ptr) = NULL;                                                              \
  } while (0)

#define STATIC_ARRAY_SIZE(a) (sizeof(a) / sizeof(*(a)))

#define IS_TRUE(s)                                                             \
  ((strcasecmp("true", (s)) == 0) || (strcasecmp("yes", (s)) == 0) ||          \
   (strcasecmp("on", (s)) == 0))
#define IS_FALSE(s)                                                            \
  ((strcasecmp("false", (s)) == 0) || (strcasecmp("no", (s)) == 0) ||          \
   (strcasecmp("off", (s)) == 0))

void *smalloc(size_t size) {
  void *r;

  if ((r = malloc(size)) == NULL) {
    ERROR("Not enough memory.");
    exit(3);
  }

  return (r);
} /* void *smalloc */

#if 0
void sfree (void **ptr)
{
	if (ptr == NULL)
		return;

	if (*ptr != NULL)
		free (*ptr);

	*ptr = NULL;
}
#endif


// vim: set ts=4 sw=4 sts=4 et:

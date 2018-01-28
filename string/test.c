#include "sstring.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void test_strarray()
{
    char **array = NULL;
    size_t len = 0;
    strarray_add(&array, &len, "11111");
    strarray_add(&array, &len, "22222");
    strarray_add(&array, &len, "33333");
    printf("========%s========\n", __FUNCTION__);
    for (int i = 0; i < len; i++)
        printf("\t%d:%s\n", i, array[i]);
    strarray_free(array, len);
}

void test_sstrcpy()
{
    char *dup = sstrdup("aaaaa");
    printf("========%s========\n", __FUNCTION__);
    printf("\tdup=%s\n", dup);

    char *alloc = ssnprintf_alloc("%s %d", "bbbbbbbbbb", 1);
    printf("\talloc=%s\n", alloc);

    char *cpy = sstrncpy(alloc, dup, strlen(alloc));
    printf("\tcpy=%s\n", cpy);
    free(dup);
    free(alloc);
}

void test_split()
{
    char test[] = {"11111a22222b3333"};
    char* fileds[3] = {0};
    strsplit(test, fileds, 3, "ab");
    printf("========%s========\n", __FUNCTION__);
    printf("\tstrsplit\n");
    printf("\t\t%s;%s;%s\n", fileds[0], fileds[1], fileds[2]);

    char test_join[25] = {0};
    strjoin(test_join, sizeof(test_join),
            fileds, 3, "xx");

    printf("\n\tstrjoin=%s\n", test_join);

    char **result = NULL;
    size_t result_size = 0;
    strsplit_alloc(test_join, "xx", &result, &result_size);
    printf("\tstrsplit_alloc\n\t\t");
    for (int i = 0; i < result_size; i++) {
        printf("%d:%s; ", i, result[i]);
    }
    printf("\n");
    strarray_free(result, result_size);

}

int main()
{
    test_strarray();
    test_sstrcpy();
    test_split();
}


// vim: set ts=4 sw=4 sts=4 et:

#define VECTOR_TYPE int
#include <srxk_vector.h>

#define VECTOR_TYPE char
#include <srxk_vector.h>

#include <stdio.h>

void test_vector (void);

int main (void)
{
	test_vector();

	return 0;
}

void test_vector (void)
{
	vec_int *v = vec_int_new();
	vec_int_push(v, 4);
	printf("%d\n", vec_int_last(v));
	printf("%d\n", vec_int_pop(v));
	vec_int_free(v);

	vec_char *cv = vec_char_new();
	vec_char_push(cv, 'a');
	printf("%c\n", vec_char_last(cv));
	printf("%c\n", vec_char_pop(cv));
	vec_char_free(cv);
}
// This creates a vector type of int
#define VECTOR_TYPE int
#include <srxk_vector.h>


// This creates a vector type of char*
typedef char* string;
#define VECTOR_TYPE string
#include <srxk_vector.h>


struct mystruct
{
	int x;
	char* str;
};
// This creates a vector type of struct mystruct*
typedef struct mystruct* mystruct_ptr;
#define VECTOR_TYPE mystruct_ptr
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
	// Create, push, last, pop, free
	vec_int *v = vec_int_new();
	vec_int_push(v, 4);
	printf("%d\n", vec_int_last(v));
	printf("%d\n", vec_int_pop(v));
	vec_int_free(v);

	vec_string *sv = vec_string_new();
	vec_string_push(sv, "test");
	printf("%s\n", vec_string_last(sv));
	printf("%s\n", vec_string_pop(sv));
	vec_string_free(sv);


	struct mystruct ms = { 420, "epic"};
	vec_mystruct_ptr *mv = vec_mystruct_ptr_new();
	vec_mystruct_ptr_push(mv, &ms);
	printf("%d\n", vec_mystruct_ptr_last(mv)->x);
	printf("%s\n", vec_mystruct_ptr_pop(mv)->str);
	vec_mystruct_ptr_free(mv);
}

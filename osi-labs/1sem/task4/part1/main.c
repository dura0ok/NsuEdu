#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int global_init = 8;
int global_not_initialize;

static int global_static_init = 3;
static int global_static_not_init;

int bar(){
	int x = 3;
	printf("local init in bar 0x%p %d\n", &x, x);
	return x;
}

void foo(){
	int local_init = 5;
	static int static_var_in_func_init = 6;
	static int static_var_in_func_not_init;
	const int const_init_int = 7;
	const int const_uninit_int;



	printf("local init 0x%p %d\n", &local_init, local_init);
	printf("static var in func init 0x%p %d\n", &static_var_in_func_init, static_var_in_func_init);
	printf("static var in func not init 0x%p %d\n", &static_var_in_func_not_init, static_var_in_func_not_init);
	printf("const init 0x%p %d\n", &const_init_int, const_init_int);
	printf("const uninit 0x%p\n", &const_uninit_int);
}

void baz(){
	char* local_heap = (char*)malloc(100 * sizeof(char));
	strcpy(local_heap, "OS cool!");
	printf("Baz buffer 0x%p %s\n", local_heap, local_heap);
	free(local_heap);
	printf("Baz buffer after free 0x%p %s\n", local_heap, local_heap);

	char* local_heap2 = (char*)malloc(100  * sizeof(char));
	strcpy(local_heap2, "PHP TOP!");
	printf("Baz buffer2 0x%p %s\n", local_heap2, local_heap2);
	local_heap2+=50;
	free(local_heap2);
	printf("Baz buffer2 after free 0x%p %s\n", local_heap2, local_heap2);
}

void env_foo(){
	char *res = getenv("OS_SUBJECT");
	printf("Env foo get env 0x%p %s\n", res, res);
	// overwrite if nonzero 1
	setenv("OS_SUBJECT", "easy", 1);
	res = getenv("OS_SUBJECT");
	printf("Env foo get env 0x%p %s\n", res, res);
}

int main(int argc, char const *argv[]){
	foo();
	env_foo();

	printf("Global init 0x%p %d\n", &global_init, global_init);
	printf("Global not init 0x%p %d\n", &global_not_initialize, global_not_initialize);
	printf("Global static init 0x%p %d\n", &global_static_init, global_static_init);
	printf("Global static not init 0x%p %d\n", &global_static_not_init, global_static_not_init);

	int d = bar();
	printf("returned local from bar init 0x%p %d\n", &d, d);

	printf("pid %d\n", getpid());
	sleep(10);
	baz();

	sleep(60);

	return 0;
}
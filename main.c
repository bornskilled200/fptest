#include <stdio.h>
#include <float.h>

typedef unsigned char *byte_pointer;

typedef struct {
    float x, y;
    float vx, vy;
} UF_physicObject;

typedef struct {
    UF_physicObject object;

} uf_animal;

/*show bytes takes byte pointer as an argument
  and prints memory contents from byte_pointer
  to byte_pointer + len */
void show_bytes(byte_pointer start, int len)
{
    int i;
    for (i = 0; i < len; i++)
        printf("%.2x ", start[i]);
}

void show_int(int x)
{
    show_bytes((byte_pointer) &x, sizeof(int));
    puts("");
}

void show_float(float x)
{
    show_bytes((byte_pointer) &x, sizeof(float));
    printf(": %f\n", x);
}

void show_pointer(void *x)
{
    show_bytes((byte_pointer) &x, sizeof(void *));
    puts("");
}

#define uf_printbytes(a) show_bytes((byte_pointer) &a, sizeof(a))

struct {
    int hi;
    int bye;
} super;

void test_intermediatestore()
{
    float  q;
    q = 3.0f/7.0f;
    if (q == 3.0f/7.0f) printf("Equal\n");
    else printf("Not Equal\n");
}

float divf (float a, float b) {
    return a / b;
}

float divfr (float a, float b) {
    float result = a / b;
    return result;
}
/* Drover program to test above functions */
int main()
{
    printf("%d\n", FLT_EVAL_METHOD);
    int i = 1;
    float f = 1.0;
    int *p = &i;
    float f2 = 1.0f/10;
    show_float(f);
    show_int(i);
    show_pointer(p);
    show_int(i);
    uf_printbytes(i);
    puts("");
    uf_printbytes(super);
    puts("");
    test_intermediatestore();
    show_float(f2);
    puts("test2");


    float       af = 2877.0f; float       bf = 1000000.0f;
    show_float(0.002877f);
    show_float(divf (af, bf));
    show_float(divfr (af, bf));
    printf("%a\n", af);
    return 0;
}

#include <stdio.h>
#include <float.h>
#include <time.h>
#include <math.h>
#include <windows.h>

#define UNUSED(x) (void)(x)

typedef void (*UFupdate)(void **, void *, double);

typedef struct {
    float x, y;
    float vx, vy;
    UFupdate updatefun;
} UFobject;

typedef struct {
    UFobject object;
    UFobject *target;
} UFanimal;

void UFdefaultUpdate_fun(UFobject **world, UFobject *self, double delta) {
    UNUSED(world);

    self->x += self->vx * delta;
    self->y += self->vy * delta;
}

void UFanimalUpdate_fun(UFobject **world, UFanimal *self, double delta) {
    if (self->target == NULL)
        return;

    float dx = self->target->x - self->object.x;
    float dy = self->target->y - self->object.y;
    float len = sqrtf(dx * dx + dy * dy);
    self->object.vx += dx / len * 2;
    self->object.vy += dy / len * 2;
    UFdefaultUpdate_fun(world, (UFobject *) self, delta);
}

typedef unsigned char *byte_pointer;

/*show bytes takes byte pointer as an argument
  and prints memory contents from byte_pointer
  to byte_pointer + len */
void show_bytes(byte_pointer start, int len) {
    int i;
    for (i = 0; i < len; i++)
        printf("%.2x ", start[i]);
}

#define uf_printbytes(a) show_bytes((byte_pointer) &a, sizeof(a))

void show_float(float x) {
    show_bytes((byte_pointer) &x, sizeof(float));
    printf(": %f\n", x);
}

void testIntermediateStore() {
    float f = 3.f / 7.f;
    puts("------------------");
    puts("Test Immediate store\n");
    printf("%a %f\n", f, f);
    printf("%a %f\n", 3.f / 7.f, 3.f / 7.f);
    puts(f == 3.f / 7.f ? "PASSED" : "FAILED");
    puts("------------------");
}

double getDelta(time_t *previousTime) {
    time_t temp = *previousTime;
    time_t currentTime = time(previousTime);
    return difftime(currentTime, temp);
}

#define SECONDS_PER_UPDATE 1.0/60.0
#define SIMULATE_TICKS 300

void UFprintWorld(UFobject **world, int size, int tick) {
    int i;
    printf("tick: %d\n", tick);
    for (i = 0; i < size; i++) {
        UFobject *object = world[i];
        printf("%f, %f, %f, %f, %p\n", object->x, object->y, object->vx, object->vy, object->updatefun);
    }
}
#include <x86intrin.h>
#include <cpuid.h>

int main() {
    int i, j, ticks;
    double accumulator;
    time_t previousTime;
    UFobject player = {.x = 5, .vx = 1, .vy =1, .updatefun = UFdefaultUpdate_fun};
    UFanimal animal = {.object.vx = 1, .object.vy =1, .target=&player, .object.updatefun = UFanimalUpdate_fun};
    UFobject *world[2] = {&player, &animal};
    int size = sizeof(world) / sizeof(*world);

    puts("# System Information Begin");
    #ifdef __gnu_linux__
    FILE *cpuinfo = fopen("/proc/cpuinfo", "rb");
    char *arg = 0;
    size_t size = 0;
    while(getdelim(&arg, &size, 0, cpuinfo) != -1)
    {
       puts(arg);
    }
    free(arg);
    fclose(cpuinfo);
    #elif _WIN32
    system("wmic cpu get name");
    #endif

    printf("FLT_EVAL_METHOD = %d\n", FLT_EVAL_METHOD);
    testIntermediateStore();
    puts("# System Information End");

    ticks = 0;
    UFprintWorld(world, size, ticks);
    accumulator = 0;
    previousTime = time(NULL);
    while (1) {
        for (i = 0, accumulator += getDelta(&previousTime); accumulator > SECONDS_PER_UPDATE || i < 5; i++) {
            for (j = 0; j < size; j++) {
                UFobject *object = world[j];
                (object->updatefun)(world, object, SECONDS_PER_UPDATE);
            }
            accumulator -= SECONDS_PER_UPDATE;
            ticks++;
            UFprintWorld(world, size, ticks);
            if (ticks == SIMULATE_TICKS)
                return 0;
        }
    }
}

#define __USE_MINGW_ANSI_STDIO 1
#include <stdio.h>

#include <time.h>
#include <math.h>

#include <float.h>
#include <stdint.h>

#ifndef FLT_EVAL_METHOD
#define FLT_EVAL_METHOD -1
#endif

#define UNUSED(x) (void)(x)
#ifndef COMPILER_ARGS
#define COMPILER_ARGS ""
#endif

void systemInformation() {
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
    char psBuffer[128];
    FILE *pPipe;

    if ((pPipe = _popen("wmic cpu get name", "rt")) == NULL) {
        fputs("Could not access wmic", stderr);
        return;
    }
    while (fgets(psBuffer, sizeof(psBuffer) / sizeof(*psBuffer), pPipe)) {
        puts(psBuffer);
    }
    if (feof(pPipe))
        printf("\nProcess returned %d\n", _pclose(pPipe));
    else
        printf("Error: Failed to read the pipe to the end.\n");
#endif
}

void show_bytes(void *pointer, int len) {
    unsigned char *start = pointer;
    int i;

    for (i = 0; i < len; i++)
        printf("%.2x ", start[i]);
}

#define uf_printbytes(a) show_bytes(&a, sizeof(a))

union float32 {
    int32_t i;
    float f;
};

void testEvaluationf() {
    float g_one = 1.0f;
    float g_small_1 = (float) (FLT_EPSILON * 0.5);
    float g_small_2 = (float) (DBL_EPSILON * 0.5);
    float g_small_3 = (float) (DBL_EPSILON * 0.5);
    int evaluationMethod;
    union float32 fl0, fl1, fl2, eval;
    fl0.i = 0x3f800000;
    UNUSED(fl1);
    fl2.i = 0x3f800001;

    eval.f = ((g_one + g_small_1) + g_small_2) + g_small_3;
    if (eval.i == fl0.i) evaluationMethod = 0;
    else if (eval.i == fl2.i) evaluationMethod = 2;
    else evaluationMethod = -1;

    puts("------------------");
    puts("Test Float Immediate store\n");
    uf_printbytes(eval.f);
    printf("; %f \n", eval.f);
    printf("Runtime FLT_EVAL_METHOD: %d\n", evaluationMethod);
    puts("------------------");
}

union float64 {
    int64_t i;
    double f;
};

void testEvaluationd() {
    int evaluationMethod;
    union float64 fl0, fl1, fl2;
    UNUSED(fl0);
    UNUSED(fl1);
    UNUSED(fl2);

    evaluationMethod = -1;

    puts("------------------");
    puts("Test Double Immediate store\n");
    printf("Runtime FLT_EVAL_METHOD: %d\n", evaluationMethod);
    puts("------------------");
}

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

    self->x += (float)(self->vx * delta);
    self->y += (float)(self->vy * delta);
}

void UFanimalUpdate_fun(UFobject **world, UFanimal *self, double delta) {
    float dx, dy, len;

    if (self->target == NULL)
        return;

    dx = self->target->x - self->object.x;
    dy = self->target->y - self->object.y;
    len = sqrtf(dx * dx + dy * dy);
    self->object.vx += dx / len * 2;
    self->object.vy += dy / len * 2;
    UFdefaultUpdate_fun(world, (UFobject *) self, delta);
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
    printf("tick: %0" "3" "d\n", tick);
    for (i = 0; i < size; i++) {
        UFobject *object = world[i];
        printf("%d: ", i);
        uf_printbytes(object->x);
        fputs(", ", stdout);
        uf_printbytes(object->y);
        fputs(", ", stdout);
        uf_printbytes(object->vx);
        fputs(", ", stdout);
        uf_printbytes(object->vy);
        puts("");
    }
    puts("");
}

void testPhysicsf() {
    int i, j, ticks;
    double accumulator;
    time_t previousTime;
    UFobject player;
    UFanimal animal;
    UFobject *world[2] = {&player, (UFobject *) &animal};
    int size = sizeof(world) / sizeof(*world);

    player.x=5;
    player.vx =1;
    player.vy =1;
    player.updatefun = (UFupdate) UFdefaultUpdate_fun;

    animal.object.vx = 1;
    animal.object.vy = 1;
    animal.object.updatefun = (UFupdate) UFanimalUpdate_fun;
    animal.target=&player;

    ticks = 0;
    UFprintWorld(world, size, ticks);
    accumulator = 0;
    previousTime = time(NULL);
    while (1) {
        for (i = 0, accumulator += getDelta(&previousTime); accumulator > SECONDS_PER_UPDATE || i < 5; i++) {
            for (j = 0; j < size; j++) {
                UFobject *object = world[j];
                (object->updatefun)((void **) world, object, SECONDS_PER_UPDATE);
            }
            accumulator -= SECONDS_PER_UPDATE;
            ticks++;
            UFprintWorld(world, size, ticks);
            if (ticks == SIMULATE_TICKS)
                return;
        }
    }
}

int main() {
    puts("# Compililation Information Begin");
    puts(COMPILER_NAME COMPILER_ARGS);
    printf("FLT_EVAL_METHOD = %d\n", FLT_EVAL_METHOD);
    puts("# Compililation Information End\n");

    puts("# System Information Begin");
    systemInformation();
    puts("# System Information End\n");

    puts("# Test Information Begin");
    testEvaluationf();
    testEvaluationd();
    testPhysicsf();
    puts("# Test Information End\n");
}

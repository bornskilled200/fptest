#define __USE_MINGW_ANSI_STDIO 1

#include <stdio.h>
#include <float.h>
#include <time.h>
#include <math.h>

#define UNUSED(x) (void)(x)

typedef void (*UFupdate) (void **, void *, double);

typedef struct
{
    float    x, y;
    float    vx, vy;
    UFupdate updatefun;
}            UFobject;

typedef struct
{
    UFobject object;
    UFobject *target;
}            UFanimal;

void UFdefaultUpdate_fun (UFobject **world, UFobject *self, double delta)
{
    UNUSED(world);

    self->x += self->vx * delta;
    self->y += self->vy * delta;
}

void UFanimalUpdate_fun (UFobject **world, UFanimal *self, double delta)
{
    if (self->target == NULL)
        return;

    float dx  = self->target->x - self->object.x;
    float dy  = self->target->y - self->object.y;
    float len = sqrtf(dx * dx + dy * dy);
    self->object.vx += dx / len * 2;
    self->object.vy += dy / len * 2;
    UFdefaultUpdate_fun(world, (UFobject *) self, delta);
}


void systemInformation ()
{

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

    /* Run DIR so that it writes its output to a pipe. Open this
     * pipe with read text attribute so that we can read it
     * like a text file.
     */

    if ((pPipe = _popen("wmic cpu get name", "rt")) == NULL)
    {
        fputs("Could not access wmic", stderr);
        return;
    }

    /* Read pipe until end of file, or an error occurs. */

    while (fgets(psBuffer, 128, pPipe))
    {
        printf(psBuffer);
    }


    /* Close pipe and print return value of pPipe. */
    if (feof(pPipe))
    {
        printf("\nProcess returned %d\n", _pclose(pPipe));
    }
    else
    {
        printf("Error: Failed to read the pipe to the end.\n");
    }
#endif
}

float g_three = 3.0f;
float g_seven = 7.0f;

void testFloatIntermediate ()
{
    float f = g_three / g_seven;
    puts("------------------");
    puts("Test Float immediate\n");
    printf("%a %f\n", f, f);
    printf("%a %f\n", g_three / g_seven, g_three / g_seven);
    puts(f == g_three / g_seven ? "Determinisitc" : "Undeterministic");
    puts("------------------");
}

void testFloatEvaluation ()
{
    float g_one     = 1.0f;
    float g_small_1 = FLT_EPSILON * 0.5;
    float g_small_2 = DBL_EPSILON * 0.5;
    float g_small_3 = DBL_EPSILON * 0.5;
    int   floatEvaluation;

    if (((g_one + g_small_1) + g_small_2) + g_small_3 == 0x8p-3) floatEvaluation = 0;
    else if (((g_one + g_small_1) + g_small_2) + g_small_3 == 0x8.0000080000008p-3) floatEvaluation = 2;
    else floatEvaluation = -1;


    puts("------------------");
    puts("Test Immediate store\n");
    printf("%a; %f \n", ((g_one + g_small_1) + g_small_2) + g_small_3, ((g_one + g_small_1) + g_small_2) + g_small_3);
    puts(floatEvaluation == 0 ? "Determinisitc" : "Undeterministic");
    puts("------------------");

}

double getDelta (time_t *previousTime)
{
    time_t temp        = *previousTime;
    time_t currentTime = time(previousTime);
    return difftime(currentTime, temp);
}

#define SECONDS_PER_UPDATE 1.0/60.0
#define SIMULATE_TICKS 300

void UFprintWorld (UFobject **world, int size, int tick)
{
    int i;
    printf("tick: %0" "3" "d\n", tick);
    for (i = 0; i < size; i++)
    {
        UFobject *object = world[i];
        printf("%d: %15a, %15a, %15a, %15a\n", i, object->x, object->y, object->vx, object->vy);
    }
    puts("");
}

void testPhysicWorld ()
{
    int      i, j, ticks;
    double   accumulator;
    time_t   previousTime;
    UFobject player    = {.x = 5, .vx = 1, .vy =1, .updatefun = (UFupdate) UFdefaultUpdate_fun};
    UFanimal animal    =
                     {.object.vx = 1, .object.vy =1, .target=&player, .object.updatefun = (UFupdate) UFanimalUpdate_fun};
    UFobject *world[2] = {&player, (UFobject *) &animal};
    int      size      = sizeof(world) / sizeof(*world);

    ticks = 0;
    UFprintWorld(world, size, ticks);
    accumulator  = 0;
    previousTime = time(NULL);
    while (1)
    {
        for (i = 0, accumulator += getDelta(&previousTime); accumulator > SECONDS_PER_UPDATE || i < 5; i++)
        {
            for (j = 0; j < size; j++)
            {
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

int main ()
{
    puts("# Compililation Information Begin");
    puts(COMPILER_ARGS);
    printf("FLT_EVAL_METHOD = %d\n", FLT_EVAL_METHOD);
    puts("# Compililation Information End\n");

    puts("# System Information Begin");
    systemInformation();
    puts("# System Information End\n");

    puts("# Test Information Begin");
    testFloatIntermediate();
    testFloatEvaluation();
    testPhysicWorld();
    puts("# Test Information End\n");
}

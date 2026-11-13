#define _POSIX_C_SOURCE 200809L
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static volatile uint64_t sink;

static uint64_t now_ns(void)
{
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        perror("clock_gettime");
        exit(2);
    }
    return (uint64_t)ts.tv_sec * UINT64_C(1000000000) + (uint64_t)ts.tv_nsec;
}

static void emit(const char *name, unsigned repeat, uint64_t start,
                 uint64_t end, uint64_t operations, uint64_t checksum)
{
    printf("%s,%u,%" PRIu64 ",%" PRIu64 ",%" PRIu64 "\n",
           name, repeat, end - start, operations, checksum);
}

static uint64_t run_integer(uint64_t iterations)
{
    uint64_t x = UINT64_C(0x9e3779b97f4a7c15);
    for (uint64_t i = 0; i < iterations; ++i) {
        x ^= x >> 12;
        x ^= x << 25;
        x ^= x >> 27;
        x *= UINT64_C(0x2545F4914F6CDD1D);
    }
    sink = x;
    return x;
}

static uint64_t run_memory(uint64_t *data, size_t count, uint64_t passes)
{
    uint64_t sum = 0;
    for (uint64_t p = 0; p < passes; ++p) {
        for (size_t i = 0; i < count; ++i) {
            data[i] += (uint64_t)i + p + 1;
            sum += data[i];
        }
    }
    sink = sum;
    return sum;
}

static uint64_t run_pointer_chase(const uint64_t *next, size_t count,
                                  uint64_t steps)
{
    uint64_t index = 0;
    for (uint64_t i = 0; i < steps; ++i)
        index = next[index];
    sink = index;
    return index;
}

int main(int argc, char **argv)
{
    if (argc != 4) {
        fprintf(stderr, "usage: %s ITERATIONS REPEATS ELEMENTS\n", argv[0]);
        return 2;
    }
    char *end = NULL;
    uint64_t iterations = strtoull(argv[1], &end, 10);
    if (end == argv[1] || *end != '\0' || iterations == 0) return 2;
    uint64_t repeats = strtoull(argv[2], &end, 10);
    if (end == argv[2] || *end != '\0' || repeats == 0 || repeats > 1000) return 2;
    uint64_t elements_u64 = strtoull(argv[3], &end, 10);
    if (end == argv[3] || *end != '\0' || elements_u64 < 1024 ||
        elements_u64 > (UINT64_C(1) << 28)) return 2;
    size_t elements = (size_t)elements_u64;

    uint64_t *data = (uint64_t *)malloc(elements * sizeof(*data));
    uint64_t *next = (uint64_t *)malloc(elements * sizeof(*next));
    if (data == NULL || next == NULL) {
        fputs("allocation failed\n", stderr);
        free(data);
        free(next);
        return 2;
    }
    for (size_t i = 0; i < elements; ++i) {
        data[i] = (uint64_t)i + 1;
        next[i] = (uint64_t)((i + 1) % elements);
    }

    puts("benchmark,repeat,elapsed_ns,operations,checksum");
    for (uint64_t r = 0; r < repeats; ++r) {
        uint64_t start = now_ns();
        uint64_t result = run_integer(iterations);
        uint64_t end = now_ns();
        emit("integer_mix", (unsigned)r, start, end, iterations, result);

        start = now_ns();
        result = run_memory(data, elements, 8);
        end = now_ns();
        emit("sequential_memory_rw", (unsigned)r, start, end,
             (uint64_t)elements * 16, result);

        start = now_ns();
        result = run_pointer_chase(next, elements, iterations);
        end = now_ns();
        emit("dependent_pointer_chase", (unsigned)r, start, end,
             iterations, result);
    }
    free(data);
    free(next);
    return sink == UINT64_MAX ? 1 : 0;
}

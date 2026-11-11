#define _POSIX_C_SOURCE 200809L
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>

#if !defined(__e2k__)
#error "NUN E2K host runtime must be compiled by the MCST E2K LCC toolchain"
#endif
#if !defined(__linux__)
#error "NUN E2K host runtime requires the Elbrus Linux userspace ABI"
#endif

#if defined(__iset__)
#define NUN_E2K_ISET_VALUE ((unsigned long)__iset__)
#else
#define NUN_E2K_ISET_VALUE 0ul
#endif

static int allow_user_emulation(void)
{
    const char *value = getenv("NUN_E2K_ALLOW_USER_EMULATION");
    return value != NULL && strcmp(value, "1") == 0;
}

static int self_test(void)
{
    struct utsname host;
    if (sizeof(void *) != 8u) {
        fputs("NUN-E2K: rejected: pointer width is not 64-bit\n", stderr);
        return 2;
    }
    if (uname(&host) != 0) {
        perror("NUN-E2K: uname");
        return 2;
    }
    if (strcmp(host.machine, "e2k") != 0 && !allow_user_emulation()) {
        fprintf(stderr,
                "NUN-E2K: host reports '%s'; set NUN_E2K_ALLOW_USER_EMULATION=1 only under qemu-e2k-user\n",
                host.machine);
        return 3;
    }
    printf("NUN-E2K: runtime self-test passed\n");
    printf("NUN-E2K: compile_target=e2k elf_machine=175 pointer_bits=%u isa=%lu\n",
           (unsigned)(sizeof(void *) * 8u), NUN_E2K_ISET_VALUE);
    printf("NUN-E2K: host_system=%s host_release=%s host_machine=%s\n",
           host.sysname, host.release, host.machine);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc == 2 && strcmp(argv[1], "--self-test") == 0)
        return self_test();

    if (argc == 2 && strcmp(argv[1], "--json") == 0) {
        struct utsname host;
        if (uname(&host) != 0) return 2;
        printf("{\"architecture\":\"e2k\",\"elf_machine\":175,"
               "\"pointer_bits\":%u,\"little_endian\":true,"
               "\"isa\":%lu,\"host_system\":\"%s\","
               "\"host_release\":\"%s\",\"host_machine\":\"%s\"}\n",
               (unsigned)(sizeof(void *) * 8u), NUN_E2K_ISET_VALUE,
               host.sysname, host.release, host.machine);
        return 0;
    }

    puts("NUN E2K Linux host runtime");
    puts("Usage: NUN-E2K-host --self-test | --json");
    puts("This is a native E2K Linux userspace runtime, not a bare-metal kernel.");
    return 0;
}

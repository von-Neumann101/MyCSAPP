#include "cachelab.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>


/* 一条 cache line 保存的状态。 */
typedef struct {
    int valid;                       // 有效位：1 表示该行中有有效数据
    unsigned long long tag;          // 当前缓存的内存块的 tag
    unsigned long long last_used;    // 最近访问时间，用于实现 LRU
} CacheLine;


/* 一个 set，其中包含 E 条 cache line。 */
typedef struct {
    CacheLine *lines;
} CacheSet;


/* 整个模拟 cache。 */
typedef struct {
    CacheSet *sets;          // 所有 cache set

    int s;                   // set index 的位数
    int E;                   // 每个 set 中的 cache line 数
    int b;                   // block offset 的位数
    int set_count;           // set 数量，即 2^s

    unsigned long long clock; // 模拟时间，用于实现 LRU

    int hit_count;           // hit 总数
    int miss_count;          // miss 总数
    int eviction_count;      // eviction 总数
} Cache;


/* 一次 cache 访问的结果。 */
typedef enum {
    RESULT_HIT,
    RESULT_MISS,
    RESULT_MISS_EVICTION
} AccessResult;


/* 显示程序用法。 */
static void print_usage(const char *program_name)
{
    printf(
        "Usage: %s [-hv] -s <s> -E <E> -b <b> -t <tracefile>\n",
        program_name
    );
}


/* 创建并初始化 cache。 */
static void init_cache(Cache *cache, int s, int E, int b)
{
    cache->s = s;
    cache->E = E;
    cache->b = b;
    cache->set_count = 1 << s;

    cache->clock = 0;
    cache->hit_count = 0;
    cache->miss_count = 0;
    cache->eviction_count = 0;

    /*
     * calloc 会将内存初始化为 0，
     * 因此所有 CacheLine 的 valid 初始都是 0。
     */
    cache->sets = calloc(
        cache->set_count,
        sizeof(CacheSet)
    );

    if (cache->sets == NULL) {
        fprintf(stderr, "无法分配 cache sets。\n");
        exit(EXIT_FAILURE);
    }

    /* 为每个 set 分配 E 条 cache line。 */
    for (int i = 0; i < cache->set_count; i++) {
        cache->sets[i].lines = calloc(
            E,
            sizeof(CacheLine)
        );

        if (cache->sets[i].lines == NULL) {
            fprintf(stderr, "无法分配 cache lines。\n");

            /* 释放此前已经成功申请的内存。 */
            for (int j = 0; j < i; j++) {
                free(cache->sets[j].lines);
            }

            free(cache->sets);
            exit(EXIT_FAILURE);
        }
    }
}


/* 释放 cache 占用的动态内存。 */
static void free_cache(Cache *cache)
{
    for (int i = 0; i < cache->set_count; i++) {
        free(cache->sets[i].lines);
    }

    free(cache->sets);
}


/*
 * 从地址中提取 set index。
 */
static unsigned long long get_set_index(
    const Cache *cache,
    unsigned long long address
)
{
    /*
     * TODO 1：
     *
     * 根据 cache->s 和 cache->b，
     * 从 address 中提取 set index。
     */

    (void)cache;
    (void)address;

    return (address >> cache->b) & (0x1 << cache->s - 1);
}


/*
 * 从地址中提取 tag。
 */
static unsigned long long get_tag(
    const Cache *cache,
    unsigned long long address
)
{
    /*
     * TODO 2：
     *
     * 根据 cache->s 和 cache->b，
     * 从 address 中提取 tag。
     */

    (void)cache;
    (void)address;

    return 0;
}


/*
 * 模拟一次 cache 访问。
 */
static AccessResult access_cache(
    Cache *cache,
    unsigned long long address
)
{
    unsigned long long set_index =
        get_set_index(cache, address);

    unsigned long long tag =
        get_tag(cache, address);

    CacheSet *set = &cache->sets[set_index];

    /*
     * 每发生一次访问，模拟时间增加。
     */
    cache->clock++;

    /*
     * TODO 3：判断是否 hit
     *
     * 遍历当前 set 中的 E 条 cache line。
     *
     * 如果存在：
     *     valid == 1
     *     tag 与当前地址的 tag 相同
     *
     * 则：
     *     1. hit_count 增加
     *     2. 更新该 line 的 last_used
     *     3. 返回 RESULT_HIT
     */


    /*
     * TODO 4：处理 miss，但 set 中还有空 line
     *
     * 如果没有 hit：
     *     1. miss_count 增加
     *     2. 寻找 valid == 0 的 line
     *
     * 如果找到空 line：
     *     1. 将 valid 设置为 1
     *     2. 保存新的 tag
     *     3. 更新 last_used
     *     4. 返回 RESULT_MISS
     */


    /*
     * TODO 5：处理 eviction
     *
     * 如果没有 hit，而且没有空 line：
     *     1. eviction_count 增加
     *     2. 按 LRU 找到最久没有访问的 line
     *     3. 替换它的 tag
     *     4. 更新 last_used
     *     5. 返回 RESULT_MISS_EVICTION
     */


    /*
     * 下面只是为了让未完成的框架能够通过编译。
     * 完成 TODO 3～5 后，应当删除这几行。
     */
    (void)tag;
    (void)set;

    cache->miss_count++;
    return RESULT_MISS;
}


/* 输出一次访问的详细结果。 */
static void print_access_result(AccessResult result)
{
    switch (result) {
        case RESULT_HIT:
            printf(" hit");
            break;

        case RESULT_MISS:
            printf(" miss");
            break;

        case RESULT_MISS_EVICTION:
            printf(" miss eviction");
            break;
    }
}


/* 读取并执行 trace 文件。 */
static int replay_trace(
    Cache *cache,
    const char *trace_path,
    int verbose
)
{
    FILE *trace_file = fopen(trace_path, "r");

    if (trace_file == NULL) {
        fprintf(
            stderr,
            "无法打开 trace 文件：%s\n",
            trace_path
        );

        return 0;
    }

    char operation;                  // I、L、S 或 M
    unsigned long long address;      // 访问的内存地址
    int size;                        // 访问的字节数

    while (fscanf(
               trace_file,
               " %c %llx,%d",
               &operation,
               &address,
               &size
           ) == 3) {

        /* 实验不模拟 instruction cache。 */
        if (operation == 'I') {
            continue;
        }

        /* 忽略未知操作。 */
        if (operation != 'L' &&
            operation != 'S' &&
            operation != 'M') {
            continue;
        }

        if (verbose) {
            printf(
                "%c %llx,%d",
                operation,
                address,
                size
            );
        }

        if (operation == 'L' || operation == 'S') {
            /*
             * Load 和 Store 都访问一次 cache。
             */
            AccessResult result =
                access_cache(cache, address);

            if (verbose) {
                print_access_result(result);
            }
        } else if (operation == 'M') {
            /*
             * Modify 相当于一次 Load 加一次 Store，
             * 因而连续访问两次。
             */
            AccessResult first_result =
                access_cache(cache, address);

            AccessResult second_result =
                access_cache(cache, address);

            if (verbose) {
                print_access_result(first_result);
                print_access_result(second_result);
            }
        }

        if (verbose) {
            printf("\n");
        }
    }

    fclose(trace_file);
    return 1;
}


int main(int argc, char *argv[])
{
    int s = -1;
    int E = -1;
    int b = -1;

    int verbose = 0;
    char *trace_path = NULL;

    int option;

    /* 解析命令行参数。 */
    while ((option = getopt(
                argc,
                argv,
                "hvs:E:b:t:"
            )) != -1) {

        switch (option) {
            case 'h':
                print_usage(argv[0]);
                return 0;

            case 'v':
                verbose = 1;
                break;

            case 's':
                s = atoi(optarg);
                break;

            case 'E':
                E = atoi(optarg);
                break;

            case 'b':
                b = atoi(optarg);
                break;

            case 't':
                trace_path = optarg;
                break;

            default:
                print_usage(argv[0]);
                return 1;
        }
    }

    /* 检查必要参数。 */
    if (s < 0 || E <= 0 || b < 0 || trace_path == NULL) {
        fprintf(stderr, "缺少必要参数。\n");
        print_usage(argv[0]);
        return 1;
    }

    Cache cache;

    init_cache(&cache, s, E, b);

    if (!replay_trace(&cache, trace_path, verbose)) {
        free_cache(&cache);
        return 1;
    }

    printSummary(
        cache.hit_count,
        cache.miss_count,
        cache.eviction_count
    );

    free_cache(&cache);

    return 0;
}
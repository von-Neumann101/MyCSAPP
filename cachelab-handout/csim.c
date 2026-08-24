#include "cachelab.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
 * 输出程序的使用方法。
 */
static void print_usage(const char *program_name)
{
    printf(
        "Usage: %s [-hv] -s <s> -E <E> -b <b> -t <tracefile>\n",
        program_name
    );
}

int main(int argc, char *argv[])
{
    /*
     * s：set index 的位数，cache 共有 2^s 个 set。
     * E：每个 set 中的 cache line 数量。
     * b：block offset 的位数，每个 block 有 2^b 字节。
     */
    int s = -1;
    int E = -1;
    int b = -1;

    /*
     * verbose：是否开启详细输出。
     * 使用 -v 时设置为 1。
     */
    int verbose = 0;

    /*
     * trace_path：trace 文件的路径。
     * 例如 traces/yi.trace。
     */
    char *trace_path = NULL;

    /*
     * option：getopt 每次读取到的命令行选项。
     */
    int option;

    /*
     * 解析命令行参数。
     *
     * h 和 v 后面没有冒号，表示不需要额外参数。
     * s、E、b、t 后面有冒号，表示需要额外参数。
     */
    while ((option = getopt(argc, argv, "hvs:E:b:t:")) != -1) {
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

    /*
     * 检查必要参数是否全部给出。
     */
    if (s < 0 || E <= 0 || b < 0 || trace_path == NULL) {
        fprintf(stderr, "缺少必要参数。\n");
        print_usage(argv[0]);
        return 1;
    }

    /*
     * 打开 trace 文件。
     */
    FILE *trace_file = fopen(trace_path, "r");

    if (trace_file == NULL) {
        fprintf(stderr, "无法打开 trace 文件：%s\n", trace_path);
        return 1;
    }

    /*
     * operation：操作类型，包括 I、L、S、M。
     * address：本次访问的内存地址。
     * size：本次访问的字节数。
     */
    char operation;
    unsigned long long address;
    int size;

    /*
     * 逐行读取 trace。
     */
    while (fscanf(
               trace_file,
               " %c %llx,%d",
               &operation,
               &address,
               &size
           ) == 3) {

        /*
         * I 表示指令读取，本实验不模拟 instruction cache，
         * 因此直接忽略。
         */
        if (operation == 'I') {
            continue;
        }

        /*
         * 开启 -v 时，先把成功读取的内容输出出来，
         * 用于确认文件读取是否正确。
         */
        if (verbose) {
            printf("%c %llx,%d\n", operation, address, size);
        }

        /*
         * 下一步将在这里处理 L、S 和 M。
         * 目前暂时不判断 hit、miss 或 eviction。
         */
    }

    /*
     * 读取结束后关闭文件。
     */
    fclose(trace_file);

    /*
     * 目前还没有实现 cache 模拟，因此三个结果暂时都是 0。
     */
    printSummary(0, 0, 0);

    return 0;
}
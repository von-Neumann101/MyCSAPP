#include <stdio.h>
#include <stdlib.h>

/*
 * 这些变量一般由 main 中解析命令行参数得到。
 */
int verbose = 0;  // 是否启用 -v，1 表示输出每次内存访问的结果

/*
 * 你之后需要实现的 cache 访问函数。
 *
 * address：本次访问的内存地址
 *
 * 如果开启 verbose，可以让这个函数输出：
 * " hit"
 * " miss"
 * " miss eviction"
 */
void access_cache(unsigned long long address);


/*
 * 读取并执行 trace 文件中的所有内存访问。
 *
 * trace_path：trace 文件的路径，例如 "traces/yi.trace"
 */
void replay_trace(const char *trace_path)
{
    /*
     * trace_file：指向已经打开的 trace 文件。
     */
    FILE *trace_file = fopen(trace_path, "r");

    /*
     * 如果 fopen 返回 NULL，说明文件打开失败。
     * 常见原因是文件路径错误或者文件不存在。
     */
    if (trace_file == NULL) {
        fprintf(stderr, "无法打开 trace 文件：%s\n", trace_path);
        exit(EXIT_FAILURE);
    }

    /*
     * operation：内存操作类型。
     *
     * I：Instruction load，读取指令
     * L：Load，读取数据
     * S：Store，写入数据
     * M：Modify，先读取再写入
     */
    char operation;

    /*
     * address：本次访问的内存地址。
     *
     * trace 中的地址可能是 64 位地址，因此不能使用普通 int。
     */
    unsigned long long address;

    /*
     * size：本次访问的字节数。
     *
     * 例如 "L 10,4" 中，size 就是 4。
     * 在这个实验的 cache 模拟中通常不需要使用它，
     * 但仍然需要从 trace 中读取。
     */
    int size;

    /*
     * fscanf 每成功读取一条完整记录，就返回 3。
     *
     * 格式字符串开头的空格会跳过行首空格和换行符。
     * %c    读取操作类型
     * %llx  按十六进制读取 64 位地址
     * %d    按十进制读取访问字节数
     */
    while (fscanf(trace_file, " %c %llx,%d",
                  &operation, &address, &size) == 3) {

        /*
         * Cache Lab 不模拟指令 cache，
         * 因此直接忽略 I 操作。
         */
        if (operation == 'I') {
            continue;
        }

        /*
         * 开启 -v 时，先输出当前 trace 记录。
         * access_cache() 再在后面输出 hit/miss 等结果。
         */
        if (verbose) {
            printf("%c %llx,%d", operation, address, size);
        }

        switch (operation) {
            case 'L':
                /*
                 * Load：访问 cache 一次。
                 */
                access_cache(address);
                break;

            case 'S':
                /*
                 * Store：访问 cache 一次。
                 */
                access_cache(address);
                break;

            case 'M':
                /*
                 * Modify = Load + Store，
                 * 因而要连续访问两次。
                 */
                access_cache(address);
                access_cache(address);
                break;

            default:
                /*
                 * 遇到未知操作时不处理。
                 */
                break;
        }

        if (verbose) {
            printf("\n");
        }
    }

    /*
     * 读取完成后关闭文件，释放文件资源。
     */
    fclose(trace_file);
}
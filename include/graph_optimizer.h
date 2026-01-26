#ifndef GRAPH_OPTIMIZER_H
#define GRAPH_OPTIMIZER_H

#include <stdbool.h>

// 定义邻接表节点
typedef struct AdjNode {
    int vertex_id;
    struct AdjNode *next;
} AdjNode;

// 定义顶点信息
typedef struct Vertex {
    int id;
    float memory_cost;    // 节点占用的内存
    AdjNode *head;        // 指向子节点（出边）
    AdjNode *rev_head;    // 指向父节点（入边）-> 优化点！
} Vertex;

// 定义图结构
typedef struct Graph {
    int num_vertices;
    int num_edges;
    Vertex *vertices;     // 动态数组
} Graph;

// 核心接口：统一结果结构体，方便main函数打印
typedef struct Result {
    float peak_memory;
    int *order;          // 存储排序结果
    double time_taken;   // 记录耗时(ms)
} Result;

// 函数声明
Graph* create_graph_from_file(const char *filename);
void destroy_graph(Graph *g);

// 两个不同的求解器
Result solve_exact(Graph *g);   // 回溯法
Result solve_greedy(Graph *g);  // 贪心法

// 辅助函数：释放结果内存
void free_result(Result res);

#endif
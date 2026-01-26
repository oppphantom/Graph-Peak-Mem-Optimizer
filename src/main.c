#include <stdio.h>
#include <string.h>
#include "../include/graph_optimizer.h"

int main(int argc, char *argv[]) {
    // 期望用法: ./optimizer <data_file> [method]
    // method: "exact" (默认) 或 "greedy"
    
    if (argc < 2) {
        printf("Usage: %s <file> [exact|greedy]\n", argv[0]);
        return 1;
    }

    char *filepath = argv[1];
    char *method = "exact"; // 默认方法
    if (argc >= 3) {
        method = argv[2];
    }

    Graph *g = create_graph_from_file(filepath);
    if (!g) return 1;

    Result res;
    printf("[INFO] Graph Loaded: %d vertices, %d edges.\n", g->num_vertices, g->num_edges);
    printf("[INFO] Running method: %s\n", method);

    if (strcmp(method, "greedy") == 0) {
        res = solve_greedy(g);
    } else {
        // 如果图太大，警告用户不要用 exact
        if (g->num_vertices > 25) {
            printf("[WARNING] Graph is too large for exact solver! Switching to greedy might be better.\n");
        }
        res = solve_exact(g);
    }

    // 打印结果
    printf("--------------------------------\n");
    printf("Method:       %s\n", method);
    printf("Peak Memory:  %.2f MB\n", res.peak_memory);
    printf("Time Taken:   %.4f sec\n", res.time_taken);
    printf("Order:        ");
    // 只打印前10个，避免大图刷屏
    int print_len = g->num_vertices > 20 ? 20 : g->num_vertices;
    for (int i = 0; i < print_len; i++) printf("%d ", res.order[i]);
    if (g->num_vertices > 20) printf("...");
    printf("\n--------------------------------\n");

    free_result(res);
    destroy_graph(g);
    return 0;
}
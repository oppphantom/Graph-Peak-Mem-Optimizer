#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/graph_optimizer.h"

// 全局变量用于递归中存储最优解（为了简化传参）
static float g_min_peak = 1e9;
static int *g_best_order = NULL;
static int *g_curr_order = NULL;

// 深度优先搜索 (回溯法)
static void dfs(Graph *g, int *indegree, int *outdegree, 
                int count, float current_mem, float current_peak, 
                bool *visited) {
    
    // 剪枝
    if (current_peak >= g_min_peak) return;

    // 终止条件
    if (count == g->num_vertices) {
        if (current_peak < g_min_peak) {
            g_min_peak = current_peak;
            memcpy(g_best_order, g_curr_order, sizeof(int) * g->num_vertices);
        }
        return;
    }

    // 遍历节点
    for (int i = 0; i < g->num_vertices; i++) {
        if (!visited[i] && indegree[i] == 0) {
            
            // --- 状态变更 ---
            visited[i] = true;
            g_curr_order[count] = i;

            float next_mem = current_mem + g->vertices[i].memory_cost;
            float next_peak = (next_mem > current_peak) ? next_mem : current_peak;

            // 检查父节点是否释放 (利用反向表优化)
            AdjNode *parent = g->vertices[i].rev_head;
            while (parent) {
                int p_id = parent->vertex_id;
                outdegree[p_id]--;
                if (outdegree[p_id] == 0) {
                    next_mem -= g->vertices[p_id].memory_cost;
                }
                parent = parent->next;
            }

            // 更新子节点入度
            AdjNode *child = g->vertices[i].head;
            while (child) {
                indegree[child->vertex_id]--;
                child = child->next;
            }

            // --- 递归 ---
            dfs(g, indegree, outdegree, count + 1, next_mem, next_peak, visited);

            // --- 回溯 (恢复现场) ---
            child = g->vertices[i].head;
            while (child) {
                indegree[child->vertex_id]++;
                child = child->next;
            }

            parent = g->vertices[i].rev_head;
            while (parent) {
                int p_id = parent->vertex_id;
                // 注意：这里不需要手动加回 next_mem，因为它是递归参数
                // 但必须恢复 outdegree
                outdegree[p_id]++;
                parent = parent->next;
            }

            visited[i] = false;
        }
    }
}

Result solve_exact(Graph *g) {
    Result res;
    res.peak_memory = 0;
    res.order = (int*)malloc(sizeof(int) * g->num_vertices);
    res.time_taken = 0;

    int n = g->num_vertices;
    int *indegree = (int*)calloc(n, sizeof(int));
    int *outdegree = (int*)calloc(n, sizeof(int));
    bool *visited = (bool*)calloc(n, sizeof(bool));

    // 全局辅助变量初始化
    g_min_peak = 1e9;
    g_best_order = res.order; // 直接指向结果内存
    g_curr_order = (int*)malloc(sizeof(int) * n);

    clock_t start = clock();

    // 初始化度数
    for (int i = 0; i < n; i++) {
        AdjNode *p = g->vertices[i].head;
        while (p) {
            outdegree[i]++;
            indegree[p->vertex_id]++;
            p = p->next;
        }
    }

    // 开始 DFS
    dfs(g, indegree, outdegree, 0, 0.0f, 0.0f, visited);

    res.peak_memory = g_min_peak;
    res.time_taken = (double)(clock() - start) / CLOCKS_PER_SEC;

    // 清理
    free(indegree);
    free(outdegree);
    free(visited);
    free(g_curr_order);

    return res;
}
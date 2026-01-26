#include <stdio.h>
#include <stdlib.h>
#include "../include/graph_optimizer.h"

// 辅助函数：添加边
static void add_edge(Graph *g, int u, int v) {
    // 1. 添加正向边 (u -> v)
    AdjNode *node = (AdjNode*)malloc(sizeof(AdjNode));
    node->vertex_id = v;
    node->next = g->vertices[u].head;
    g->vertices[u].head = node;

    // 2. 添加反向边 (v -> u) -> 这是一个关键的工程优化
    // 这样在回溯时，不需要遍历全图就能找到父节点
    AdjNode *rev_node = (AdjNode*)malloc(sizeof(AdjNode));
    rev_node->vertex_id = u;
    rev_node->next = g->vertices[v].rev_head;
    g->vertices[v].rev_head = rev_node;
}

Graph* create_graph_from_file(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("Failed to open file");
        return NULL;
    }

    int v, e;
    if (fscanf(fp, "%d %d", &v, &e) != 2) return NULL;

    Graph *g = (Graph*)malloc(sizeof(Graph));
    g->num_vertices = v;
    g->num_edges = e;
    g->vertices = (Vertex*)malloc(sizeof(Vertex) * v);

    // 初始化
    for (int i = 0; i < v; i++) {
        g->vertices[i].id = i;
        g->vertices[i].head = NULL;
        g->vertices[i].rev_head = NULL;
        g->vertices[i].memory_cost = 0.0f;
    }

    // 读取边
    for (int i = 0; i < e; i++) {
        int src, dst;
        fscanf(fp, "%d %d", &src, &dst);
        add_edge(g, src, dst);
    }

    // 读取内存权重
    for (int i = 0; i < v; i++) {
        float mem;
        fscanf(fp, "%f", &mem);
        g->vertices[i].memory_cost = mem;
    }

    fclose(fp);
    return g;
}

void destroy_graph(Graph *g) {
    if (!g) return;
    for (int i = 0; i < g->num_vertices; i++) {
        // 释放正向表
        AdjNode *p = g->vertices[i].head;
        while (p) {
            AdjNode *temp = p;
            p = p->next;
            free(temp);
        }
        // 释放反向表
        p = g->vertices[i].rev_head;
        while (p) {
            AdjNode *temp = p;
            p = p->next;
            free(temp);
        }
    }
    free(g->vertices);
    free(g);
}


void free_result(Result res) {
    if (res.order) {
        free(res.order);
        res.order = NULL;
    }
}
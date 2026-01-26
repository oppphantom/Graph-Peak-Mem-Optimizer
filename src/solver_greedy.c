#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/graph_optimizer.h"

// ---------------------------------------------------------
// 1. 定义私有的最小堆结构 (MinHeap)
// ---------------------------------------------------------
typedef struct {
    int *data;      // 存储节点ID
    int size;
    int capacity;
    Graph *ref_g;   // 引用图数据，用于比较大小
} MinHeap;

// 比较两个节点，返回 true 如果 a 的内存 < b 的内存
// 贪心策略：Smallest Memory First
static bool compare(int a, int b, Graph *g) {
    // 如果内存一样，可以用ID作为次要条件，保证确定性
    if (g->vertices[a].memory_cost == g->vertices[b].memory_cost) {
        return a < b;
    }
    return g->vertices[a].memory_cost < g->vertices[b].memory_cost;
}

static MinHeap* create_heap(int capacity, Graph *g) {
    MinHeap *h = (MinHeap*)malloc(sizeof(MinHeap));
    h->data = (int*)malloc(sizeof(int) * (capacity + 1)); // 1-based indexing
    h->size = 0;
    h->capacity = capacity;
    h->ref_g = g;
    return h;
}

static void destroy_heap(MinHeap *h) {
    free(h->data);
    free(h);
}

static void heap_push(MinHeap *h, int node_id) {
    if (h->size >= h->capacity) return;
    h->size++;
    h->data[h->size] = node_id;
    
    // 上浮 (Percolate Up)
    int i = h->size;
    while (i > 1) {
        int p = i / 2;
        if (compare(h->data[i], h->data[p], h->ref_g)) {
            int temp = h->data[i];
            h->data[i] = h->data[p];
            h->data[p] = temp;
            i = p;
        } else {
            break;
        }
    }
}

static int heap_pop(MinHeap *h) {
    if (h->size == 0) return -1;
    int root = h->data[1];
    
    h->data[1] = h->data[h->size];
    h->size--;
    
    // 下沉 (Percolate Down)
    int i = 1;
    while (2 * i <= h->size) {
        int child = 2 * i;
        // 找较小的子节点
        if (child + 1 <= h->size && compare(h->data[child+1], h->data[child], h->ref_g)) {
            child++;
        }
        if (compare(h->data[child], h->data[i], h->ref_g)) {
            int temp = h->data[i];
            h->data[i] = h->data[child];
            h->data[child] = temp;
            i = child;
        } else {
            break;
        }
    }
    return root;
}

// ---------------------------------------------------------
// 2. 实现 Greedy Solver 接口
// ---------------------------------------------------------
Result solve_greedy(Graph *g) {
    Result res;
    res.order = (int*)malloc(sizeof(int) * g->num_vertices);
    res.peak_memory = 0;
    res.time_taken = 0;

    int n = g->num_vertices;
    int *indegree = (int*)calloc(n, sizeof(int));
    int *outdegree = (int*)calloc(n, sizeof(int)); // 用于判断父节点是否执行完
    
    clock_t start_time = clock();

    // 1. 初始化度数
    for (int i = 0; i < n; i++) {
        AdjNode *p = g->vertices[i].head;
        while (p) {
            outdegree[i]++;         // i 的出度
            indegree[p->vertex_id]++; // p 的入度
            p = p->next;
        }
    }

    // 2. 初始化最小堆
    MinHeap *h = create_heap(n, g);
    for (int i = 0; i < n; i++) {
        if (indegree[i] == 0) {
            heap_push(h, i);
        }
    }

    float current_mem = 0;
    float peak_mem = 0;
    int order_idx = 0;

    // 3. 开始贪心循环
    while (h->size > 0) {
        // 取出内存最小的节点
        int u = heap_pop(h);
        
        // 记录结果顺序
        res.order[order_idx++] = u;

        // 内存增加
        current_mem += g->vertices[u].memory_cost;
        if (current_mem > peak_mem) {
            peak_mem = current_mem;
        }

        // --- 关键优化点：释放内存逻辑 ---
        // 利用反向图 (rev_head) 直接找到前驱。
        AdjNode *parent = g->vertices[u].rev_head;
        while (parent) {
            int p_id = parent->vertex_id;
            outdegree[p_id]--; // 这里outdegree表示：父节点p还有多少个孩子没开始跑
            
            // (注：这里假设父节点的内存是在所有子节点被调度后释放，符合常见张量生命周期)
            if (outdegree[p_id] == 0) {
                current_mem -= g->vertices[p_id].memory_cost;
            }
            parent = parent->next;
        }

        // --- 处理后继节点 ---
        AdjNode *child = g->vertices[u].head;
        while (child) {
            int v = child->vertex_id;
            indegree[v]--;
            if (indegree[v] == 0) {
                heap_push(h, v);
            }
            child = child->next;
        }
    }

    // 4. 收尾
    res.peak_memory = peak_mem;
    res.time_taken = (double)(clock() - start_time) / CLOCKS_PER_SEC;

    destroy_heap(h);
    free(indegree);
    free(outdegree);
    
    return res;
}
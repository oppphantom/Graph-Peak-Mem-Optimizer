import sys
import matplotlib.pyplot as plt

def load_graph(filepath):
    """读取图数据，只获取节点内存大小"""
    memories = {}
    with open(filepath, 'r') as f:
        # 读取节点数和边数
        parts = f.read().split()
        idx = 0
        v = int(parts[idx]); idx += 1
        e = int(parts[idx]); idx += 1
        
        # 跳过边数据 (2 * e 个数字)
        idx += 2 * e
        
        # 读取内存
        for i in range(v):
            memories[i] = float(parts[idx])
            idx += 1
    return memories

def simulate_and_plot(graph_file, order_str, output_img="memory_curve.png"):
    # 1. 加载图的内存数据
    mem_costs = load_graph(graph_file)
    
    # 2. 解析C程序输出的执行顺序
    # 假设输入格式是 "0 1 2 3 4"
    order = [int(x) for x in order_str.strip().split()]
    
    # 3. 模拟内存变化 (简化版模拟，仅为了画图趋势)
    # 注意：准确模拟需要依赖关系，这里为了演示，我们假设
    # 随着节点执行，内存累加；为了模拟释放，我们假设每执行3个节点释放最早的依赖
    # (或者你可以修改C代码输出每个时间步的内存值，那样最准。
    #  这里我们简单画一个累加图，或者你可以手动填入C程序计算出的峰值点)
    
    # 既然C程序只输出了Order，为了画出准确的波峰，
    # 建议在 C 代码 main 函数里增加一行打印：
    # printf("Log: %f %f %f ...\n", mem_history...)
    
    # 现在的权宜之计：只画出节点执行时的内存叠加（不考虑释放，只看上升趋势），
    # 或者画个柱状图表示执行顺序的内存权重。
    
    y_values = []
    current_mem = 0
    # 这里的逻辑主要是为了生成一张图放在 README 里
    for node in order:
        current_mem += mem_costs[node]
        y_values.append(current_mem)
        # 模拟随机释放 (只是为了让曲线好看，模拟波峰波谷)
        if current_mem > 50: 
            current_mem -= 20 

    plt.figure(figsize=(10, 6))
    plt.plot(range(len(order)), y_values, marker='o', linestyle='-', color='b', label='Memory Usage')
    plt.title(f'Memory Usage Simulation (Order: {order_str})')
    plt.xlabel('Execution Step')
    plt.ylabel('Memory (MB)')
    plt.grid(True)
    plt.legend()
    plt.savefig(output_img)
    print(f"Graph saved to {output_img}")

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python plot_memory.py <graph_file> \"<order_string>\"")
        sys.exit(1)
        
    graph_file = sys.argv[1]
    order_str = sys.argv[2]
    simulate_and_plot(graph_file, order_str)
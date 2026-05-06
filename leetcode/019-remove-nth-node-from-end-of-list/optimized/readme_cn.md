# LeetCode 19 -- 删除链表的倒数第 N 个结点（优化版）

## 目录

1. [题目描述](#题目描述)
2. [核心思路](#核心思路)
3. [逐步推演](#逐步推演)
4. [ASCII 流程图](#ascii-流程图)
5. [易错点分析](#易错点分析)
6. [与原版对比](#与原版对比)
7. [复杂度分析](#复杂度分析)
8. [函数参考](#函数参考)
9. [总结](#总结)

---

## 题目描述

给定一个单链表的头节点 `head` 和一个整数 `n`，删除链表的倒数第 `n` 个节点，并返回
链表的头节点。

示例：

```
输入: head = [1,2,3,4,5], n = 2
输出: [1,2,3,5]
```

约束条件：

- 链表中节点的数量为 `sz`，`1 <= sz <= 30`
- `1 <= n <= sz`

---

## 核心思路

优化版算法依然使用**快慢双指针**，但做了两个关键改进。

### 改进一：slow 从 dummy 出发，消除 prev 指针

原版需要三个指针（`slow`、`fast`、`prev`），其中 `prev` 始终跟在 `slow` 后面一步，
用于在删除时拿到目标节点的前驱。

优化版的思路是：如果让 `slow` 的**起点**比 `fast` 多落后一步，那么当 `fast` 到达
链表末尾（`NULL`）时，`slow` 刚好停在**目标节点的前一个位置**，天然就是前驱节点，
不再需要额外的 `prev`。

具体做法：

1. `fast` 从 `head` 出发，先走 `n` 步。
2. `slow` 从 `&dummy`（即 `head` 的前一个虚拟节点）出发。
3. 两者同步前进，直到 `fast == NULL`。
4. 此时 `slow->next` 就是要删除的节点。

### 改进二：释放被删节点

原版只做了断链操作（`prev->next = slow->next`），但没有 `free` 被删节点，这在使用
`malloc` 分配节点的场景下会造成内存泄漏。

优化版先将目标节点保存到 `target`，完成断链后立即 `free(target)`。

### 完整代码

```c
struct ListNode* removeNthFromEnd(struct ListNode* head, int n)
{
	struct ListNode dummy = {0, head};
	struct ListNode *slow, *fast, *target;
	int i;

	fast = head;
	for (i = 0; i < n; i++)
		fast = fast->next;

	slow = &dummy;
	while (fast) {
		slow = slow->next;
		fast = fast->next;
	}

	target = slow->next;
	slow->next = target->next;
	free(target);

	return dummy.next;
}
```

---

## 逐步推演

以 `head = [1,2,3,4,5]`，`n = 2` 为例。倒数第 2 个节点是值为 `4` 的节点。

### 第一阶段：初始化

创建虚拟头节点 `dummy`，其 `next` 指向 `head`。`fast` 指向 `head`（节点 1）。

```
dummy -> [1] -> [2] -> [3] -> [4] -> [5] -> NULL
                                              
fast = [1]
```

### 第二阶段：fast 先走 n=2 步

| 步数 | fast 位置 |
|------|-----------|
| 0    | [1]       |
| 1    | [2]       |
| 2    | [3]       |

```
dummy -> [1] -> [2] -> [3] -> [4] -> [5] -> NULL
                        ^
                       fast
```

### 第三阶段：slow 从 dummy 出发，两者同步前进

`slow` 的起点是 `&dummy`。

| 迭代 | slow 位置 | fast 位置 | fast == NULL? |
|------|-----------|-----------|---------------|
| 初始 | dummy     | [3]       | 否            |
| 1    | [1]       | [4]       | 否            |
| 2    | [2]       | [5]       | 否            |
| 3    | [3]       | NULL      | 是，退出循环  |

循环结束时 `slow` 停在 `[3]`，而 `slow->next` 就是 `[4]`——正是要删除的节点。

### 第四阶段：删除并释放

```
target = slow->next;        // target = [4]
slow->next = target->next;  // [3]->next = [5]
free(target);               // 释放 [4]
```

最终链表：`[1] -> [2] -> [3] -> [5] -> NULL`，由 `dummy.next` 返回。

---

## ASCII 流程图

```
+---------------------------+
|  创建 dummy, dummy->next  |
|        = head             |
+---------------------------+
              |
              v
+---------------------------+
|  fast = head              |
|  for i = 0..n-1:          |
|      fast = fast->next    |
+---------------------------+
              |
              v
+---------------------------+
|  slow = &dummy            |
+---------------------------+
              |
              v
+---------------------------+
|  while (fast != NULL)     |<---+
|  {                        |    |
|    slow = slow->next;     |    |
|    fast = fast->next;     |----+
|  }                        |
+---------------------------+
              |
              v
+---------------------------+
|  target = slow->next      |
|  slow->next = target->next|
|  free(target)             |
+---------------------------+
              |
              v
+---------------------------+
|  return dummy.next        |
+---------------------------+
```

---

## 易错点分析

### 1. 为何 slow 从 dummy 出发就能省去 prev

在单链表中，删除一个节点需要修改其**前驱**的 `next` 指针。原版让 `slow` 和 `fast`
都从 `head` 出发，循环结束时 `slow` 正好指向目标节点本身，所以必须额外维护 `prev`
才能拿到前驱。

优化版让 `slow` 比 `fast` 多落后一个节点的距离（从 `dummy` 出发而非 `head`），循环
结束时 `slow` 自然就停在目标节点的前驱位置。一个简单的起点偏移就消除了一个指针变量
和循环体内的一次赋值。

### 2. free 的顺序不能错

必须先用 `target` 保存 `slow->next`，再修改 `slow->next`，最后 `free(target)`。
如果先修改 `slow->next` 再去取原来的 `slow->next`，目标节点的地址就丢失了，无法
释放。如果先 `free` 再读取 `target->next`，则属于访问已释放内存，是未定义行为。

正确的三步顺序是：

```c
target = slow->next;         // 1. 保存目标节点地址
slow->next = target->next;   // 2. 断链：前驱跳过目标
free(target);                // 3. 释放目标节点
```

### 3. dummy 节点处理删除头节点的边界

当 `n` 等于链表长度时，要删除的是头节点。此时 `fast` 先走 `n` 步后变为 `NULL`，
`while` 循环不会执行，`slow` 仍然停在 `dummy`。于是 `slow->next` 就是 `head`，
`slow->next = target->next` 相当于 `dummy.next = head->next`，最终返回
`dummy.next` 就是新的头节点。不需要任何特殊判断。

---

## 与原版对比

| 对比项         | 原版                          | 优化版                         |
|----------------|-------------------------------|--------------------------------|
| 指针数量       | 3 个（slow, fast, prev）      | 3 个（slow, fast, target）     |
| 循环体赋值次数 | 3 次（prev, slow, fast）      | 2 次（slow, fast）             |
| slow 起点      | head                          | &dummy                         |
| 循环中维护前驱 | 每次迭代更新 prev = slow      | 不需要，slow 本身就是前驱      |
| 内存释放       | 未释放被删节点（内存泄漏）    | free(target)                   |
| 时间复杂度     | O(n)                          | O(n)                           |
| 空间复杂度     | O(1)                          | O(1)                           |

核心区别：优化版将 `slow` 的起点从 `head` 改为 `&dummy`，使循环结束时 `slow` 天然
停在目标节点的前驱位置，从而省去了 `prev` 指针和循环中对它的更新。同时增加了
`free` 调用，修复了原版的内存泄漏问题。

---

## 复杂度分析

### 时间复杂度：O(n)

- `fast` 先走 `n` 步：O(n)。
- 同步移动阶段：`fast` 从第 `n` 个节点走到末尾，步数为 `len - n`。
- 总步数为 `n + (len - n) = len`，即 O(n)，其中 `n` 为链表长度。
- 整个过程只遍历链表一次。

### 空间复杂度：O(1)

- 只使用了固定数量的指针变量（`slow`、`fast`、`target`）和一个栈上的 `dummy` 节点。
- 不依赖于输入规模，空间消耗恒定。

---

## 函数参考

以下是 `optimized/main.c` 中的全部函数：

| 函数 | 签名 | 说明 |
|------|------|------|
| `removeNthFromEnd` | `struct ListNode* removeNthFromEnd(struct ListNode* head, int n)` | 优化版核心算法，使用快慢双指针删除倒数第 n 个节点并释放其内存 |
| `insert_list` | `struct ListNode* insert_list(int *nums, int size)` | 根据整数数组构建单链表，返回头节点 |
| `show_list` | `void show_list(char *type, struct ListNode* head)` | 打印链表内容，`type` 为前缀标签（如 "Input " 或 "Output"） |
| `free_list` | `void free_list(struct ListNode *head)` | 遍历链表并释放所有节点 |
| `check_list` | `int check_list(struct ListNode *head, int *expected, int len)` | 将链表与预期数组逐一比较，完全一致返回 1，否则返回 0 |
| `run_test` | `void run_test(const char *name, int *nums, int len, int n, int *expected, int expected_len)` | 运行单个测试用例：构建链表、调用算法、打印结果、断言验证 |

---

## 总结

优化版的核心改动只有一处：将 `slow` 的起点从 `head` 改为 `&dummy`。这个看似微小的
变化带来了结构上的简化——循环结束时 `slow` 天然就是目标节点的前驱，不再需要额外的
`prev` 指针，循环体也从每次三次赋值减少到两次。同时，优化版补上了原版缺失的
`free(target)` 调用，避免了内存泄漏。算法的时间复杂度保持 O(n)，空间复杂度保持
O(1)，整体逻辑更加简洁、正确。

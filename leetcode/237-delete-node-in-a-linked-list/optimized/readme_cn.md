# 237 - 删除链表中的节点（优化版）

## 目录

- [题目描述](#题目描述)
- [核心思路](#核心思路)
- [逐步推演](#逐步推演)
- [ASCII 流程图](#ascii-流程图)
- [易错点分析](#易错点分析)
- [与原版对比](#与原版对比)
- [复杂度分析](#复杂度分析)
- [函数参考](#函数参考)
- [总结](#总结)

---

## 题目描述

有一个单链表，你**无法访问头节点**，只拿到了一个指向链表中某个节点的指针
`node`。请你删除该节点。

所谓「删除」，是指：

1. 该节点的值不再出现在链表中。
2. 链表的节点数减少一个。
3. 该节点之前和之后的所有节点仍保持原来的相对顺序。

**约束条件：** 被删除的节点不是尾节点（即 `node->next != NULL`）。

**示例：**

```
输入:  head = [4, 5, 1, 9],  node = 5
输出:  [4, 1, 9]
```

---

## 核心思路

因为我们拿不到被删节点的**前驱节点**，所以无法像普通链表删除那样让
`prev->next` 跳过当前节点。这道题的经典思路是**复制 + 跳过**：把下一个
节点的值复制到当前节点，然后跳过下一个节点，效果等同于删除了当前节点。

原版代码只做了两件事——复制值、跳过指针——但**忘记释放被跳过的节点**，
造成内存泄漏。优化版的核心改进是在跳过之前先用 `victim` 指针保存被跳过
节点的地址，操作完成后调用 `free(victim)` 将其释放。

三步走：

| 步骤 | 操作 | 代码 |
|:----:|------|------|
| 1 | 用 `victim` 保存即将被删除的下一个节点 | `victim = node->next` |
| 2 | 将 `victim` 的值复制到当前节点，并跳过 `victim` | `node->val = victim->val; node->next = victim->next` |
| 3 | 释放 `victim` 指向的内存 | `free(victim)` |

完整函数：

```c
void deleteNode(struct ListNode* node)
{
	struct ListNode *victim = node->next;

	node->val = victim->val;
	node->next = victim->next;
	free(victim);
}
```

用费曼的话来说：你不能把自己从链条中拆掉（因为没人帮你改前面的指针），
但你可以**变成你后面那个人**——把他的名字写在自己身上，然后把他踢出队伍。
优化版多做了一件事：踢出去之后还帮他收拾了行李（释放内存），不留垃圾。

---

## 逐步推演

以链表 `[4, 5, 1, 9]`、删除节点 `5` 为例，逐步观察整个过程。

**初始状态：**

```
head -> [4] -> [5] -> [1] -> [9] -> NULL
                ^
                |
              node（要删除的节点）
```

**第 1 步：`victim = node->next`**

用 `victim` 指针保存节点 `[1]` 的地址。

```
head -> [4] -> [5] -> [1] -> [9] -> NULL
                ^      ^
                |      |
              node   victim
```

**第 2 步：`node->val = victim->val`**

将 `victim` 的值 `1` 复制到 `node` 中。此时 `node` 的值从 `5` 变为 `1`。

```
head -> [4] -> [1] -> [1] -> [9] -> NULL
                ^      ^
                |      |
              node   victim
```

**第 3 步：`node->next = victim->next`**

让 `node->next` 跳过 `victim`，直接指向 `[9]`。此时 `victim` 已脱离链表。

```
head -> [4] -> [1] ---------> [9] -> NULL
                ^      [1]
                |       ^
              node    victim（已脱离链表，但内存仍在）
```

**第 4 步：`free(victim)`**

释放 `victim` 所指向的内存，避免泄漏。

```
head -> [4] -> [1] -> [9] -> NULL
```

**最终结果：** 链表变为 `[4, 1, 9]`，节点 `5` 已被删除，无内存泄漏。

---

## ASCII 流程图

```
                        ┌──────────┐
                        │  START   │
                        └────┬─────┘
                             │
                             ▼
                ┌──────────────────────────┐
                │ victim = node->next      │
                └────────────┬─────────────┘
                             │
                             ▼
                ┌──────────────────────────┐
                │ node->val = victim->val  │
                │ （复制下一节点的值）      │
                └────────────┬─────────────┘
                             │
                             ▼
                ┌──────────────────────────┐
                │ node->next = victim->next│
                │ （跳过 victim 节点）     │
                └────────────┬─────────────┘
                             │
                             ▼
                ┌──────────────────────────┐
                │ free(victim)             │
                │ （释放被删节点的内存）   │
                └────────────┬─────────────┘
                             │
                             ▼
                        ┌──────────┐
                        │   END    │
                        └──────────┘
```

---

## 易错点分析

### (a) 为什么必须先用 victim 保存 node->next？

如果你直接写：

```c
node->val = node->next->val;
node->next = node->next->next;  /* node->next 已经改了！ */
free(node->next);               /* 错！释放的是 node->next->next，不是原来的那个节点 */
```

第二行执行完后，`node->next` 已经指向了新的下一个节点。此时再
`free(node->next)` 释放的是错误的节点，会导致链表断裂甚至程序崩溃。

正确做法是在修改任何指针之前，先把要释放的地址存到 `victim` 里。这和反转
链表中「断链前先保存 next」是同一个道理——**先存后改，永远不会丢指针**。

### (b) 悬空指针风险

如果外部还有其他指针指向被删除的 `victim` 节点，`free(victim)` 之后那些
指针就变成了**悬空指针**（dangling pointer）——指向一块已释放的内存。访问
悬空指针是未定义行为。

在本题的约束下这不是问题（题目只给了一个 `node` 指针），但在实际工程中
需要格外小心：释放内存前确保没有其他活跃引用。

### (c) 为什么不能删除尾节点？

如果 `node` 是尾节点，`node->next` 为 `NULL`，那么 `victim = node->next`
就是 `NULL`。后续对 `victim->val` 的访问会解引用空指针，导致程序崩溃。
这正是题目约束「被删节点不是尾节点」的原因。

---

## 与原版对比

| 对比项 | 原版 | 优化版 |
|--------|------|--------|
| 代码行数（函数体） | 2 行 | 4 行 |
| 内存泄漏 | 有——被跳过的节点未释放 | 无——`free(victim)` 释放被跳过的节点 |
| 额外变量 | 无 | `victim` 指针（1 个局部变量） |
| 时间复杂度 | O(1) | O(1) |
| 空间复杂度 | O(1) | O(1) |
| 适用场景 | 在线评测（不检测内存泄漏） | 生产环境、嵌入式系统、长时间运行的程序 |

原版代码：

```c
void deleteNode(struct ListNode* node)
{
	node->val = node->next->val;
	node->next = node->next->next;
}
```

优化版代码：

```c
void deleteNode(struct ListNode* node)
{
	struct ListNode *victim = node->next;

	node->val = victim->val;
	node->next = victim->next;
	free(victim);
}
```

多出来的两行（声明 `victim` 和 `free(victim)`）换来的是零泄漏。在刷题
环境中原版可以通过，但写进真实项目里就是一颗定时炸弹——程序每删一个节点
就多泄漏一块内存，运行时间越长，泄漏越严重。

---

## 复杂度分析

| 维度 | 复杂度 | 说明 |
|------|--------|------|
| **时间** | **O(1)** | 不涉及遍历，只操作常数个指针和一次 `free` 调用 |
| **空间** | **O(1)** | 只用了一个额外的 `victim` 指针，与链表长度无关 |

与原版完全相同——优化**纯粹在内存安全性上**，不改变算法的时空开销。

---

## 函数参考

以下是 `optimized/main.c` 中定义的所有函数：

| 函数签名 | 说明 |
|----------|------|
| `void deleteNode(struct ListNode *node)` | **核心算法**——复制下一节点的值到当前节点，跳过并释放下一节点 |
| `struct ListNode *insert_list(int *nums, int size)` | 从整型数组构建单链表，返回头节点 |
| `void show_list(char *type, struct ListNode *head)` | 以 `type: [ v1 v2 ... ]` 格式打印链表 |
| `static void free_list(struct ListNode *head)` | 遍历并释放链表的所有节点，防止内存泄漏 |
| `static int check_list(struct ListNode *head, int *expected, int len)` | 验证链表内容是否与期望数组逐一匹配，匹配返回 1 |
| `static struct ListNode *find_node(struct ListNode *head, int val)` | 在链表中查找值为 `val` 的节点，返回该节点指针 |
| `static void run_test(const char *name, int *nums, int len, int del_val, int *expected, int expected_len)` | 运行单个测试用例：构建链表 -> 查找目标节点 -> 删除 -> 打印 -> 断言 -> 释放 |

---

## 总结

优化版 `deleteNode` 在原版「复制值 + 跳过指针」的基础上，增加了一个
`victim` 指针和一次 `free` 调用，彻底修复了内存泄漏问题。算法仍然是
**O(1) 时间、O(1) 空间**，没有任何性能代价。核心原则很简单：**改指针之前
先存地址，用完之后及时释放**。这不仅是这道题的最佳实践，也是 C 语言链表
操作中最基本的内存管理纪律——每一次 `malloc` 都应该有一次对应的 `free`，
不多不少。

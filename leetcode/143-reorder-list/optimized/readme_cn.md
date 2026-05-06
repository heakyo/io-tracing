# 143 - 重排链表（优化版）

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

给定一个单链表 `L` 的头节点 `head`，将其重新排列为：

```
L0 -> Ln -> L1 -> Ln-1 -> L2 -> Ln-2 -> ...
```

不能只改变节点内部的值，需要进行实际的节点交换。

**示例：**

```
输入:  1 -> 2 -> 3 -> 4 -> 5 -> NULL
输出:  1 -> 5 -> 2 -> 4 -> 3 -> NULL

输入:  1 -> 2 -> 3 -> 4 -> NULL
输出:  1 -> 4 -> 2 -> 3 -> NULL
```

---

## 核心思路

把问题拆成三步，就像拆解一个魔术：

1. **找中点**——用快慢指针一次遍历找到链表的中间节点。
2. **反转后半段**——把中点之后的链表就地反转。
3. **交替合并**——把前半段和反转后的后半段交替拼接。

这三步各自独立、逻辑清晰，合在一起就解决了整个问题。

### 第一步：快慢指针找中点

两个指针 `slow` 和 `fast` 都从 `head` 出发。`slow` 每次走一步，`fast` 每次
走两步。当 `fast` 到达末尾时，`slow` 恰好停在中点。

```c
slow = head;
fast = head;
while (fast->next && fast->next->next) {
    slow = slow->next;
    fast = fast->next->next;
}
```

为什么这样就能找到中点？因为 `fast` 的速度是 `slow` 的两倍。当 `fast` 走完
全程时，`slow` 刚好走了一半——这就是快慢指针的核心直觉。

### 第二步：反转后半段

从 `slow->next` 开始，用经典的三指针迭代法（`prev`/`curr`/`next`）反转链表。
同时用 `slow->next = NULL` 将前半段截断。

```c
prev = NULL;
curr = slow->next;
slow->next = NULL;

while (curr) {
    next = curr->next;
    curr->next = prev;
    prev = curr;
    curr = next;
}
```

### 第三步：交替合并

`first` 指向前半段头（`head`），`second` 指向反转后的后半段头（`prev`）。
每轮循环从两条链上各取一个节点，交替拼接。

```c
first = head;
second = prev;
while (second) {
    tmp = first->next;
    first->next = second;
    second = second->next;
    first->next->next = tmp;
    first = tmp;
}
```

完整函数：

```c
void reorderList(struct ListNode* head)
{
    struct ListNode *slow, *fast, *prev, *curr, *next;
    struct ListNode *first, *second, *tmp;

    if (head == NULL || head->next == NULL)
        return;

    /* Step 1: find the middle using slow/fast pointers */
    slow = head;
    fast = head;
    while (fast->next && fast->next->next) {
        slow = slow->next;
        fast = fast->next->next;
    }

    /* Step 2: reverse the second half */
    prev = NULL;
    curr = slow->next;
    slow->next = NULL;

    while (curr) {
        next = curr->next;
        curr->next = prev;
        prev = curr;
        curr = next;
    }

    /* Step 3: merge the two halves */
    first = head;
    second = prev;
    while (second) {
        tmp = first->next;
        first->next = second;
        second = second->next;
        first->next->next = tmp;
        first = tmp;
    }
}
```

变量分组清晰，每组只在对应步骤中使用：

| 步骤 | 变量 | 用途 |
|------|------|------|
| 找中点 | `slow`, `fast` | 慢指针走一步，快指针走两步 |
| 反转 | `prev`, `curr`, `next` | 经典三指针反转 |
| 合并 | `first`, `second`, `tmp` | 两条链交替拼接 |

---

## 逐步推演

以 `[1, 2, 3, 4, 5]` 为例，完整展示三个阶段。

### 初始状态

```
head -> 1 -> 2 -> 3 -> 4 -> 5 -> NULL
```

### 第一步：快慢指针找中点

| 轮次 | slow | fast | fast->next | fast->next->next | 继续？ |
|:----:|:----:|:----:|:----------:|:----------------:|:------:|
| 初始 | 1 | 1 | 2 | 3 | 是 |
| 1 | 2 | 3 | 4 | 5 | 是 |
| 2 | 3 | 5 | NULL | — | 否 |

`fast->next == NULL`，循环结束。`slow` 停在节点 3，这就是中点。

```
head -> 1 -> 2 -> 3 -> 4 -> 5 -> NULL
                  ^
                 slow
```

### 第二步：反转后半段

先截断：`curr = slow->next`（节点 4），`slow->next = NULL`。

```
前半段：  1 -> 2 -> 3 -> NULL
后半段：  4 -> 5 -> NULL       (curr 指向 4, prev = NULL)
```

反转过程：

| 轮次 | curr | next = curr->next | curr->next = prev | prev = curr | curr = next | prev 链 |
|:----:|:----:|:-----------------:|:-----------------:|:-----------:|:-----------:|:-------:|
| 1 | 4 | 5 | 4->NULL | 4 | 5 | 4->NULL |
| 2 | 5 | NULL | 5->4 | 5 | NULL | 5->4->NULL |

循环结束，`curr == NULL`。反转后的后半段：

```
prev -> 5 -> 4 -> NULL
```

此时两条链：

```
前半段 (first)：  1 -> 2 -> 3 -> NULL
后半段 (second)： 5 -> 4 -> NULL
```

### 第三步：交替合并

`first = head`（节点 1），`second = prev`（节点 5）。

**第 1 轮（second = 5, 不为 NULL, 进入循环）：**

```
tmp = first->next                // tmp = 2
first->next = second             // 1 -> 5
second = second->next            // second = 4
first->next->next = tmp          // 5 -> 2
first = tmp                      // first = 2

结果：1 -> 5 -> 2 -> 3 -> NULL    second = 4
                ^
              first
```

**第 2 轮（second = 4, 不为 NULL, 继续循环）：**

```
tmp = first->next                // tmp = 3
first->next = second             // 2 -> 4
second = second->next            // second = NULL
first->next->next = tmp          // 4 -> 3
first = tmp                      // first = 3

结果：1 -> 5 -> 2 -> 4 -> 3 -> NULL    second = NULL
                          ^
                        first
```

**退出循环（second == NULL）。**

### 最终结果

```
1 -> 5 -> 2 -> 4 -> 3 -> NULL
```

---

## ASCII 流程图

```
┌─────────────────────────────────────────┐
│             reorderList 入口             │
│             接收 head                    │
└───────────────────┬─────────────────────┘
                    │
                    ▼
         ┌─────────────────────┐
         │ head == NULL ||     │── 是 ──> return
         │ head->next == NULL? │
         └──────────┬──────────┘
                    │ 否
                    ▼
         ┌─────────────────────┐
         │ slow = head         │
         │ fast = head         │
         └──────────┬──────────┘
                    │
                    ▼
         ┌─────────────────────────────┐
    ┌───>│ fast->next &&               │── 否 ──┐
    │    │ fast->next->next ?          │        │
    │    └──────────┬──────────────────┘        │
    │               │ 是                        │
    │               ▼                           │
    │    ┌─────────────────────┐                │
    │    │ slow = slow->next   │                │
    │    │ fast = fast->next   │                │
    │    │       ->next        │                │
    │    └──────────┬──────────┘                │
    │               │                           │
    └───────────────┘                           │
                    ┌───────────────────────────┘
                    ▼
         ┌─────────────────────┐
         │ prev = NULL         │
         │ curr = slow->next   │
         │ slow->next = NULL   │
         └──────────┬──────────┘
                    │
                    ▼
         ┌─────────────────────┐
    ┌───>│ curr != NULL ?      │── 否 ──┐
    │    └──────────┬──────────┘        │
    │               │ 是                │
    │               ▼                   │
    │    ┌─────────────────────┐        │
    │    │ next = curr->next   │        │
    │    │ curr->next = prev   │        │
    │    │ prev = curr         │        │
    │    │ curr = next         │        │
    │    └──────────┬──────────┘        │
    │               │                   │
    └───────────────┘                   │
                    ┌───────────────────┘
                    ▼
         ┌─────────────────────────┐
         │ first = head            │
         │ second = prev           │
         └──────────┬──────────────┘
                    │
                    ▼
         ┌─────────────────────────┐
    ┌───>│ second != NULL ?        │── 否 ──> return
    │    └──────────┬──────────────┘
    │               │ 是
    │               ▼
    │    ┌─────────────────────────────┐
    │    │ tmp = first->next           │
    │    │ first->next = second        │
    │    │ second = second->next       │
    │    │ first->next->next = tmp     │
    │    │ first = tmp                 │
    │    └──────────┬──────────────────┘
    │               │
    └───────────────┘
```

---

## 易错点分析

### (a) 快慢指针的停止条件

循环条件是 `fast->next && fast->next->next`，而不是 `fast && fast->next`。

这里的区别很关键。我们需要 `slow` 停在**前半段的最后一个节点**，而不是后半段
的第一个节点。用 `fast->next && fast->next->next` 作为条件时：

- **奇数长度**（如 5 个节点）：`slow` 停在第 3 个节点（正中间）。
- **偶数长度**（如 4 个节点）：`slow` 停在第 2 个节点（前半段的末尾）。

这样 `slow->next` 就是后半段的起点，`slow->next = NULL` 恰好截断前半段。

如果改用 `fast && fast->next`，`slow` 会多走一步，前半段会多包含一个节点，
导致合并时节点对不齐。

### (b) 奇数与偶数长度的差异

| 长度 | 示例 | slow 停在 | 前半段 | 后半段 |
|:----:|:----:|:---------:|:------:|:------:|
| 奇数 5 | 1,2,3,4,5 | 节点 3 | 1->2->3 | 4->5 |
| 偶数 4 | 1,2,3,4 | 节点 2 | 1->2 | 3->4 |
| 偶数 6 | 1,2,3,4,5,6 | 节点 3 | 1->2->3 | 4->5->6 |
| 奇数 7 | 1,2,3,4,5,6,7 | 节点 4 | 1->2->3->4 | 5->6->7 |

奇数长度时，前半段比后半段多一个节点。偶数长度时，前后等长。

合并循环用 `while (second)` 控制——后半段用完就停。如果前半段多出一个节点
（奇数情况），它自然挂在链表最后，不需要特殊处理。

### (c) 合并循环中的指针顺序

```c
tmp = first->next;              /* 1. 保存 first 的下一个节点 */
first->next = second;           /* 2. first 指向 second */
second = second->next;          /* 3. second 前进（必须在第 4 步之前！） */
first->next->next = tmp;        /* 4. 刚插入的 second 节点指向 tmp */
first = tmp;                    /* 5. first 前进到 tmp */
```

第 3 步和第 4 步的顺序不能对调。如果先执行第 4 步（`first->next->next = tmp`），
那么 `second` 原本的 `next` 指针就被覆盖了，`second = second->next` 会拿到
错误的值（`tmp` 而非真正的下一个后半段节点）。

### (d) 提前返回的必要性

```c
if (head == NULL || head->next == NULL)
    return;
```

空链表时 `fast->next` 会访问空指针，导致段错误。单节点链表无需重排，直接返回。
这个提前检查虽然只有一行，但保护了后续所有代码的安全假设。

---

## 与原版对比

| 对比维度 | 原版（计数法） | 优化版（快慢指针） |
|----------|---------------|-------------------|
| 找中点方式 | 先遍历计数 `size`，再用 `size >> 1` 算出反转起点，**第二次遍历**走到该位置 | 快慢指针**一次遍历**直接定位中点 |
| 遍历次数 | 第一次：计数全链表；第二次：走到中点位置。两次遍历前半段 | 一次遍历，`fast` 到末尾时 `slow` 恰好在中点 |
| 辅助变量 | `size`, `revert_size`, `i`（索引变量） | 无需任何计数变量 |
| 找中点的变量名 | `cur`, `pre`（通用名，看不出"找中点"的意图） | `slow`, `fast`（一眼就知道是快慢指针） |
| 反转的变量名 | `revert_head`, `cur`, `tmp`（`cur` 在找中点和反转中复用，语义模糊） | `prev`, `curr`, `next`（经典三指针命名，各司其职） |
| 合并的变量名 | `cur`, `tmp`, `tmp2`（三个临时变量，需要仔细跟踪） | `first`, `second`, `tmp`（前半段、后半段、临时保存，语义直白） |
| 提前返回 | 只检查 `head == NULL` | 检查 `head == NULL \|\| head->next == NULL`，更完整 |
| 合并循环 | 使用 `tmp` 和 `tmp2` 两个临时变量 | 只需一个 `tmp`，通过巧妙的赋值顺序省掉了第二个临时变量 |
| 时间复杂度 | O(n) | O(n) |
| 空间复杂度 | O(1) | O(1) |

原版找中点的代码：

```c
size = 0;
for (cur = head; cur; cur = cur->next)
    size++;

revert_size = size >> 1;
cur = head;
for (i = 0; i < size - revert_size; i++) {
    pre = cur;
    cur = cur->next;
}
pre->next = NULL;
```

优化版找中点的代码：

```c
slow = head;
fast = head;
while (fast->next && fast->next->next) {
    slow = slow->next;
    fast = fast->next->next;
}
/* slow 就是中点，slow->next 就是后半段起点 */
```

优化版用 6 行代码替代了原版的 9 行，同时消除了 `size`、`revert_size`、`i`
三个辅助变量。更重要的是，快慢指针是链表问题中的经典模式——掌握了这个技巧，
在环检测、找倒数第 k 个节点等问题中都能直接复用。

---

## 复杂度分析

| 维度 | 复杂度 | 说明 |
|------|--------|------|
| 时间 | **O(n)** | 找中点遍历 n/2 步（`fast` 走完全链表），反转后半段 n/2 步，合并 n/2 步。三步加起来仍是 O(n)，常数系数约 1.5n |
| 空间 | **O(1)** | 只使用了 `slow`、`fast`、`prev`、`curr`、`next`、`first`、`second`、`tmp` 共 8 个指针变量，全部在栈上，不随输入规模增长 |

与原版完全相同。优化版的改进在于**代码清晰度和找中点的技巧**，算法本质和
时空复杂度没有变化。

---

## 函数参考

以下是 `optimized/main.c` 中定义的所有函数：

| 函数签名 | 说明 |
|----------|------|
| `void reorderList(struct ListNode *head)` | **核心算法**。三步法重排链表：快慢指针找中点、反转后半段、交替合并。原地修改，无返回值 |
| `struct ListNode *insert_list(int *nums, int size)` | 从整型数组构建单链表。从尾到头依次 `malloc` 并头插，返回头节点 |
| `void show_list(char *type, struct ListNode *head)` | 遍历并打印链表内容，格式为 `type: [ val1 val2 ... ]` |
| `static void free_list(struct ListNode *head)` | 遍历链表，逐节点 `free`，释放所有堆内存 |
| `static int check_list(struct ListNode *head, int *expected, int len)` | 将链表与期望数组逐元素比较，长度也必须匹配。返回 1 表示通过，0 表示不匹配 |
| `static void run_test(const char *name, int *nums, int len, int *expected, int expected_len)` | 运行单个测试用例：构建链表、调用 `reorderList`、打印结果、用 `assert` 校验、释放内存 |

---

## 总结

优化版用**快慢指针 + 反转 + 交替合并**三步法解决重排链表问题，与原版相比最大的
改进是用快慢指针一次遍历找中点，省去了显式计数和二次定位，消除了 `size`、
`revert_size`、`i` 三个辅助变量；变量命名按步骤分组——`slow`/`fast` 找中点、
`prev`/`curr`/`next` 反转、`first`/`second`/`tmp` 合并——每个名字都在说"我是
谁、我在哪一步被用到"；合并阶段只需一个 `tmp` 而非原版的 `tmp`+`tmp2`，
通过巧妙安排赋值顺序实现了更简洁的交替拼接。时间 O(n)、空间 O(1)，与原版
完全相同，改进纯粹在代码清晰度上——让读代码的人一眼看出算法的三个阶段，
不需要在变量复用和命名歧义中反复猜测。

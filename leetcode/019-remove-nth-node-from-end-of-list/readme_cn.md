# 19 - 删除链表的倒数第 N 个结点

## 目录

- [问题描述](#问题描述)
- [核心思路](#核心思路)
- [逐步推演](#逐步推演)
- [ASCII 流程图](#ascii-流程图)
- [易错点分析](#易错点分析)
- [复杂度分析](#复杂度分析)
- [函数参考](#函数参考)
- [总结](#总结)

---

## 问题描述

给定一个链表的头节点 `head`，删除链表的**倒数第 n 个**节点，并返回链表的头节点。

换句话说：从链表末尾开始倒着数，数到第 `n` 个节点，把它从链表中移除。

**进阶：** 你能否用一趟扫描完成这个操作？

**约束条件：**

- 链表中节点的数量为 `sz`，`1 <= sz <= 30`
- `0 <= Node.val <= 100`
- `1 <= n <= sz`

**示例：**

```
输入：head = [1, 2, 3, 4, 5], n = 2
输出：[1, 2, 3, 5]
解释：倒数第 2 个节点是值为 4 的节点，将其删除。
```

---

## 核心思路

> 想象两个人站在同一条走廊上，第一个人先向前走 n 步，然后两人同时开始走。
> 当第一个人走到走廊尽头时，第二个人恰好站在倒数第 n 个位置上。

这就是**快慢指针间距法**的核心直觉。

具体做法：

1. 让 `fast` 指针先走 `n` 步，此时 `fast` 和 `slow` 之间恰好隔了 `n` 个节点
2. 然后 `slow` 和 `fast` 同时向前移动，每次各走一步
3. 当 `fast` 走到 `NULL`（链表末尾的下一个位置）时，`slow` 恰好指向倒数第 `n` 个节点
4. 用 `prev` 记录 `slow` 的前驱节点，执行 `prev->next = slow->next` 完成删除

为了统一处理"删除头节点"这一边界情况，算法在栈上创建一个 `dummy` 哨兵节点，
让 `prev` 初始指向 `dummy`。这样即使要删除的是第一个节点，`prev->next = slow->next`
也能正确地将 `dummy.next` 更新为新的头节点。

```c
struct ListNode* removeNthFromEnd(struct ListNode* head, int n)
{
	struct ListNode dummy = {0, head};
	struct ListNode *slow, *fast, *prev;
	int i;

	slow = head;
	fast = head;
	prev = &dummy;
	for (i = 0; i < n; i++)
		fast = fast->next;

	while (fast) {
		prev = slow;
		slow = slow->next;
		fast = fast->next;
	}

	prev->next = slow->next;

	return dummy.next;
}
```

关键不变量：在整个遍历过程中，`fast` 始终领先 `slow` 恰好 `n` 步。

---

## 逐步推演

以 `[1, 2, 3, 4, 5]`，`n = 2` 为例。

链表结构：

```
1 -> 2 -> 3 -> 4 -> 5 -> NULL
```

目标：删除倒数第 2 个节点（值为 4）。

**初始化：**

- `dummy = {0, head}`，即 `dummy -> 1 -> 2 -> 3 -> 4 -> 5 -> NULL`
- `slow = head` -> 节点 1
- `fast = head` -> 节点 1
- `prev = &dummy` -> dummy 节点

**阶段一：fast 先走 n=2 步**

| 步骤 | fast 移动 | fast 指向 |
|:----:|:--------:|:---------:|
| i=0 | `fast = fast->next` | 节点 2 |
| i=1 | `fast = fast->next` | 节点 3 |

此时 fast 指向节点 3，slow 仍在节点 1，两者间距为 2。

**阶段二：slow 和 fast 同时移动**

| 轮次 | fast != NULL? | prev 移动后 | slow 移动后 | fast 移动后 |
|:----:|:------------:|:-----------:|:-----------:|:-----------:|
| 1 | 节点 3，是 | 节点 1 | 节点 2 | 节点 4 |
| 2 | 节点 4，是 | 节点 2 | 节点 3 | 节点 5 |
| 3 | 节点 5，是 | 节点 3 | 节点 4 | NULL |

循环结束时：
- `fast = NULL`
- `slow` 指向节点 4（倒数第 2 个）
- `prev` 指向节点 3（slow 的前驱）

**执行删除：**

```
prev->next = slow->next
节点 3 ->next = 节点 4 ->next = 节点 5
```

删除后链表：`1 -> 2 -> 3 -> 5 -> NULL`

返回 `dummy.next` = 节点 1，结果正确。

---

## ASCII 流程图

```
              ┌────────────────────┐
              │       START        │
              └─────────┬──────────┘
                        │
                        ▼
              ┌────────────────────┐
              │ dummy = {0, head}  │
              │ slow = head        │
              │ fast = head        │
              │ prev = &dummy      │
              └─────────┬──────────┘
                        │
                        ▼
              ┌────────────────────┐
              │   i = 0            │
              └─────────┬──────────┘
                        │
                        ▼
                ┌───────────────┐
          ┌────>│   i < n ?     │
          │     └───┬───────┬───┘
          │         │       │
          │      是 │       │ 否
          │         ▼       │
          │  ┌─────────────┐│
          │  │fast =       ││
          │  │ fast->next  ││
          │  │i++          ││
          │  └──────┬──────┘│
          │         │       │
          └─────────┘       │
                            ▼
                ┌───────────────┐
          ┌────>│  fast != NULL?│
          │     └───┬───────┬───┘
          │         │       │
          │      是 │       │ 否
          │         ▼       ▼
          │  ┌─────────────┐  ┌──────────────────┐
          │  │prev = slow  │  │prev->next =      │
          │  │slow =       │  │  slow->next      │
          │  │ slow->next  │  │                  │
          │  │fast =       │  │返回 dummy.next   │
          │  │ fast->next  │  └────────┬─────────┘
          │  └──────┬──────┘           │
          │         │                  ▼
          └─────────┘          ┌──────────────┐
                               │     END      │
                               └──────────────┘
```

---

## 易错点分析

### (a) 为什么需要 dummy 节点？

考虑链表只有一个节点 `[1]`，`n = 1` 的情况。此时需要删除的恰好是头节点本身。

如果没有 `dummy`，`prev` 没有一个合法的初始值来指向头节点的"前一个"。
引入 `dummy` 后，`dummy.next = head`，`prev` 初始化为 `&dummy`。
当 `slow` 指向头节点且需要删除时，执行 `prev->next = slow->next` 就是
`dummy.next = slow->next`，正确更新了新的头节点。

注意这里的 `dummy` 是**栈上变量**（`struct ListNode dummy = {0, head}`），
不需要 `malloc`，函数返回后自动回收，不会造成内存泄漏。

### (b) 间距不变量

整个算法的正确性依赖于一个关键不变量：

> fast 和 slow 之间始终相隔恰好 n 个节点。

第一阶段让 fast 先走 n 步建立间距，第二阶段两个指针同步移动保持间距不变。
当 fast 到达 NULL 时，slow 距离 NULL 恰好 n 步，即 slow 就是倒数第 n 个节点。

如果 n 等于链表长度（删除头节点），fast 先走 n 步后直接变为 NULL，
while 循环不执行，slow 仍指向头节点，prev 仍指向 dummy，删除逻辑依然正确。

### (c) 额外的 prev 指针

当前实现维护了三个指针 `slow`、`fast`、`prev`。其实可以省略 `prev`，
让 `slow` 从 `&dummy` 出发而不是从 `head` 出发。这样当 fast 到达 NULL 时，
`slow` 恰好指向目标节点的前驱，直接执行 `slow->next = slow->next->next` 即可：

```c
slow = &dummy;
fast = head;
for (i = 0; i < n; i++)
    fast = fast->next;
while (fast) {
    slow = slow->next;
    fast = fast->next;
}
slow->next = slow->next->next;
```

这种写法更简洁，只需两个移动指针。

### (d) 被删节点的内存泄漏

执行 `prev->next = slow->next` 后，被删除的节点（`slow` 指向的节点）从链表中
断开了，但代码中**没有对它调用 `free`**。在 LeetCode 的判题环境中这不影响通过，
但在生产代码中这是一个轻微的内存泄漏。严格来说应该：

```c
struct ListNode *to_delete = slow;
prev->next = slow->next;
free(to_delete);
```

---

## 复杂度分析

| 维度 | 复杂度 | 说明 |
|------|--------|------|
| **时间** | O(n) | 一趟扫描完成。fast 先走 n 步，然后 slow 和 fast 同步走到末尾，总步数不超过链表长度的两倍 |
| **空间** | O(1) | 仅使用 dummy、slow、fast、prev 四个指针变量和一个循环计数器，无额外数据结构 |

其中 `n` 为链表节点总数。

与之对比，先遍历一遍求链表长度、再遍历一遍定位目标节点的做法也是 O(n) 时间，
但需要两趟扫描。快慢指针法的优势在于**只需一趟扫描**即可完成。

---

## 函数参考

以下是 `main.c` 中所有函数的说明：

| 函数 | 说明 |
|------|------|
| `removeNthFromEnd(head, n)` | 核心算法：使用快慢指针间距法删除链表倒数第 n 个节点并返回头节点 |
| `insert_list(nums, size)` | 根据整数数组从尾到头构建单链表，返回头节点 |
| `show_list(type, head)` | 打印链表内容，`type` 为显示前缀（如 `"Input "` 或 `"Output"`） |
| `free_list(head)` | 遍历链表并逐节点释放内存 |
| `check_list(head, expected, len)` | 将链表与期望数组逐元素比较，验证结果是否正确 |
| `run_test(name, nums, len, n, expected, expected_len)` | 执行一组测试：建表、调用 `removeNthFromEnd`、验证并打印结果 |

---

## 总结

快慢指针间距法的核心直觉极为简单：让两个指针保持固定间距 n，同步移动到链表末尾，
慢指针自然停在倒数第 n 个位置。栈上 dummy 节点统一了"删除头节点"的边界情况，
避免了额外的条件分支和 malloc 开销。当前实现使用了额外的 prev 指针来记录前驱，
实际上可以通过让 slow 从 dummy 出发来省略它。唯一需要注意的瑕疵是被删节点未被
free，在 LeetCode 环境下无碍，但在生产代码中应当补上释放逻辑。整体算法 O(n)
时间、O(1) 空间，一趟扫描完成，是本题的最优解法。

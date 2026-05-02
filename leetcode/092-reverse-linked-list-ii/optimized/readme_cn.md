# 092 - 反转链表 II（优化版）

## 目录

1. [问题描述](#问题描述)
2. [核心思路](#核心思路)
3. [逐步推演](#逐步推演)
4. [ASCII 流程图](#ascii-流程图)
5. [函数参考](#函数参考)
6. [易错点分析](#易错点分析)
7. [复杂度分析](#复杂度分析)
8. [总结](#总结)

---

## 问题描述

给你一个单链表的头节点 `head` 和两个整数 `left` 与 `right`，其中 `left <= right`。
请你反转从位置 `left` 到位置 `right` 的链表节点，返回反转后的链表。

**示例：**

```
输入：head = [1,2,3,4,5], left = 2, right = 4
输出：[1,4,3,2,5]
```

---

## 核心思路

想象你在排队。队伍里第 2 到第 4 个人需要反转顺序，但其他人不动。

我们用的技巧叫 **头插法**——每次从反转区间的"尾巴"抽出一个节点，插到区间的
"头部"前面。重复操作 `right - left` 次，区间就翻转了。

优化版和原版用的是**完全相同的算法**，但代码更干净：

| 改进点 | 原版 | 优化版 |
|--------|------|--------|
| dummy 节点 | `malloc` 分配（堆上），忘了 `free` 会泄漏 | 栈上声明，函数返回自动销毁，**零泄漏** |
| 变量命名 | `pre`、`cur`、`tmp`（含义模糊） | `prev`（前驱）、`tail`（下沉节点）、`move`（被搬运的节点） |
| 导航循环 | 同时推进 `pre` 和 `cur` 两个指针 | 只推进 `prev` 一个指针，`tail = prev->next` 一行搞定 |
| 反转循环体 | 4 行指针操作，中间有 `cur->next->next` 链式访问 | 4 行指针操作，每行只做一件事，更易读 |

关键代码：

```c
struct ListNode dummy = { .val = 0, .next = head };
struct ListNode *prev = &dummy, *tail, *move;
int i;

for (i = 1; i < left; i++)
	prev = prev->next;

tail = prev->next;
for (i = left; i < right; i++) {
	move = tail->next;
	tail->next = move->next;
	move->next = prev->next;
	prev->next = move;
}

return dummy.next;
```

---

## 逐步推演

以 `[1,2,3,4,5]`，`left=2`，`right=4` 为例。

### 第一阶段：导航到 `left` 前一个位置

初始状态：

```
dummy -> 1 -> 2 -> 3 -> 4 -> 5 -> NULL
  ^
 prev
```

循环 `i = 1`（`i < left` 即 `1 < 2`），`prev = prev->next`：

```
dummy -> 1 -> 2 -> 3 -> 4 -> 5 -> NULL
         ^
        prev
```

设置 `tail = prev->next`：

```
dummy -> 1 -> 2 -> 3 -> 4 -> 5 -> NULL
         ^    ^
        prev tail
```

> **费曼要点**：`prev` 永远站在反转区间的"门口"外面，`tail` 指向原始第 `left`
> 个节点。随着反转的进行，`tail` 会一路"下沉"到区间末尾——但指针本身从不改变，
> 只是它前面不断有新节点插入。

### 第二阶段：反转循环

#### 第 1 轮（`i=2`，`i < 4`）

```
move = tail->next          // move 指向节点 3
```

```
dummy -> 1 -> 2 -> 3 -> 4 -> 5 -> NULL
         ^    ^    ^
        prev tail move
```

四步指针操作：

```
① tail->next = move->next     // 节点 2 跳过节点 3，指向节点 4
② move->next = prev->next     // 节点 3 指向 prev 后面的节点（即节点 2）
③ prev->next = move           // prev 指向节点 3
```

结果：

```
dummy -> 1 -> 3 -> 2 -> 4 -> 5 -> NULL
         ^         ^
        prev      tail
```

> 节点 3 被"搬"到了区间最前面，`tail`（节点 2）下沉了一位。

#### 第 2 轮（`i=3`，`i < 4`）

```
move = tail->next          // move 指向节点 4
```

```
dummy -> 1 -> 3 -> 2 -> 4 -> 5 -> NULL
         ^         ^    ^
        prev      tail move
```

四步指针操作：

```
① tail->next = move->next     // 节点 2 跳过节点 4，指向节点 5
② move->next = prev->next     // 节点 4 指向 prev 后面的节点（即节点 3）
③ prev->next = move           // prev 指向节点 4
```

结果：

```
dummy -> 1 -> 4 -> 3 -> 2 -> 5 -> NULL
         ^              ^
        prev           tail
```

循环结束（`i=4` 不满足 `i < 4`）。

### 最终结果

返回 `dummy.next`，即 `[1,4,3,2,5]`。

---

## ASCII 流程图

```
┌─────────────────────────────────────────────┐
│              reverseBetween 入口              │
└──────────────────┬──────────────────────────┘
                   │
                   ▼
        ┌──────────────────────┐
        │  栈上创建 dummy 节点  │
        │  dummy.next = head   │
        │  prev = &dummy       │
        └──────────┬───────────┘
                   │
                   ▼
        ┌──────────────────────┐
        │  i = 1               │
        │  i < left ?          │──── 否 ────┐
        └──────────┬───────────┘            │
                   │ 是                     │
                   ▼                        │
        ┌──────────────────────┐            │
        │  prev = prev->next   │            │
        │  i++                 │            │
        └──────────┬───────────┘            │
                   │                        │
                   └── 回到判断 ◄────────────┘
                                            │
                   ┌────────────────────────┘
                   ▼
        ┌──────────────────────┐
        │  tail = prev->next   │
        │  i = left            │
        └──────────┬───────────┘
                   │
                   ▼
        ┌──────────────────────┐
        │  i < right ?         │──── 否 ────┐
        └──────────┬───────────┘            │
                   │ 是                     │
                   ▼                        │
     ┌─────────────────────────────┐        │
     │  move = tail->next          │        │
     │  tail->next = move->next    │        │
     │  move->next = prev->next    │        │
     │  prev->next = move          │        │
     │  i++                        │        │
     └─────────────┬───────────────┘        │
                   │                        │
                   └── 回到判断 ◄────────────┘
                                            │
                   ┌────────────────────────┘
                   ▼
        ┌──────────────────────┐
        │  return dummy.next   │
        └──────────────────────┘
```

---

## 函数参考

以下是 `optimized/main.c` 中的所有函数：

| 函数签名 | 说明 |
|----------|------|
| `struct ListNode *insert_list(int *nums, int size)` | 从整型数组构建单链表，从尾到头依次 `malloc` 并头插，返回链表头 |
| `void show_list(char *type, struct ListNode *head)` | 遍历并打印链表，`type` 为标签字符串（如 `"Input "` / `"Output"`） |
| `struct ListNode *reverseBetween(struct ListNode *head, int left, int right)` | **核心算法**：用头插法原地反转链表中第 `left` 到第 `right` 个节点 |
| `static void free_list(struct ListNode *head)` | 遍历链表，逐节点 `free`，释放所有堆内存 |
| `static int check_list(struct ListNode *head, int *expected, int len)` | 将链表与期望数组逐元素比较，长度也必须匹配，返回 1 表示通过 |
| `static void run_test(const char *name, int *nums, int len, int left, int right, int *expected, int expected_len)` | 构建链表、调用 `reverseBetween`、打印结果、用 `assert` 校验、最后释放内存 |

---

## 易错点分析

### (a) 栈 dummy vs malloc dummy

原版代码：

```c
dummy = malloc(sizeof(*dummy));
dummy->next = head;
/* ... */
return dummy->next;    /* dummy 没有 free，内存泄漏！ */
```

优化版代码：

```c
struct ListNode dummy = { .val = 0, .next = head };
/* ... */
return dummy.next;     /* dummy 在栈上，函数返回自动销毁 */
```

为什么栈上 dummy 可行？因为我们返回的是 `dummy.next`——一个指向堆上真实节点的
指针，而不是 `&dummy` 本身。函数返回后 `dummy` 虽然销毁了，但返回值已经拷贝
出去，完全安全。

### (b) tail 不移动

`tail` 始终指向**同一个节点**（原始第 `left` 个节点）。它"看起来"在下沉，是因为
不断有新节点被插入到它前面。

```
开始：prev -> [2] -> 3 -> 4 -> 5
                ^
               tail

结束：prev -> 4 -> 3 -> [2] -> 5
                         ^
                        tail（同一个节点，值还是 2）
```

如果你错误地在循环中移动了 `tail`（比如 `tail = tail->next`），反转就会
"跑偏"——从错误的位置开始抽节点。

### (c) 反转循环体 4 行操作必须严格按序执行

```c
move = tail->next;          /* ① 抓住要搬的节点 */
tail->next = move->next;    /* ② tail 跳过 move */
move->next = prev->next;    /* ③ move 指向区间当前头部 */
prev->next = move;          /* ④ prev 指向 move（新的区间头） */
```

如果打乱顺序——比如先执行 ④ 再执行 ③——`prev->next` 已经被改写，③ 中
`move->next = prev->next` 就会指向错误的节点，整条链表断裂。

**口诀：抓、跳、接、插。**

---

## 复杂度分析

| 维度 | 复杂度 | 说明 |
|------|--------|------|
| 时间 | **O(n)** | 导航循环最多走 `left - 1` 步，反转循环走 `right - left` 步，总计最多 `right - 1` 步，`right <= n`，故 O(n) |
| 空间 | **O(1)** | 只用了 `dummy`、`prev`、`tail`、`move`、`i` 五个局部变量。`dummy` 在栈上，**真正的 O(1)**——不像原版 `malloc` 了一个堆节点 |

---

## 总结

优化版和原版解决的是**同一个问题**，用的是**同一个算法**（头插法），时间复杂度
都是 O(n)。改进纯粹是**代码风格层面**的：

1. **栈上 dummy** — 一行声明代替 `malloc`，函数返回自动销毁，消除内存泄漏
2. **描述性命名** — `prev`（前驱）、`tail`（下沉节点）、`move`（被搬运节点），
   读代码时一眼看出每个指针的角色
3. **简化导航** — 只推进 `prev` 一个指针，`tail` 在循环后一行赋值
4. **更清晰的循环体** — 每行只做一件事，没有 `cur->next->next` 这种链式访问

用费曼的话说：如果你不能用简单的语言解释一段代码，说明你还没真正理解它。
优化版的意义就在于——让代码**自己解释自己**。

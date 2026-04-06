# libhashtable — C 语言哈希表库

## 目录

- [概述](#概述)
- [编译说明](#编译说明)
- [API 参考](#api-参考)
- [使用示例](#使用示例)
- [函数使用 Q&A](#函数使用-qa)
  - [内部函数 (ht_hash, ht_resize)](#内部函数)
  - [公开函数 (ht_create, ht_destroy, ht_set, ht_get, ht_remove, ht_contains, ht_size, ht_iter_init, ht_iter_next)](#公开函数)

---

## 概述

`libhashtable` 是一个用 C 语言编写的通用哈希表库，编译为动态共享库（`.so`）。采用**开放寻址 + 线性探测**和 **FNV-1a 哈希算法**，支持字符串键和任意 `void*` 值。

特性：
- 字符串键，`void*` 值
- 负载因子超过 75% 时自动扩容
- 基于墓碑（tombstone）的删除机制，不破坏探测链
- 迭代器支持遍历所有条目
- 无外部依赖 —— 纯 C99

---

## 编译说明

```bash
# 编译动态库
cd mylib/hashtable
make

# 编译并运行 demo
cd demo
make
./demo
```

---

## API 参考

### 类型

| 类型 | 说明 |
|------|------|
| `hashtable_t` | 哈希表不透明句柄。由 `ht_create` 创建，由 `ht_destroy` 释放。 |
| `ht_iter_t` | 迭代器结构体。每次 `ht_iter_next` 调用后，可访问 `key` 和 `value` 字段。 |

### 函数

| 函数 | 签名 | 说明 |
|------|------|------|
| `ht_create` | `hashtable_t *ht_create(size_t capacity)` | 创建新哈希表。传 `0` 使用默认容量（16）。失败返回 `NULL`。 |
| `ht_destroy` | `void ht_destroy(hashtable_t *ht)` | 销毁哈希表并释放内部内存。**不会**释放存储的值。 |
| `ht_set` | `int ht_set(hashtable_t *ht, const char *key, void *value)` | 插入或更新键值对。键会在内部复制保存。成功返回 `0`，失败返回 `-1`。 |
| `ht_get` | `void *ht_get(hashtable_t *ht, const char *key)` | 根据键查找值。返回值指针，未找到返回 `NULL`。 |
| `ht_remove` | `void *ht_remove(hashtable_t *ht, const char *key)` | 移除键值对。返回被移除的值，未找到返回 `NULL`。 |
| `ht_contains` | `int ht_contains(hashtable_t *ht, const char *key)` | 检查键是否存在。存在返回 `1`，否则返回 `0`。 |
| `ht_size` | `size_t ht_size(hashtable_t *ht)` | 返回已存储的条目数。 |
| `ht_iter_init` | `void ht_iter_init(hashtable_t *ht, ht_iter_t *iter)` | 初始化哈希表迭代器。 |
| `ht_iter_next` | `int ht_iter_next(ht_iter_t *iter)` | 前进到下一个条目。有效返回 `1`，遍历结束返回 `0`。通过 `iter->key` 和 `iter->value` 访问数据。 |

---

## 使用示例

```c
#include <stdio.h>
#include "hashtable.h"

int main(void)
{
    hashtable_t *ht = ht_create(0);
    int val = 42;

    ht_set(ht, "answer", &val);

    int *result = (int *)ht_get(ht, "answer");
    if (result)
        printf("answer = %d\n", *result);

    ht_destroy(ht);
    return 0;
}
```

编译命令：

```bash
gcc -I/path/to/include -L/path/to/lib -lhashtable -o myapp myapp.c
```

---

## 函数使用 Q&A

### 内部函数

以下是 `static` 内部函数，未在公共头文件中暴露，但它们是库的核心引擎。

#### ht_hash

**Q: 这个函数做什么？**
为给定的字符串键计算桶索引。这是将每个键映射到桶数组中某个位置的哈希函数。

**Q: 使用什么算法？**
**FNV-1a**（Fowler-Noll-Vo）。以偏移基数 `2166136261` 开始，对键的每个字节：将该字节异或到哈希值中，然后乘以 FNV 素数 `16777619`。最后对 `capacity` 取模得到桶索引。

**Q: 函数签名是什么？**
```c
static unsigned long ht_hash(const char *key, size_t cap)
```
- `key` — 要哈希的空终止字符串键。
- `cap` — 当前桶数组容量（用于取模）。
- 返回值范围 `[0, cap)`。

**Q: 为什么选择 FNV-1a？**
简单（无查找表）、快速（每字节一次异或 + 一次乘法）、对哈希表中常见的短字符串键有良好的分布性。

**Q: 需要注意什么？**
- 这是内部函数 —— 你永远不会直接调用它。
- 该函数的质量决定了键在桶中分布的均匀程度，直接影响冲突率和性能。

---

#### ht_resize

**Q: 这个函数做什么？**
当负载因子超过阈值（75%）时扩容桶数组。分配更大的新数组，并将所有现有条目重新插入。

**Q: 函数签名是什么？**
```c
static int ht_resize(hashtable_t *ht, size_t new_cap)
```
- `ht` — 哈希表句柄。
- `new_cap` — 新容量（通常为 `旧容量 * 2`）。
- 成功返回 `0`，失败返回 `-1`。

**Q: 需要注意什么？**
- 由 `ht_set` 自动调用 —— 你永远不会直接调用它。
- 失败时，原表保持不变。

---

### 公开函数

#### ht_create

**Q: 这个函数做什么？**
创建一个新的空哈希表。

**Q: 参数是什么？**
- `capacity`（`size_t`）— 初始桶数量。传 `0` 使用默认值（16）。

**Q: 返回什么？**
成功返回 `hashtable_t *` 句柄，内存分配失败返回 `NULL`。

**Q: 怎么用？**
```c
hashtable_t *ht = ht_create(0);    /* 默认容量 */
hashtable_t *ht = ht_create(1024); /* 预分配 1024 个桶 */
```

**Q: 需要注意什么？**
- 务必检查返回值是否为 `NULL`。
- 使用完毕必须调用 `ht_destroy()` 以避免内存泄漏。

---

#### ht_destroy

**Q: 这个函数做什么？**
销毁哈希表并释放所有内部内存（键和桶数组）。

**Q: 参数是什么？**
- `ht`（`hashtable_t *`）— 要销毁的哈希表句柄。

**Q: 返回什么？**
无返回值（`void`）。

**Q: 怎么用？**
```c
ht_destroy(ht);
ht = NULL; /* 好习惯 */
```

**Q: 需要注意什么？**
- **不会**释放你存储的值。如果值是堆分配的，需要先释放（例如用 `ht_iter` 遍历并逐个释放）。
- 传入 `NULL` 是安全的（无操作）。

---

#### ht_set

**Q: 这个函数做什么？**
插入新的键值对，如果键已存在则更新其值。

**Q: 参数是什么？**
- `ht`（`hashtable_t *`）— 哈希表句柄。
- `key`（`const char *`）— 空终止字符串键。内部会复制保存。
- `value`（`void *`）— 要存储的值指针。

**Q: 返回什么？**
成功返回 `0`，失败返回 `-1`（内存分配错误）。

**Q: 怎么用？**
```c
int score = 100;
ht_set(ht, "alice", &score);

/* 更新已存在的键 */
int new_score = 200;
ht_set(ht, "alice", &new_score);
```

**Q: 需要注意什么？**
- 键在内部复制保存，调用后你可以修改或释放原始字符串。
- 值**不会**被复制 —— 只存储指针。确保指向的数据在表中期间保持有效。
- 更新已存在的键时，旧值指针会被静默替换。如果旧值是堆分配的，应先取回并释放。

---

#### ht_get

**Q: 这个函数做什么？**
根据键查找值。

**Q: 参数是什么？**
- `ht`（`hashtable_t *`）— 哈希表句柄。
- `key`（`const char *`）— 要查找的键。

**Q: 返回什么？**
找到返回存储的 `void *` 值，未找到返回 `NULL`。

**Q: 怎么用？**
```c
int *val = (int *)ht_get(ht, "alice");
if (val)
    printf("alice = %d\n", *val);
else
    printf("alice not found\n");
```

**Q: 需要注意什么？**
- 如果你存储了 `NULL` 作为值，将无法区分"键存在但值为 NULL"和"键不存在"。此时用 `ht_contains()` 来检查。

---

#### ht_remove

**Q: 这个函数做什么？**
从哈希表中移除一个键值对。

**Q: 参数是什么？**
- `ht`（`hashtable_t *`）— 哈希表句柄。
- `key`（`const char *`）— 要移除的键。

**Q: 返回什么？**
找到返回被移除的 `void *` 值，未找到返回 `NULL`。

**Q: 怎么用？**
```c
int *old = (int *)ht_remove(ht, "alice");
if (old)
    printf("removed alice, value was %d\n", *old);
```

**Q: 需要注意什么？**
- 返回的值由你负责释放（如果是堆分配的）。
- 内部使用墓碑标记，不会破坏探测链。

---

#### ht_contains

**Q: 这个函数做什么？**
检查键是否存在于哈希表中。

**Q: 参数是什么？**
- `ht`（`hashtable_t *`）— 哈希表句柄。
- `key`（`const char *`）— 要检查的键。

**Q: 返回什么？**
存在返回 `1`，不存在返回 `0`。

**Q: 怎么用？**
```c
if (ht_contains(ht, "alice"))
    printf("alice is in the table\n");
```

**Q: 需要注意什么？**
- 内部调用 `ht_get`，所以如果你存储了 `NULL` 作为值，即使键已插入，`ht_contains` 也会返回 `0`。这是已知限制。

---

#### ht_size

**Q: 这个函数做什么？**
返回当前存储的键值对数量。

**Q: 参数是什么？**
- `ht`（`hashtable_t *`）— 哈希表句柄。

**Q: 返回什么？**
`size_t` 类型的条目计数。`ht` 为 `NULL` 时返回 `0`。

**Q: 怎么用？**
```c
printf("table has %zu entries\n", ht_size(ht));
```

**Q: 需要注意什么？**
- 时间复杂度 O(1) —— 计数在内部维护。

---

#### ht_iter_init

**Q: 这个函数做什么？**
初始化一个迭代器，用于遍历哈希表中的所有条目。

**Q: 参数是什么？**
- `ht`（`hashtable_t *`）— 哈希表句柄。
- `iter`（`ht_iter_t *`）— 要初始化的迭代器结构体指针。

**Q: 返回什么？**
无返回值（`void`）。

**Q: 怎么用？**
```c
ht_iter_t iter;
ht_iter_init(ht, &iter);
```

**Q: 需要注意什么？**
- 必须在 `ht_iter_next` 之前调用。
- 遍历期间不要修改哈希表（插入/删除）—— 行为未定义。

---

#### ht_iter_next

**Q: 这个函数做什么？**
将迭代器前进到下一个条目。

**Q: 参数是什么？**
- `iter`（`ht_iter_t *`）— 已初始化的迭代器指针。

**Q: 返回什么？**
找到下一个条目返回 `1`（`iter->key` 和 `iter->value` 有效），遍历结束返回 `0`。

**Q: 怎么用？**
```c
ht_iter_t iter;
ht_iter_init(ht, &iter);
while (ht_iter_next(&iter))
    printf("key=%s, value=%d\n", iter.key, *(int *)iter.value);
```

**Q: 需要注意什么？**
- 遍历顺序**不保证**（取决于哈希桶布局）。
- 解引用前务必将 `iter.value` 转换为正确的类型。

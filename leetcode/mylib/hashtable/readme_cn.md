# libhashtable — C 语言哈希表库

## 目录

- [概述](#概述)
- [编译说明](#编译说明)
- [API 参考](#api-参考)
- [使用示例](#使用示例)

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

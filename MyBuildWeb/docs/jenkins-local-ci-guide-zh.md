# 本地 Jenkins CI — 用户指南

> **费曼学习法原则**：如果你不能用简单的语言解释清楚，说明你还没有真正理解它。
> 本指南以费曼学习法的方式，教你如何通过 Docker 在本地运行 Jenkins CI 服务器，并用它自动编译 Hello World C 程序。

---

## 目录

1. [这是什么？](#1-这是什么)
2. [整体架构](#2-整体架构)
3. [前置条件](#3-前置条件)
4. [快速开始：启动 Jenkins](#4-快速开始启动-jenkins)
5. [从另一台机器访问 Jenkins](#5-从另一台机器访问-jenkins)
6. [构建任务：Build_HelloWorld_Simple](#6-构建任务build_helloworld_simple)
7. [触发构建](#7-触发构建)
8. [查看构建结果](#8-查看构建结果)
9. [常用操作](#9-常用操作)
10. [文件结构](#10-文件结构)
11. [常见问题排查](#11-常见问题排查)
12. [术语表](#12-术语表)

---

## 1. 这是什么？

把 Jenkins 想象成工厂里的一个**机器人助手**。你告诉它："每次我说开始，就帮我编译 C 程序、运行它、告诉我结果。" Jenkins 就会精确地执行 — 自动化、一致、不会遗漏步骤。

本项目搭建了：
- 一个运行在 Docker 容器里的 **Jenkins 服务器**
- 同一个容器里预装了 **GCC 编译器工具链**
- 一个**预配置的构建任务**，用来编译和运行 `src/` 目录下的 Hello World C 程序

### 打个比方

想象你有一个私人厨师（Jenkins），住在一个移动厨房里（Docker 容器）。厨房里已经备好了所有厨具（GCC、Make）。你把菜谱（Jenkinsfile / 任务配置）交给厨师，他随时可以按你的要求做菜（编译代码）。

---

## 2. 整体架构

```
 你的机器（Linux 宿主机）
 ┌─────────────────────────────────────────────────────┐
 │                                                     │
 │  docker-compose.yml                                 │
 │  ┌───────────────────────────────────────────────┐  │
 │  │  容器：jenkins-hello                          │  │
 │  │  ┌─────────────┐  ┌────────────────────────┐  │  │
 │  │  │  Jenkins     │  │  GCC 15 + Make 4.4     │  │  │
 │  │  │  (端口 8080) │  │  (编译器工具链)        │  │  │
 │  │  └─────────────┘  └────────────────────────┘  │  │
 │  │                                               │  │
 │  │  卷挂载：./ → /var/jenkins_home/project       │  │
 │  └───────────────────────────────────────────────┘  │
 │                                                     │
 │  src/main.c + Makefile  ←── 你的源代码              │
 └─────────────────────────────────────────────────────┘

 Windows / 远程机器
 ┌────────────────────────────┐
 │  浏览器 → SSH 隧道         │──── SSH (端口 22) ────→ 宿主机:8080
 │  http://localhost:8080     │
 └────────────────────────────┘
```

### 各组件的角色

| 组件 | 作用 | 比喻 |
|---|---|---|
| `docker-compose.yml` | 定义 Jenkins 的运行方式 | 厨房的蓝图 |
| `Dockerfile` | 构建带 GCC 的 Jenkins 镜像 | 厨房设备清单 |
| `Jenkinsfile` | 构建流水线定义 | 菜谱 |
| `src/main.c` | 要编译的 C 程序 | 食材 |
| `src/Makefile` | Make 的构建规则 | 烹饪步骤 |

---

## 3. 前置条件

| 依赖 | 检查命令 |
|---|---|
| Docker 引擎 | `docker --version` |
| Docker Compose | `docker-compose --version` |
| SSH 客户端（远程访问用） | `ssh -V` |

---

## 4. 快速开始：启动 Jenkins

**第 1 步** — 进入项目目录：
```bash
cd /path/to/io-tracing/MyBuildWeb
```

**第 2 步** — 构建并启动 Jenkins：
```bash
docker-compose up -d
```

这条命令做了三件事：
1. 构建自定义 Docker 镜像（Jenkins LTS + GCC 工具链）
2. 创建名为 `jenkins-hello` 的容器
3. 在 **8080** 端口启动 Jenkins

**第 3 步** — 等待约 20 秒让 Jenkins 完成初始化，然后验证：
```bash
curl -s -o /dev/null -w "%{http_code}" http://localhost:8080/
# 期望输出：200
```

**第 4 步** — 在浏览器中打开：
```
http://localhost:8080
```

> **注意**：安装向导已被禁用（`-Djenkins.install.runSetupWizard=false`），Jenkins 开箱即用 — 无需解锁密钥或安装插件。

---

## 5. 从另一台机器访问 Jenkins

### 问题

如果你在另一台机器上（比如 Windows 笔记本），即使 `ping` 能通，端口 8080 也可能被两台机器之间的网络防火墙拦截。

### 解决方案：SSH 隧道

SSH 隧道就像一条**秘密地下通道** — 它把你的 HTTP 流量包裹在加密的 SSH 连接（端口 22）里传输，而防火墙通常允许 SSH 通过。

```
 Windows 笔记本                          Linux 宿主机
 ┌──────────┐     SSH（端口 22）     ┌──────────────┐
 │ 浏览器    │ ═══════════════════> │ SSH 服务器    │
 │ localhost │     （加密传输）      │    ↓          │
 │ :8080     │ <═══════════════════ │ Jenkins :8080 │
 └──────────┘                       └──────────────┘
```

#### 方式 A：PowerShell / Windows 终端（自带 OpenSSH）

```powershell
ssh -L 8080:localhost:8080 root@<LINUX宿主机IP>
```

然后在浏览器打开：**http://localhost:8080**

#### 方式 B：PuTTY

1. 打开 PuTTY，输入 Host: `<LINUX宿主机IP>`，Port: `22`
2. 进入 **Connection → SSH → Tunnels**
3. Source port: `8080`
4. Destination: `localhost:8080`
5. 点击 **Add**
6. 点击 **Open**，登录
7. 在浏览器打开：**http://localhost:8080**

#### 方式 C：VS Code Remote SSH

如果你使用 VS Code 的 Remote-SSH 扩展：
1. 连接到 Linux 宿主机
2. VS Code 会自动转发端口
3. 在本地浏览器打开 **http://localhost:8080**

#### 验证连通性（在 Windows 上）

```powershell
# 测试 SSH 是否能通
ssh root@<LINUX宿主机IP> "echo OK"

# 建立 SSH 连接后，测试隧道是否生效
Test-NetConnection localhost -Port 8080
# 期望结果：TcpTestSucceeded : True
```

---

## 6. 构建任务：Build_HelloWorld_Simple

这个任务运行一个 Shell 脚本，分为 3 个阶段：

```
 阶段 1：准备             阶段 2：编译             阶段 3：测试
 ──────────────>         ──────────────>         ──────────────>
 打印 gcc 和             make clean               运行 ./main
 make 的版本             make all                 验证输出
                         （编译 main.c）
```

### 内部执行过程

```bash
# 阶段 1 - 准备
gcc --version
make --version

# 阶段 2 - 编译
cd /var/jenkins_home/project/src
make clean        # 清除旧的编译产物
make all          # 编译：main.c → main.o → main

# 阶段 3 - 测试
./main            # 输出："Hello, World!"
```

### 首次创建任务

如果任务不存在（例如删除了 Docker 卷之后），通过 API 创建：

```bash
# 获取 CSRF 令牌
COOKIE_JAR=/tmp/jenkins-cookies.txt
CRUMB_JSON=$(curl -s -c "$COOKIE_JAR" 'http://localhost:8080/crumbIssuer/api/json')
CRUMB_VALUE=$(echo "$CRUMB_JSON" | python3 -c "import sys,json; print(json.load(sys.stdin)['crumb'])")

# 从 XML 配置创建任务
curl -s -X POST "http://localhost:8080/createItem?name=Build_HelloWorld_Simple" \
  -b "$COOKIE_JAR" \
  -H "Content-Type: application/xml" \
  -H "Jenkins-Crumb: $CRUMB_VALUE" \
  --data-binary @job-config.xml
```

---

## 7. 触发构建

### 通过 Web 界面

1. 打开 **http://localhost:8080**
2. 点击 **Build_HelloWorld_Simple**
3. 点击左侧栏的 **Build Now**（立即构建）

### 通过命令行

```bash
# 获取 CSRF 令牌
COOKIE_JAR=/tmp/jenkins-cookies.txt
CRUMB_JSON=$(curl -s -c "$COOKIE_JAR" 'http://localhost:8080/crumbIssuer/api/json')
CRUMB_VALUE=$(echo "$CRUMB_JSON" | python3 -c "import sys,json; print(json.load(sys.stdin)['crumb'])")

# 触发构建
curl -s -X POST "http://localhost:8080/job/Build_HelloWorld_Simple/build?delay=0sec" \
  -b "$COOKIE_JAR" \
  -H "Jenkins-Crumb: $CRUMB_VALUE"
```

---

## 8. 查看构建结果

### 通过 Web 界面

1. 点击 **Build_HelloWorld_Simple**
2. 在构建历史中点击构建编号（如 **#1**）
3. 点击 **Console Output**（控制台输出）查看完整日志

### 通过命令行

```bash
# 获取最近一次构建状态
curl -s http://localhost:8080/job/Build_HelloWorld_Simple/lastBuild/api/json | \
  python3 -c "import sys,json; d=json.load(sys.stdin); print(f'构建 #{d[\"number\"]}：{d[\"result\"]}')"

# 获取完整控制台日志
curl -s http://localhost:8080/job/Build_HelloWorld_Simple/lastBuild/consoleText
```

### 成功构建的预期输出

```
Started by user unknown or anonymous
Running as SYSTEM
Building in workspace /var/jenkins_home/workspace/Build_HelloWorld_Simple
[Build_HelloWorld_Simple] $ /bin/sh -xe /tmp/jenkins*.sh
+ echo === Build Environment ===
=== Build Environment ===
+ gcc --version
gcc (GCC) 15.2.0
...
+ echo === Building Hello World ===
=== Building Hello World ===
+ cd /var/jenkins_home/project/src
+ make clean
rm -rf main *.o
+ make all
cc -O0 -g -c -o main.o main.c
cc -O0 -g -o main main.o
+ echo === Running Hello World ===
=== Running Hello World ===
+ ./main
Hello, World!
+ echo === Build SUCCESS ===
=== Build SUCCESS ===
Finished: SUCCESS
```

---

## 9. 常用操作

| 操作 | 命令 |
|---|---|
| 启动 Jenkins | `docker-compose up -d` |
| 停止 Jenkins | `docker-compose down` |
| 重启 Jenkins | `docker-compose restart` |
| 查看容器日志 | `docker logs jenkins-hello` |
| 重新构建镜像（修改 Dockerfile 后） | `docker-compose build && docker-compose up -d` |
| 进入容器终端 | `docker exec -it jenkins-hello bash` |
| 在容器内测试 GCC | `docker exec jenkins-hello gcc --version` |
| 删除所有数据（全新开始） | `docker-compose down -v` |

---

## 10. 文件结构

```
MyBuildWeb/
├── Dockerfile              ← 自定义 Jenkins + GCC 镜像定义
├── docker-compose.yml      ← Docker Compose 服务配置
├── Jenkinsfile             ← 流水线定义（用于 SCM 类型的任务）
├── src/
│   ├── main.c              ← Hello World C 源代码
│   └── Makefile            ← 构建规则：main.c → main.o → main
└── docs/
    ├── jenkins-local-ci-guide-en.md  ← 本文档（英文版）
    └── jenkins-local-ci-guide-zh.md  ← 本文档（中文版）
```

### Dockerfile — 多阶段构建

Dockerfile 使用多阶段构建的方式，把 GCC 注入到 Jenkins 镜像中：

```
 gcc:latest 镜像                      jenkins/jenkins:lts 镜像
 ┌────────────────┐                  ┌────────────────────┐
 │ /usr/local/bin/ │ ── 复制 ──→     │ gcc, cc, as, ld    │
 │ /usr/include/   │ ── 复制 ──→     │ C 头文件           │
 │ /usr/lib/       │ ── 复制 ──→     │ libc, libgcc       │
 └────────────────┘                  │ + Jenkins LTS      │
                                     └────────────────────┘
                                       = mybuildweb-jenkins
```

---

## 11. 常见问题排查

### Jenkins 无法启动
```bash
docker logs jenkins-hello     # 查看错误信息
docker-compose down && docker-compose up -d   # 重启
```

### 编译时报 "stdio.h: No such file"
说明 Docker 镜像缺少 C 头文件，需要重新构建：
```bash
docker-compose down
docker-compose build --no-cache
docker-compose up -d
```

### 从远程机器访问不了 8080 端口
1. 在宿主机上确认 Jenkins 正常运行：`curl http://localhost:8080/`
2. 检查防火墙：`iptables -L INPUT -n`
3. 使用 SSH 隧道（参见[第 5 节](#5-从另一台机器访问-jenkins)）

### API 调用报 "No valid crumb" 错误
必须使用会话 Cookie 配合 CSRF 令牌：
```bash
COOKIE_JAR=/tmp/jenkins-cookies.txt
CRUMB_JSON=$(curl -s -c "$COOKIE_JAR" 'http://localhost:8080/crumbIssuer/api/json')
# 后续请求中传入 -b "$COOKIE_JAR"
```

### 完全重置（删除所有 Jenkins 数据）
```bash
docker-compose down -v    # -v 会删除命名卷
docker-compose up -d      # 重新创建所有内容
```
> **警告**：这会删除所有任务配置和构建历史。

---

## 12. 术语表

| 术语 | 解释 |
|---|---|
| **Docker** | 将应用打包到隔离容器中运行的平台 — 就像软件的集装箱 |
| **Docker Compose** | 通过 YAML 文件定义和运行多容器 Docker 应用的工具 |
| **Jenkins** | 开源 CI/CD 自动化服务器 — 自动执行构建任务的"机器人助手" |
| **Jenkins Job** | Jenkins 中配置的一个任务（例如"编译这段代码"） |
| **Freestyle Project** | 一种简单的 Jenkins 任务类型，直接运行 Shell 命令 |
| **Pipeline** | 一种更高级的 Jenkins 任务类型，通过 Jenkinsfile 定义 |
| **CSRF Crumb** | Jenkins 要求 API 请求携带的安全令牌，防止跨站请求伪造 |
| **GCC** | GNU 编译器套件 — C/C++ 编译器 |
| **Make** | 构建自动化工具，读取 Makefile 来编译源代码 |
| **SSH 隧道** | 通过 SSH 加密连接转发网络端口的技术 |
| **Volume（卷）** | Docker 的数据持久化机制，数据不会随容器销毁而丢失 |
| **LTS** | 长期支持版 — Jenkins 的稳定、经过充分测试的发行版 |

---

*生成日期：2026-03-27*

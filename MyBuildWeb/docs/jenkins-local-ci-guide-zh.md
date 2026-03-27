# 本地 Jenkins CI — 用户指南

> **费曼学习法原则**：如果你不能用简单的语言解释清楚，说明你还没有真正理解它。
> 本指南以费曼学习法的方式，教你如何通过 Docker 在本地运行 Jenkins CI 服务器，用它自动编译 Hello World C 程序，并通过 MyBuildWeb 浏览构建历史和下载产物。

---

## 目录

1. [这是什么？](#1-这是什么)
2. [整体架构](#2-整体架构)
3. [前置条件](#3-前置条件)
4. [快速开始：启动 Jenkins](#4-快速开始启动-jenkins)
5. [从另一台机器访问（Windows）](#5-从另一台机器访问windows)
5a. [Windows 上的典型使用流程](#5a-windows-上的典型使用流程)
6. [构建任务：Build_HelloWorld_Simple（参数化构建）](#6-构建任务build_helloworld_simple参数化构建)
7. [触发构建](#7-触发构建)
8. [查看构建结果](#8-查看构建结果)
9. [查找编译产物](#9-查找编译产物)
10. [MyBuildWeb：构建历史仪表盘](#10-mybuildweb构建历史仪表盘)
11. [常用操作](#11-常用操作)
12. [文件结构](#12-文件结构)
13. [常见问题排查](#13-常见问题排查)
14. [术语表](#14-术语表)

---

## 1. 这是什么？

把 Jenkins 想象成工厂里的一个**机器人助手**。你告诉它："每次我说开始，就帮我编译 C 程序、运行它、告诉我结果。" Jenkins 就会精确地执行 — 自动化、一致、不会遗漏步骤。

本项目搭建了：
- 一个运行在 Docker 容器里的 **Jenkins 服务器**
- 同一个容器里预装了 **GCC 编译器工具链**
- 一个**预配置的构建任务**，用来编译和运行 `src/` 目录下的 Hello World C 程序
- 一个 **MyBuildWeb 仪表盘**（灵感来自 Isilon BuildWeb），用于浏览构建历史和下载产物

### 打个比方

想象你有一个私人厨师（Jenkins），住在一个移动厨房里（Docker 容器）。厨房里已经备好了所有厨具（GCC、Make）。你把菜谱（Jenkinsfile / 任务配置）交给厨师，他随时可以按你的要求做菜（编译代码）。

---

## 2. 整体架构

```
 你的机器（Linux 宿主机）
 ┌──────────────────────────────────────────────────────────────┐
 │                                                              │
 │  docker-compose.yml                                          │
 │  ┌───────────────────────────────────────────────┐           │
 │  │  容器：jenkins-hello                          │           │
 │  │  ┌─────────────┐  ┌────────────────────────┐  │           │
 │  │  │  Jenkins     │  │  GCC 15 + Make 4.4     │  │           │
 │  │  │  (端口 8080) │  │  (编译器工具链)        │  │           │
 │  │  └─────────────┘  └────────────────────────┘  │           │
 │  │                                               │           │
 │  │  卷：jenkins_home（构建数据 + 产物）          │           │
 │  └─────────────┬─────────────────────────────────┘           │
 │                │ 读取 buildweb_data/                          │
 │                ▼                                              │
 │  ┌───────────────────────────────────────────────┐           │
 │  │  MyBuildWeb 服务器（Python）                  │           │
 │  │  端口 9090 — 构建历史 + 产物下载              │           │
 │  └───────────────────────────────────────────────┘           │
 │                                                              │
 │  src/main.c + Makefile  ←── 你的源代码                       │
 └──────────────────────────────────────────────────────────────┘

 Windows / 远程机器
 ┌─────────────────────────────────┐
 │  浏览器 → SSH 隧道              │──── SSH (端口 22) ────→ 宿主机:8080 (Jenkins)
 │  http://localhost:8080 (Jenkins)│──── SSH (端口 22) ────→ 宿主机:9090 (BuildWeb)
 │  http://localhost:9090 (Build)  │
 └─────────────────────────────────┘
```

### 各组件的角色

| 组件 | 作用 | 比喻 |
|---|---|---|
| `docker-compose.yml` | 定义 Jenkins 的运行方式 | 厨房的蓝图 |
| `Dockerfile` | 构建带 GCC 的 Jenkins 镜像 | 厨房设备清单 |
| `Jenkinsfile` | 构建流水线定义 | 菜谱 |
| `src/main.c` | 要编译的 C 程序 | 食材 |
| `src/Makefile` | Make 的构建规则 | 烹饪步骤 |
| `buildweb/server.py` | 构建历史仪表盘 + 产物下载服务 | 餐厅的菜单展示板 |

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

## 5. 从另一台机器访问（Windows）

### 问题

如果你在另一台机器上（比如 Windows 笔记本），即使 `ping` 能通，端口 8080（Jenkins）和 9090（BuildWeb）也可能被两台机器之间的网络防火墙拦截。

### 解决方案：SSH 隧道

SSH 隧道就像一条**秘密地下通道** — 它把你的 HTTP 流量包裹在加密的 SSH 连接（端口 22）里传输，而防火墙通常允许 SSH 通过。

```
 Windows 笔记本                          Linux 宿主机 (10.227.226.50)
 ┌──────────┐     SSH（端口 22）     ┌────────────────────┐
 │ 浏览器    │ ═══════════════════> │ SSH 服务器          │
 │ localhost │     （加密传输）      │    ↓                │
 │ :8080     │ <═══════════════════ │ Jenkins :8080       │
 │ :9090     │ <═══════════════════ │ BuildWeb :9090      │
 └──────────┘                       └────────────────────┘
```

#### 方式 A：PowerShell / Windows 终端（自带 OpenSSH）

```powershell
ssh -L 8080:localhost:8080 -L 9090:localhost:9090 root@<LINUX宿主机IP>
```

保持这个窗口不要关闭（关了隧道就断了）。然后在浏览器打开：
- **http://localhost:8080** — Jenkins
- **http://localhost:9090** — BuildWeb

#### 方式 B：PuTTY

1. 打开 PuTTY，输入 Host: `<LINUX宿主机IP>`，Port: `22`
2. 进入 **Connection → SSH → Tunnels**
3. Source port: `8080`，Destination: `localhost:8080` → 点击 **Add**
4. Source port: `9090`，Destination: `localhost:9090` → 点击 **Add**
5. 点击 **Open**，登录
6. 在浏览器打开：
   - **http://localhost:8080** — Jenkins
   - **http://localhost:9090** — BuildWeb

#### 方式 C：VS Code Remote SSH

如果你使用 VS Code 的 Remote-SSH 扩展：
1. 连接到 Linux 宿主机
2. VS Code 会自动转发端口
3. 在本地浏览器打开：
   - **http://localhost:8080** — Jenkins
   - **http://localhost:9090** — BuildWeb

#### 验证连通性（在 Windows 上）

```powershell
# 测试 SSH 是否能通
ssh root@<LINUX宿主机IP> "echo OK"

# 建立 SSH 连接后，测试两个隧道是否生效
Test-NetConnection localhost -Port 8080
# 期望结果：TcpTestSucceeded : True
Test-NetConnection localhost -Port 9090
# 期望结果：TcpTestSucceeded : True
```

---

## 5a. Windows 上的典型使用流程

SSH 隧道建立后，以下是从 Windows 机器上完成"构建代码 → 查看结果 → 下载产物"的完整流程：

### 第 1 步：触发构建（Jenkins）

1. 在浏览器中打开 **http://localhost:8080**
2. 点击 **Build_HelloWorld_Simple**
3. 点击左侧栏的 **Build with Parameters**
4. 填写参数（使用默认值即可）：

| 参数 | 值 | 说明 |
|---|---|---|
| REPO_NAME | `hello_world` | Git 仓库名 |
| BRANCH | `main` | 要构建的分支 |
| GIT_BASE_URL | `https://eos2git.cec.lab.emc.com/mam28` | 默认值，无需修改 |

5. 点击 **Build**（构建）
6. 等待约 15 秒，构建完成

### 第 2 步：查看构建结果（BuildWeb）

1. 在浏览器中打开 **http://localhost:9090**
2. 你会看到构建仪表盘：

```
 ┌──────────────────────────────────────────────────────┐
 │  MyBuildWeb                         Builds: 3        │
 ├──────────────────────────────────────────────────────┤
 │  Branch: hello_world | Succeeded: 3 | Failed: 0     │
 ├──────────────────────────────────────────────────────┤
 │  ┌──────────────────────（绿色）──────────────────┐  │
 │  │ hello_world_003                                │  │
 │  │ 状态: Succeeded  耗时: 0:00:12                 │  │
 │  │ Git: a0cb1d6be0e4  机器: sles15sp6             │  │
 │  │ [main (16.3 KB)] [main.o (3.2 KB)] [.tar.gz]  │  │
 │  └────────────────────────────────────────────────┘  │
 │  ┌──────────────────────（绿色）──────────────────┐  │
 │  │ hello_world_002                                │  │
 │  │ ...                                            │  │
 │  └────────────────────────────────────────────────┘  │
 └──────────────────────────────────────────────────────┘
```

3. **绿色卡片** = 构建成功，**红色卡片** = 构建失败
4. 每张卡片显示：构建名称、状态、耗时、git hash、机器名

### 第 3 步：下载产物

**通过 Web 界面：**
- 点击构建卡片上的绿色/蓝色下载按钮
- `main` — 编译好的 ELF 二进制文件
- `main.o` — 目标文件
- `hello_world-src.tar.gz` — 源代码压缩包

**通过 PowerShell：**
```powershell
# 下载编译好的二进制文件
Invoke-WebRequest http://localhost:9090/download/hello_world_003/main -OutFile main

# 下载源码压缩包
Invoke-WebRequest http://localhost:9090/download/hello_world_003/hello_world-src.tar.gz -OutFile hello_world-src.tar.gz
```

### 第 4 步：查看控制台日志

点击 BuildWeb 仪表盘上的构建名称（如 `hello_world_003`），即可查看完整的控制台输出 — 与 Jenkins Console Output 中看到的日志内容相同。

### 快速参考卡

| 操作 | 位置 | URL |
|---|---|---|
| 触发构建 | Jenkins | http://localhost:8080/job/Build_HelloWorld_Simple/build |
| 查看所有构建 | BuildWeb | http://localhost:9090 |
| 查看构建详情 | BuildWeb | http://localhost:9090/build/hello_world_003 |
| 下载产物 | BuildWeb | http://localhost:9090/download/hello_world_003/main |
| 构建列表（JSON） | BuildWeb API | http://localhost:9090/api/builds |
| Jenkins 控制台日志 | Jenkins | http://localhost:8080/job/Build_HelloWorld_Simple/lastBuild/console |

---

## 6. 构建任务：Build_HelloWorld_Simple（参数化构建）

这个任务支持 **Build with Parameters（参数化构建）** — 你只需填写 Git 仓库名称，Jenkins 就会自动从你的 Git 服务器上 clone 代码并编译。这与生产环境 Jenkins 上的 `Build_OneFS_Simple` 工作方式类似。

### 构建参数

| 参数 | 默认值 | 说明 |
|---|---|---|
| **REPO_NAME** | `hello_world` | 你 Git 账号下的仓库名（如 `hello_world`、`my_project`） |
| **BRANCH** | `main` | 要构建的分支 |
| **GIT_BASE_URL** | `https://eos2git.cec.lab.emc.com/mam28` | Git 服务器基础 URL |

### 打个比方

就像去餐厅点菜：你不需要自己进厨房，只需填好点菜单（参数）— 点哪道菜（仓库）、什么口味（分支）— 厨师（Jenkins）会搞定一切。

### 构建阶段

任务自动运行 4 个阶段：

```
 阶段 1：环境检查       阶段 2：克隆代码        阶段 3：编译            阶段 4：测试
 ──────────────>       ──────────────>         ──────────────>        ──────────────>
 打印 gcc/make/git     从 eos2git 服务器       make clean              运行 ./main
 版本信息              git clone 代码          make all                验证输出
```

### 内部执行过程

```bash
# 阶段 1 - 环境检查
gcc --version
make --version
git --version

# 阶段 2 - 克隆仓库
git clone --branch ${BRANCH} --single-branch \
    ${GIT_BASE_URL}/${REPO_NAME}.git ${WORKSPACE}/repo

# 阶段 3 - 编译
cd ${WORKSPACE}/repo
make clean
make all          # 编译：main.c → main.o → main

# 阶段 4 - 测试
./main            # 输出："Hello, World!"
```

### Git 认证配置

Jenkins 容器使用 `.netrc` 文件存储 Git 凭据。配置方法（首次使用或容器重建后）：

```bash
docker exec jenkins-hello bash -c "cat > /root/.netrc << 'EOF'
machine eos2git.cec.lab.emc.com
login <你的用户名>
password <你的Personal Access Token>
EOF
chmod 600 /root/.netrc"
```

> **注意**：在 `https://eos2git.cec.lab.emc.com/settings/tokens/new` 生成经典 Personal Access Token，勾选 **repo** 权限。

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

### 通过 Web 界面（参数化构建）

1. 打开 **http://localhost:8080**
2. 点击 **Build_HelloWorld_Simple**
3. 点击左侧栏的 **Build with Parameters**（参数化构建）
4. 填写参数：
   - **REPO_NAME**：例如 `hello_world`
   - **BRANCH**：例如 `main`
   - **GIT_BASE_URL**：默认值适用于 `mam28` 的仓库，无需修改
5. 点击 **Build**（构建）

### 通过命令行

```bash
# 获取 CSRF 令牌
COOKIE_JAR=/tmp/jenkins-cookies.txt
CRUMB_JSON=$(curl -s -c "$COOKIE_JAR" 'http://localhost:8080/crumbIssuer/api/json')
CRUMB_VALUE=$(echo "$CRUMB_JSON" | python3 -c "import sys,json; print(json.load(sys.stdin)['crumb'])")

# 触发参数化构建
curl -s -X POST "http://localhost:8080/job/Build_HelloWorld_Simple/buildWithParameters" \
  -b "$COOKIE_JAR" \
  -H "Jenkins-Crumb: $CRUMB_VALUE" \
  --data-urlencode "REPO_NAME=hello_world" \
  --data-urlencode "BRANCH=main" \
  --data-urlencode "GIT_BASE_URL=https://eos2git.cec.lab.emc.com/mam28"
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
+ set -e
============================================
  Build_HelloWorld_Simple (Parameterized)
============================================
  REPO_NAME : hello_world
  BRANCH    : main
  GIT_URL   : https://eos2git.cec.lab.emc.com/mam28/hello_world.git
============================================

=== Stage 1: Environment ===
gcc (GCC) 15.2.0
GNU Make 4.4.1
git version 2.47.3

=== Stage 2: Clone Repository ===
Cloning into '/var/jenkins_home/workspace/Build_HelloWorld_Simple/repo'...

=== Stage 3: Build ===
cc -O0 -g -c -o main.o main.c
cc -O0 -g -o main main.o

=== Stage 4: Test ===
Hello, World!

============================================
  BUILD SUCCESS
============================================
Finished: SUCCESS
```

---

## 9. 查找编译产物

构建成功后，编译生成的文件保存在 **Jenkins 容器内部**。把它想象成一个仓库 — 成品放在特定的货架（目录）上，你需要知道货架编号才能找到它们。

### 产物位置

```
容器内路径：
/var/jenkins_home/workspace/Build_HelloWorld_Simple/repo/
├── main      ← 可执行二进制文件
├── main.o    ← 目标文件
├── main.c    ← 源代码（clone 下来的）
└── Makefile  ← 构建规则（clone 下来的）
```

### 取出产物

#### 方式 A：docker cp（最简单）

把编译好的二进制文件拷贝到宿主机：
```bash
# 拷贝可执行文件
docker cp jenkins-hello:/var/jenkins_home/workspace/Build_HelloWorld_Simple/repo/main ./main

# 拷贝全部构建产物
docker cp jenkins-hello:/var/jenkins_home/workspace/Build_HelloWorld_Simple/repo/ ./build-output/
```

#### 方式 B：进入容器

```bash
docker exec -it jenkins-hello bash
cd /var/jenkins_home/workspace/Build_HelloWorld_Simple/repo/
ls -la
```

#### 方式 C：通过 Jenkins API 查看

```bash
# 列出工作空间文件
curl -s http://localhost:8080/job/Build_HelloWorld_Simple/ws/repo/

# 下载指定文件
curl -s -O http://localhost:8080/job/Build_HelloWorld_Simple/ws/repo/main
```

### 产物生命周期

- 每次新构建会 **覆盖** 上一次的产物（构建前会清理工作空间再重新 clone）
- 只要 Jenkins 容器和它的卷还在，产物就会一直保留
- 执行 `docker-compose down -v` 会**删除所有产物**（连同卷一起删除）

---

## 10. MyBuildWeb：构建历史仪表盘

MyBuildWeb 是一个轻量级 HTTP 服务器（灵感来自 Isilon 的 `build.west.isilon.com`），提供**基于网页的仪表盘**，用于浏览构建历史和下载产物。它从 Jenkins 的 Docker 卷中读取构建元数据 — 不需要数据库。

### 打个比方

如果 Jenkins 是做菜的厨师，那 MyBuildWeb 就是**餐厅的展示橱窗** — 它展示厨师做过的每道菜（构建）：什么时候做的、做得好不好、还能让你把成品打包带走（下载产物）。

### 工作原理

```
 Jenkins 容器 (jenkins-hello)         BuildWeb 容器 (mybuildweb)
 ┌──────────────────────┐            ┌──────────────────────────────┐
 │ 构建完成 →           │            │                              │
 │ 保存到卷：           │            │  从共享卷读取：              │
 │  buildweb_data/      │───────────→│  /var/jenkins_home/          │
 │   hello_world_001/   │  (共享的)  │    buildweb_data/            │
 │    build_meta.json   │  jenkins   │                              │
 │    artifacts/        │  _home     │  在端口 9090 提供服务：      │
 │     main             │  卷        │  - 网页界面（构建历史）      │
 │     main.o           │            │  - JSON API                  │
 │     *.tar.gz         │            │  - 文件下载                  │
 └──────────────────────┘            └──────────────────────────────┘
```

### 启动 BuildWeb 服务器

BuildWeb 作为 Docker 容器与 Jenkins 一起运行，由 `docker-compose` 统一管理：

```bash
cd /path/to/io-tracing/MyBuildWeb

# 同时启动 Jenkins 和 BuildWeb
docker-compose up -d

# 验证 BuildWeb 是否运行
curl -s -o /dev/null -w "%{http_code}" http://localhost:9090/
# 期望输出：200

# 查看容器状态
docker ps --format "table {{.Names}}\t{{.Status}}" | grep -E "jenkins|buildweb"
# jenkins-hello   Up 10 minutes
# mybuildweb      Up 10 minutes
```

> **注意**：BuildWeb 以只读方式挂载 `jenkins_home` 卷。Jenkins 每次构建完成后，BuildWeb 会自动读取到新数据 — 无需重启。

### 访问 Web 界面

在浏览器中打开：
```
http://localhost:9090
```

你会看到一个仪表盘，包含：
- **摘要栏** — 总构建数、成功数、失败数
- **构建卡片** — 每次构建一张卡片，颜色编码：
  - 绿色：成功（Succeeded）
  - 红色：失败（Failed）
  - 青色：运行中（Running）
- **构建元数据** — 名称、状态、耗时、开始/结束时间、git hash、机器名、构建编号
- **下载按钮** — 点击即可下载任何产物（二进制文件、目标文件、源码压缩包）

### 从远程机器访问（SSH 隧道）

和 Jenkins 一样，MyBuildWeb 也需要 SSH 隧道才能从远程访问。可以在一条 SSH 命令中同时转发两个端口：

```powershell
# 同时转发 Jenkins（8080）和 BuildWeb（9090）
ssh -L 8080:localhost:8080 -L 9090:localhost:9090 root@<LINUX宿主机IP>
```

然后在浏览器中打开：
- Jenkins：**http://localhost:8080**
- BuildWeb：**http://localhost:9090**

### 构建详情页

点击任意构建名称（如 `hello_world_001`）可以进入**构建详情页**，包含：
- 完整的构建元数据表格
- 控制台输出日志（与 Jenkins Console Output 相同）

URL 格式：`http://localhost:9090/build/hello_world_001`

### JSON API

MyBuildWeb 提供 JSON API 供程序化访问：

```bash
# 列出所有构建
curl -s http://localhost:9090/api/builds | python3 -m json.tool
```

示例响应：
```json
[
  {
    "build_name": "hello_world_003",
    "status": "Succeeded",
    "duration": "12",
    "start_time": "2026-03-27 04:30:00",
    "end_time": "2026-03-27 04:30:12",
    "started_by": "anonymous",
    "machine": "sles15sp6",
    "build_number": "003",
    "git_hash": "a0cb1d6be0e4...",
    "git_repo": "hello_world",
    "last_step": "Test"
  }
]
```

### 下载产物

#### 通过 Web 界面

每个构建卡片上都有下载按钮，点击即可直接下载对应的产物。

#### 通过命令行

```bash
# 下载编译好的二进制文件
curl -s -O http://localhost:9090/download/hello_world_001/main

# 下载源码压缩包
curl -s -O http://localhost:9090/download/hello_world_001/hello_world-src.tar.gz

# 下载目标文件
curl -s -O http://localhost:9090/download/hello_world_001/main.o
```

#### URL 格式

```
http://localhost:9090/download/{构建名称}/{文件名}
```

### 构建数据目录结构

每次构建都保存为 `buildweb_data/` 下的一个目录：

```
buildweb_data/
├── hello_world_001/
│   ├── build_meta.json      ← 构建元数据（状态、时间、git hash 等）
│   ├── console.log          ← Jenkins 控制台输出
│   └── artifacts/
│       ├── main             ← 编译好的 ELF 二进制文件
│       ├── main.o           ← 目标文件
│       └── hello_world-src.tar.gz  ← 源代码压缩包
├── hello_world_002/
│   ├── build_meta.json
│   ├── console.log
│   └── artifacts/
│       └── ...
└── ...
```

### 环境变量

| 变量 | 默认值 | 说明 |
|---|---|---|
| `BUILDS_DIR` | `/var/jenkins_home/buildweb_data` | 构建数据目录路径 |
| `BUILDWEB_PORT` | `8081` | 监听端口（我们覆盖为 `9090`） |

### 停止 / 重启 BuildWeb 服务器

```bash
# 仅停止 BuildWeb（Jenkins 继续运行）
docker-compose stop buildweb

# 仅重启 BuildWeb
docker-compose restart buildweb

# 停止所有服务（Jenkins + BuildWeb）
docker-compose down

# 查看 BuildWeb 容器日志
docker logs mybuildweb
```

---

## 11. 常用操作

| 操作 | 命令 |
|---|---|
| 启动所有服务（Jenkins + BuildWeb） | `docker-compose up -d` |
| 停止所有服务（Jenkins + BuildWeb） | `docker-compose down` |
| 重启 Jenkins | `docker-compose restart` |
| 查看容器日志 | `docker logs jenkins-hello` |
| 重新构建镜像（修改 Dockerfile 后） | `docker-compose build && docker-compose up -d` |
| 进入容器终端 | `docker exec -it jenkins-hello bash` |
| 在容器内测试 GCC | `docker exec jenkins-hello gcc --version` |
| 删除所有数据（全新开始） | `docker-compose down -v` |
| 启动 BuildWeb | `docker-compose up -d buildweb` |
| 停止 BuildWeb | `docker-compose stop buildweb` |
| 重启 BuildWeb | `docker-compose restart buildweb` |
| 查看 BuildWeb 日志 | `docker logs mybuildweb` |
| 打开 BuildWeb | `http://localhost:9090` |

---

## 12. 文件结构

```
MyBuildWeb/
├── Dockerfile              ← Jenkins + GCC 镜像定义
├── docker-compose.yml      ← Docker Compose：Jenkins + BuildWeb 双服务配置
├── Jenkinsfile             ← 流水线定义（用于 SCM 类型的任务）
├── buildweb/
│   ├── Dockerfile          ← BuildWeb 镜像定义（python:3-slim）
│   └── server.py           ← MyBuildWeb 仪表盘服务器（Python，端口 9090）
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

## 13. 常见问题排查

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
3. 使用 SSH 隧道（参见[第 5 节](#5-从另一台机器访问windows)）

### API 调用报 "No valid crumb" 错误
必须使用会话 Cookie 配合 CSRF 令牌：
```bash
COOKIE_JAR=/tmp/jenkins-cookies.txt
CRUMB_JSON=$(curl -s -c "$COOKIE_JAR" 'http://localhost:8080/crumbIssuer/api/json')
# 后续请求中传入 -b "$COOKIE_JAR"
```

### BuildWeb 显示 "No builds yet"
1. 确认构建数据存在：`ls /var/lib/docker/volumes/mybuildweb_jenkins_home/_data/buildweb_data/`
2. 检查启动服务器时 `BUILDS_DIR` 设置是否正确
3. 从 Jenkins 触发一次构建并等待完成

### BuildWeb 9090 端口无法访问
1. 确认服务器在运行：`ps aux | grep server.py`
2. 本地验证：`curl http://localhost:9090/`
3. 远程访问需要 SSH 隧道：`ssh -L 9090:localhost:9090 root@<LINUX宿主机IP>`

### 完全重置（删除所有 Jenkins 数据）
```bash
docker-compose down -v    # -v 会删除命名卷
docker-compose up -d      # 重新创建所有内容
```
> **警告**：这会删除所有任务配置和构建历史。

---

## 14. 术语表

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
| **Build with Parameters** | Jenkins 的参数化构建功能，触发构建时可以传入输入值（如仓库名、分支） |
| **PAT** | Personal Access Token — 通过 HTTPS 访问 Git 服务器时的密码替代品 |
| **.netrc** | 存储机器凭据的文件，供 `git`、`curl` 等工具自动登录使用 |
| **Workspace（工作空间）** | Jenkins 中用于检出代码和执行构建的目录 |
| **MyBuildWeb** | 轻量级构建历史仪表盘，灵感来自 Isilon BuildWeb — 展示构建元数据和可下载产物 |
| **BuildWeb** | Isilon 内部的构建结果网站（`build.west.isilon.com`）；MyBuildWeb 是它的简化本地版 |
| **build_meta.json** | 每次 Jenkins 构建生成的 JSON 文件，包含元数据（状态、时间、git hash 等） |

---

*更新日期：2026-03-27*

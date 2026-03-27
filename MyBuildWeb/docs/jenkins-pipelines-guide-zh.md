# Jenkins Pipelines — 费曼学习法指南

> **费曼学习法原则**：如果你不能用简单的语言解释清楚，说明你还没有真正理解它。
> 本指南以费曼学习法的方式，像讲故事一样，把 PowerScale (OneFS) Jenkins CI/CD 流水线系统讲明白。我们从全局入手，再逐步深入。

---

## 目录

1. [这套系统到底是干什么的？](#1-这套系统到底是干什么的)
2. [全局视角：OneFS 是怎么被构建出来的](#2-全局视角onefs-是怎么被构建出来的)
3. [主构建流水线 — 一步一步讲清楚](#3-主构建流水线--一步一步讲清楚)
4. [打包：把构建产物变成可交付的东西](#4-打包把构建产物变成可交付的东西)
5. [代码签名：确保没有人篡改过](#5-代码签名确保没有人篡改过)
6. [QA 测试包：构建测试工具](#6-qa-测试包构建测试工具)
7. [云构建：AMI 和 VHD 镜像](#7-云构建ami-和-vhd-镜像)
8. [工具类流水线和脚本](#8-工具类流水线和脚本)
9. [节点管理](#9-节点管理)
10. [发布流程：测试 → 预发布 → 生产](#10-发布流程测试--预发布--生产)
11. [目录结构地图](#11-目录结构地图)
12. [术语表](#12-术语表)

---

## 1. 这套系统到底是干什么的？

想象你要盖一栋房子，你需要：

- **原材料**（源代码）
- **工人**（构建服务器）
- **工头**（Jenkins）来告诉工人做什么、按什么顺序做
- **质检员**（代码签名、测试）
- **运输车**（打包成安装包、U 盘镜像、模拟器、云镜像）

这个代码仓库就是**工头的施工手册** — 一系列 Jenkins 流水线定义文件（Jenkinsfile），它们编排了 Dell PowerScale OneFS 操作系统从构建、签名、打包到分发的整个过程。

### 为什么这很重要？

如果没有这些流水线，工程师需要手动编译代码、签名二进制文件、创建安装包、上传产物 — 全部手工操作。这些流水线把成千上万个步骤自动化成了一个"按下按钮"的工作流。

---

## 2. 全局视角：OneFS 是怎么被构建出来的

把构建过程想象成一场**接力赛**，有四棒：

```
   [1] 构建            [2] 签名            [3] 打包             [4] 分发
   ─────────>         ─────────>         ─────────>          ─────────>
   在 FreeBSD jail    对所有二进制       创建安装包、          上传到
   中编译 OneFS       文件进行密码        U 盘镜像、OVA、      制品服务器
   源代码             学签名              云镜像               并通知用户
```

每一"棒"都是一条独立的 Jenkins 流水线，它们会自动串联起来。

### 打个比方

把它想象成一条**汽车工厂的流水线**：
1. **构建** = 焊接底盘和发动机（编译代码）
2. **签名** = 打上车辆识别号和安全认证标签（密码学证明）
3. **打包** = 把车装进运输集装箱（创建安装包）
4. **分发** = 送到经销商那里（上传到服务器）

---

## 3. 主构建流水线 — 一步一步讲清楚

**文件**：`Jenkinsfile_onefs_build` — 整个乐团的"指挥"

这是最重要的一个文件。一切都从它开始。下面逐步讲解：

### 阶段 1：准备 — "备料"

就像厨师做菜前准备食材：

1. **校验分支名** — 确保没人把 "BR_MAIN" 打成 "br-main"
2. **获取 Git SHA** — 问 GitHub："这个分支上最新的提交是哪个？"这就像给我们要构建的代码版本取一个序列号。
3. **向 BuildWeb 注册** — 告诉内部构建追踪系统："嘿，我正在开始 BR_MAIN 分支的第 47 号构建。"
4. **创建目录软链接** — 设置好构建产物存放的文件路径。

### 阶段 2：构建 QA（异步）— "订购测试设备"

就像在主构建运行的同时订购测试设备 — 我们不等它完成：

- 触发一个独立的任务来构建 Linux/Windows 测试包。
- `wait: false` — 主构建立即继续，不等待。

### 阶段 3：检出 OneFS — "拿到设计图纸"

- 克隆 OneFS 源代码仓库。
- 使用**引用仓库**来加速（类似缓存 — 不用下载所有内容，而是引用本地副本）。

### 阶段 4：准备 Jail — "搭建施工现场"

这是 FreeBSD/OneFS 特有的：

- **Jail** 是 FreeBSD 的轻量级容器。OneFS 必须在 FreeBSD jail 内构建，因为它本身就是基于 FreeBSD 的操作系统。
- 不同版本的 OneFS 需要不同类型的 jail（FreeBSD 11 vs 12 vs 13）。

### 阶段 5：检出 Driver — "拿到工头的工具箱"

- 检出 `tools-driver`，一个在 jail 内编排实际编译步骤的 Python 脚本。

### 阶段 6：构建 OneFS — "真正的施工"

这是奇迹发生的地方：

```
sudo jexec -n ${jail.name} sh -c "python ./driver.py $driverArgs"
```

翻译：**"进入 FreeBSD jail 并运行 driver 脚本，它会编译整个 OneFS。"**

**关键细节**：driver 还会通过 HTTP 调用签名 Jenkins 任务来触发代码签名。这就是构建系统和签名系统之间通信的方式。

### 阶段 7：Coverity 分析（可选）— "安全检查"

如果启用了，会运行 Coverity 静态分析来在不运行代码的情况下发现 bug。

### 阶段 8：打包 OneFS — "装箱"

触发 `Jenkinsfile_onefs_packaging` 来创建所有可交付产物。

### 阶段 9：云构建（可选）— "为云平台构建"

如果需要，创建 AMI（AWS）和 VHD（Azure）镜像。

---

## 4. 打包：把构建产物变成可交付的东西

**文件**：`Jenkinsfile_onefs_packaging` — "装配车间"

原始构建完成后，我们需要把它变成用户可以实际安装的东西。这条流水线编排四个子任务：

| 产物 | 流水线 | 类比 |
|------|--------|------|
| **安装包** | `Jenkinsfile_pkg_install` | 就像 "setup.exe" — 用来升级现有集群 |
| **重装 U 盘** | `Jenkinsfile_pkg_reimage_stick` | 一个可启动的 USB 驱动器，用于完全重装节点 |
| **配置包** | `Jenkinsfile_pkg_provisioning` | IPS（Isilon 配置服务）包，用于重铺操作 |
| **模拟器 OVA** | `Jenkinsfile_pkg_simulator` | VMware 虚拟机，用于在没有硬件的情况下测试 OneFS |

### 工作原理

1. **安装包**先运行（它是前置条件）
2. 然后**重装 U 盘**、**配置包**和**模拟器****并行运行** — 因为它们互不依赖

就像：先烤好蛋糕（安装包），然后同时把蛋糕装盒、在副本上插蜡烛、给网站拍照。

---

## 5. 代码签名：确保没有人篡改过

**文件**：`codesign/Jenkinsfile_codesign` — "公证人"

### 为什么要签名？

想象你收到一封信。你怎么知道它真的是声称的那个人寄的？你检查火漆封印。代码签名就是数字版的火漆封印 — 它用密码学方法证明软件在构建后没有被修改过。

### 工作原理（简化版）

```
1. 从构建服务器 → 签名服务器 复制文件
2. 启动一个带有 Garasign（HSM 客户端）的 Docker 容器
3. 从 HSM（硬件安全模块）导出私钥
4. 使用 OpenSSL 签名每个文件
5. 验证签名是否有效
6. 把签名后的文件复制回构建服务器
7. 写入一个"哨兵"文件告诉构建服务器"我完成了"
```

### 四种签名模式

| 模式 | 签名对象 | 现实世界类比 |
|------|---------|-------------|
| **Packaging** | 通用文件（RSA 签名） | 文件上的公证章 |
| **SecureBoot_PE** | 固件（Authenticode 格式） | 政府颁发的安全贴纸 |
| **SecureBoot_RSA** | 内核清单 | 真品证书 |
| **OVA** | VMware 虚拟机 | 包裹上的防拆封条 |

### HSM 连接机制

签名密钥永远不会离开 HSM（一个物理安全设备）。取而代之的是：
1. Garasign **导出密钥的混淆副本**
2. OpenSSL 使用这个混淆密钥进行签名
3. 使用后密钥被删除

打个比方：银行给你一张临时门禁卡来访问保险箱，用完后门禁卡自动销毁。

---

## 6. QA 测试包：构建测试工具

**文件**：`Jenkinsfile_qa_linux`、`Jenkinsfile_test_package_windows7`、`Jenkinsfile_qa_packaging`

这些流水线为不同平台构建测试包，让 QA 工程师可以对 OneFS 构建进行测试。

### 支持的平台（通过 Docker）

每个平台在 `qabuild/` 中有自己的 Dockerfile：

| Dockerfile | 平台 |
|-----------|------|
| `Dockerfile.centos7` | CentOS 7 |
| `Dockerfile.centos8` | CentOS 8 |
| `Dockerfile.bionic` | Ubuntu 18.04 |
| `Dockerfile.focal` | Ubuntu 20.04 |
| `Dockerfile.jammy` | Ubuntu 22.04 |
| `Dockerfile.noble` | Ubuntu 24.04 |
| `Dockerfile.xenial` | Ubuntu 16.04 |
| `Dockerfile.opensuse15` | OpenSUSE 15 |

测试包的构建方式：检出 OneFS 源码，在匹配目标平台的 Docker 容器内运行测试构建，然后上传结果。

---

## 7. 云构建：AMI 和 VHD 镜像

**文件**：`Jenkinsfile_cloud_packaging`

这条流水线从成功的 OneFS 构建中创建云原生镜像：

- **AMI** — Amazon Machine Image，用于 AWS
- **VHD** — Virtual Hard Disk，用于 Azure

打个比方：你造了一辆车，现在你需要把它改造成一艘船（AMI）和一架飞机（VHD），因为它们要在不同的"路"上跑。

---

## 8. 工具类流水线和脚本

### 工具目录（`utilities/`）

| 流水线 | 功能 |
|--------|------|
| `Jenkinsfile_helloworld` | 一个简单的测试流水线 — Jenkins 的 "Hello World" |
| `Jenkinsfile_node_maintenance` | 在构建节点上运行维护任务 |
| `Jenkinsfile_node_updater` | 更新构建节点上的软件 |
| `Jenkinsfile_builder_upgrade` | 升级构建机器 |
| `Jenkinsfile_heap_report` | 生成 JVM 堆报告，用于 Jenkins 诊断 |
| `Jenkinsfile_ps_image_builder` | 构建 PowerScale 虚拟机镜像 |
| `Jenkinsfile_ps_image_tester` | 测试 PowerScale 虚拟机镜像 |
| `Jenkinsfile_maintenance_launcher` | 编排多个维护任务 |
| `Jenkinsfile_prep_for_dev` | 准备开发环境 |
| `Jenkinsfile_win7_maintenance` | 维护 Windows 7 构建节点 |

### Shell 脚本

| 脚本 | 用途 |
|------|------|
| `fastmerge.sh` | 快速合并 dev → stage → prod（带安全提示）。警告："这不是正规的 Git 礼仪" |
| `promote-cec.sh` | 推进 cec-tst → cec-staging → cec-prd（正规方式） |
| `sshcmd.sh` | SSH 命令助手 |

---

## 9. 节点管理

**文件**：`nodes/nodecmd.py`

### 是什么？

一个 Python 工具，用于在多个 Jenkins 构建节点上同时运行 SSH 命令。

### 打个比方

想象你管理着 50 台服务器。与其逐个 SSH 进去运行 `uptime`，你直接告诉 `nodecmd.py`："在所有标记为 `builder` 的服务器上运行 `uptime`。"

### 工作原理

1. 读取一个 Jenkins CasC（配置即代码）YAML 文件，里面列出了所有节点
2. 按标签过滤节点（如 `builder`、`packager`、`docker-agent`）
3. 在所有匹配的节点上运行给定的 SSH 命令（可选并行执行）

```bash
# 示例：检查所有 packager 节点的磁盘空间
python nodecmd.py -f osj-isi-02-prd.yaml -l packager -c "df -h"

# 示例：并行更新所有 builder 节点的软件包
python nodecmd.py -f osj-isi-02-prd.yaml -l builder -p -c "sudo pkg update"
```

### 节点配置文件

| 文件 | 用途 |
|------|------|
| `osj-isi-02-prd.yaml` | **生产**环境 Jenkins 控制器配置（60+ 节点） |
| `osj-isi-02-tst.yaml` | **测试**环境 Jenkins 控制器配置 |
| `dummy_nodes.yaml` | 测试用的假节点配置 |

生产环境配置包含：FreeBSD 构建机（ps-build-500 ~ 531）、打包机（ps-build-pkg-100 ~ 109）、Linux Docker 代理、Coverity 工作节点、AMI 构建机、VHD 构建机、Windows QA 构建节点等。

---

## 10. 发布流程：测试 → 预发布 → 生产

### 三个环境

```
  cec-tst（测试）  →  cec-staging（预发布）  →  cec-prd（生产）
```

这是一个标准的部署模式：

1. **测试（cec-tst）**：开发者合并功能分支的地方。这里的流水线是实验性的。
2. **预发布（cec-staging）**：生产环境的镜像。测试通过后，变更被推进到这里做最终验证。
3. **生产（cec-prd）**：真正构建和发布 OneFS 的活跃流水线。

### `promote-cec.sh` 脚本

```
步骤 1：合并 cec-tst → cec-staging，推送
步骤 2：暂停 — 人工验证预发布环境是否正常
步骤 3：合并 cec-staging → cec-prd，推送
步骤 4：切回 cec-tst 分支
```

打个比方：把新菜谱从测试厨房 → 搬到预演厨房让主厨审核 → 再搬到正式餐厅的厨房。

---

## 11. 目录结构地图

```
jenkins-pipelines/
├── Jenkinsfile_onefs_build          # 主编排器
├── Jenkinsfile_onefs_packaging      # 打包编排器
├── Jenkinsfile_cloud_packaging      # 云镜像构建
├── Jenkinsfile_coverity             # 静态分析
├── Jenkinsfile_ime_onefs_launcher   # IME 构建
├── Jenkinsfile_pkg_*                # 各个打包任务
├── Jenkinsfile_qa_*                 # QA 测试包任务
├── Jenkinsfile_test_package_*       # 平台特定的测试包
│
├── codesign/                        # 代码签名子系统
│   ├── Jenkinsfile_codesign         # 核心签名引擎
│   ├── Jenkinsfile_isi_packager     # .isi 包创建
│   ├── Jenkinsfile_manifest_signing # 清单/补丁签名
│   ├── Jenkinsfile_sign_and_package_launcher  # 签名编排器
│   ├── Jenkinsfile_mpa_signer       # 多方授权签名
│   ├── Jenkinsfile_sign_simulator   # 模拟器签名
│   └── container/                   # 签名用的 Docker 镜像
│       ├── Dockerfile               # Garasign/CSSv3 容器
│       ├── config.ini               # Garasign 配置
│       └── ...                      # 签名证书和配置
│
├── qabuild/                         # QA 构建用 Docker 镜像
│   ├── Dockerfile.centos7           # CentOS 7 构建环境
│   ├── Dockerfile.focal             # Ubuntu 20.04 构建环境
│   └── ...                          # 其他平台
│
├── nodes/                           # 节点管理
│   ├── nodecmd.py                   # SSH 命令多路复用器
│   ├── osj-isi-02-prd.yaml         # 生产节点配置
│   └── osj-isi-02-tst.yaml         # 测试节点配置
│
├── utilities/                       # 维护和辅助流水线
├── deprecated/                      # 已弃用的流水线（保留供参考）
├── ducttape/                        # 快速修复/临时流水线
├── experimental/                    # 实验性/原型流水线
├── hc/                              # 健康检查流水线
├── misc/                            # 杂项（NFS 客户端测试）
├── pscale-admin/                    # 管理任务
│
├── promote-cec.sh                   # 发布脚本（tst→staging→prd）
├── fastmerge.sh                     # 快速合并脚本（dev→stage→prod）
├── sshcmd.sh                        # SSH 助手
└── README.md                        # 原始代码地图
```

---

## 12. 术语表

| 术语 | 含义 |
|------|------|
| **OneFS** | Dell PowerScale 的集群文件系统操作系统 |
| **BuildWeb** | 内部构建追踪系统（追踪构建编号、状态） |
| **Jail** | FreeBSD 轻量级容器，用于构建 OneFS |
| **Driver** | `tools-driver` — 一个在 jail 内编排实际编译的 Python 脚本 |
| **Garasign** | Dell 的代码签名服务（CSSv3），管理 HSM 密钥 |
| **HSM** | 硬件安全模块 — 存储密码学密钥的物理设备 |
| **CasC** | 配置即代码 — 用 YAML 格式定义的 Jenkins 节点配置 |
| **Sentinel（哨兵）** | 一个监视文件，用于在构建系统和签名系统之间传递完成信号 |
| **Bits（产物）** | 构建产物（编译后的输出） |
| **bitsDir** | 构建产物的目录命名模式（如 `b.hexie.jdoe.001`） |
| **IPS** | Isilon 配置服务 — 用于重铺操作 |
| **OVA** | 开放虚拟设备 — VMware 虚拟机格式 |
| **AMI** | Amazon Machine Image — AWS 虚拟机格式 |
| **VHD** | Virtual Hard Disk — Azure 虚拟机格式 |
| **PE** | 可移植可执行文件 — Windows 二进制格式（用于固件签名） |
| **RSA** | Rivest-Shamir-Adleman — 一种公钥密码学算法 |
| **CEC** | 企业工程中心 — Dell 的内部基础设施 |
| **MPA** | 多方授权 — 签名需要多方审批 |
| **Coverity** | 一种静态代码分析工具，用于发现 bug |

---

*本文档使用费曼学习法编写：用简单的语言解释复杂系统，借助类比和渐进式细节。最后更新：2026-03-27。*

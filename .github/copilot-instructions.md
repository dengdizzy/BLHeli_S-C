# BLHeliS Copilot 项目开发规范

## 项目目标

本项目将 BLHeliS 的 8051/SiLabs 汇编实现逐步迁移为可移植的标准 C 架构：

```text
BLHeliS ESC Control Core
            |
            v
Hardware Abstraction Layer (HAL)
            |
            v
MCU-specific Implementation
```

长期目标是支持 STM32、AT32、GD32 等 32 位 MCU，同时保持原始 BLHeliS
电机控制行为不变。

## 原始算法优先

1. BLHeliS 原始算法始终优先。`BLHeli_S SiLabs/BLHeli_S.asm` 是主要行为参考，
   其 `*.inc` 文件是硬件、FET、PWM、ADC、比较器和 MCU 配置参考。
2. 不允许未经要求修改算法。
3. 不允许未经要求优化 timing-critical code。
4. 必须保留原始换相、Startup、BEMF、Zero-crossing、PWM、Timing advance、
   Demag、Brake、Throttle、Protection 和 Communication 行为。
5. 必须保留原始整数宽度、截断、溢出、饱和、阈值、操作顺序和时序。
6. 不得以“更清晰”“更现代”或“更高效”为理由改变行为。
7. 不得未经明确要求重写整个仓库。

## Core、HAL 与 MCU Hardware

### Core

Core 只能使用标准 C，推荐使用 `stdint.h` 中的明确宽度类型。Core 只实现
算法、状态机、数据处理和控制决策，并通过 HAL 获取硬件状态或提交硬件操作。

Core 禁止：

- 访问 MCU registers、SFR、寄存器地址或寄存器位；
- 使用 assembly；
- 使用 8051-specific syntax；
- 使用 `sfr`、`sbit`、`EQU`、`DS`、`CSEG`、`ISEG` 等专用语法；
- 直接操作 GPIO、PWM、Timer、ADC、Comparator 或 Flash；
- 直接声明 interrupt vector；
- 直接配置、启用或关闭 MCU interrupt；
- 包含 MCU 或平台专用 header；
- 依赖编译器私有扩展。

### HAL

所有硬件操作必须通过 HAL，包括 FET、PWM、Damped-light PWM、Comparator、
ADC、Timer、deadline、输入捕获、interrupt 原子操作、Flash/EEPROM、LED、
Beeper、时钟和平台初始化。HAL 必须明确时间单位、数据范围、原子性、调用
时序、中断上下文限制和失败行为。

### MCU-specific implementation

MCU-specific 代码必须位于 Core 外部，负责 registers、interrupt vectors、
Timer/PWM/ADC/Comparator 配置、GPIO/FET 映射、时钟、Flash，以及 MCU-specific
latency 和 atomicity。不同 MCU 的实现必须彼此隔离。

## 增量迁移

1. 一次只能迁移一个模块。
2. 不得一次性重写整个控制核心。
3. 每个模块迁移前必须确认原始实现位置、输入、输出、全局变量、硬件依赖、
   中断依赖、timing-critical 操作和未知行为。
4. 每个模块迁移后必须保留原始实现，并能够与原始实现进行行为比较。
5. 不允许删除原始代码，除非用户明确要求。
6. 推荐迁移顺序：
   1. Portable C architecture
   2. HAL interface
   3. Data structures
   4. Six-step commutation
   5. PWM
   6. BEMF
   7. Zero crossing
   8. Startup
   9. Closed-loop operation
   10. Throttle
   11. Timing advance
   12. Demag
   13. Brake
   14. Protection
   15. Storage/configuration
   16. Communication
   17. MCU implementation
   18. Testing
   19. Legacy removal

## 修改前规则

Copilot Agent 修改代码前必须阅读相关文件、调用关系、全局变量、硬件依赖和
对应的原始汇编实现，并确认本次只涉及一个模块。必须明确列出修改文件、
函数、修改原因、原始参考、预期行为、不应改变的行为、验证方法和风险。

不确定功能时不得猜测，必须标记为 `UNKNOWN`，保留原行为并请求确认。不得
修改无关文件或测试。

## 修改原因与行为等价

每次修改必须说明：

1. 为什么需要修改；
2. 对应哪个迁移阶段；
3. 原始实现在哪里；
4. 为什么不会改变算法；
5. 如何验证行为一致；
6. 是否涉及 timing-critical code 或硬件行为；
7. 是否改变数据宽度、阈值、计数、顺序或时序。

必须保持六步换相正反转序列、FET 关闭/开启顺序、PWM 极性与更新时机、
Startup 计数与超时、BEMF 采样与超时、Zero-crossing、Timing advance、
Demag、Brake、Throttle scaling、PPM/OneShot/Multishot/DShot、保护逻辑、
参数布局、TX programming 和 bootloader 行为。

## Timing-critical code

不得未经要求改变 Timer、tick、时钟频率、时间单位、中断屏蔽范围、PWM commit
时机、FET deadtime、Comparator 采样时机、轮询、阻塞等待或 deadline 调度。
任何此类修改必须提供原始代码引用、原因、时序影响分析、编译/warning 检查
和测试或硬件验证方法。

## 编译、警告和测试

1. 每个模块迁移后必须检查编译错误和 warning。
2. 不得通过关闭 warning、降低检查级别或无依据的强制转换隐藏问题。
3. 优先运行仓库已有命令：`make test` 和 `make check-core`。
4. 必须根据修改范围运行相关单元测试、Core 依赖检查和目标 MCU 构建。
5. timing-critical 模块还必须检查操作顺序、中断延迟、Timer deadline、PWM
   更新、Comparator 采样和 FET switching。
6. 测试失败时必须报告真实原因，不得修改测试掩盖问题。
7. 不得声称执行过未实际执行的命令或验证过未实际验证的行为。

## 数据、配置、存储和通信

- 保持原始全局变量含义、宽度、生命周期、同步和原子性。
- 保持参数默认值、编码、范围、布局、签名和兼容性。
- 存储操作必须通过 HAL；不得猜测未使用参数或增加未经证实的保护行为。
- 将协议捕获、协议解码、Throttle interpretation 和 DShot command 分离。
- 不得改变协议阈值、单位转换、checksum 或 command repeat count。
- 不得从 `check_temp_voltage_and_limit_power` 的名称推断额外电压保护；
  未知行为必须先取得证据。

## Copilot Agent 工作规则

1. 先理解任务和仓库，再修改文件。
2. 做最小、精确、可审查的修改。
3. 不修改任务范围之外的代码，不删除原始实现。
4. 临时脚本或实验文件只能放在 `/tmp`。
5. 不提交 secrets、tokens、credentials 或 API keys。
6. 提交前扫描所有修改或新增文件中的 secrets。
7. 及时报告编译错误、warning、测试失败和未解决风险。
8. 若需求与原始算法冲突，暂停并请求确认。
9. 只运行仓库已有的构建、测试和检查工具，除非用户明确要求增加工具。
10. Pull Request 描述必须包含修改内容、原因、原始参考、验证命令、测试
    结果、未解决风险以及是否涉及 timing-critical code。
11. 未经明确要求不得创建 Pull Request。

## 提交前检查清单

- [ ] 本次只迁移一个模块。
- [ ] 已阅读对应原始汇编和硬件宏。
- [ ] 未未经要求修改算法或优化 timing-critical code。
- [ ] Core 与 MCU Hardware 已分离。
- [ ] Core 仅使用标准 C。
- [ ] Core 未访问 MCU registers、assembly 或 8051-specific syntax。
- [ ] 所有硬件操作均通过 HAL。
- [ ] 原始换相、Startup、BEMF 和 PWM 行为保持不变。
- [ ] 未知功能未被猜测。
- [ ] 未删除原始代码。
- [ ] 已说明每个修改的原因。
- [ ] 已检查编译错误和 compiler warnings。
- [ ] 已运行相关测试并记录结果。
- [ ] 已检查 timing-critical 影响。
- [ ] 已扫描修改文件中的 secrets。
- [ ] 已记录剩余风险。

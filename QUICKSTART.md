# 快速开始指南

## 项目概述

本项目是Python卫星编队控制系统的C语言完整实现，包含模块化的设计和完整的API接口。

## 系统要求

- **编译器**: GCC 4.8+ 或 Clang 3.5+
- **构建工具**: CMake 3.10+ (推荐) 或 Make
- **操作系统**: Linux, macOS, Windows (MSYS2/MinGW)
- **库依赖**: 标准C库 + 数学库 (libm)

## 项目结构

```
satellite-formation-c/
├── README.md                # 详细文档
├── QUICKSTART.md           # 本文件
├── CMakeLists.txt          # CMake配置（推荐）
├── Makefile                # Make配置（备选）
│
├── include/                # 头文件（接口定义）
│   ├── constants.h         # 常数
│   ├── types.h             # 数据类型
│   ├── vector3.h           # 向量运算
│   ├── quaternion.h        # 四元数
│   ├── satellite.h         # 卫星模块
│   ├── orbit.h             # 轨道机动
│   ├── attitude.h          # 姿态跟踪
│   ├── kinematics.h        # 运动学引擎
│   ├── formation/          # 编队模块接口
│   ├── decision/           # 决策模块接口
│   └── config/             # 配置模块接口
│
├── src/                    # 源文件（实现）
│   ├── main.c              # 主程序入口
│   ├── vector3.c
│   ├── quaternion.c
│   ├── satellite.c
│   ├── orbit.c
│   ├── attitude.c
│   ├── kinematics.c
│   ├── formation/          # 编队实现
│   ├── decision/           # 决策实现
│   └── config/             # 配置实现
│
├── config/                 # 配置文件
│   └── config.json         # 卫星初始配置（用户修改）
│
├── build/                  # 构建输出（自动生成）
│   ├── bin/
│   │   └── satellite_sim
│   ├── lib/
│   │   └── libsatellite.a
│   └── obj/
│
├── output/                 # 仿真结果（运行时生成）
│   ├── state.json
│   ├── maneuver.json
│   └── trajectory.json
│
└── scripts/                # 辅助脚本
    ├── build.sh
    ├── run.sh
    └── clean.sh
```

## 快速开始

### 方式1: 使用CMake（推荐）

```bash
# 创建构建目录
mkdir build && cd build

# 配置
cmake ..

# 编译（多进程加速）
make -j4

# 运行
./bin/satellite_sim -c ../config/config.json -s 10000

# 返回项目根目录
cd ..
```

### 方式2: 使用Makefile

```bash
# 编译
make

# 运行
./build/bin/satellite_sim -c config/config.json -s 10000

# 清理
make clean
```

### 方式3: 使用脚本

```bash
# 赋予执行权限
chmod +x scripts/*.sh

# 构建
./scripts/build.sh

# 运行
./scripts/run.sh

# 清理
./scripts/clean.sh
```

## 配置卫星

编辑 `config/config.json` 文件：

```json
{
  "simulation": {
    "time_step": 1.0,      // 时间步长(分钟)
    "max_steps": 10000,    // 最大仿真步数
    "save_interval": 100   // 保存间隔
  },
  "satellites": [
    {
      "id": 1000,                // 卫星ID
      "type": 0,                 // 类型(0=HF, 1=LF)
      "function_type": 2,        // 功能(1=攻击, 2=侦察, 3=防护)
      "a": 42164.0,              // 半长轴(km)
      "e": 0.01,                 // 偏心率
      "i": 0.5,                  // 倾角(度)
      "omega_big": 0.0,          // 升交点赤经(度)
      "omega_small": 0.0,        // 近地点幅角(度)
      "m0": 0.0,                 // 平近点角(度)
      "fuel": 500.0              // 燃料(kg)
    }
    // ... 更多卫星 ...
  ],
  "strategy_thresholds": {
    "attack_distance": 3000.0,
    "inspect_distance": 2000.0,
    // ... 更多参数 ...
  }
}
```

## 命令行参数

```bash
./build/bin/satellite_sim [选项]

选项:
  -c FILE        配置文件路径 (默认: config/config.json)
  -s STEPS       仿真最大步数 (默认: 10000)
  -v             启用详细日志输出
  -h             显示帮助信息

例子:
  ./build/bin/satellite_sim
  ./build/bin/satellite_sim -c config/test.json -s 50000 -v
```

## 输出文件

仿真完成后，`output/` 目录中会生成：

- **state.json** - 每个时间步的卫星状态（位置、速度、燃料等）
- **maneuver.json** - 所有机动命令的历史
- **trajectory.json** - 完整的轨迹历史数据

## 核心功能

### 1. 轨道计算 (orbit.h)
- Kepler方程求解
- 轨道六根数转换
- Hohmann转移
- Lambert轨迹求解
- RK4轨道外推

### 2. 编队控制 (formation/)
- **巡视ZC**: Lambert强制逼近 + 椭圆绕飞
- **环绕HS**: 球形编队配置
- **环绕HSFY**: 高椭圆绕飞
- **撤离CT**: 安全撤离轨道

### 3. 决策系统 (decision/)
- **决策树**: K-means聚类分组
- **微分博弈**: 策略分配与资源优化
- **编队管理**: 编队转换规则与状态跟踪

### 4. 姿态控制 (attitude.h)
- 四元数运动学
- PD控制律
- 角速度/角加速度约束
- 多种姿态指向方式

## 编程指南

### 添加新的编队类型

1. **在 `include/formation/` 中创建头文件**
```c
#ifndef MY_FORMATION_H
#define MY_FORMATION_H

// 定义接口函数
int my_formation_init(Formation *formation);
int my_formation_update(Formation *formation, Satellite *chaser, Satellite *target);
int my_formation_check_status(Formation *formation, Satellite *chaser);
void my_formation_destroy(Formation *formation);

#endif
```

2. **在 `src/formation/` 中实现**
```c
#include "my_formation.h"

int my_formation_init(Formation *formation) {
    // 初始化逻辑
    return 0;
}

// ... 实现其他函数 ...
```

3. **在主程序中注册**
```c
Formation *my_form = formation_create(
    "MyFormation",
    my_formation_init,
    my_formation_update,
    my_formation_check_status,
    my_formation_destroy
);
```

### 添加新的决策规则

1. **定义规则函数**
```c
int my_transition_rule(int sat_id, FormationTrigger *triggers, void *context) {
    // 检查转换条件
    if (triggers[sat_id].circle_count >= 2) {
        return 1;  // 满足条件
    }
    return 0;     // 不满足
}
```

2. **注册规则**
```c
formation_manager_add_transition_rule(
    manager,
    FORMATION_INSPECT,
    FORMATION_RETREAT,
    my_transition_rule,
    "完成2圈后撤离"
);
```

## 调试技巧

### 启用详细日志
```bash
./build/bin/satellite_sim -v
```

### 查看编译警告
```bash
make clean
make CFLAGS="-Wall -Wextra -Wpedantic -O2 -g"
```

### 使用GDB调试
```bash
gdb ./build/bin/satellite_sim
(gdb) run -c config/config.json -s 1000
(gdb) break main
(gdb) continue
```

### 检查内存泄漏
```bash
valgrind --leak-check=full ./build/bin/satellite_sim
```

## 性能优化

### 编译优化
```bash
# 高性能编译
make clean
make CFLAGS="-O3 -march=native -ffast-math"
```

### 降低日志输出
修改 `config.json` 中的 `save_interval` 为更大的值（如1000）

### 减少历史缓存
修改 `MAX_HISTORY_PER_SAT` 常数（在 `include/constants.h`）

## 常见问题

**Q: 编译错误"undefined reference to sqrt"**
A: 确保链接了数学库。检查 `Makefile` 中 `LDFLAGS = -lm`

**Q: 仿真运行缓慢**
A: 尝试以下方法：
  1. 增加 `time_step` 参数
  2. 减少 `save_interval` 值（输出间隔）
  3. 使用编译优化标志 `-O3`

**Q: 无法读取配置文件**
A: 检查：
  1. 文件路径是否正确
  2. JSON格式是否有效（使用JSON验证器）
  3. 文件权限（需要读权限）

**Q: 编队无法切换**
A: 检查：
  1. 转换规则是否正确注册
  2. 条件参数是否合理
  3. 燃料是否充足（机动需要燃料）

## 参考资源

- **文档**: 查看 `README.md` 获得详细的API文档
- **示例**: `config/config.json` 提供了配置示例
- **代码**: 所有源文件都有详细注释

## 许可证

MIT License

## 支持和反馈

如有问题，请检查：
1. 日志输出是否有错误信息
2. 配置文件格式是否正确
3. 是否有足够的磁盘空间和内存

祝你使用愉快！🚀

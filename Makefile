# ==================== 卫星编队仿真系统 - Makefile ====================

# 编译器和标志
CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -O2 -fPIC -std=c11 -I./include
LDFLAGS = -lm

# 调试模式（可选，取消下面的注释启用）
# CFLAGS += -g -O0 -DDEBUG

# 目录结构
SRC_DIR = src
OBJ_DIR = build/obj
BIN_DIR = build/bin
LIB_DIR = build/lib
INCLUDE_DIR = include

# 源文件
BASE_SOURCES = $(SRC_DIR)/vector3.c $(SRC_DIR)/quaternion.c $(SRC_DIR)/satellite.c $(SRC_DIR)/orbit.c $(SRC_DIR)/attitude.c
FORMATION_SOURCES = $(SRC_DIR)/formation/formation_base.c $(SRC_DIR)/formation/inspect_formation.c $(SRC_DIR)/formation/around_formation.c $(SRC_DIR)/formation/circumnavigate_formation.c $(SRC_DIR)/formation/retreat_formation.c
DECISION_SOURCES = $(SRC_DIR)/decision/decision_tree.c $(SRC_DIR)/decision/differential_game.c $(SRC_DIR)/decision/formation_manager.c
OTHER_SOURCES = $(SRC_DIR)/config/config.c $(SRC_DIR)/kinematics.c
ALL_SOURCES = $(BASE_SOURCES) $(FORMATION_SOURCES) $(DECISION_SOURCES) $(OTHER_SOURCES)

# 对象文件
BASE_OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(BASE_SOURCES))
FORMATION_OBJECTS = $(patsubst $(SRC_DIR)/formation/%.c, $(OBJ_DIR)/formation/%.o, $(FORMATION_SOURCES))
DECISION_OBJECTS = $(patsubst $(SRC_DIR)/decision/%.c, $(OBJ_DIR)/decision/%.o, $(DECISION_SOURCES))
OTHER_OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(OTHER_SOURCES))
ALL_OBJECTS = $(BASE_OBJECTS) $(FORMATION_OBJECTS) $(DECISION_OBJECTS) $(OTHER_OBJECTS)

# 主程序
MAIN_SOURCE = $(SRC_DIR)/main.c
MAIN_OBJECT = $(OBJ_DIR)/main.o
EXECUTABLE = $(BIN_DIR)/satellite_sim

# 库
STATIC_LIB = $(LIB_DIR)/libsatellite.a

# 默认目标
.PHONY: all clean rebuild help directories test run

all: directories $(EXECUTABLE) $(STATIC_LIB)

# ==================== 目录创建 ====================
directories:
	@mkdir -p $(OBJ_DIR)/formation
	@mkdir -p $(OBJ_DIR)/decision
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(LIB_DIR)
	@mkdir -p output

# ==================== 对象文件编译 ====================

# 基础模块
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "编译: $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# 编队模块
$(OBJ_DIR)/formation/%.o: $(SRC_DIR)/formation/%.c
	@echo "编译: $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# 决策模块
$(OBJ_DIR)/decision/%.o: $(SRC_DIR)/decision/%.c
	@echo "编译: $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# 主程序
$(MAIN_OBJECT): $(MAIN_SOURCE)
	@echo "编译: $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# ==================== 链接 ====================

# 可执行文件
$(EXECUTABLE): directories $(ALL_OBJECTS) $(MAIN_OBJECT)
	@echo "链接: $(EXECUTABLE)"
	@$(CC) $(ALL_OBJECTS) $(MAIN_OBJECT) $(LDFLAGS) -o $@
	@echo "✓ 可执行文件已生成: $@"

# 静态库
$(STATIC_LIB): directories $(ALL_OBJECTS)
	@echo "生成库: $(STATIC_LIB)"
	@ar rcs $@ $(ALL_OBJECTS)
	@echo "✓ 静态库已生成: $@"

# ==================== 清理 ====================

clean:
	@echo "清理构建文件..."
	@rm -rf $(OBJ_DIR) $(BIN_DIR) $(LIB_DIR)
	@echo "✓ 清理完成"

rebuild: clean all
	@echo "✓ 重新构建完成"

# ==================== 帮助 ====================

help:
	@echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
	@echo "卫星编队仿真系统 - Makefile 命令"
	@echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
	@echo "make              - 构建可执行文件和库"
	@echo "make clean        - 删除所有构建文件"
	@echo "make rebuild      - 清理后重新构建"
	@echo "make run          - 编译并运行程序"
	@echo "make help         - 显示本帮助信息"
	@echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

# ==================== 运行 ====================

run: all
	@echo "✓ 构建完成，开始运行仿真..."
	@echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
	@./$(EXECUTABLE) -c config/config.json -s 10000 -v
	@echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

# ==================== 编译信息 ====================

info:
	@echo "╔════════════════════════════════════════════════════════════╗"
	@echo "║           卫星编队仿真系统 - 构建配置信息                  ║"
	@echo "╠════════════════════════════════════════════════════════════╣"
	@echo "│ 编译器: $(CC)"
	@echo "│ 编译标志: $(CFLAGS)"
	@echo "│ 链接标志: $(LDFLAGS)"
	@echo "│"
	@echo "│ 源文件数量:"
	@echo "│   基础模块: $(words $(BASE_SOURCES))"
	@echo "│   编队模块: $(words $(FORMATION_SOURCES))"
	@echo "│   决策模块: $(words $(DECISION_SOURCES))"
	@echo "│   其他模块: $(words $(OTHER_SOURCES))"
	@echo "│   总计: $(words $(ALL_SOURCES))"
	@echo "│"
	@echo "│ 输出文件:"
	@echo "│   可执行文件: $(EXECUTABLE)"
	@echo "│   静态库: $(STATIC_LIB)"
	@echo "╚════════════════════════════════════════════════════════════╝"

# ==================== 源代码统计 ====================

count:
	@echo "源代码行数统计:"
	@find $(SRC_DIR) -name "*.c" | xargs wc -l | tail -1
	@find $(INCLUDE_DIR) -name "*.h" | xargs wc -l | tail -1

.DEFAULT_GOAL := all

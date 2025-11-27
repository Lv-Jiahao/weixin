#!/bin/bash

# ==================== 卫星编队仿真系统 - 构建脚本 ====================

set -e  # 出错时停止

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# 项目信息
PROJECT_NAME="卫星编队仿真系统"
PROJECT_VERSION="1.0.0"

# 打印函数
print_header() {
    echo -e "${BLUE}╔════════════════════════════════════════════════════════════╗${NC}"
    echo -e "${BLUE}║${NC}          $1"
    echo -e "${BLUE}╚════════════════════════════════════════════════════════════╝${NC}"
}

print_info() {
    echo -e "${GREEN}✓${NC} $1"
}

print_warn() {
    echo -e "${YELLOW}⚠${NC} $1"
}

print_error() {
    echo -e "${RED}✗${NC} $1"
}

# 检查依赖
check_dependencies() {
    print_header "检查构建依赖"
    
    # 检查GCC
    if ! command -v gcc &> /dev/null; then
        print_error "未找到GCC编译器"
        echo "请安装: sudo apt-get install build-essential (Ubuntu/Debian) 或相应包管理器"
        exit 1
    fi
    print_info "GCC: $(gcc --version | head -n1)"
    
    # 检查Make
    if ! command -v make &> /dev/null; then
        print_warn "未找到Make"
        echo "  这不是必需的，可以使用CMake替代"
    else
        print_info "Make: $(make --version | head -n1)"
    fi
    
    # 检查CMake
    if ! command -v cmake &> /dev/null; then
        print_warn "未找到CMake"
        echo "  可以使用Makefile构建"
    else
        print_info "CMake: $(cmake --version | head -n1)"
    fi
    
    echo ""
}

# 清理旧构建
clean_build() {
    if [ -d "build" ]; then
        print_info "清理旧构建文件..."
        rm -rf build
    fi
}

# 使用CMake构建
build_with_cmake() {
    print_header "使用CMake构建"
    
    # 创建构建目录
    mkdir -p build
    cd build
    
    # 运行CMake
    print_info "配置CMake..."
    cmake .. -DCMAKE_BUILD_TYPE=Release
    
    # 编译
    print_info "编译代码..."
    make -j4
    
    cd ..
    print_info "CMake构建完成"
}

# 使用Makefile构建
build_with_make() {
    print_header "使用Makefile构建"
    
    print_info "编译代码..."
    make -j4
    
    print_info "Makefile构建完成"
}

# 验证构建
verify_build() {
    print_header "验证构建结果"
    
    if [ -f "build/bin/satellite_sim" ]; then
        print_info "可执行文件: build/bin/satellite_sim"
    else
        print_error "可执行文件未生成"
        return 1
    fi
    
    if [ -f "build/lib/libsatellite.a" ]; then
        print_info "静态库: build/lib/libsatellite.a"
    else
        print_warn "静态库未生成（可能仅使用可执行文件）"
    fi
    
    return 0
}

# 显示使用信息
show_usage() {
    echo ""
    echo "构建完成！"
    echo ""
    echo "使用方法:"
    echo "  运行仿真: ./build/bin/satellite_sim"
    echo "  查看帮助: ./build/bin/satellite_sim -h"
    echo "  自定义配置: ./build/bin/satellite_sim -c config/config.json -s 50000"
    echo ""
    echo "详情见: QUICKSTART.md"
    echo ""
}

# 主程序
main() {
    clear
    
    print_header "$PROJECT_NAME v$PROJECT_VERSION - 构建脚本"
    echo ""
    
    # 检查依赖
    check_dependencies
    
    # 选择构建方式
    echo "选择构建方式:"
    echo "  1) CMake (推荐)"
    echo "  2) Makefile"
    echo "  3) 清理后重新构建"
    echo "  4) 只清理"
    echo "  5) 退出"
    echo ""
    
    read -p "请选择 [1-5]: " choice
    
    case $choice in
        1)
            build_with_cmake
            verify_build
            show_usage
            ;;
        2)
            build_with_make
            verify_build
            show_usage
            ;;
        3)
            clean_build
            build_with_cmake
            verify_build
            show_usage
            ;;
        4)
            clean_build
            print_info "清理完成"
            ;;
        5)
            print_info "退出"
            exit 0
            ;;
        *)
            print_error "无效选择"
            exit 1
            ;;
    esac
}

# 运行主程序
main

#!/bin/bash

# 机器狗RobotServer SDK依赖安装脚本
# 此脚本用于自动安装SDK所需的所有依赖库

set -e  # 遇到错误立即退出

# 显示彩色输出
GREEN='\033[0;32m'
BLUE='\033[0;34m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # 无颜色

# 打印带颜色的信息
print_info() {
    echo -e "${BLUE}[信息]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[成功]${NC} $1"
}

print_error() {
    echo -e "${RED}[错误]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[警告]${NC} $1"
}

# 检测操作系统
detect_os() {
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        OS=$NAME
        VER=$VERSION_ID
    elif type lsb_release >/dev/null 2>&1; then
        OS=$(lsb_release -si)
        VER=$(lsb_release -sr)
    elif [ -f /etc/lsb-release ]; then
        . /etc/lsb-release
        OS=$DISTRIB_ID
        VER=$DISTRIB_RELEASE
    else
        OS=$(uname -s)
        VER=$(uname -r)
    fi
}

# 安装依赖 - Ubuntu/Debian
install_deps_debian() {
    print_info "正在更新软件包列表..."
    sudo apt-get update

    print_info "正在安装编译工具和基本依赖..."
    sudo apt-get install -y build-essential cmake git

    print_info "正在安装Boost库..."
    sudo apt-get install -y libboost-all-dev

    print_info "正在安装nlohmann/json库..."
    sudo apt-get install -y nlohmann-json3-dev

    print_info "正在安装rapidxml库..."
    # rapidxml通常是header-only库，可以直接下载
    if [ ! -d "rapidxml" ]; then
        print_info "下载rapidxml..."
        mkdir -p rapidxml
        wget -q https://sourceforge.net/projects/rapidxml/files/rapidxml/rapidxml%201.13/rapidxml-1.13.zip/download -O rapidxml.zip
        unzip -q rapidxml.zip -d rapidxml
        print_info "将rapidxml安装到系统目录..."
        sudo cp rapidxml/rapidxml-1.13/*.hpp /usr/local/include/
        rm -rf rapidxml rapidxml.zip
    fi
}

# 安装依赖 - CentOS/RHEL/Fedora
install_deps_redhat() {
    print_info "正在更新软件包列表..."
    sudo yum -y update

    print_info "正在安装编译工具和基本依赖..."
    sudo yum -y install gcc gcc-c++ make cmake git

    print_info "正在安装Boost库..."
    sudo yum -y install boost-devel

    print_info "正在安装nlohmann/json库..."
    # CentOS/RHEL可能没有现成的nlohmann/json包，从源码安装
    if [ ! -d "json" ]; then
        print_info "从源码安装nlohmann/json..."
        git clone https://github.com/nlohmann/json.git
        cd json
        mkdir -p build && cd build
        cmake ..
        make -j$(nproc)
        sudo make install
        cd ../..
        rm -rf json
    fi

    print_info "正在安装rapidxml库..."
    # rapidxml通常是header-only库，可以直接下载
    if [ ! -d "rapidxml" ]; then
        print_info "下载rapidxml..."
        mkdir -p rapidxml
        wget -q https://sourceforge.net/projects/rapidxml/files/rapidxml/rapidxml%201.13/rapidxml-1.13.zip/download -O rapidxml.zip
        unzip -q rapidxml.zip -d rapidxml
        print_info "将rapidxml安装到系统目录..."
        sudo cp rapidxml/rapidxml-1.13/*.hpp /usr/local/include/
        rm -rf rapidxml rapidxml.zip
    fi
}

# 安装依赖 - macOS
install_deps_macos() {
    # 检查是否安装了Homebrew
    if ! command -v brew &> /dev/null; then
        print_error "未检测到Homebrew，请先安装Homebrew: https://brew.sh/"
        exit 1
    fi

    print_info "正在更新Homebrew..."
    brew update

    print_info "正在安装编译工具和基本依赖..."
    brew install cmake git

    print_info "正在安装Boost库..."
    brew install boost

    print_info "正在安装nlohmann/json库..."
    brew install nlohmann-json

    print_info "正在安装rapidxml库..."
    # rapidxml通常是header-only库，可以直接下载
    if [ ! -d "rapidxml" ]; then
        print_info "下载rapidxml..."
        mkdir -p rapidxml
        curl -s -L https://sourceforge.net/projects/rapidxml/files/rapidxml/rapidxml%201.13/rapidxml-1.13.zip/download -o rapidxml.zip
        unzip -q rapidxml.zip -d rapidxml
        print_info "将rapidxml安装到系统目录..."
        sudo cp rapidxml/rapidxml-1.13/*.hpp /usr/local/include/
        rm -rf rapidxml rapidxml.zip
    fi
}

# 验证安装
verify_installation() {
    print_info "验证安装..."

    # 检查CMake
    if command -v cmake &> /dev/null; then
        CMAKE_VERSION=$(cmake --version | head -n1 | cut -d" " -f3)
        print_success "CMake已安装: $CMAKE_VERSION"
    else
        print_error "CMake安装失败"
        INSTALL_SUCCESS=false
    fi

    # 检查Boost
    if [ -d "/usr/include/boost" ] || [ -d "/usr/local/include/boost" ]; then
        print_success "Boost库已安装"
    else
        print_error "Boost库安装失败"
        INSTALL_SUCCESS=false
    fi

    # 检查nlohmann/json
    if [ -f "/usr/include/nlohmann/json.hpp" ] || [ -f "/usr/local/include/nlohmann/json.hpp" ]; then
        print_success "nlohmann/json库已安装"
    else
        print_warning "未检测到nlohmann/json库，可能需要手动安装"
    fi

    # 检查rapidxml
    if [ -f "/usr/include/rapidxml.hpp" ] || [ -f "/usr/local/include/rapidxml.hpp" ]; then
        print_success "rapidxml库已安装"
    else
        print_warning "未检测到rapidxml库，可能需要手动安装"
    fi

    if [ "$INSTALL_SUCCESS" = false ]; then
        print_error "部分依赖安装失败，请查看上面的错误信息"
        exit 1
    fi
}

# 主函数
main() {
    print_info "机器狗RobotServerSDK依赖安装脚本"
    print_info "=============================="

    # 检测操作系统
    detect_os
    print_info "检测到操作系统: $OS $VER"

    INSTALL_SUCCESS=true

    # 根据操作系统安装依赖
    case "$OS" in
        *Ubuntu*|*Debian*)
            install_deps_debian
            ;;
        *CentOS*|*Red*|*Fedora*)
            install_deps_redhat
            ;;
        *Darwin*|*Mac*)
            install_deps_macos
            ;;
        *)
            print_error "不支持的操作系统: $OS"
            print_info "请手动安装以下依赖:"
            print_info "- C++17或更高版本的编译器"
            print_info "- CMake 3.10或更高版本"
            print_info "- Boost 1.66或更高版本"
            print_info "- nlohmann/json库"
            print_info "- rapidxml库"
            exit 1
            ;;
    esac

    # 验证安装
    verify_installation

    print_success "所有依赖安装完成！"
    print_info "现在您可以编译和使用机器狗RobotServerSDK了。"
}

# 执行主函数
main

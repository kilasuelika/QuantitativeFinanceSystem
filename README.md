# Quantitative Finance System
 
## 整体架构

整体系统分成三个组件：

1. 数据服务器：提供数据获取与查询功能，用C\+\+开发，使用C\+\+ Rest SDK对外提供Restful接口以查询数据。数据通过python从开源库获取，存储在本地时使用SQLite。数据频率为小时。只支持国内市场、股票。
2. 策略服务器：提供账户管理功能（目前主要是模拟，不接入真实的交易接口）、策略运行环境、回测框架。暂定为支持C\+\+与python两种语言编写策略，优先支持C\+\+。每个策略编译为一个dll供服务器调用。python就调用C++的接口。
1. 前端：提供UI界面，使用nodejs开发。

通过启动器启动三个组件来运行整个系统，系统是持续运行的。这三个组件可以单独运行，相互之间采用HTTP通信，便于重复利用。开发中使用C++ 20标准，x64架构。

## 依赖

### Visual Studio 2022

需要安装C++、Nodejs组件。

### Vcpkg

需要安装的包：

```cmd
.\vcpkg install boost:x64-windows eigen:x64-windows sqlitecpp:x64-windows cpprestsdk:x64-windows
```

## 数据服务器

### 配置
配置文件为`DataServer-config.json`，配置项如下：

```json
{
    "port": 8080,
    "dbPath": "data.db",
    "startDate":  "2010-01-01"
}
```

### Restful接口


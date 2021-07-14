# KidWebServer

## 概述

Linux环境（高性能）HTTP 服务器

## 开发环境
vscode + docker开箱即用，无需其他依赖

## 介绍
- 解析HTTP请求
- 定时器处理非活跃连接
- Reactor编程模型，基于Epoll的IO多路复用
- 多线程利用多核能力，线程池减轻资源开销

## todo
- 状态机解析HTTP请求
- config配置
- 单元测试
- 实现循环缓冲区

## 总结


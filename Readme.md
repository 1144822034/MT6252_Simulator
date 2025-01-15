# 基于Unicorn Engine的手机固件模拟器
### 使用的是手机型号IHD 316的固件，手机搭载CPU为MT6252

![预览图](./docs/img/01.png)
![预览图](./docs/img/02.png)
![预览图](./docs/img/03.png)
![预览图](./docs/img/04.png)
![预览图](./docs/img/05.png)


### 已实现LCD显示功能、按键功能、储存功能

### 部分实现功能
- SIM卡模拟
### 未实现功能
- 摄像机模拟
- 声音播放
- 网络通信


# 编译环境

- mingw32

# 编译

- 双击build.bat文件

# 运行

- bin/Rom目录下的08000000.bin就是手机的固件导出
- fat32.img为SD卡镜像文件，需要手动点击bat生成并移动到bin/Rom目录下
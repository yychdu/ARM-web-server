# ARM-web-server
web服务器移植进IMX6ULL开发板运行，可实现局域网读取传感器数据

将web页面数据通过字符串存储后发送给Clientfd

-----

使用方法



make

复制进开发板根文件系统 挂载驱动 ./运行之

访问192.168.xx.xx:8080即可



注意：

本仓库采用imx6ull开发板 挂载的是icm20608驱动读取传感器数据

![image-20200813230010489](https://github.com/yychdu/ARM-web-server/blob/master/7.png)


## 加密小工具

### 功能: 
- 对本地文件加解密
- 自定义密码
- 支持密码提示
- 支持自定义超级密码superkey(即后门, 可以直接解密, 详见源码 crypto.h -> crypto -> superkey)

### 使用方法:
- 加密: <exe_name> -e -f <in_filename> -o <out_filename> -k <your_keyword>
- 解密: <exe_name> -d -f <in_filename> -o <out_filename> -k <your_keyword>
- 后门: <exe_name> -f <in_filename> -s
- 密码提示: <exe_name> -f <in_filename> -r
- 更多使用细节请使用 <exe_name> -h 查看

## 实现原理

### 加密
输入口令 t
通过 keyGen(t) 生成密钥 S
询问是否生成提示信息
将信息写入一个文件头A(后面介绍)
将 S 和文件异或加密
将文件头A和加密后的文件一起存放, 加密完成

### 解密
先输入口令t并读取文件头, 验证是否匹配版本和密钥
通过 keyGen(superkey) ^ key_in_head == keyGen(t)
成功验证之后, 解密


### 文件头
加密文件中有512 bits (64 bytes)的文件头, 结构如下
```cpp
struct file_header{ // 512 bytes
    char author[8]; // max 8 bytes , note: the last char must be '\0'
    int version1; // 4 bytes, to check if the file is right version
    int version2; // 4 bytes
    unsigned int key[KEY_LEN_BYTES / 4]; // key = keygen(t) ^ kegen(superkey)
    char tips[512 - 8 - 8 - KEY_LEN_BYTES]; // tips for recover key
};
```

### 超级密码
超级密码写死在程序里, 可以通过 keygen(superkey) 来提升辅助加密, 以提升加密强度

### 密码提示
明文存在文件头A中, 直接读取即可

## todo
进度条
加入md5完整性校验
加速
图形化

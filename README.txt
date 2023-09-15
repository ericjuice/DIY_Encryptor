## 加密小工具

功能: 
- 对本地文件加解密
- 自定义密码
- 支持密码提示
- 支持自定义超级密码(即后门, 可以直接解密)

## 实现原理

### 加密
通过 keyGen(key) 生成密钥S
询问是否生成提示信息
然后写入一个文件头A(后面介绍)
将S和文件异或加密
将文件头A和加密后的文件一起存放, 加密完成

### 解密
先输入密码, 验证是否匹配文件的头(版本, 密钥)
通过 keyGen(superkey) ^ key_in_head == keyGen(key)
成功验证之后, 解密


### 文件头
加密文件中有512 bits (64 bytes)的文件头, 结构如下
```cpp
struct file_header{ // 512 bytes
    char author[8]; // max 8 bytes , note: the last char must be '\0'
    int version1; // 4 bytes, to check if the file is right version
    int version2; // 4 bytes
    char key[KEY_LEN_BYTES]; // current it is 16 bytes, this key is XORed with super key
    char tips[512 - 8 - 8 - KEY_LEN_BYTES]; // tips for recover key
};
```

### 超级密码
超级密码写死在程序里

### 密码提示
存在密码块中, 直接读取即可

## todo
进度条
加入md5完整性校验

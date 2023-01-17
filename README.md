# PSP

## Git

[如何使用 Git 提交更新内容](./how-to-commit.md)

## SSH

### 配置免密登陆

- `cd ~/.ssh`
- 本地机器 `ssh-keygen -t rsa` 生成公私钥对 (记得生成时输入`p100`)
- 上传公钥到服务器 `ssh-copy-id -i p100.pub zhiaan@114.212.134.157`
- `~/.ssh/config` 添加如下配置
```
Host p100
	HostName 114.212.134.157
	User zhiaan
	IdentityFile ~/.ssh/p100
```

### 传输文件

```
scp -r 本地路径 p100:服务器路径
```

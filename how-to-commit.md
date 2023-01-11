# 如何使用 Git 提交更新内容

下面是一个简单的工作流。

### 0. git 设置 ssh

由于私有仓库只能采用ssh方式clone代码，需要设置ssh。

如下链接

[git 设置 ssh](https://zhuanlan.zhihu.com/p/108972475)

### 1. 将仓库下载到本地。
```
git clone git@github.com:Zhiaan/PSP.git
```

### 2. 本地修改

**每次修改前记得拉取远程仓库上的最新代码，避免冲突**。

```
git pull --rebase
```

如果发生冲突，手动修改冲突的文件。

### 3. 提交文件到远程仓库

修改完成后，提交文件到远程仓库。
```
git add .
git commit -m "some useful message"
git push
```

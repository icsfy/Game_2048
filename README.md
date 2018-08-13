# Game 2048

通过 `ncurses` 库实现的终端下的 2048 小游戏。

## 编译运行

编译需要链接 `ncurses` 库:

```shell
$ gcc -o game_2048 2048.c -lncurses
# 或者通过 makefile 编译
$ cmake . && make
```

## 游戏说明

- 可以通过上下左右或者 `w` `s` `a` `d` 按键进行控制

- `r` 键可以重新开始游戏, `q` 键退出游戏

- 游戏默认 `4 x 4` 方格, 可以通过命令行参数指定 `n x n` 的方格

- 游戏会记录 5 个最高得分, 存储到文件 `game_2048_records.dat`

![游戏示例](tty.gif)

# 模型权重

[English](README.en.md)

将训练好的模型权重放在此目录中用于本地推理。

当前本地推理 checkpoint 路径为：

```text
model_data/seam_unet.pth
```

当前本地 checkpoint 是从项目训练日志中按验证集损失选择得到的：

```text
ep100-loss0.045-val_loss0.018-pretrain.pth
```

大型 `.pth` 文件默认被 Git 忽略。如果要随项目公开发布 checkpoint，请优先使用 GitHub Releases 或外部存储，避免写入 Git 历史。

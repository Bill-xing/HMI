# Model Weights

[中文](README.zh-CN.md)

Put trained model weights in this directory for local inference.

The current local inference checkpoint is:

```text
model_data/seam_unet.pth
```

The current local checkpoint was selected from the project training logs by validation loss:

```text
ep100-loss0.045-val_loss0.018-pretrain.pth
```

Large `.pth` files are ignored by Git by default. If you want to publish the checkpoint with the open-source project, prefer GitHub Releases or external storage instead of Git history.

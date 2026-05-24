# HMI 焊缝分割数据集

[English](hmi-weld-seam-voc-dataset.en.md)

本数据集包含 HMI 焊缝识别演示使用的自采集焊缝图像及像素级分割 mask。

## 格式

数据集以 Pascal VOC 风格的分割数据包发布：

```text
hmi-weld-seam-voc-dataset/
└── VOC2007/
    ├── JPEGImages/
    ├── SegmentationClass/
    └── ImageSets/
        └── Segmentation/
```

## 类别

```text
0: background
1: seam
```

## 划分

```text
train: 845
val: 94
trainval: 939
test: 0
```

## 发布资产

- 公开包名：`hmi-weld-seam-voc-dataset.zip`
- 生成包大小：725 MB
- SHA256：`916616ae5c9fa2015e956e0f08acf21ad36ac43d88835d201eaabd6597af28eb`
- 发布包已排除 `*.baiduyun.uploading.cfg` 等临时同步文件。

## 许可证

该数据集由项目作者采集，并以 Creative Commons Attribution 4.0 International License 发布。

- SPDX 标识：`CC-BY-4.0`
- 许可证说明：[docs/licenses/dataset-cc-by-4.0.zh-CN.md](../licenses/dataset-cc-by-4.0.zh-CN.md)
- 许可证地址：https://creativecommons.org/licenses/by/4.0/

用户可以共享和改编该数据集，包括商业使用，但需要给出适当署名并说明是否做过修改。

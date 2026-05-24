# HMI Weld Seam Segmentation Dataset

This dataset contains self-collected weld seam images and pixel-level segmentation masks for the HMI weld seam recognition demo.

## Format

The dataset is distributed as a Pascal VOC-style segmentation package:

```text
hmi-weld-seam-voc-dataset/
└── VOC2007/
    ├── JPEGImages/
    ├── SegmentationClass/
    └── ImageSets/
        └── Segmentation/
```

## Classes

```text
0: background
1: seam
```

## Split

```text
train: 845
val: 94
trainval: 939
test: 0
```

## Release Asset

- Public package name: `hmi-weld-seam-voc-dataset.zip`
- Generated package size: 725 MB
- SHA256: `3884d5defca276ea33f0f1777610d7ebe2c90f8c5a6ea4e8c8a1011ed691895a`
- Temporary sync files such as `*.baiduyun.uploading.cfg` are excluded from the release package.

## License

The dataset was collected by the project author. Choose and document the intended dataset license before publishing the dataset asset publicly.

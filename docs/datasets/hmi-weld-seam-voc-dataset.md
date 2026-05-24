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
- SHA256: `7be92087bfc5e2bd8afc039057e50c3953694128efe48e21f6bda90b22019970`
- Temporary sync files such as `*.baiduyun.uploading.cfg` are excluded from the release package.

## License

The dataset was collected by the project author and is released under the Creative Commons Attribution 4.0 International License.

- SPDX identifier: `CC-BY-4.0`
- License notice: `docs/licenses/dataset-cc-by-4.0.md`
- License URL: https://creativecommons.org/licenses/by/4.0/

Users may share and adapt the dataset, including for commercial use, provided that appropriate attribution is given and changes are indicated.

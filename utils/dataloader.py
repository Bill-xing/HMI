import os

import cv2
import numpy as np
from PIL import Image
from torch.utils.data.dataset import Dataset

from utils.utils import cvtColor, preprocess_input


class UnetDataset(Dataset):
    def __init__(self, annotation_lines, input_shape, num_classes, train, dataset_path):
        super(UnetDataset, self).__init__()
        self.annotation_lines   = annotation_lines
        self.length             = len(annotation_lines)
        self.input_shape        = input_shape
        self.num_classes        = num_classes
        self.train              = train
        self.dataset_path       = dataset_path

    def __len__(self):
        return self.length

    def __getitem__(self, index):
        annotation_line = self.annotation_lines[index]
        name            = annotation_line.split()[0]

        #-------------------------------#
        #   从文件中读取图像
        #-------------------------------#
        jpg         = Image.open(os.path.join(os.path.join(self.dataset_path, "VOC2007/JPEGImages"), name + ".jpg"))
        png         = Image.open(os.path.join(os.path.join(self.dataset_path, "VOC2007/SegmentationClass"), name + ".png"))
        #-------------------------------#
        #   数据增强
        #-------------------------------#
        jpg, png    = self.get_random_data(jpg, png, self.input_shape, random = self.train)
        # jpg, png = self.get_simple_data(jpg, png, self.input_shape)
        # 对图像进行预处理，通常包括：
        # - 归一化（除以255，将像素值缩放到0-1范围）
        # - 减均值
        # - 除以标准差
        # 将维度从(H, W, C)转换为(C, H, W)
        # - 原始：(512, 512, 3)
        # - 转换后：(3, 512, 512)
        # 这是因为PyTorch期望的输入格式是(C, H, W)
        jpg         = np.transpose(preprocess_input(np.array(jpg, np.float64)), [2,0,1])
        png         = np.array(png)
        png[png >= self.num_classes] = self.num_classes
        
        #   转化成one_hot的形式
        seg_labels  = np.eye(self.num_classes + 1)[png.reshape([-1])]
        seg_labels  = seg_labels.reshape((int(self.input_shape[0]), int(self.input_shape[1]), self.num_classes + 1))

        return jpg, png, seg_labels

    def rand(self, a=0, b=1):
        return np.random.rand() * (b - a) + a

    def get_random_data(self, image, label, input_shape, jitter=.3, hue=.1, sat=1.5, val=1.5, random=True):
        # 将图像转换为RGB格式，确保图像格式统一
        image = cvtColor(image)
        # 将标签转换为PIL图像格式，便于后续处理
        label = Image.fromarray(np.array(label))
        # 获取目标尺寸（512x512）
        h, w = input_shape

        # 验证模式：不进行数据增强，只进行简单的中心缩放
        if not random:
            # 获取原始图像尺寸
            iw, ih  = image.size
            # 计算缩放比例，保持宽高比
            scale   = min(w/iw, h/ih)
            # 计算新的宽度和高度
            nw      = int(iw*scale)
            nh      = int(ih*scale)

            # 使用双线性插值缩放图像，保持图像质量
            image       = image.resize((nw,nh), Image.BICUBIC)
            # 创建新的背景图像（灰色填充）
            new_image   = Image.new('RGB', [w, h], (128,128,128))
            # 将缩放后的图像粘贴到中心位置
            new_image.paste(image, ((w-nw)//2, (h-nh)//2))

            # 使用最近邻插值缩放标签，保持标签值不变
            label       = label.resize((nw,nh), Image.NEAREST)
            # 创建新的背景标签（黑色填充）
            new_label   = Image.new('L', [w, h], (0))
            # 将缩放后的标签粘贴到中心位置
            new_label.paste(label, ((w-nw)//2, (h-nh)//2))
            return new_image, new_label




#-------------------------------------------------------------

        # 训练模式：进行数据增强
        # 随机缩放比例，用于调整图像大小
        rand_jit1 = self.rand(1-jitter,1+jitter)
        rand_jit2 = self.rand(1-jitter,1+jitter)
        # 计算新的宽高比
        new_ar = w/h * rand_jit1/rand_jit2

        # 随机缩放范围在0.25到2之间
        scale = self.rand(0.25, 2)
        # 根据新的宽高比计算新的尺寸
        if new_ar < 1:
            nh = int(scale*h)
            nw = int(nh*new_ar)
        else:
            nw = int(scale*w)
            nh = int(nw/new_ar)
        # 缩放图像和标签
        image = image.resize((nw,nh), Image.BICUBIC)
        label = label.resize((nw,nh), Image.NEAREST)
        

#--------------------------------------------------------------------------

        # 随机水平翻转
        flip = self.rand()<.5
        if flip: 
            image = image.transpose(Image.FLIP_LEFT_RIGHT)
            label = label.transpose(Image.FLIP_LEFT_RIGHT)
        
#--------------------------------------------------------------------------



        # 随机位置放置
        # 计算随机偏移量
        dx = int(self.rand(0, w-nw))
        dy = int(self.rand(0, h-nh))
        # 创建新的背景图像和标签
        new_image = Image.new('RGB', (w,h), (128,128,128))
        new_label = Image.new('L', (w,h), (0))
        # 将图像和标签粘贴到随机位置
        new_image.paste(image, (dx, dy))
        new_label.paste(label, (dx, dy))
        image = new_image
        label = new_label


#--------------------------------------------------------------------------
        # 颜色增强
        # 随机调整色调、饱和度和亮度
        hue = self.rand(-hue, hue)
        sat = self.rand(1, sat) if self.rand()<.5 else 1/self.rand(1, sat)
        val = self.rand(1, val) if self.rand()<.5 else 1/self.rand(1, val)
        
        # 将图像转换到HSV颜色空间
        x = cv2.cvtColor(np.array(image,np.float32)/255, cv2.COLOR_RGB2HSV)
        # 调整色调
        x[..., 0] += hue*360
        x[..., 0][x[..., 0]>1] -= 1
        x[..., 0][x[..., 0]<0] += 1
        # 调整饱和度
        x[..., 1] *= sat
        # 调整亮度
        x[..., 2] *= val
        # 确保值在有效范围内
        x[x[:,:, 0]>360, 0] = 360
        x[:, :, 1:][x[:, :, 1:]>1] = 1
        x[x<0] = 0
        # 转换回RGB颜色空间
        image_data = cv2.cvtColor(x, cv2.COLOR_HSV2RGB)*255
        return image_data,label

    def get_simple_data(self, image, label, input_shape):
        """
        简单的图像处理函数，不进行数据增强
        Args:
            image: 输入图像
            label: 分割标签
            input_shape: 目标尺寸 [height, width]
        Returns:
            image_data: 处理后的图像
            label: 处理后的标签
        """
        # print("get_simple_data")
        # 将图像转换为RGB格式
        image = cvtColor(image)
        # 将标签转换为PIL图像格式
        label = Image.fromarray(np.array(label))
        h, w = input_shape

        # 获取原始图像尺寸
        iw, ih = image.size
        # 计算缩放比例，保持宽高比
        scale = min(w/iw, h/ih)
        # 计算新的宽度和高度
        nw = int(iw*scale)
        nh = int(ih*scale)

        # 使用双线性插值缩放图像
        image = image.resize((nw,nh), Image.BICUBIC)
        # 创建新的背景图像（灰色填充）
        new_image = Image.new('RGB', [w, h], (128,128,128))
        # 将缩放后的图像粘贴到中心位置
        new_image.paste(image, ((w-nw)//2, (h-nh)//2))

        # 使用最近邻插值缩放标签
        label = label.resize((nw,nh), Image.NEAREST)
        # 创建新的背景标签（黑色填充）
        new_label = Image.new('L', [w, h], (0))
        # 将缩放后的标签粘贴到中心位置
        new_label.paste(label, ((w-nh)//2, (h-nh)//2))

        # 转换为numpy数组
        image_data = np.array(new_image, np.uint8)
        return image_data, new_label


# DataLoader中collate_fn使用
def unet_dataset_collate(batch):
    images      = []
    pngs        = []
    seg_labels  = []
    for img, png, labels in batch:
        images.append(img)
        pngs.append(png)
        seg_labels.append(labels)
    images      = np.array(images)
    pngs        = np.array(pngs)
    seg_labels  = np.array(seg_labels)
    return images, pngs, seg_labels

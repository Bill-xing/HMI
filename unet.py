import colorsys
import copy
import time

import cv2
import numpy as np
import torch
import torch.nn.functional as F
from PIL import Image
from torch import nn
from pathlib import Path

from nets.unet import Unet as unet
from utils.utils import cvtColor, preprocess_input, resize_image


BASE_DIR = Path(__file__).resolve().parent


class Unet(object):
    _defaults = {

        "model_path"    : str(BASE_DIR / 'model_data' / 'seam_unet.pth'),

        "num_classes"   : 2,

        "backbone"      : "vgg",
        # "backbone"      : "unet",

        "input_shape"   : [512, 512],

        "mix_type"          : 0,

        "cuda"          : True,
    }

    def __init__(self, **kwargs):
        self.__dict__.update(self._defaults)
        for name, value in kwargs.items():
            setattr(self, name, value)
        self.cuda = self.cuda and torch.cuda.is_available()

        if self.num_classes <= 21:
            self.colors = [ (0, 0, 0), (128, 0, 0), (0, 128, 0), (128, 128, 0), (0, 0, 128), (128, 0, 128), (0, 128, 128), 
                            (128, 128, 128), (64, 0, 0), (192, 0, 0), (64, 128, 0), (192, 128, 0), (64, 0, 128), (192, 0, 128), 
                            (64, 128, 128), (192, 128, 128), (0, 64, 0), (128, 64, 0), (0, 192, 0), (128, 192, 0), (0, 64, 128), 
                            (128, 64, 12)]
        else:
            hsv_tuples = [(x / self.num_classes, 1., 1.) for x in range(self.num_classes)]
            self.colors = list(map(lambda x: colorsys.hsv_to_rgb(*x), hsv_tuples))
            self.colors = list(map(lambda x: (int(x[0] * 255), int(x[1] * 255), int(x[2] * 255)), self.colors))

        self.generate()


    def generate(self):
        # 创建UNet模型实例，指定类别数和主干网络
        self.net = unet(num_classes = self.num_classes, backbone=self.backbone)

        # 设置设备（GPU或CPU）
        device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')
        # 加载预训练模型权重
        self.net.load_state_dict(torch.load(self.model_path, map_location=device))
        # 将模型设置为评估模式
        self.net = self.net.eval()
        print('{} model, and classes loaded.'.format(self.model_path))

        # 如果使用CUDA，配置多GPU训练
        if self.cuda:
            self.net = nn.DataParallel(self.net)  # 使用DataParallel进行多GPU训练
            self.net = self.net.cuda()  # 将模型移动到GPU

    #---------------------------------------------------#
    #   检测图片
    #---------------------------------------------------#
    def detect_image(self, image):
        #---------------------------------------------------------#
        #   在这里将图像转换成RGB图像，防止灰度图在预测时报错。
        #   代码仅仅支持RGB图像的预测，所有其它类型的图像都会转化成RGB
        #---------------------------------------------------------#
        image       = cvtColor(image)
        #---------------------------------------------------#
        #   对输入图像进行一个备份，后面用于绘图
        #---------------------------------------------------#
        old_img     = copy.deepcopy(image)
        orininal_h  = np.array(image).shape[0]
        orininal_w  = np.array(image).shape[1]
        #---------------------------------------------------------#
        #   给图像增加灰条，实现不失真的resize
        #   也可以直接resize进行识别
        #---------------------------------------------------------#
        image_data, nw, nh  = resize_image(image, (self.input_shape[1],self.input_shape[0]))
        #---------------------------------------------------------#
        #   添加上batch_size维度
        #---------------------------------------------------------#
        image_data  = np.expand_dims(np.transpose(preprocess_input(np.array(image_data, np.float32)), (2, 0, 1)), 0)

        with torch.no_grad():
            images = torch.from_numpy(image_data)
            if self.cuda:
                images = images.cuda()
                

            pr = self.net(images)[0]

            pr = F.softmax(pr.permute(1,2,0),dim = -1).cpu().numpy()

            pr = pr[int((self.input_shape[0] - nh) // 2) : int((self.input_shape[0] - nh) // 2 + nh), \
                    int((self.input_shape[1] - nw) // 2) : int((self.input_shape[1] - nw) // 2 + nw)]

            pr = cv2.resize(pr, (orininal_w, orininal_h), interpolation = cv2.INTER_LINEAR)

            pr = pr.argmax(axis=-1)

        if self.mix_type == 0:

            seg_img = np.reshape(np.array(self.colors, np.uint8)[np.reshape(pr, [-1])], [orininal_h, orininal_w, -1])

            image   = Image.fromarray(np.uint8(seg_img))

            image   = Image.blend(old_img, image, 0.7)        
        return image



    def get_miou_png(self, image):

        image       = cvtColor(image)
        orininal_h  = np.array(image).shape[0]
        orininal_w  = np.array(image).shape[1]

        image_data, nw, nh  = resize_image(image, (self.input_shape[1],self.input_shape[0]))

        image_data  = np.expand_dims(np.transpose(preprocess_input(np.array(image_data, np.float32)), (2, 0, 1)), 0)

        with torch.no_grad():
            images = torch.from_numpy(image_data)
            if self.cuda:
                images = images.cuda()
                

            pr = self.net(images)[0]

            pr = F.softmax(pr.permute(1,2,0),dim = -1).cpu().numpy()

            pr = pr[int((self.input_shape[0] - nh) // 2) : int((self.input_shape[0] - nh) // 2 + nh), \
                    int((self.input_shape[1] - nw) // 2) : int((self.input_shape[1] - nw) // 2 + nw)]

            pr = cv2.resize(pr, (orininal_w, orininal_h), interpolation = cv2.INTER_LINEAR)

            pr = pr.argmax(axis=-1)
    
        image = Image.fromarray(np.uint8(pr))
        return image

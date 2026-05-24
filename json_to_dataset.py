import base64
import json
import os
import os.path as osp

import numpy as np
import PIL.Image
from labelme import utils

if __name__ == '__main__':
    # 定义数据集路径
    jpgs_path   = "datasets/JPEGImages"      # 存放原始图片的路径
    pngs_path   = "datasets/SegmentationClass" # 存放分割标签的路径
    # 定义类别，第一个类别必须是背景，值为0
    classes     = ["_background_","seam"]     # 背景类索引为0，接缝类索引为1
    
    # 获取before文件夹中的所有文件
    count = os.listdir("./datasets/before/") 
    for i in range(0, len(count)):
        path = os.path.join("./datasets/before", count[i])

        # 只处理json文件
        if os.path.isfile(path) and path.endswith('json'):
            # 读取json文件
            data = json.load(open(path))
            
            # 获取图像数据
            if data['imageData']:
                # 如果json中包含图像数据，直接使用
                imageData = data['imageData']
            else:
                # 如果json中没有图像数据，则从文件读取
                imagePath = os.path.join(os.path.dirname(path), data['imagePath'])
                with open(imagePath, 'rb') as f:
                    imageData = f.read()
                    # 将图像数据转换为base64编码
                    imageData = base64.b64encode(imageData).decode('utf-8')

            # 将base64编码的图像数据转换为numpy数组
            img = utils.img_b64_to_arr(imageData)
            
            # 创建标签名称到值的映射字典，背景类默认为0
            label_name_to_value = {'_background_': 0}
            # 遍历json中的每个标注形状
            for shape in data['shapes']:
                label_name = shape['label']
                # 如果标签名称已存在，使用其值；否则创建新的值
                if label_name in label_name_to_value:
                    label_value = label_name_to_value[label_name]
                else:
                    label_value = len(label_name_to_value)
                    label_name_to_value[label_name] = label_value
            
            # 确保标签值是连续的
            label_values, label_names = [], []
            for ln, lv in sorted(label_name_to_value.items(), key=lambda x: x[1]):
                label_values.append(lv)
                label_names.append(ln)
            # 验证标签值是否连续
            assert label_values == list(range(len(label_values)))
            
            # 将标注形状转换为标签图，img.shape是图片的尺寸,data['shapes']是标注的形状,
            # label_name_to_value是标签名称到值的映射字典
            lbl = utils.shapes_to_label(img.shape, data['shapes'], label_name_to_value)

            # 保存原始图像为jpg格式。先把图像从numpy转换成pil，然后将其保存为jpg格式。
            #count[i].split(".")[0]  中count[i].split(".")表示字符串按照.分割成一个列表
            #count[i].split(".")[0]表示取列表中的第一个元素，即文件名
            PIL.Image.fromarray(img).save(osp.join(jpgs_path, count[i].split(".")[0]+'.jpg'))

            # 创建新的标签图，初始化为全0，长宽和img都一样。作为背景黑图
            new = np.zeros([np.shape(img)[0],np.shape(img)[1]])
            # 将json中的标签值映射到预定义的类别索引，
            # 也就是把图像中是seam的区域变成1，其他区域变成0。
            # 这一步是为了让标签图的像素值和你自己规定的类别顺序完全一致，避免因标注顺序不同导致的编号混乱。
            for name in label_names:
                index_json = label_names.index(name)  # 获取json中的标签索引label_names = ['_background_', 'seam']，'seam'的index_json就是1。
                index_all = classes.index(name)       # 获取预定义类别中的索引classes = ['_background_','seam']，'seam'的index_all就是1。
                # 将json中的标签值替换为预定义类别的索引
                new = new + index_all*(np.array(lbl) == index_json)

            # 保存标签图为png格式调用工具函数，把new这个标签掩码数组保存为png格式的图片，
            # 文件名和原始json一致，只是扩展名变成.png，存放在pngs_path目录下。
            utils.lblsave(osp.join(pngs_path, count[i].split(".")[0]+'.png'), new)
            print('Saved ' + count[i].split(".")[0] + '.jpg and ' + count[i].split(".")[0] + '.png')

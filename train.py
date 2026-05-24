import os

import numpy as np
import torch
import torch.backends.cudnn as cudnn
import torch.optim as optim
from torch.utils.data import DataLoader
from torchvision import datasets
import torchvision.transforms as transforms
from nets.unet import Unet
from nets.unet_training import weights_init
from utils.callbacks import LossHistory
from utils.dataloader import UnetDataset, unet_dataset_collate
from utils.utils_fit import fit_one_epoch


if __name__ == "__main__":

    Cuda = True

    num_classes = 2

    backbone    = "vgg"
    # backbone    = "unet"

    pretrained  = True
    model_path  = "model_data/seam_unet.pth"
    # model_path =''

    input_shape = [512, 512]
    

    Init_Epoch          = 0      # 初始训练轮次
    Freeze_Epoch        = 50     # 冻结阶段训练轮次
    Freeze_batch_size   = 2      # 冻结阶段batch_size，设置为2是为了避免显存溢出
    Freeze_lr           = 1e-4   # 冻结阶段学习率

    UnFreeze_Epoch      = 100    # 解冻阶段训练轮次
    Unfreeze_batch_size = 2      # 解冻阶段batch_size
    Unfreeze_lr         = 1e-5   # 解冻阶段学习率

    VOCdevkit_path  = 'VOCdevkit'

    dice_loss       = True      # 是否使用dice loss

    focal_loss      = False     # 是否使用focal loss

    cls_weights     = np.ones([num_classes], np.float32)  # 类别权重，默认平衡


    num_workers     = 4         # 数据加载的线程数

    # 创建模型实例
    model = Unet(num_classes=num_classes, pretrained=pretrained, backbone=backbone).train()
    # 如果未使用预训练权重，则初始化权重
    if not pretrained:
        weights_init(model)
    # 如果指定了模型路径，则加载预训练权重
    if model_path != '':
        print('Load weights {}.'.format(model_path))
        # 设置设备（GPU或CPU）
        device          = torch.device('cuda' if torch.cuda.is_available() else 'cpu')
        # 获取当前模型的参数字典
        model_dict      = model.state_dict()
        # 加载预训练权重文件，并映射到指定设备
        pretrained_dict = torch.load(model_path, map_location = device)

        pretrained_dict = {k: v for k, v in pretrained_dict.items() if np.shape(model_dict[k]) == np.shape(v)}

        model_dict.update(pretrained_dict)
        # 将更新后的参数字典加载到模型中
        model.load_state_dict(model_dict)

    # 设置模型为训练模式
    model_train = model.train()
    # 如果使用CUDA，则使用DataParallel进行多GPU训练
    if Cuda:
        model_train = torch.nn.DataParallel(model)
        cudnn.benchmark = True
        model_train = model_train.cuda()

    # 创建损失历史记录器
    loss_history = LossHistory("logs/")

    with open(os.path.join(VOCdevkit_path, "VOC2007/ImageSets/Segmentation/train.txt"),"r") as f:
        train_lines = f.readlines()

    with open(os.path.join(VOCdevkit_path, "VOC2007/ImageSets/Segmentation/val.txt"),"r") as f:
        val_lines = f.readlines()
        

    if True:
        # 设置冻结阶段的训练参数
        batch_size  = Freeze_batch_size
        lr          = Freeze_lr
        start_epoch = Init_Epoch
        end_epoch   = Freeze_Epoch

        # 计算每个epoch的步数
        epoch_step      = len(train_lines) // batch_size
        print(epoch_step)
        epoch_step_val  = len(val_lines) // batch_size
        print(epoch_step_val)
        
        # 检查数据集大小
        if epoch_step == 0 or epoch_step_val == 0:
            raise ValueError("数据集过小，无法进行训练，请扩充数据集。")

        # 创建优化器和学习率调度器
        optimizer       = optim.Adam(model_train.parameters(), lr)
        lr_scheduler    = optim.lr_scheduler.StepLR(optimizer, step_size = 1, gamma = 0.96)

        # 创建数据集和数据加载器
        train_dataset   = UnetDataset(train_lines, input_shape, num_classes, True, VOCdevkit_path)
        val_dataset     = UnetDataset(val_lines, input_shape, num_classes, False, VOCdevkit_path)
        gen             = DataLoader(train_dataset, shuffle = True, batch_size = batch_size, num_workers = num_workers, pin_memory=True,
                                    drop_last = True, collate_fn = unet_dataset_collate)
        gen_val         = DataLoader(val_dataset  , shuffle = True, batch_size = batch_size, num_workers = num_workers, pin_memory=True, 
                                    drop_last = True, collate_fn = unet_dataset_collate)



        #开始训练
        for epoch in range(start_epoch, end_epoch):
            fit_one_epoch(model_train, model, loss_history, optimizer, epoch, 
                    epoch_step, epoch_step_val, gen, gen_val, end_epoch, Cuda, dice_loss, focal_loss, cls_weights, num_classes)
            lr_scheduler.step()

    if True:
        # 设置解冻阶段的训练参数
        batch_size  = Unfreeze_batch_size
        lr          = Unfreeze_lr
        start_epoch = Freeze_Epoch
        end_epoch   = UnFreeze_Epoch

        # 计算验证集步数
        epoch_step_val  = len(val_lines) // batch_size

        # 检查数据集大小
        if epoch_step == 0 or epoch_step_val == 0:
            raise ValueError("数据集过小，无法进行训练，请扩充数据集。")

        # 创建优化器和学习率调度器
        optimizer       = optim.Adam(model_train.parameters(), lr)
        lr_scheduler    = optim.lr_scheduler.StepLR(optimizer, step_size = 1, gamma = 0.96)

        # 创建数据集和数据加载器
        train_dataset   = UnetDataset(train_lines, input_shape, num_classes, True, VOCdevkit_path)
        val_dataset     = UnetDataset(val_lines, input_shape, num_classes, False, VOCdevkit_path)
        gen             = DataLoader(train_dataset, shuffle = True, batch_size = batch_size, num_workers = num_workers, pin_memory=True,
                                    drop_last = True, collate_fn = unet_dataset_collate)
        gen_val         = DataLoader(val_dataset  , shuffle = True, batch_size = batch_size, num_workers = num_workers, pin_memory=True, 
                                    drop_last = True, collate_fn = unet_dataset_collate)



        for epoch in range(start_epoch,end_epoch):
            fit_one_epoch(model_train, model, loss_history, optimizer, epoch, 
                    epoch_step, epoch_step_val, gen, gen_val, end_epoch, Cuda, dice_loss, focal_loss, cls_weights, num_classes)
            lr_scheduler.step()

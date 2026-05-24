import os

import matplotlib
matplotlib.use('Agg')  # 设置matplotlib后端为Agg，这是一个非交互式后端，适合在没有GUI的环境中运行
import scipy.signal
from matplotlib import pyplot as plt


class LossHistory():
    def __init__(self, log_dir, val_loss_flag = True):
        # 导入datetime模块用于生成时间戳
        import datetime
        # 生成当前时间的时间戳，格式为：年_月_日_时_分_秒
        self.time_str       = datetime.datetime.strftime(datetime.datetime.now(), '%Y_%m_%d_%H_%M_%S')
        # 设置损失曲线和数据的保存路径
        self.save_path      = os.path.join(log_dir, "loss_" + str(self.time_str))  
        # 是否记录验证集损失
        self.val_loss_flag  = val_loss_flag

        # 初始化训练损失列表
        self.losses         = []
        # 如果需要记录验证损失，则初始化验证损失列表
        if self.val_loss_flag:
            self.val_loss   = []
        
        # 创建保存目录
        os.makedirs(self.save_path)


    def append_loss(self, loss, val_loss = 0):
        # 将训练损失添加到列表中
        self.losses.append(loss)
        # 将训练损失写入文本文件
        with open(os.path.join(self.save_path, "epoch_loss_" + str(self.time_str) + ".txt"), 'a') as f:
            f.write(str(loss))
            f.write("\n")

        # 如果需要记录验证损失
        if self.val_loss_flag:
            # 将验证损失添加到列表中
            self.val_loss.append(val_loss)
            # 将验证损失写入文本文件
            with open(os.path.join(self.save_path, "epoch_val_loss_" + str(self.time_str) + ".txt"), 'a') as f:
                f.write(str(val_loss))
                f.write("\n")
        # 调用损失曲线绘制函数
        self.loss_plot()


    def loss_plot(self):
        # 创建迭代次数列表，用于x轴
        iters = range(len(self.losses))

        # 创建新的图形
        plt.figure()
        
        # 绘制原始训练损失曲线（红色实线）
        plt.plot(iters, self.losses, 'red', linewidth = 2, label='train loss')
        # 尝试绘制平滑后的训练损失曲线（绿色虚线）
        try:
            # 使用Savitzky-Golay滤波器进行平滑处理
            # 如果数据点少于25个，使用5点平滑窗口，否则使用15点平滑窗口
            plt.plot(iters, scipy.signal.savgol_filter(self.losses, 5 if len(self.losses) < 25 else 15, 3), 'green', linestyle = '--', linewidth = 2, label='smooth train loss')
        except:
            pass

        # 如果需要绘制验证损失
        if self.val_loss_flag:
            # 绘制原始验证损失曲线（珊瑚色实线）
            plt.plot(iters, self.val_loss, 'coral', linewidth = 2, label='val loss')
            # 尝试绘制平滑后的验证损失曲线（棕色虚线）
            try:
                plt.plot(iters, scipy.signal.savgol_filter(self.val_loss, 5 if len(self.losses) < 25 else 15, 3), '#8B4513', linestyle = '--', linewidth = 2, label='smooth val loss')
            except:
                pass

        # 设置图形属性
        plt.grid(True)                    # 添加网格线
        plt.xlabel('Epoch')               # 设置x轴标签
        plt.ylabel('Loss')                # 设置y轴标签
        plt.legend(loc="upper right")     # 在右上角添加图例

        # 保存图形到文件
        plt.savefig(os.path.join(self.save_path, "epoch_loss_" + str(self.time_str) + ".png"))

        # 清理图形资源
        plt.cla()                         # 清除当前图形
        plt.close("all")                  # 关闭所有图形

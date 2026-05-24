import torch
from nets.unet_training import CE_Loss, Dice_loss, Focal_Loss
from tqdm import tqdm

from utils.utils import get_lr
from utils.utils_metrics import f_score


def fit_one_epoch(model_train, model, loss_history, optimizer, epoch, epoch_step, epoch_step_val, gen, gen_val, Epoch, cuda, dice_loss, focal_loss, cls_weights, num_classes):
    # 初始化训练和验证的损失与f_score
    total_loss      = 0
    total_f_score   = 0

    val_loss        = 0
    val_f_score     = 0

    # 设置模型为训练模式
    model_train.train()
    print('Start Train')
    
    # 创建进度条，显示训练进度
    with tqdm(total=epoch_step,desc=f'Epoch {epoch + 1}/{Epoch}',postfix=dict,mininterval=0.3) as pbar:
        # 遍历训练数据
        for iteration, batch in enumerate(gen):
            if iteration >= epoch_step: 
                break
            # 解包批次数据
            imgs, pngs, labels = batch

            # 将数据转换为tensor并移动到GPU（如果可用）
            with torch.no_grad():
                imgs    = torch.from_numpy(imgs).type(torch.FloatTensor)
                pngs    = torch.from_numpy(pngs).long()
                labels  = torch.from_numpy(labels).type(torch.FloatTensor)
                weights = torch.from_numpy(cls_weights)
                if cuda:
                    imgs    = imgs.cuda()
                    pngs    = pngs.cuda()
                    labels  = labels.cuda()
                    weights = weights.cuda()

            # 清空优化器的梯度
            optimizer.zero_grad()

            # 前向传播
            outputs = model_train(imgs)
            # 根据设置选择损失函数
            if focal_loss:
                loss = Focal_Loss(outputs, pngs, weights, num_classes = num_classes)
            else:
                loss = CE_Loss(outputs, pngs, weights, num_classes = num_classes)

            # 如果使用dice loss，将其添加到总损失中
            if dice_loss:
                main_dice = Dice_loss(outputs, labels)
                loss      = loss + main_dice

            # 计算f_score（不计算梯度）
            with torch.no_grad():
                _f_score = f_score(outputs, labels)

            # 反向传播和优化
            loss.backward()
            optimizer.step()

            # 累加损失和f_score
            total_loss      += loss.item()
            total_f_score   += _f_score.item()
            
            # 更新进度条显示的信息
            pbar.set_postfix(**{'total_loss': total_loss / (iteration + 1), 
                                'f_score'   : total_f_score / (iteration + 1),
                                'lr'        : get_lr(optimizer)})
            pbar.update(1)

    print('Finish Train')

    # 设置模型为评估模式
    model_train.eval()
    print('Start Validation')
    
    # 创建验证进度条
    with tqdm(total=epoch_step_val, desc=f'Epoch {epoch + 1}/{Epoch}',postfix=dict,mininterval=0.3) as pbar:
        # 遍历验证数据
        for iteration, batch in enumerate(gen_val):
            if iteration >= epoch_step_val:
                break
            # 解包验证批次数据
            imgs, pngs, labels = batch
            # 将数据转换为tensor并移动到GPU（如果可用）
            with torch.no_grad():
                imgs    = torch.from_numpy(imgs).type(torch.FloatTensor)
                pngs    = torch.from_numpy(pngs).long()
                labels  = torch.from_numpy(labels).type(torch.FloatTensor)
                weights = torch.from_numpy(cls_weights)
                if cuda:
                    imgs    = imgs.cuda()
                    pngs    = pngs.cuda()
                    labels  = labels.cuda()
                    weights = weights.cuda()

                # 前向传播（验证时不计算梯度）
                outputs     = model_train(imgs)
                # 计算损失
                if focal_loss:
                    loss = Focal_Loss(outputs, pngs, weights, num_classes = num_classes)
                else:
                    loss = CE_Loss(outputs, pngs, weights, num_classes = num_classes)

                # 如果使用dice loss，将其添加到总损失中
                if dice_loss:
                    main_dice = Dice_loss(outputs, labels)
                    loss  = loss + main_dice
                # 计算f_score
                _f_score    = f_score(outputs, labels)

                # 累加验证损失和f_score
                val_loss    += loss.item()
                val_f_score += _f_score.item()
                
            # 更新验证进度条显示的信息
            pbar.set_postfix(**{'total_loss': val_loss / (iteration + 1),
                                'f_score'   : val_f_score / (iteration + 1),
                                'lr'        : get_lr(optimizer)})
            pbar.update(1)
            
    # 记录损失历史
    loss_history.append_loss(total_loss/(epoch_step+1), val_loss/(epoch_step_val+1))
    print('Finish Validation')
    print('Epoch:'+ str(epoch+1) + '/' + str(Epoch))
    print('Total Loss: %.3f || Val Loss: %.3f ' % (total_loss / (epoch_step + 1), val_loss / (epoch_step_val + 1)))
    # 保存模型权重
    torch.save(model.state_dict(), 'logs/ep%03d-loss%.3f-val_loss%.3f.pth'%((epoch + 1), total_loss / (epoch_step + 1), val_loss / (epoch_step_val + 1)))

def fit_one_epoch_no_val(model_train, model, loss_history, optimizer, epoch, epoch_step, gen, Epoch, cuda, dice_loss, focal_loss, cls_weights, num_classes):
    """
    不使用验证集的单轮训练函数
    参数说明：
    model_train: 训练模式的模型
    model: 原始模型
    loss_history: 损失记录器
    optimizer: 优化器
    epoch: 当前轮次
    epoch_step: 每轮的步数
    gen: 数据生成器
    Epoch: 总轮次
    cuda: 是否使用GPU
    dice_loss: 是否使用Dice损失
    focal_loss: 是否使用Focal损失
    cls_weights: 类别权重
    num_classes: 类别数量
    """
    # 初始化总损失和F1分数
    total_loss = 0
    total_f_score = 0
    
    print('Start Train')
    # 使用tqdm创建进度条，显示训练进度
    with tqdm(total=epoch_step, desc=f'Epoch {epoch + 1}/{Epoch}', postfix=dict, mininterval=0.3) as pbar:
        # 遍历数据生成器
        for iteration, batch in enumerate(gen):
            # 如果达到每轮步数上限，跳出循环
            if iteration >= epoch_step: 
                break
            # 解包批次数据：图像、分割图、标签
            imgs, pngs, labels = batch

            # 数据转换块：不需要计算梯度
            with torch.no_grad():
                # 将numpy数组转换为PyTorch张量
                imgs = torch.from_numpy(imgs).type(torch.FloatTensor)  # 图像转换为浮点型
                pngs = torch.from_numpy(pngs).long()  # 分割图转换为长整型
                labels = torch.from_numpy(labels).type(torch.FloatTensor)  # 标签转换为浮点型
                weights = torch.from_numpy(cls_weights)  # 类别权重转换为张量
                
                # 如果使用GPU，将数据移至GPU
                if cuda:
                    imgs = imgs.cuda()
                    pngs = pngs.cuda()
                    labels = labels.cuda()
                    weights = weights.cuda()

            # 清空优化器的梯度
            optimizer.zero_grad()

            # 前向传播，获取模型输出
            outputs = model_train(imgs)
            
            # 根据设置选择损失函数
            if focal_loss:
                # 使用Focal Loss计算损失
                loss = Focal_Loss(outputs, pngs, weights, num_classes=num_classes)
            else:
                # 使用交叉熵损失计算损失
                loss = CE_Loss(outputs, pngs, weights, num_classes=num_classes)

            # 如果使用Dice Loss，添加到总损失中
            if dice_loss:
                main_dice = Dice_loss(outputs, labels)
                loss = loss + main_dice

            # 计算F1分数（不需要梯度）
            with torch.no_grad():
                _f_score = f_score(outputs, labels)

            # 反向传播，计算梯度
            loss.backward()
            # 更新模型参数
            optimizer.step()

            # 累加损失和F1分数
            total_loss += loss.item()
            total_f_score += _f_score.item()
            
            # 更新进度条显示信息
            pbar.set_postfix(**{
                'total_loss': total_loss / (iteration + 1),  # 平均损失
                'f_score': total_f_score / (iteration + 1),  # 平均F1分数
                'lr': get_lr(optimizer)  # 当前学习率
            })
            # 更新进度条
            pbar.update(1)

    print('Finish Train')
            
    # 记录本轮平均损失
    loss_history.append_loss(total_loss/(epoch_step+1))
    print('Finish Validation')
    print('Epoch:'+ str(epoch+1) + '/' + str(Epoch))
    print('Total Loss: %.3f' % (total_loss / (epoch_step + 1)))
    
    # 保存模型权重
    # 文件名格式：ep{轮次:03d}-loss{损失值:.3f}.pth
    torch.save(model.state_dict(), 
              'logs/ep%03d-loss%.3f.pth'%((epoch + 1), total_loss / (epoch_step + 1)))

import time
import sys
import cv2
# import Opencv
import numpy as np
from PIL import Image
from pathlib import Path

from unet import Unet

def process_cp(image_path):
    unet = Unet()
    return image_path
def process_image(image_path):
    # int a
    unet = Unet()
    image = Image.open(image_path)
    r_image = unet.detect_image(image)

    # 保存分割结果
    result_path = Path.cwd() / "segmented_image.png"  # 可以自定义保存路径
    r_image.save(result_path)
    return str(result_path)  # 返回分割结果的路径

if __name__ == "__main__":

    if len(sys.argv) > 1:
        # 命令行模式
        image_path = sys.argv[1]
        print(f"从命令行接收到图片路径: {image_path}")
        result = process_image(image_path)
        if result:
            print(f"处理成功，结果保存在: {result}")
            sys.exit(0)
        else:
            print("处理失败")
            sys.exit(1)

    else:


        unet = Unet()

        mode = "predict"


        if mode == "predict":
        
            while True:

                img = input('Input image filename:')
                try:
                    image = Image.open(img)
                except:
                    print('Open Error! Try again!')
                    continue
                else:
                    r_image = unet.detect_image(image)
                    r_image.show()

        else:
            raise AssertionError("Please specify the correct mode: 'predict'")

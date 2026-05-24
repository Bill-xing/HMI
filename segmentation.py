from PyQt5.QtWidgets import QWidget, QLabel, QVBoxLayout, QHBoxLayout, QPushButton, QFileDialog
from PyQt5.QtGui import QPixmap
from PyQt5.QtCore import Qt
from predict import process_image  # 确保您在predict.py中定义了这个函数

class AnotherUi(QWidget):
    def __init__(self):
        super().__init__()
        self.init_ui()

    def init_ui(self):
        layout = QVBoxLayout()

        # 添加选择图片按钮
        self.select_image_button = QPushButton("选择图片", self)
        self.select_image_button.clicked.connect(self.select_image)
        layout.addWidget(self.select_image_button)

        # 创建一个水平布局用于显示原始图像和分割结果
        self.image_layout = QHBoxLayout()

        # 创建原始图像的垂直布局
        self.original_layout = QVBoxLayout()
        self.original_image_label = QLabel(self)
        self.original_image_label.setFixedSize(300, 300)  # 设置固定大小
        self.original_image_label.setScaledContents(True)  # 使标签内容自适应大小
        self.original_layout.addWidget(self.original_image_label)
        self.original_image_title = QLabel("原图", self)
        self.original_image_title.setAlignment(Qt.AlignCenter)  # 设置标题居中
        self.original_layout.addWidget(self.original_image_title)

        # 创建分割结果的垂直布局
        self.result_layout = QVBoxLayout()
        self.result_label = QLabel(self)
        self.result_label.setFixedSize(300, 300)  # 设置固定大小
        self.result_label.setScaledContents(True)  # 使标签内容自适应大小
        self.result_layout.addWidget(self.result_label)
        self.result_image_title = QLabel("UNet 分割", self)
        self.result_image_title.setAlignment(Qt.AlignCenter)  # 设置标题居中
        self.result_layout.addWidget(self.result_image_title)

        # 将原始图像和分割结果的垂直布局添加到水平布局中
        self.image_layout.addLayout(self.original_layout)
        self.image_layout.addLayout(self.result_layout)

        layout.addLayout(self.image_layout)
        self.setLayout(layout)

    def select_image(self):
        options = QFileDialog.Options()
        file_name, _ = QFileDialog.getOpenFileName(self, "选择图片", "",
                                                   "Images (*.png *.jpg *.jpeg *.bmp);;All Files (*)", options=options)
        if file_name:
            # 显示原始图像
            self.display_original_image(file_name)

            # 处理图像并显示结果
            result_image_path = self.process_image(file_name)
            self.display_result(result_image_path)

    def display_original_image(self, image_path):
        # 将原始图像显示在标签上
        pixmap = QPixmap(image_path)
        self.original_image_label.setPixmap(pixmap)

    def process_image(self, image_path):
        # 调用predict.py中的处理函数
        return process_image(image_path)

    def display_result(self, result_image_path):
        # 将分割结果显示在标签上
        pixmap = QPixmap(result_image_path)
        self.result_label.setPixmap(pixmap)
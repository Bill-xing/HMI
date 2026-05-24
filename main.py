import os
import sys
import time
import configparser

import serial
# import serial
import serial.tools.list_ports
from PyQt5.QtWidgets import (QApplication, QMessageBox, QFileDialog, QPushButton, QLineEdit, QCheckBox,QMainWindow)
from PyQt5.QtGui import QColor, QIcon, QTextCursor
from PyQt5.QtCore import QTimer

from control import Ui_Dialog
import segmentation

class SerialAssistant(Ui_Dialog):
    def __init__(self):
        super().__init__()
        self.ser=serial.Serial()
        self.unit_serial()
    # 初始化串口，给各个信号绑定槽
    def unit_serial(self):

        #串口检测按钮
        self.serialcheck_button.clicked.connect(self.port_detect)
            # self.ser.port=self
        #打开关闭串口
        self.pushButton_2.clicked.connect(self.port_open_close)

        #发送信息
        self.message_send.clicked.connect(self.single_send)

        # self.servoslider_1.valueChanged.connect(self.show_sliderNum,1)
        # self.servoslider_2.valueChanged.connect(self.show_sliderNum,2)
        # 连接信号和槽函数，每个滑块的变化都触发 show_sliderNum，并传递滑块编号
        self.servoslider_1.valueChanged.connect(lambda: self.show_sliderNum(1))
        self.servoslider_2.valueChanged.connect(lambda: self.show_sliderNum(2))
        self.servoslider_3.valueChanged.connect(lambda: self.show_sliderNum(3))
        self.servoslider_4.valueChanged.connect(lambda: self.show_sliderNum(4))
        self.servoslider_5.valueChanged.connect(lambda: self.show_sliderNum(5))
        self.servoslider_6.valueChanged.connect(lambda: self.show_sliderNum(6))

        # self.servoslider_3.valueChanged.connect(self.show_sliderNum(3))
        # self.servoslider_4.valueChanged.connect(self.show_sliderNum(4))
        # self.servoslider_5.valueChanged.connect(self.show_sliderNum(5))
        # self.servoslider_6.valueChanged.connect(self.show_sliderNum(6))

        # self.servodisplay_1.editingFinished.connect(lambda :self.servoslider_1.setValue(int(self.servodisplay_1.text())))
        # self.servodisplay_2.editingFinished.connect(lambda: self.servoslider_2.setValue(int(self.servodisplay_2.text())))
        self.servodisplay_1.editingFinished.connect(
            lambda: self.servoslider_1.setValue(int(self.servodisplay_1.text())))
        self.servodisplay_2.editingFinished.connect(
            lambda: self.servoslider_2.setValue(int(self.servodisplay_2.text())))
        self.servodisplay_3.editingFinished.connect(
            lambda: self.servoslider_3.setValue(int(self.servodisplay_3.text())))
        self.servodisplay_4.editingFinished.connect(
            lambda: self.servoslider_4.setValue(int(self.servodisplay_4.text())))
        self.servodisplay_5.editingFinished.connect(
            lambda: self.servoslider_5.setValue(int(self.servodisplay_5.text())))
        self.servodisplay_6.editingFinished.connect(
            lambda: self.servoslider_6.setValue(int(self.servodisplay_6.text())))
    def show_sliderNum(self, numberslider):
        # 获取当前舵机滑块的值
        servo_value = getattr(self, f"servoslider_{numberslider}").value()

        print(f"Servo {numberslider}: {servo_value}")


        # 更新对应舵机的显示框
        getattr(self, f"servodisplay_{numberslider}").setText(f"{servo_value}")

        # 获取舵机的高8位和低8位
        servo_value_int = int(servo_value)

        high_byte = (servo_value_int >> 8) & 0xFF  # 获取高8位
        low_byte = servo_value_int & 0xFF  # 获取低8位

        # 将高低8位转为16进制字符串，格式化为两位
        high_byte_hex = format(high_byte, '02X')  # 高8位转为16进制并格式化为2位
        low_byte_hex = format(low_byte, '02X')  # 低8位转为16进制并格式化为2位

        # 构建当前舵机的编号（01, 02, ..., 06）
        servo_id_hex = format(numberslider, '02X')  # 根据舵机编号生成16进制表示，01, 02, ..., 06

        # 构建 send_string，动态更换舵机编号
        send_string = "55 55 08 03 01 C8 00"
        send_string += f" {servo_id_hex} {low_byte_hex} {high_byte_hex}"

        # 将 send_string 分割成单个的16进制字符串并转换为列表
        send_list = []
        for hex_value in send_string.split():
            try:
                num = int(hex_value, 16)  # 将十六进制字符串转换为整数
                send_list.append(num)  # 添加到列表中
                print(send_list)
            except ValueError:
                QMessageBox.critical(self, 'Wrong Data', '输入的数据格式不正确')
                return None

        # 将 send_list 转换为字节类型
        single_sent_string = bytes(send_list)
        print(single_sent_string)
        # 如果串口打开，发送数据
        if self.ser.isOpen():
            sent_num = self.ser.write(single_sent_string)
            # print(sent_num)
            print(f"发送字节数: {sent_num}")
    def single_send(self):
        #获取已经输入的字符串
        single_sent_string=self.lineEdit.text()
        # print(single_sent_string)
        self.send_text(single_sent_string)
    def get_port_name(self):
        full_name=self.serialchoose_box.currentText()
        com_name=full_name[0:full_name.rfind(':')]
        return com_name
    def port_open_close(self):
        if(self.pushButton_2.text()=='打开串口') and self.port_dict:
            self.ser.port=self.get_port_name() #获取端口名称
            self.ser.baudrate=int(self.botelv_botton.currentText())
            self.ser.bytesize=int(self.shujuwei_button.currentText())
            self.ser.parity=self.jiaoyanwei_button.currentText()
            self.ser.stopbits=int(self.tingzhiwei_button.currentText())
            # 打印当前串口设置到终端
            print(f"串口设置：")
            print(f"端口: {self.ser.port}")
            print(f"波特率: {self.ser.baudrate}")
            print(f"数据位: {self.ser.bytesize}")
            print(f"校验位: {self.ser.parity}")
            print(f"停止位: {self.ser.stopbits}")
            QMessageBox.information(self,'Open port mesage','open port success')
            try:
                self.ser.open()
            except serial.SerialException:
                QMessageBox.critical(self, 'Open Port Error', '此串口不能正常打开！')
                return None

            if self.ser.isOpen():
                self.pushButton_2.setText('关闭串口')
        elif self.pushButton_2.text()=='关闭串口':

            try:
                self.ser.close()
            except:
                QMessageBox.critical(self, 'Open Port Error', '此串口不能正常关闭！')
                return None
            self.pushButton_2.setText('打开串口')
    def send_text(self,send_string):
        if self.ser.isOpen():
            #发送的要是是非空信息
            if send_string !='':
                if self.hex_check.isChecked():
                    #移除头尾的空格和换行符
                    send_string=send_string.strip()
                    send_list=[]
                    while send_string != '':
                        try:
                            num=int(send_string[0:2],16)
                        except ValueError:
                            QMessageBox.critical(self,'Wrong Data','请输入16进制数据，以空格分开')
                            return None
                        else:
                            send_string=send_string[2:].strip()
                            send_list.append(num)

                    single_sent_string =bytes(send_list)
                else:
                    single_sent_string=(send_string).encode('utf-8')

                print(single_sent_string)
                sent_num=self.ser.write(single_sent_string)
        else:
            QMessageBox.warning(self, 'Port Warning', '没有可用的串口，请先打开串口！')
            return None
    def port_detect(self):
        print("开始串口检测")
        self.port_dict={}
        port_list=list(serial.tools.list_ports.comports())
        print(port_list)
        self.serialchoose_box.clear()

        for port in port_list:
            self.port_dict["%s" % port[0]] = "%s" % port[1]
            self.serialchoose_box.addItem(port[0] +':' +port[1])
        if len(self.port_dict)==0:
            self.serialchoose_box.addItem('无串口')
        print(self.port_dict)
        self.serialchoose_box.setEnabled(True)
if __name__ == '__main__':
    app = QApplication(sys.argv)
    su=SerialAssistant()

    # 创建第二个主窗口
    mainWindow2 = QMainWindow()
    mainWindow2.setWindowTitle("主窗口 2")
    mainWindow2.setGeometry(950, 100, 800, 600)
    # 创建第二个界面
    another_ui_instance = segmentation.AnotherUi()  # 假设您在 another_ui.py 中定义了 AnotherUi 类
    another_ui_instance.setParent(mainWindow2)  # 将界面设置为主窗口的子部件
    mainWindow2.setCentralWidget(another_ui_instance)
    mainWindow2.show()


    sys.exit(app.exec_())
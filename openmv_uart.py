
import time, image,sensor,math,pyb,ustruct
from image import SEARCH_EX, SEARCH_DS
from pyb import Pin, Timer,LED
uart = pyb.UART(3, 115200, timeout_char = 1000)
# OpenMV4 H7 Plus, OpenMV4 H7, OpenMV3 M7, OpenMV2 M4 的UART(3)是P4-TX P5-RX

########串口发送数据函数处理#########
def UartSendDate(data):  #这个函数不能运行太快，否则会导致串口读取太快导致出错

    prefix_elements = [0x2C, 0x12]
    suffix_elements = [0x5B]
    # 在列表前面插入元素
    data[:0] = prefix_elements  # 切片赋值

    # 将元素添加到列表后面
    data.extend(suffix_elements)
    sendData=bytearray(data)
    uart.write(sendData)
    print(sendData)
########串口发送数据函数处理完毕#############

########串口接收数据函数处理#########
def UartReceiveDate():  #这个函数不能运行太快，否则会导致串口读取太快导致出错
    global Find_Task
    global Target_Num
    global x
    global data
    data[0] = uart.readchar()
    data[1] = uart.readchar()
    data[2] = uart.readchar()
    data[3] = uart.readchar()
    data[4] = uart.readchar()
    data[5] = uart.readchar()
    data[6] = uart.readchar()
    data[7] = uart.readchar()


    if data[x] == 42 and data[x+3] == 38 and x < 5:
        Find_Task =  data[x+1]
        Target_Num = data[x+2]
        Find_Task =  Find_Task - 48
        Target_Num = Target_Num - 48
        print(Find_Task, Target_Num)
        x = 0
    elif x >= 5: x = 0
    x+=1
    #blue_led.toggle



########串口接收数据函数处理完毕#############

sensor.reset()  # Reset and initialize the sensor.
sensor.set_pixformat(sensor.RGB565)  # Set pixel format to RGB565 (or GRAYSCALE)
sensor.set_framesize(sensor.QVGA)  # Set frame size to QVGA (320x240)
sensor.skip_frames(time=2000)  # Wait for settings take effect.
clock = time.clock()  # Create a clock object to track the FPS.

while True:
    clock.tick()  # Update the FPS clock.
    img = sensor.snapshot()  # Take a picture and return the image.
    data=[0,5,15,150,250]
    UartSendDate(data)
    print(clock.fps())  # Note: OpenMV Cam runs about half as fast when connected
    # to the IDE. The FPS should increase once disconnected.

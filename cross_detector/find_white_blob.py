
import sensor, time, image
from machine import UART

# OpenMV4 H7 Plus, OpenMV4 H7, OpenMV3 M7, OpenMV2 M4 ��UART(3)��P4-TX P5-RX
uart = UART(3, 19200)
black_threshold = (90, 100, -128, 0, -128, 5)



########串口发送数据函数处理#########
def UartSendDate(data):

    prefix_elements = []
    suffix_elements = [0x00, 0x00, 0x80, 0x7f]
    # 在列表前面插入元素
    data[:0] = prefix_elements  # 切片赋值

    # 将元素添加到列表后面
    data.extend(suffix_elements)
    sendData=bytearray(data)
    uart.write(sendData)
    print(sendData)
########串口发送数据函数处理完毕#############

# Reset sensor
sensor.reset()

# Sensor settings
sensor.set_framesize(sensor.QVGA)
#sensor.set_windowing((320, 240))
sensor.set_pixformat(sensor.RGB565)

sensor.skip_frames(time = 2000)
sensor.set_auto_gain(False)


clock = time.clock()

while (True):
    clock.tick()
    img = sensor.snapshot()
    img.lens_corr(0.8) # for 2.8mm lens...
    #img.gaussian(1)
    img.binary([black_threshold])
    #img.invert()
    #img.gaussian(1)
    max_area=0
    max_blob=None
    img.dilate(3)
    img.erode(2)
    #

    #img.find_edges(image.EDGE_CANNY, threshold=(50, 80))
    #img.find_edges(image.EDGE_SIMPLE, threshold=(100, 255))
    img.bilateral(1, color_sigma=1, space_sigma=1)
    for blob in img.find_blobs([(90, 100, -128, 0, -128, 5)]):
        if blob.area()*blob.density()>max_area:
            max_area=blob.area()*blob.density()
            max_blob=blob
         #img.draw_rectangle(blob.rect())
    if max_blob:
        img.draw_rectangle(max_blob.rect())
        data=[int(max_blob.cxf()),int(max_blob.cyf())]
        UartSendDate(data)
    else:
        print("nofind")
    # Draw FPS
    img.draw_string(0, 0, "FPS:%.2f"%(clock.fps()))


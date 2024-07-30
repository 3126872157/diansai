#注意事项：
#1.调试参数都在下面，需要调试的参数都在注释中标明
#2.旋转棋盘请幅度大一点，45度左右最佳
#3.OpenMV4 H7 Plus的UART(3)是P4-TX P5-RX 波特率为115200
#4.串口发送1/2.1:更新棋子状态2:更新棋盘状态(！！！！！仅在要旋转棋盘时使用！！！！！)
#需要调试的参数---------------------------------------------------------------------------------------------------------
lens_corr_threshold = 1.3                               # 畸变矫正参数调高方形四角变尖，调低方形变圆
black_threshold = (23, 45, -44, -13, 9, 34)             # 边框颜色阈值，调第二位（现在是55），调高识别更宽松，调低识别更严格
standard_edge_rect_length = 82                          # 标准边框边长的一半
background_color_threshold = (0,0,0,0,0,0)              # 背景颜色阈值(不重要，没用上)    背景value为0
black_chess_threshold = (0, 30, -50, 50, -50, 50)       # 黑棋阈值                      黑子value为1
white_chess_threshold = (70, 100, -50, 50, -50, 50)     # 白棋阈值                      白子value为-1
show_continually = True                                    # 是否卡死在show_board()里,调试standard_edge_rect_length时请打开
#----------------------------------------------------------------------------------------------------------------------

import sensor
import cmath
import sensor,pyb
from pyb import Pin, Timer

class Block:
    def __init__(self, x=0, y=0, value=0):
        self.x = x
        self.y = y
        self.value = value

standard_edge_rect_corners = ((160 - standard_edge_rect_length, 120 - standard_edge_rect_length),
                             (160 + standard_edge_rect_length, 120 + standard_edge_rect_length))  # 标准边框左上，右下角的坐标
rect_theta = 0  # 现在的边框的斜度
real_edge_rect_corners = ((standard_edge_rect_corners[0][0], standard_edge_rect_corners[0][1]),
                          (standard_edge_rect_corners[1][0], standard_edge_rect_corners[0][1]),
                          (standard_edge_rect_corners[1][0], standard_edge_rect_corners[1][1]),
                          (standard_edge_rect_corners[0][0], standard_edge_rect_corners[1][1]))


uart = pyb.UART(3, 115200, timeout_char = 1000)

blocks = [Block() for _ in range(9)]
block_centers = [None] * 9  # 初始化为None
data = [0]*10
x = 0

########串口发送数据函数处理#########
def UartSendDate(data):
    prefix_elements = [0x43,0x4B]
    suffix_elements = [0x59,0x46]
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
    global x
    global data
    global mode

    data[0] = uart.readchar()
    data[1] = uart.readchar()
    data[2] = uart.readchar()
    data[3] = uart.readchar()
    data[4] = uart.readchar()
    data[5] = uart.readchar()
    data[6] = uart.readchar()
    data[7] = uart.readchar()
    data[8] = uart.readchar()
    data[9] = uart.readchar()
    if data[x+3] == 0x43 and data[x+4] == 0x4B and data[x] == 0x59 and data[x+1] == 0x46 and x < 6:
        mode =  data[x+2]-48
    elif x >= 6: x = 0
    x+=1
########串口接收数据函数处理完毕#############
def rotate_point(x, y, angle):
    x -= 320 / 2
    y -= 240 / 2
    angle_rad = cmath.pi * angle / 180
    new_x = x * cmath.cos(angle_rad).real - y * cmath.sin(angle_rad).real
    new_y = x * cmath.sin(angle_rad).real + y * cmath.cos(angle_rad).real
    new_x += 320 / 2
    new_y += 240 / 2
    return (new_x, new_y)

# 旋转棋盘后更新棋盘角点坐标和棋盘状态
def renew_board():
    global real_edge_rect_corners
    global block_centers
    real_edge_rect_corners = (rotate_point(standard_edge_rect_corners[0][0], standard_edge_rect_corners[0][1], rect_theta),
                              rotate_point(standard_edge_rect_corners[1][0], standard_edge_rect_corners[0][1], rect_theta),
                              rotate_point(standard_edge_rect_corners[1][0], standard_edge_rect_corners[1][1], rect_theta),
                              rotate_point(standard_edge_rect_corners[0][0], standard_edge_rect_corners[1][1], rect_theta))
    #对角线算出1379
    block_centers[0] = Block(int(real_edge_rect_corners[0][0] + (real_edge_rect_corners[2][0] - real_edge_rect_corners[0][0]) / 6),
                             int(real_edge_rect_corners[0][1] + (real_edge_rect_corners[2][1] - real_edge_rect_corners[0][1]) / 6))
    block_centers[8] = Block(int(real_edge_rect_corners[2][0] - (real_edge_rect_corners[2][0] - real_edge_rect_corners[0][0]) / 6),
                             int(real_edge_rect_corners[2][1] - (real_edge_rect_corners[2][1] - real_edge_rect_corners[0][1]) / 6))
    block_centers[2] = Block(int(real_edge_rect_corners[1][0] + (real_edge_rect_corners[3][0] - real_edge_rect_corners[1][0]) / 6),
                             int(real_edge_rect_corners[1][1] + (real_edge_rect_corners[3][1] - real_edge_rect_corners[1][1]) / 6))
    block_centers[6] = Block(int(real_edge_rect_corners[3][0] - (real_edge_rect_corners[3][0] - real_edge_rect_corners[1][0]) / 6),
                             int(real_edge_rect_corners[3][1] - (real_edge_rect_corners[3][1] - real_edge_rect_corners[1][1]) / 6))
    #1379算出24568
    block_centers[1] = Block(int((block_centers[0].x + block_centers[2].x) / 2),
                             int((block_centers[0].y + block_centers[2].y) / 2))
    block_centers[3] = Block(int((block_centers[0].x + block_centers[6].x) / 2),
                             int((block_centers[0].y + block_centers[6].y) / 2))
    block_centers[4] = Block(int((block_centers[0].x + block_centers[8].x) / 2),
                             int((block_centers[0].y + block_centers[8].y) / 2))
    block_centers[5] = Block(int((block_centers[8].x + block_centers[2].x) / 2),
                             int((block_centers[8].y + block_centers[2].y) / 2))
    block_centers[7] = Block(int((block_centers[6].x + block_centers[8].x) / 2),
                             int((block_centers[6].y + block_centers[8].y) / 2))

def show_board():
    img=sensor.snapshot()
    img.lens_corr(lens_corr_threshold)
    # img.binary([black_threshold])
    # img.invert()
    # img.bilateral(1, color_sigma=1, space_sigma=1)
    img.draw_rectangle(standard_edge_rect_corners[0][0],standard_edge_rect_corners[0][1],standard_edge_rect_corners[1][0]-standard_edge_rect_corners[0][0],standard_edge_rect_corners[1][1]-standard_edge_rect_corners[0][1],color=(255,0,0))
    for i in range(9):
        if block_centers[i] is not None:
            img.draw_circle(block_centers[i].x, block_centers[i].y, 9, color=(((block_centers[i].value)+1)*126, ((block_centers[i].value)+1)*126, ((block_centers[i].value)+1)*126),thickness=3)
    while (show_continually):
        img=sensor.snapshot()
        # img = sensor.snapshot()
        # img.lens_corr(lens_corr_threshold)
        # img.binary([black_threshold])
        # img.invert()
        # img.bilateral(1, color_sigma=1, space_sigma=1)
        img.draw_rectangle(standard_edge_rect_corners[0][0],standard_edge_rect_corners[0][1],standard_edge_rect_corners[1][0]-standard_edge_rect_corners[0][0],standard_edge_rect_corners[1][1]-standard_edge_rect_corners[0][1],color=(255,0,0))
        img.draw_circle(int((standard_edge_rect_corners[0][0]+standard_edge_rect_corners[1][0])/2),int((standard_edge_rect_corners[0][1]+standard_edge_rect_corners[1][1])/2),3,color=(255,0,0))
        for i in range(9):
            if block_centers[i] is not None:
                img.draw_circle(block_centers[i].x, block_centers[i].y, 9, color=(((block_centers[i].value)+1)*126, ((block_centers[i].value)+1)*126, ((block_centers[i].value)+1)*126),thickness=3)


def find_theta():
    kernel_size = 1 # 3x3==1, 5x5==2, 7x7==3, etc.
    kernel = [-2, -1,  0, \
              -1,  1,  1, \
               0,  1,  2]
    matching_counts = 0
    error_time=0
    loop = True
    last_theta = 0
    while (loop):
        theta = 0
        target_line_num = 0
        standard_line_cnt = 0
        img = sensor.snapshot()
        img.lens_corr(lens_corr_threshold)
        img.morph(kernel_size, kernel)
        #img.binary([black_threshold])
        img.invert()
        #img.dilate(1)
        #img.erode(1)
        #img.bilateral(2, color_sigma=1, space_sigma=1)
        lines = img.find_line_segments(merge_distance=10, max_theta_difference=5)
        for l in lines:
            if l.length() > img.height() / 2 and ((l.theta() <= 180 and l.theta() > 175) or (l.theta() < 5 and l.theta() >= 0) or (l.theta() < 95 and l.theta() > 85)):
                standard_line_cnt += 1
                continue
            if l.theta() <= 180 and l.theta() >= 0 and l.length() > img.height() / 2:
                target_line_num += 1
                img.draw_line(l.line(), color=(255, 255, 0))
                if l.theta() > 90:
                    theta += (l.theta() - 90)
                else:
                    theta += l.theta()
        if target_line_num > 0:
            theta /= target_line_num
            if abs(theta - last_theta) < 5 or matching_counts == 0:
                matching_counts += 1
                print(matching_counts)
            else:
                matching_counts = 0
                error_time+=1
                if error_time>20:
                    loop=False
                    return 0#识别失败返回默认零度
                print('识别失败 theta: %f last_theta:%f' % (theta, last_theta))
            last_theta = theta
            if matching_counts > 20:
                loop = False
                print('识别成功')
                return last_theta

def color_recognition():
    global block_centers
    img = sensor.snapshot()
    img.lens_corr(lens_corr_threshold)
    black_chess_map=img.binary([black_chess_threshold],to_bitmap=True,copy=True)
    for i in range(9):
        trust_value=0
        for j in range(-5,5):
            for k in range(-5,5):
                if black_chess_map.get_pixel(block_centers[i].x+j, block_centers[i].y+k)==1:
                    trust_value+=1
        #print("black_chess_value_%d:%d" % (i,trust_value))
        if trust_value>90:
            block_centers[i].value=1
    del black_chess_map
    white_chess_map=img.binary([white_chess_threshold],to_bitmap=True,copy=True)
    for i in range(9):
        trust_value=0
        for j in range(-5,5):
            for k in range(-5,5):
                if white_chess_map.get_pixel(block_centers[i].x+j, block_centers[i].y+k)==1:
                    trust_value+=1
        #print("white_chess_value_%d:%d" % (i,trust_value))
        if trust_value>90:
            block_centers[i].value=-1
    del white_chess_map


def init_mode_choose():
    while(mode==0):
        pass


sensor.reset()
sensor.set_framesize(sensor.VGA)
sensor.set_pixformat(sensor.RGB565)
sensor.set_windowing(160,120,320,240)

#show_board()#取消注释以调试参数edge_rect_corners
mode=2#等待选择模式,1:更新棋子状态2:更新棋盘状态(！！！！！仅在要旋转棋盘时使用！！！！！)
init_mode_choose()#用串口启动
renew_board()#初始化棋盘

while(True):
    if mode== 2:
        rect_theta=find_theta()
        renew_board()
        print(rect_theta)
        show_board()#取消注释以调试参数
        while(mode==2):
            UartSendDate([mode,block_centers[0].x,block_centers[0].y,block_centers[1].x,block_centers[1].y,
                          block_centers[2].x,block_centers[2].y,block_centers[3].x,block_centers[3].y,
                          block_centers[4].x,block_centers[4].y,block_centers[5].x,block_centers[5].y,
                          block_centers[6].x,block_centers[6].y,block_centers[7].x,block_centers[7].y,
                          block_centers[8].x,block_centers[8].y])#棋盘格子中心坐标
    #test_theta()#取消注释以观察theta值，请先find_theta()
    if mode==1:
        color_recognition()#请先renew_block()
        #show_board()#取消注释以调试参数
        UartSendDate([mode,block_centers[0].value,block_centers[1].value,block_centers[2].value,
                      block_centers[3].value,block_centers[4].value,block_centers[5].value,
                      block_centers[6].value,block_centers[7].value,block_centers[8].value])#格子状态

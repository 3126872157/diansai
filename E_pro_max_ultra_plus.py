#注意事项：
#1.调试参数都在下面，需要调试的参数都在注释中标明
#2.旋转棋盘请幅度大一点，45度左右最佳
#3.OpenMV4 H7 Plus的UART(3)是P4-TX P5-RX 波特率为115200
#4.串口发送1/2.1:更新棋子状态2:更新棋盘状态(！！！！！仅在要旋转棋盘时使用！！！！！)
#5.mode==2:识别棋盘方向；mode==1:识别棋子；mode==3:发现手指！
#6.流程如下：上电自动识别棋盘角度，循环发送九个坐标，等待单片机发送‘1’后开始循环识别颜色，传回棋子状态，若识别到手指则发送‘3，手指指向编号’。若收到2则开始重新识别棋盘角度并发送，并等待‘1’
#需要调试的参数---------------------------------------------------------------------------------------------------------
lens_corr_threshold = 1.3                               # 畸变矫正参数调高方形四角变尖，调低方形变圆
black_threshold = (23, 45, -44, -13, 9, 34)             # 边框颜色阈值，调第二位（现在是55），调高识别更宽松，调低识别更严格
standard_edge_rect_length = 82                          # 标准边框边长的一半
background_color_threshold = (0,0,0,0,0,0)              # 背景颜色阈值绿色(用于识别手指)  背景value为0
black_chess_threshold = (0, 30, -50, 50, -50, 50)       # 黑棋阈值                      黑子value为1
white_chess_threshold = (70, 100, -50, 50, -50, 50)     # 白棋阈值                      白子value为-1
show_continually = True                                 # 是否卡死在show_board()里,调试standard_edge_rect_length时请打开
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
uart = pyb.UART(3, 115200, timeout_char = 1000)
standard_edge_rect_corners = ((160 - standard_edge_rect_length, 120 - standard_edge_rect_length),
                             (160 + standard_edge_rect_length, 120 + standard_edge_rect_length))  # 标准边框左上，右下角的坐标
rect_theta = 0  # 现在的边框的斜度
real_edge_rect_corners = ((standard_edge_rect_corners[0][0], standard_edge_rect_corners[0][1]),
                          (standard_edge_rect_corners[1][0], standard_edge_rect_corners[0][1]),
                          (standard_edge_rect_corners[1][0], standard_edge_rect_corners[1][1]),
                          (standard_edge_rect_corners[0][0], standard_edge_rect_corners[1][1]))

thresholds = (white_chess_threshold,background_color_threshold,black_chess_threshold)


blocks = [Block() for _ in range(9)]
block_centers = [None] * 9  # 初始化为None
data = [0]*10
not_finger_flag= [0]*9#为零时可能为手指
edge_finger_flag = [0]*(4*standard_edge_rect_length)
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
    for x in range(6):
        if data[(x)%10] == 0x43 and data[(x+1)%10] == 0x4B and data[(x+3)%10] == 0x59 and data[(x+4)%10] == 0x46:
            mode =  data[(x+2)%10]-48
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
    img.draw_rectangle(standard_edge_rect_corners[0][0],standard_edge_rect_corners[0][1],
                       standard_edge_rect_corners[1][0]-standard_edge_rect_corners[0][0],
                       standard_edge_rect_corners[1][1]-standard_edge_rect_corners[0][1],color=(255,0,0))
    for i in range(0, len(real_edge_rect_corners) - 1):
        img.draw_line(int(real_edge_rect_corners[i][0]),int( real_edge_rect_corners[i][1]),
                      int(real_edge_rect_corners[i + 1][0]), int(real_edge_rect_corners[i + 1][1]),
                      color=(255, 0, 0))  # 红色线条

    # 画出矩形的最后一个边
    img.draw_line(int(real_edge_rect_corners[-1][0]), int(real_edge_rect_corners[-1][1]),
                  int(real_edge_rect_corners[0][0]), int(real_edge_rect_corners[0][1]),
                  color=(255, 0, 0))  # 红色线条
    for i in range(9):
        if block_centers[i] is not None:
            img.draw_circle(block_centers[i].x, block_centers[i].y, 9, color=(((block_centers[i].value)+1)*126, ((block_centers[i].value)+1)*126, ((block_centers[i].value)+1)*126),thickness=3)
    while (show_continually):
        img=sensor.snapshot()
    img.lens_corr(lens_corr_threshold)
    # img.binary([black_threshold])
    # img.invert()
    # img.bilateral(1, color_sigma=1, space_sigma=1)
    img.draw_rectangle(standard_edge_rect_corners[0][0],standard_edge_rect_corners[0][1],
                       standard_edge_rect_corners[1][0]-standard_edge_rect_corners[0][0],
                       standard_edge_rect_corners[1][1]-standard_edge_rect_corners[0][1],color=(255,0,0))
    for i in range(0, len(real_edge_rect_corners) - 1):
        img.draw_line(int(real_edge_rect_corners[i][0]),int( real_edge_rect_corners[i][1]),
                      int(real_edge_rect_corners[i + 1][0]), int(real_edge_rect_corners[i + 1][1]),
                      color=(255, 0, 0))  # 红色线条

    # 画出矩形的最后一个边
    img.draw_line(int(real_edge_rect_corners[-1][0]), int(real_edge_rect_corners[-1][1]),
                  int(real_edge_rect_corners[0][0]), int(real_edge_rect_corners[0][1]),
                  color=(255, 0, 0))  # 红色线条
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
                if error_time>10:
                    loop=False
                    return 0#识别失败返回默认零度
                print('识别失败 theta: %f last_theta:%f' % (theta, last_theta))
            last_theta = theta
            if matching_counts > 20:
                loop = False
                print('识别成功')
                return last_theta

def find_furthest_point(list, target):
    max_distance = 0
    for i in list:
        distance2 = (i%3- target%3)*(i%3- target%3) + (i/3 - target/3)*(i/3 - target/3)
        if distance2 > max_distance:
            furthest_point = i
            max_distance = distance2
    return furthest_point

def color_recognition():
    global block_centers
    global not_finger_flag
    not_finger_flag= [0]*9
    totally_recognized_flag=9
    finger_block=0
    while(totally_recognized_flag>0 and finger_block==0):
        totally_recognized_flag=9
        img = sensor.snapshot()
        img.lens_corr(lens_corr_threshold)
        for l in range(-1,1):
            threshold_chess_map=img.binary([thresholds[i]],to_bitmap=True,copy=True)
            for i in range(9):
                trust_value=0
                for j in range(-5,5):
                    for k in range(-5,5):
                        if threshold_chess_map.get_pixel(block_centers[i].x+j, block_centers[i].y+k)==1:
                            trust_value+=1
                #print("black_chess_value_%d:%d" % (i,trust_value))
                if trust_value>90:
                    block_centers[i].value=l
                    not_finger_flag[i]+=1
                    totally_recognized_flag-=1

            #手指识别
            for i in range(4):
                for j in range(standard_edge_rect_length):
                    x=int(real_edge_rect_corners[i][0]+(real_edge_rect_corners[(i+1)%4][0]-real_edge_rect_corners[i][0])/standard_edge_rect_length*j)
                    y=int(real_edge_rect_corners[i][1]+(real_edge_rect_corners[(i+1)%4][1]-real_edge_rect_corners[i][1])/standard_edge_rect_length*j)
                    if (threshold_chess_map.get_pixel(x,y)==1 or threshold_chess_map.get_pixel(x+1,y)==1 or threshold_chess_map.get_pixel(x,y+1)==1 or threshold_chess_map.get_pixel(x-1,y)==1 or threshold_chess_map.get_pixel(x,y-1)==1):
                        edge_finger_flag[i*j]+=1
            del threshold_chess_map
        for i in range(4*standard_edge_rect_length):
            if edge_finger_flag[i]==0:
                sum+=i
                count+=1
        if count!=0:
            finger_block_edge_block=sum/count
            finger_block_edge_block=round(finger_block_edge_block*3/standard_edge_rect_length)+1
            if finger_block_edge_block==4:
                finger_block_edge_block=3
            elif finger_block_edge_block==5:
                finger_block_edge_block=6
            elif finger_block_edge_block==6 or finger_block_edge_block==7:
                finger_block_edge_block=9
                for i in range(int(standard_edge_rect_length/3)):
                    if edge_finger_flag[i+int(standard_edge_rect_length/3*5)]==0:
                        finger_block_edge_block=1
                        break
            elif finger_block_edge_block==8:
                finger_block_edge_block=8
            elif finger_block_edge_block==9 or finger_block_edge_block==10:
                finger_block_edge_block=7
            elif finger_block_edge_block==11:
                finger_block_edge_block=4
            elif finger_block_edge_block==12:
                finger_block_edge_block=1
        possible_finger_block=[]
        for i in range(9):
            if not_finger_flag[i]==0:
                possible_finger_block.append(i)
        if possible_finger_block.count(finger_block_edge_block)==0:
            continue
        else:
            return find_furthest_point(possible_finger_block,finger_block_edge_block)
    return 0

def init_mode_choose():
    while(mode!=1 and mode !=2):
        UartSendDate([0x00])
        #UartReceiveDate()
        pyb.delay(100)


sensor.reset()
sensor.set_framesize(sensor.VGA)
sensor.set_pixformat(sensor.RGB565)
sensor.set_windowing(160,120,320,240)

#show_board()#取消注释以调试参数edge_rect_corners
mode=2#等待选择模式,1:更新棋子状态2:更新棋盘状态(！！！！！仅在要旋转棋盘时使用！！！！！)
renew_board()#初始化棋盘
while(True):
    if mode== 2:
        rect_theta=find_theta()
        renew_board()
        #print(rect_theta)
        #show_board()#取消注释以调试参数
        while(mode==0):
            UartSendDate([mode,block_centers[0].x,block_centers[0].y,block_centers[1].x,block_centers[1].y,
                          block_centers[2].x,block_centers[2].y,block_centers[3].x,block_centers[3].y,
                          block_centers[4].x,block_centers[4].y,block_centers[5].x,block_centers[5].y,
                          block_centers[6].x,block_centers[6].y,block_centers[7].x,block_centers[7].y,
                          block_centers[8].x,block_centers[8].y])#棋盘格子中心坐标
            #show_board()#取消注释以调试参数
            UartReceiveDate()
            pyb.delay(100)
    #test_theta()#取消注释以观察theta值，请先find_theta()
    while(mode==1):
        finger=color_recognition()#请先renew_block()
        #show_board()#取消注释以调试参数
        if finger==0:
            UartSendDate([mode,block_centers[0].value,block_centers[1].value,block_centers[2].value,
                        block_centers[3].value,block_centers[4].value,block_centers[5].value,
                        block_centers[6].value,block_centers[7].value,block_centers[8].value])#格子状态
        else:
            UartSendDate([3,finger])
        UartReceiveDate()
        pyb.delay(100)

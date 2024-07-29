lens_corr_threshold=1.3#畸变矫正参数，调高方形四角变尖，调低方形变圆
black_threshold=(0,55,-50,50,-50,50)#边框颜色（黑色）阈值，调第二位（现在是35），调高识别更宽松，调低识别更严格
standard_edge_rect_length=75#标准边框边长的一半
standard_edge_rect_corners=((160-standard_edge_rect_length,120-standard_edge_rect_length),
                            (160+standard_edge_rect_length,120+standard_edge_rect_length))#标准边框四个角的坐标，左上，右下
#sensor.QQQVGA: 80x60
#sensor.QQVGA: 160x120
#sensor.QVGA: 320x240
#sensor.VGA: 640x480

rect_theta=0#现在的边框的斜度
real_edge_rect_corners=((standard_edge_rect_corners[0][0],standard_edge_rect_corners[0][1]),
                        (standard_edge_rect_corners[1][0],standard_edge_rect_corners[0][1]),
                        (standard_edge_rect_corners[1][0],standard_edge_rect_corners[1][1]),
                        (standard_edge_rect_corners[0][0],standard_edge_rect_corners[1][1]))
#现在的边框四个角的坐标
#1 2
#4 3

block_1_center=None
block_2_center=None
block_3_center=None
block_4_center=None
block_5_center=None
block_6_center=None
block_7_center=None
block_8_center=None
block_9_center=None
#1 2 3
#4 5 6
#7 8 9

import sensor, image, cmath


def reset_standard_edge_rect():#调试参数edge_rect_corners
    while(True):
        img = sensor.snapshot()
        img.lens_corr(lens_corr_threshold)
        #img.gaussian(1)
        img.binary([black_threshold])
        img.invert()
        img.bilateral(1, color_sigma=1, space_sigma=1)
        img.draw_rectangle(standard_edge_rect_corners[0][0],standard_edge_rect_corners[0][1],standard_edge_rect_corners[1][0]-standard_edge_rect_corners[0][0],standard_edge_rect_corners[1][1]-standard_edge_rect_corners[0][1],color=(255,0,0))
        if block_1_center is not None:img.draw_circle(block_1_center[0],block_1_center[1],1,color=(0,255,0))
        if block_2_center is not None:img.draw_circle(block_2_center[0],block_2_center[1],2,color=(0,255,0))
        if block_3_center is not None:img.draw_circle(block_3_center[0],block_3_center[1],3,color=(0,255,0))
        if block_4_center is not None:img.draw_circle(block_4_center[0],block_4_center[1],4,color=(0,255,0))
        if block_5_center is not None:img.draw_circle(block_5_center[0],block_5_center[1],5,color=(0,255,0))
        if block_6_center is not None:img.draw_circle(block_6_center[0],block_6_center[1],6,color=(0,255,0))
        if block_7_center is not None:img.draw_circle(block_7_center[0],block_7_center[1],7,color=(0,255,0))
        if block_8_center is not None:img.draw_circle(block_8_center[0],block_8_center[1],8,color=(0,255,0))
        if block_9_center is not None:img.draw_circle(block_9_center[0],block_9_center[1],9,color=(0,255,0))
def find_theta():#识别角度
    matching_counts=0#用于判断是否稳定。
    loop=True
    last_theta = 0
    while(loop):
        theta = 0
        target_line_num = 0
        standard_line_cnt = 0
        img = sensor.snapshot()
        img.lens_corr(lens_corr_threshold)
        #img.gaussian(1)
        img.binary([black_threshold])
        img.invert()
        img.erode(1)
        img.bilateral(1, color_sigma=1, space_sigma=1)
        lines = img.find_line_segments(merge_distance=10, max_theta_difference=5) # theta和ρ值差异小于边际，则它们合并。
        #img.draw_rectangle(blob.rect(), size=5, color=(150))
        #print("newpicture-----------------------")
        for l in lines:
            if l.length()>img.height()/2 and ((l.theta()<=180 and l.theta()>175) or (l.theta()<5 and l.theta()>=0) or (l.theta()<95 and l.theta()>85)):
                standard_line_cnt+=1
                continue
            if l.theta()<=180 and l.theta()>=0 and l.length()>img.height()/2:
                target_line_num+=1
                img.draw_line(l.line(),color=(255,255,0))
                if  l.theta()>90:#需修改
                    img.draw_line(l.x1(),l.y1(),l.x2(),l.y2(), 5,color = (255,0,255))
                    theta +=l.theta()-90
                else:
                    img.draw_line(l.x1(),l.y1(),l.x2(),l.y2(), 5,color = (255,0,255))
                    theta +=l.theta()
        if target_line_num >0:
            if standard_line_cnt == target_line_num:
                theta = 0
            else:
                theta/=target_line_num
            if abs(theta-last_theta) < 5 or matching_counts == 0:#如果本次矩形与上一次矩形的第n个顶点的n2坐标相差小于5，或本次矩形第一次识别，则判定为该坐标稳定。
                matching_counts += 1#稳定次数+1。
                print(matching_counts)
            else:#只要这四个顶点中有一个不符合稳定要求，则本次的整个循环都作废。
                matching_counts = 0
                print('识别失败 theta: %f last_theta:%f' % (theta,last_theta))
            last_theta = theta#如果稳定判定通过，则更新上次矩形的信息，并进入下一轮的判定。
                #如果稳定次数达到要求，则本次矩形是最终的正确矩形。
            if matching_counts > 20:
                loop = False#结束循环。
                print('识别成功')
                return last_theta

def renew_block():
    global block_1_center,block_2_center,block_3_center,block_4_center,block_5_center,block_6_center,block_7_center,block_8_center,block_9_center
    #对角线算出1379
    block_1_center=(int(real_edge_rect_corners[0][0]+(real_edge_rect_corners[2][0]-real_edge_rect_corners[0][0])/6),int(real_edge_rect_corners[0][1]+(real_edge_rect_corners[2][1]-real_edge_rect_corners[0][1])/6))
    block_9_center=(int(real_edge_rect_corners[2][0]-(real_edge_rect_corners[2][0]-real_edge_rect_corners[0][0])/6),int(real_edge_rect_corners[2][1]-(real_edge_rect_corners[2][1]-real_edge_rect_corners[0][1])/6))
    block_3_center=(int(real_edge_rect_corners[1][0]+(real_edge_rect_corners[3][0]-real_edge_rect_corners[1][0])/6),int(real_edge_rect_corners[1][1]+(real_edge_rect_corners[3][1]-real_edge_rect_corners[1][1])/6))
    block_7_center=(int(real_edge_rect_corners[3][0]-(real_edge_rect_corners[3][0]-real_edge_rect_corners[1][0])/6),int(real_edge_rect_corners[3][1]-(real_edge_rect_corners[3][1]-real_edge_rect_corners[1][1])/6))
   #1379算出24568
    block_2_center=(int((block_1_center[0]+block_3_center[0])/2),int((block_1_center[1]+block_3_center[1])/2))
    block_4_center=(int((block_1_center[0]+block_7_center[0])/2),int((block_1_center[1]+block_7_center[1])/2))
    block_5_center=(int((block_1_center[0]+block_9_center[0])/2),int((block_1_center[1]+block_9_center[1])/2))
    block_6_center=(int((block_9_center[0]+block_3_center[0])/2),int((block_9_center[1]+block_3_center[1])/2))
    block_8_center=(int((block_9_center[0]+block_7_center[0])/2),int((block_9_center[1]+block_7_center[1])/2))
def test_theta():
    while(True):
        img = sensor.snapshot()
        img.lens_corr(lens_corr_threshold) # for 2.8mm lens...
        #img.gaussian(1)
        img.binary([black_threshold])
        img.draw_line(int(img.width()/2), int(img.height()/2), int(img.width()/2+100*cmath.cos(rect_theta/180*cmath.pi).real), int(img.height()/2+100*cmath.sin(rect_theta/180*cmath.pi).real), color = (150,150,0))

def rotate_point(x, y, angle):
    x-=320/2
    y-=240/2
    # 计算旋转后的坐标
    # 将角度从度转换为弧度
    angle_rad = cmath.pi * angle / 180
    
    # 计算旋转后的坐标
    new_x = x * cmath.cos(angle_rad).real - y * cmath.sin(angle_rad).real
    new_y = x * cmath.sin(angle_rad).real + y * cmath.cos(angle_rad).real
    new_x+=320/2
    new_y+=240/2
    return (new_x, new_y)
def renew_real_edge_rect_corners():
    global real_edge_rect_corners
    real_edge_rect_corners=(rotate_point(standard_edge_rect_corners[0][0],standard_edge_rect_corners[0][1],rect_theta),
                            rotate_point(standard_edge_rect_corners[1][0],standard_edge_rect_corners[0][1],rect_theta),
                            rotate_point(standard_edge_rect_corners[1][0],standard_edge_rect_corners[1][1],rect_theta),
                            rotate_point(standard_edge_rect_corners[0][0],standard_edge_rect_corners[1][1],rect_theta))

sensor.reset()
#sensor.set_auto_gain(False) # 关闭自动自动增益。默认开启的，在颜色识别中，一定要关闭白平衡。
#sensor.set_auto_whitebal(False)
sensor.set_framesize(sensor.QVGA)#修改此处还需修改3，4，132，133……
sensor.set_pixformat(sensor.RGB565)

while(True):
    
    #reset_standard_edge_rect()#取消注释以调试参数edge_rect_corners
    #rect_theta=find_theta()#需要旋转再取消注释
    renew_real_edge_rect_corners()
    renew_block()
    reset_standard_edge_rect()
    
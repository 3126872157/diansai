# Untitled - By: autis - Mon Jul 29 2024

#需调整参数：
lens_corr_threshold=1.3#畸变矫正参数，调高方形四角变尖，调低方形变圆
black_threshold=(0,55,-50,50,-50,50)#边框颜色（黑色）阈值，调第二位（现在是35），调高识别更宽松，调低识别更严格
big_rect_threshold=320*240*0.5#用于判断是否为最外的边框，前两个参数是图像长宽（见下方）调最后一个小数，占图像总面积多少以上认为是外框
#sensor.QQQVGA: 80x60
#sensor.QQVGA: 160x120
#sensor.QVGA: 320x240
#sensor.VGA: 640x480

import sensor, image, time, cmath

# 计算倾斜角度
def calculate_tilt_angle(corners):
    # 取出左上角和右上角的坐标
    top_left = corners[0]
    top_right = corners[1]
    # 计算复数的实部和虚部
    real_part = top_right[0] - top_left[0]
    imag_part = top_left[1] - top_right[1]  # 注意这里我们交换了y坐标，因为atan2的参数是(y, x)
    # 创建复数
    complex_num = complex(real_part, imag_part)
    # 使用cmath.phase()来获取复数的相位角，单位是弧度
    angle_rad = cmath.phase(complex_num)
    # 将弧度转换为度
    angle_deg = angle_rad * (180 / cmath.pi)
    # 由于atan2返回的角度范围是-π到π，而我们需要的是0到90度，所以需要进行调整
    if angle_deg < 0:
        angle_deg += 180
    # 确保角度在0到90度之间
    if angle_deg > 90:
        angle_deg = 90
    return angle_deg


sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.skip_frames(time = 1000)


clock = time.clock()
edge_corners= None
while(True):
    clock.tick()
    img = sensor.snapshot()
    img.lens_corr(lens_corr_threshold) # for 2.8mm lens...
    #img.gaussian(1)
    img.binary([black_threshold])
    #img.invert()
    #img.gaussian(1)
    aver_theta=0
    cnt=0
    #img.dilate(1)
    #img.erode(1)
    for rect in img.find_rects(threshold = 10000):
        cnt+=1
        img.draw_rectangles(rect.rect(),color = (0,255,0))
        aver_theta+=calculate_tilt_angle(rect.corners())
    if cnt >0:
            aver_theta/=cnt
            img.draw_line(int(img.width()/2), int(img.height()/2), int(img.width()/2+100*cmath.cos(aver_theta/180*cmath.pi).real), int(img.height()/2+100*cmath.sin(aver_theta/180*cmath.pi).real), color = (150,150,0))
    print(clock.fps())

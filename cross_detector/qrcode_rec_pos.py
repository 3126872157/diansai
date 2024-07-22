
import sensor, time, cmath
red = (30, 100, 30, 127, -30, 127)
blue = (0, 15, 0, 40, -80, -20)
yellow = (50, 100, 30, 100, 20, 100)
green = (30, 100, -64, -8, -32, 32)
purple = (21, 80, 13, 127, -128, -13)


# 假设code.corners()返回四个角的坐标
# 示例：code.corners() = [(1, 2), (3, 4), (5, 1), (2, 0)]
# 这里使用示例坐标，实际使用时请替换为真实的坐标数据
code_corners = [(1, 2), (3, 4), (5, 1), (2, 0)]

def calculate_angle(x1, y1, x2, y2):
    # 计算复数的相位，返回值是弧度
    delta_x = x2 - x1
    delta_y = y2 - y1
    angle_rad = cmath.phase(complex(delta_x, delta_y))
    # 将弧度转换为度
    angle_deg = angle_rad * (180 / cmath.pi)
    if angle_deg<0:
        angle_deg+=180
    return angle_deg

# 计算每条线段的角度


sensor.reset()
sensor.set_pixformat(sensor.GRAYSCALE)
sensor.set_framesize(sensor.QVGA)
#sensor.set_auto_gain(False)
#sensor.set_auto_whitebal(False)
clock= time.clock()
while True:
    clock.tick()
    img = sensor.snapshot()
    img.lens_corr(1.2)
    img.binary([(80, 255)])
    img.gaussian(2)
    #img.dilate(1)
    img.erode(1)
    for code in img.find_qrcodes():
        #print(code)
        img.draw_line(code.corners()[0][0], code.corners()[0][1], code.corners()[1][0], code.corners()[1][1],color=(150), thickness=3)
        # 绘制从第二个角到第三个角的线
        img.draw_line(code.corners()[1][0], code.corners()[1][1], code.corners()[2][0], code.corners()[2][1],color=(150), thickness=3)
        # 绘制从第三个角到第四个角的线
        img.draw_line(code.corners()[2][0], code.corners()[2][1], code.corners()[3][0], code.corners()[3][1],color=(150), thickness=3)
        # 绘制从第四个角回到第一个角的线
        img.draw_line(code.corners()[3][0], code.corners()[3][1], code.corners()[0][0], code.corners()[0][1],color=(150), thickness=3)
        qrcode_cx=(code.corners()[0][0]+code.corners()[1][0]+code.corners()[2][0]+code.corners()[3][0])/4
        qrcode_cy=(code.corners()[0][1]+code.corners()[1][1]+code.corners()[2][1]+code.corners()[3][1])/4
        img.draw_cross(int(qrcode_cx), int(qrcode_cy), size=20, color=(150),thickness=3)
        #img.draw_rectangle(code.rect(),color=(0, 0, 0), thickness=5)
        angle1 = calculate_angle(code.corners()[0][0], code.corners()[0][1], code.corners()[1][0], code.corners()[1][1])
        angle2 = calculate_angle(code.corners()[1][0], code.corners()[1][1], code.corners()[2][0], code.corners()[2][1])
        angle3 = calculate_angle(code.corners()[2][0], code.corners()[2][1], code.corners()[3][0], code.corners()[3][1])
        angle4 = calculate_angle(code.corners()[3][0], code.corners()[3][1], code.corners()[0][0], code.corners()[0][1])

        print("angle:")
        print(angle1)
        if angle1 > 135:
            angle1-=90
        if angle1 < 45:
            angle1+=90
        print(angle2)
        if angle2 > 135:
            angle2-=90
        if angle2 < 45:
            angle2+=90
        print(angle3)
        if angle3 < 45:
            angle3+=90
        if angle3 > 135:
            angle3-=90
        print(angle4)
        if angle4 < 45:
            angle4+=90
        if angle4 > 135:
            angle4-=90
        #print(angle4)
        aver_theta=(angle1+angle2+angle3+angle4)/4
        img.draw_line(int(qrcode_cx-150*cmath.cos(aver_theta/180*cmath.pi).real), int(qrcode_cy-150*cmath.sin(aver_theta/180*cmath.pi).real),int(qrcode_cx+150*cmath.cos(aver_theta/180*cmath.pi).real), int(qrcode_cy+150*cmath.sin(aver_theta/180*cmath.pi).real),thickness=5, color = (150))
    print(clock.fps())

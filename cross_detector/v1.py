
import sensor, image, cmath
enable_lens_corr = False # turn on for straighter lines...打开以获得更直的线条…
# 定义一个函数来计算两条直线的交点
# def calculate_intersection(line1, line2):
#     # 获取两条直线的端点坐标
#     x1, y1 = line1.x1(), line1.y1()
#     x2, y2 = line1.x2(), line1.y2()
#     x3, y3 = line2.x1(), line2.y1()
#     x4, y4 = line2.x2(), line2.y2()

#     # 计算斜率和截距
#     slope1 = (y2 - y1) / (x2 - x1) if x2 != x1 else float('inf')
#     intercept1 = y1 - slope1 * x1 if slope1 != float('inf') else 0
#     slope2 = (y4 - y3) / (x4 - x3) if x4 != x3 else float('inf')
#     intercept2 = y3 - slope2 * x3 if slope2 != float('inf') else 0

#     # 如果斜率相同，则直线平行或重合，没有交点
#     if slope1 == slope2:
#         return None

#     # 计算交点的x坐标
#     x = (intercept2 - intercept1) / (slope1 - slope2)

#     # 计算交点的y坐标
#     y = slope1 * x + intercept1

#     # 返回交点坐标
#     return (x, y)

min_degree = 0
max_degree = 179
white = (0, 0, -5, 5, -5, 5)

sensor.reset()
#sensor.set_auto_gain(False) # 关闭自动自动增益。默认开启的，在颜色识别中，一定要关闭白平衡。
#sensor.set_auto_whitebal(False)
sensor.set_framesize(sensor.QQVGA)
sensor.set_pixformat(sensor.GRAYSCALE)

black_threshold = (0, 50)
# high_threshold = (205, 255)

while(True):
    aver_x = 0
    aver_y = 0
    aver_theta = 0
    target_line_num = 0
    # 测试低阈值
    img = sensor.snapshot()
    if enable_lens_corr: img.lens_corr(0.8) # for 2.8mm lens...
    #img.gaussian(1)
    img.binary([black_threshold])
    img.invert()
    #img.gaussian(1)
    #img.dilate(2)
    #img.erode(2)
    #img.find_edges(image.EDGE_CANNY, threshold=(50, 80))

    img.bilateral(1, color_sigma=1, space_sigma=1)

    # for code in img.find_qrcodes():
    #     print(code)
    white_blobs = img.find_blobs([white])
    #if len(white_blobs)>1 and low_threshold[1]<300:
    #    low_threshold[1]+=10
    #    continue
    max_target_area = 0
    for the_blob in white_blobs:
        if the_blob.area() > max_target_area:
            max_target_area=the_blob.area()
            blob=the_blob
    #degree=blob.rotation()/3.14*180
    #while(degree>90):
    #    degree-=90
    #print(degree)
    if blob:
        lines = img.find_line_segments(merge_distance=10, max_theta_difference=5) # theta和ρ值差异小于边际，则它们合并。
        # for i in range(len(lines)):
        #     for j in range(i + 1, len(lines)):
        #         intersection = calculate_intersection(lines[i], lines[j])

        #         if intersection and intersection[0] and intersection[1]:
        #             #img.draw_circle(int(intersection[0]),int(intersection[1]),20,(0, 0, 0))
        #             aver_x += int(intersection[0])
        #             aver_y += int(intersection[1])

        # for wide in range(blob.w()):
        #     if img.get_pixel(blob.x()+wide, blob.y())==0:
        #         img.draw_cross(blob.x()+wide, blob.y(), size=5, color=(150))
        #     #if img.get_pixel(blob.x()+wide, blob.y()+blob.h()-1)==0:
        #     #    img.draw_cross(blob.x()+wide, blob.y()+blob.h(), size=5, color=(150))
        #         break
        # for high in range(blob.h()):
        #     if img.get_pixel(blob.x(), blob.y()+high)==0:
        #         img.draw_cross(blob.x(), blob.y()+high, size=5, color=(150))
        #     #if img.get_pixel(blob.x()+blob.w()-1, blob.y()+high)==0:
        #     #    img.draw_cross(blob.x()+blob.w(), blob.y()+high, size=5, color=(150))
        #         break
        img.draw_cross(int(blob.cxf()), int(blob.cyf()), size=5, color=(100))
        #img.draw_rectangle(blob.rect(), size=5, color=(150))


        for l in lines:
            if (min_degree <= l.theta()) and (l.theta() <= max_degree) and l.length()>25:
                target_line_num+=1
                print("rowdegrees %f" % l.theta())
                #print("rowlength %f" % l.length())





                if  l.theta()>45:#需修改





                    img.draw_circle(l.x1(),l.y1(), 5,color = (200))
                    img.draw_circle(l.x2(),l.y2(), 5,color = (200))
                    aver_theta +=l.theta()-90
                else:
                    img.draw_circle(l.x1(),l.y1(), 5,color = (100))
                    img.draw_circle(l.x2(),l.y2(), 5,color = (100))
                    aver_theta +=l.theta()
        if target_line_num >0:
            aver_theta/=target_line_num
            img.draw_line(int(blob.cxf()), int(blob.cyf()), int(blob.cxf()+100*cmath.cos(aver_theta/180*cmath.pi).real), int(blob.cyf()+100*cmath.sin(aver_theta/180*cmath.pi).real), color = (150))
            #image.binary(thresholds, invert=False)此函数将在thresholds内的
            #图像部分的全部像素变为1白，将在阈值外的部分全部像素变为0黑。invert将图像
            #的0 1（黑 白）进行反转，默认为false不反转。

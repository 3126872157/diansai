lens_corr_threshold=1.3#畸变矫正参数，调高方形四角变尖，调低方形变圆
black_threshold=(0,55,-50,50,-50,50)#边框颜色（黑色）阈值，调第二位（现在是35），调高识别更宽松，调低识别更严格
standard_edge_rect_corners=((0,0),(0,0))#标准边框四个角的坐标，左上，右下
send_edge_rect_corners=None#发送现在的边框四个角的坐标
import sensor, image, cmath


sensor.reset()
#sensor.set_auto_gain(False) # 关闭自动自动增益。默认开启的，在颜色识别中，一定要关闭白平衡。
#sensor.set_auto_whitebal(False)
sensor.set_framesize(sensor.QVGA)
sensor.set_pixformat(sensor.RGB565)

# high_threshold = (205, 255)
def reset_standard_edge_rect():#调试参数edge_rect_corners
    while(True):
        img = sensor.snapshot()
        img.lens_corr(lens_corr_threshold)
        #img.gaussian(1)
        img.binary([black_threshold])
        img.invert()
        img.bilateral(1, color_sigma=1, space_sigma=1)
        img.draw_rectangle(standard_edge_rect_corners[0][0],standard_edge_rect_corners[0][1],standard_edge_rect_corners[1][0]-standard_edge_rect_corners[0][0],standard_edge_rect_corners[1][1]-standard_edge_rect_corners[0][1],color=(255,0,0))

def find_theta():#识别角度
    aver_theta = 0
    target_line_num = 0
    standard_line_cnt = 0
    img = sensor.snapshot()
    img.lens_corr(lens_corr_threshold)
    #img.gaussian(1)
    img.binary([black_threshold])
    img.invert()
    img.bilateral(1, color_sigma=1, space_sigma=1)
    lines = img.find_line_segments(merge_distance=10, max_theta_difference=5) # theta和ρ值差异小于边际，则它们合并。
    #img.draw_rectangle(blob.rect(), size=5, color=(150))
    print("newpicture-----------------------")
    for l in lines:
        if l.length()>img.height()/2 and ((l.theta()<=180 and l.theta()>175) or (l.theta()<5 and l.theta()>=0) or (l.theta()<95 and l.theta()>85)):
            standard_line_cnt+=1
            continue
        if l.theta()<=180 and l.theta()>=0 and l.length()>img.height()/2:
            target_line_num+=1
            img.draw_line(l.line(),color=(255,255,0))
            if  l.theta()>90:#需修改
                img.draw_line(l.x1(),l.y1(),l.x2(),l.y2(), 5,color = (255,0,255))
                aver_theta +=l.theta()-90
            else:
                img.draw_line(l.x1(),l.y1(),l.x2(),l.y2(), 5,color = (255,0,255))
                aver_theta +=l.theta()
    if target_line_num >0:
        if standard_line_cnt >= target_line_num-1:
            aver_theta = 0
        else:
            aver_theta/=target_line_num
    
        #img.draw_line(int(img.width()/2), int(img.height()/2), int(img.width()/2+100*cmath.cos(aver_theta/180*cmath.pi).real), int(img.height()/2+100*cmath.sin(aver_theta/180*cmath.pi).real), color = (150))
        #image.binary(thresholds, invert=False)此函数将在thresholds内的
        #图像部分的全部像素变为1白，将在阈值外的部分全部像素变为0黑。invert将图像
        #的0 1（黑 白）进行反转，默认为false不反转。

while(True):
    reset_standard_edge_rect()
    theta=find_theta
    
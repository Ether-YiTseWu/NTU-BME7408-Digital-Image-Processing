import cv2
import pywt
import numpy as np
import matplotlib.pyplot as plt
import tkinter as tk
from   tkinter import filedialog

#################
### Problem 1 ###
#################
def getPerspectiveM():
    # Set variable
    pointStr = str(point_.get())
    pointList = []
    show_text.insert('insert',"Your input is " + str(pointStr) + '\n')
    pointStr = pointStr.split()
    for i in range(4):
        point = [int(pointStr[i].split(',')[0]), int(pointStr[i].split(',')[1])]
        pointList.append(point)

    imgPoint = np.float32([[0, 0], [1200, 0], [1200, 1200], [0, 1200]])
    dst = np.float32(pointList)

    # Get perspective Matrix
    matrix = cv2.getPerspectiveTransform(dst, imgPoint)

    # Show text
    show_text.insert('insert',"The perspective matrix is \n" + str(matrix) + '\n\n')

#################
### Problem 2 ###
#################
def wavelet():
    # Set up variables
    path = filedialog.askopenfilename()
    imgRGB_part2   = cv2.imread(path)
    imgGray_part2  = cv2.cvtColor(imgRGB_part2, cv2.COLOR_BGR2GRAY)

    # Wavelet Transform
    cA, (cH, cV, cD) = pywt.dwt2(imgGray_part2, 'haar')  

    # Show images
    plt.figure(figsize = (12,4))

    approximation = plt.subplot(1, 4, 1)
    approximation.imshow(cA, cmap ='gray')
    plt.title('Approximation')

    horizontal = plt.subplot(1, 4, 2)
    horizontal.imshow(cH, cmap ='gray')
    plt.title('Horizontal Detail')

    vertical = plt.subplot(1, 4, 3)
    vertical.imshow(cV, cmap ='gray')
    plt.title('Vertical Detail')

    diagonal = plt.subplot(1, 4, 4)
    diagonal.imshow(cD, cmap ='gray')
    plt.title('Diagonal Detail')

    plt.show()

def fuseImg():
    # Set up variables
    fuseNum = int(fuseNum_.get())
    coeffsList = []
    fuseList   = []

    # Show text
    show_text.insert('insert',"Fuse " + str(fuseNum) + ' images\n\n')

    for i in range (fuseNum):
        path = filedialog.askopenfilename()
        imgRGB_part2   = cv2.imread(path)
        imgGray_part2  = cv2.cvtColor(imgRGB_part2, cv2.COLOR_BGR2GRAY)

        # Wavelet Transform
        coeffs  = pywt.wavedec2(imgGray_part2[:,:], 'haar')  
        # coeffsList[img No.(fuseNum)][row No.][coef No.(3)]
        coeffsList.append(coeffs)

    # Fuse (refer : https://stackoverflow.com/questions/42608721/image-fusion-using-wavelet-transform-in-python)
    if fuseNum == 2:
        for i in range(len(coeffsList[0])-1):
            if(i == 0):
                c = np.maximum(coeffsList[0][0], coeffsList[1][0])
                fuseList.append(c)
                continue
            c1 = np.maximum(coeffsList[0][i][0], coeffsList[1][i][0])
            c2 = np.maximum(coeffsList[0][i][1], coeffsList[1][i][1])
            c3 = np.maximum(coeffsList[0][i][2], coeffsList[1][i][2])
            c  = (c1, c2, c3)
            fuseList.append(c)
    elif fuseNum == 3:
        for i in range(len(coeffsList[0])-1):
            if(i == 0):
                c = np.maximum(coeffsList[0][0], coeffsList[1][0])
                c = np.maximum(coeffsList[2][0], c)
                fuseList.append(c)
                continue
            c1 = np.maximum(coeffsList[0][i][0], coeffsList[1][i][0])
            c1 = np.maximum(coeffsList[2][i][0], c1)
            c2 = np.maximum(coeffsList[0][i][1], coeffsList[1][i][1])
            c2 = np.maximum(coeffsList[2][i][1], c2)
            c3 = np.maximum(coeffsList[0][i][2], coeffsList[1][i][2])
            c3 = np.maximum(coeffsList[2][i][2], c3)
            c  = (c1, c2, c3)
            fuseList.append(c)

    fuseImg = pywt.waverec2(fuseList, 'haar')
    
    # Show images
    plt.figure(figsize = (6,4))

    fuseImgShow = plt.subplot(1, 1, 1)
    fuseImgShow.imshow(fuseImg, cmap ='gray')
    plt.title('Fusion Result')

    plt.show()

#################
### Problem 3 ###
#################
def cannyHough():
    # Set up variables
    path = filedialog.askopenfilename()
    imgRGB_part3  = cv2.imread(path)
    imgGray_part3 = cv2.cvtColor(imgRGB_part3, cv2.COLOR_BGR2GRAY)
    imgGray_part3_line = cv2.cvtColor(imgRGB_part3, cv2.COLOR_BGR2GRAY)
    threshold = int(threshold_.get())

    # Show text
    show_text.insert('insert',"Do hough transform, threshold is " + str(threshold) + '\n\n')

    # Hough transform
    imgEdge_part3 = cv2.Canny(imgGray_part3, threshold1 = 50, threshold2 = 150, apertureSize = 3)
    imgLine_part3 = cv2.HoughLines(imgEdge_part3, rho = 1, theta = np.pi/180, threshold = threshold)

    # Draw line
    for i in range(len(imgLine_part3)):
        for rho,theta in imgLine_part3[i]:
            a = np.cos(theta)
            b = np.sin(theta)
            x0 = a*rho
            y0 = b*rho
            x1 = int(x0 + 1000*(-b))
            y1 = int(y0 + 1000*(a))
            x2 = int(x0 - 1000*(-b))
            y2 = int(y0 - 1000*(a))
            cv2.line(imgGray_part3_line, (x1,y1),(x2,y2) , (0,0,255), 2)

    # Show images
    plt.figure(figsize = (10,4))

    originalImgGray = plt.subplot(1, 3, 1)
    originalImgGray.imshow(imgGray_part3, cmap ='gray')
    plt.title('Original Grayscale Image')

    cannyImage = plt.subplot(1, 3, 2)
    cannyImage.imshow(imgEdge_part3, cmap ='gray')
    plt.title('Canny Detection')

    lineImage = plt.subplot(1, 3, 3)
    lineImage.imshow(imgGray_part3_line, cmap ='gray')
    plt.title('Hough Transform (Threshold = '+ str(threshold) + ')')

    plt.show()

###########
### GUI ###
###########
window = tk.Tk()
window.title('DIP_HW_04')
window.geometry('705x310')

# Perspective Transform
button_perspective = tk.Button(window, text = "Get Perspective Matrix", command = getPerspectiveM)
button_perspective.place(x = 10, y = 10)

# User Input : Point wanted
point_frame = tk.Frame(window)
point_frame.place(x = 10, y = 40)
point_label = tk.Label(point_frame, text = 'Point')
point_label.pack(side = tk.LEFT)
point_ = tk.Entry(point_frame, width = 21)
point_.pack()

# Wavlet Transform Button
button_wavelet = tk.Button(window, text = "Wavelet Transform", command = wavelet)
button_wavelet.place(x = 10, y = 115)

# Fuse images with wavelet
button_fuse = tk.Button(window, text = "Fuse Images with DWT", command = fuseImg)
button_fuse.place(x = 10, y = 145)

# User Input : Fuse images numbers
fuseNum_frame = tk.Frame(window)
fuseNum_frame.place(x = 10, y = 175)
fuseNum_label = tk.Label(fuseNum_frame, text = 'Fusing Images Number')
fuseNum_label.pack(side = tk.LEFT)
fuseNum_ = tk.Entry(fuseNum_frame, width = 7)
fuseNum_.pack()

# Hough Transform Button
button_cannyHough = tk.Button(window, text = "Hough Transform", command = cannyHough)
button_cannyHough.place(x = 10, y = 245)

# User Input : Hough Transform Threshold
threshold_frame = tk.Frame(window)
threshold_frame.place(x = 10, y = 275)
threshold_label = tk.Label(threshold_frame, text = 'Hough Threshold')
threshold_label.pack(side = tk.LEFT)
threshold_ = tk.Entry(threshold_frame, width = 9)
threshold_.pack()

# Show Text
show_text = tk.Text(window, height = 22, width = 69)
show_text.place(x = 210, y = 10)

window.mainloop()
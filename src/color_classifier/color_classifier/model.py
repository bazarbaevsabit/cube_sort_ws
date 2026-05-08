import cv2
import numpy as np

def detect_color_hsv(cv_image):
    hsv = cv2.cvtColor(cv_image, cv2.COLOR_BGR2HSV)
    # Диапазоны для красного (два из-за цикличности Hue)
    lower_red1 = np.array([0, 100, 100])
    upper_red1 = np.array([10, 255, 255])
    lower_red2 = np.array([160, 100, 100])
    upper_red2 = np.array([180, 255, 255])
    # Зелёный
    lower_green = np.array([40, 50, 50])
    upper_green = np.array([80, 255, 255])
    # Синий
    lower_blue = np.array([100, 50, 50])
    upper_blue = np.array([130, 255, 255])

    mask_red = cv2.inRange(hsv, lower_red1, upper_red1) + cv2.inRange(hsv, lower_red2, upper_red2)
    mask_green = cv2.inRange(hsv, lower_green, upper_green)
    mask_blue = cv2.inRange(hsv, lower_blue, upper_blue)

    if cv2.countNonZero(mask_red) > 1000:
        return "RED"
    elif cv2.countNonZero(mask_green) > 1000:
        return "GREEN"
    elif cv2.countNonZero(mask_blue) > 1000:
        return "BLUE"
    return "UNKNOWN"

# import the necessary packages
from picamera.array import PiRGBArray
from picamera import PiCamera
import imutils
import time
import cv2
import serial
import time


def distance_to_camera(knownWidth, focalLength, perWidth):
    # compute and return the distance from the maker to the camera
    return (knownWidth * focalLength) / perWidth

def calculate_pixel_width(ref_frame):
    gray = cv2.cvtColor(ref_frame, cv2)
    gray = cv2.GaussianBlur(gray, (5, 5), 0)
    edged = cv2.Canny(gray, 35, 125)
    cnts, bounds, val = cv2.findContours(edged.copy, cv2.RETR_LIST, cv2.CHAIN_APPROX_SIMPLE)
    c = max(bounds, key = cv2.contourArea)
    pixel_width = cv2.minEnclosingCircle(c)
    return pixel_width

def tracking(frame, greenboundary, KNOWN_PIXEL_WIDTH, KNOWN_WIDTH = 7, KNOWN_DISTANCE = 30):
    # pass in a frame o    # capture frames from the camera
    #for frame in camera.capture_continuous(rawCapture, format="bgr", use_video_port=True)
    #KNOWN_PIXEL_WIDTH = calculate_distance(frame, KNOWN_WIDTH, KNOWN_DISTANCE)
    
    #KNOWN_PIXEL_WIDTH = 50 #15#250 # px

    # define the lower and upper boundaries of the "green"
    # ball in the HSV color space, then initialize the
    # list of tracked points
    focalLength = (KNOWN_PIXEL_WIDTH * KNOWN_DISTANCE) / KNOWN_WIDTH
    is_ball = False
    x, y, radius = None, None, None

    # convert colr scheme
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
         
    # construct a mask for the color "green", then perform a series of 
    # dilations and erosions to remove any small blobs left in the mask
    green_lower, green_upper = greenboundary
    mask = cv2.inRange(hsv, greenLower, greenUpper)
    mask = cv2.erode(mask, None, iterations=2)
    mask = cv2.dilate(mask, None, iterations=2)

    # find contours in the mask and initialize the current
    # (x, y) center of the ball
    cnts = cv2.findContours(mask.copy(), cv2.RETR_EXTERNAL,
                            cv2.CHAIN_APPROX_SIMPLE)[-2]  

        
    # only proceed if at least one contour was found
    if len(cnts) > 0:
    # find the largest contour in the mask, then use
    # it to compute the minimum enclosing circle and
    # centroid
    # add check to ensure that the area overlaps,  to be done
    #print( len(cnts))
        c = max(cnts, key=cv2.contourArea)

        M = cv2.moments(c)
        center = (int(M["m10"] / M["m00"]), int(M["m01"] / M["m00"]))

        if radius > 5:
            # draw the circle and centroid on the frame,
            # then update the list of tracked points
            is_ball = True
            cv2.circle(frame, (int(x), int(y)), int(radius), (0, 255, 255), 2)
            cv2.circle(frame, center, 5, (0, 0, 255), -1)

            distance = distance_to_camera(KNOWN_WIDTH, focalLength, radius)
            offset = [frame.shape[0]/2 - x, frame.shape[1]/2 - y]
            return is_ball, distance, offset
    return is_ball, None, None, None

        


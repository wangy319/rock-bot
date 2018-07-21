import cv2

def motion_detection(frame, delta_thresh, min_delta_area, frameAvg=None, framesSinceMotion=0):
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    gray = cv2.GaussianBlur(gray, (21,21), 0)

    if frameAvg is None: # Initialize frame average
        frameAvg = gray.copy().astype("float")

    cv2.accumulateWeighted(gray, frameAvg, 0.5)
    frameDelta = cv2.absdiff(gray, cv2.convertScaleAbs(frameAvg))

    threshold = cv2.threshold(frameDelta, delta_thresh, 255, cv2.THRESH_BINARY)[1]
    threshold = cv2.dilate(threshold, None, iterations=2)
    contours = cv2.findContours(threshold.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)[1]

    for contour in contours:
        if cv2.contourArea(contour) < min_delta_area:
            continue
        motionDetected = True

    framesSinceMotion = 0 if motionDetected else framesSinceMotion + 1

    return framesSinceMotion, frameAvg


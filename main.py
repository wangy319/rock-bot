import cv2
import time
import picamera
import imutils
from picamera.array import PiRGBArray
import faces
from face_client import FaceClient
import json
import argparse
import vision
import track

parser = argparse.ArgumentParser()
parser.add_argument("-c", "--conf", default="./conf.json", help="path to JSON config file")
args = vars(parser.parse_args())

conf = json.load(open(args["conf"]))

API_KEY = 'egk0ko9neatt4ftsgmhsmklf2r'
API_SECRET = '67doedp6s4dcnm5kmuhkgetj8p'
client = FaceClient(API_KEY, API_SECRET)
metrics = ['mood', 'smiling']

domain_name = 'loganrookspi.ddns.net'
filename = "/var/www/html/temp.jpg"
img_addr = 'http://{}/temp.jpg'.format(domain_name)

ref_frame = cv2.imread(conf['ref_img'])
KNOWN_PIXEL_WIDTH = track.calculate_pixel_width(ref_frame)


camera = picamera.PiCamera()
camera.resolution = conf['resolution']
camera.framerate = conf['frame_rate']
rawCapture = PiRGBArray(camera, size=conf['resolution'])
time.sleep(conf["camera_warmup_time"])

frameAvg = None
framesSinceMotion = 0
descriptions = {'identity': None, 'mood': None, 'smiling': None}
for f in camera.capture_continuous(rawCapture, format="bgr", use_video_port=True):
    frame = f.array
    frame = imutils.resize(frame, width=400)

    framesSinceMotion, frameAvg = vision.motion_detection(frame, conf["delta_thresh"], conf["min_delta_area"], frameAvg, framesSinceMotion)

    if framesSinceMotion < conf["max_frames_since_motion"]:
        is_ball, distance, ball_center = track.tracking(frame, conf["green_boundary"], KNOWN_PIXEL_WIDTH)
        is_face, face, face_center = faces.viola_jones(frame, conf["scale_factor"], conf["min_neighbors"], conf["min_size"], conf["size_thresh"])
        if is_face:
            cv2.imwrite(filename, face)
            descriptions = faces.recognize_faces(img_addr, client, metrics)[0]



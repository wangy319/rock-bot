import cv2

def extract_identity(uids, threshold):
    max_confidence = 0
    identity = None
    for id_ in uids:
        confidence = id_['confidence']
        if confidence > threshold and confidence > max_confidence:
            identity = id_['uid']
            max_confidence = confidence
    return identity

def merge_two_dicts(x, y):
    z = x.copy()
    z.update(y)
    return z

def recognize_faces(img, client, metrics):
    response = client.faces_recognize('all', img, namespace='projectfaces')  
    faces = response['photos'][0]['tags']
    face_descriptions = []
    for face in faces:
        attributes = face['attributes']
        threshold = face['threshold']
        if attributes['face']['confidence'] > (threshold-10) and attributes['face']['value'] == 'true': 
            description = merge_two_dicts(
                {
                'identity': extract_identity(face['uids'], threshold), \
                },
                {
                metric: attributes[metric]['value'] if \
                attributes[metric]['confidence'] > threshold \
                else None for metric in metrics
                })
            face_descriptions.append(description)
    return face_descriptions


def viola_jones(frame, scaleFactor=1.3, minNeighbors=5, minSize=10, threshold=2):
    face_cascade = cv2.CascadeClassifier('haarcascades/haarcascade_frontalface_default.xml')
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    faces = face_cascade.detectMultiScale(gray, scaleFactor, minNeighbors, minSize=minSize)
    if len(faces) == 0:
        return False, None, None
    else:
        maxArea = 0
        for (x_, y_, w_, h_) in faces:
            if w_*h_ > maxArea:
                x = x_
                y = y_
                w = w_
                h = h_
        center = [x + w/2, y + h/2]
        roi = frame[y-threshold:y+(h+threshold), x-threshold:x+(w+threshold)]
        return True, roi, center



    


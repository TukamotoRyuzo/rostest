#!/usr/bin/env python
#encoding: utf8
import rospy, cv2
from sensor_msgs.msg import Image
from cv_bridge import CvBridge, CvBridgeError
from pimouse_vision_control.msg import test1

class FaceToFace():
    def __init__(self):
        sub = rospy.Subscriber("/cv_camera/image_raw", Image, self.get_image)
        self.pub = rospy.Publisher("test1", test1, queue_size=10)
        self.bridge = CvBridge()
        self.image_org = None

    def monitor(self,rect,org):                                 #このメソッドを追加
        if rect is not None:
            cv2.rectangle(org,tuple(rect[0:2]),tuple(rect[0:2]+rect[2:4]),(0,255,255),4)
	    d = test1()
	    d.a = rect[0]
	    d.b = rect[1]
	    d.c = rect[2]
	    d.d = rect[3]       
	    self.pub.publish(d)
        
   
    def get_image(self,img):
        try:
            self.image_org = self.bridge.imgmsg_to_cv2(img, "bgr8")
        except CvBridgeError as e:
            rospy.logerr(e)

    def detect_face(self):
        if self.image_org is None:
            return None
    
        org = self.image_org
    
        gimg = cv2.cvtColor(org,cv2.COLOR_BGR2GRAY)
        classifier = "/usr/share/opencv/haarcascades/haarcascade_frontalface_default.xml"
        cascade = cv2.CascadeClassifier(classifier)
        face = cascade.detectMultiScale(gimg,1.1,1,cv2.CASCADE_FIND_BIGGEST_OBJECT)
    
        if len(face) == 0:    #len(face)...以下を次のように書き換え
            self.monitor(None,org)
            return None       
                              
        r = face[0]           
        self.monitor(r,org)   
        return r  

if __name__ == '__main__':
    rospy.init_node('face_to_face')
    fd = FaceToFace()

    rate = rospy.Rate(10)
    while not rospy.is_shutdown():
        rospy.loginfo(fd.detect_face())
        rate.sleep()

# Copyright 2016 Ryuichi Ueda
# Released under the MIT License.
# To make line numbers be identical with the book, this statement is written here. Don't move it to the header.

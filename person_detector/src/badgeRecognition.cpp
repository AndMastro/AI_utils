#include "opencv2/core.hpp"
#include "opencv2/face.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/imgproc/imgproc.hpp>

#include "ros/ros.h"
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include "sensor_msgs/Image.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include "geometry_msgs/PoseArray.h"
#include "std_msgs/String.h"
#include "std_msgs/Int32MultiArray.h"
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <limits.h>
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <curl/curl.h>
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>

#include "opencv2/core.hpp"
#include "opencv2/face.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include "ar_track_alvar_msgs/AlvarMarkers.h"
using namespace cv;
using namespace cv::face;
using namespace std;

bool spottedPerson = false;
double startTime;
double timeout = 20.0;

#define FROM    "<akaprojectstudios@gmail.com>"
#define TO      "<andrea.mastropietro@gmail.com>"

static const char *payload_text[] = {
  "To: " TO "\r\n",
  "From: " FROM "(CopRob Surveillance Robot)\r\n",
  "Subject: INTRUSION CONFIRMED\r\n",
  "The intrusion has been confirmed.\r\n",
  NULL
};

static const char *payload_text_false[] = {
  "To: " TO "\r\n",
  "From: " FROM "(CopRob Surveillance Robot)\r\n",
  "Subject: FALSE ALARM\r\n",
  "CopRob identified an authorized user.\r\n",
  "\r\n",
  "Surveillance system deactivated.\r\n",
  NULL
};

struct upload_status {
  int lines_read;
};

static size_t payload_source(void *ptr, size_t size, size_t nmemb, void *userp){
  struct upload_status *upload_ctx = (struct upload_status *)userp;
  const char *data;

  if((size == 0) || (nmemb == 0) || ((size*nmemb) < 1)) {
    return 0;
  }

  data = payload_text[upload_ctx->lines_read];

  if(data) {
    size_t len = strlen(data);
    memcpy(ptr, data, len);
    upload_ctx->lines_read++;

    return len;
  }

  return 0;
}

static size_t payload_source_false(void *ptr, size_t size, size_t nmemb, void *userp){
  struct upload_status *upload_ctx = (struct upload_status *)userp;
  const char *data;

  if((size == 0) || (nmemb == 0) || ((size*nmemb) < 1)) {
    return 0;
  }

  data = payload_text_false[upload_ctx->lines_read];

  if(data) {
    size_t len = strlen(data);
    memcpy(ptr, data, len);
    upload_ctx->lines_read++;

    return len;
  }

  return 0;
}

void checkPerson(const std_msgs::String::ConstPtr& msg){
	
	if ((spottedPerson == false) && !(msg->data).compare("spotted")) {
		spottedPerson = true;
		startTime = ros::Time::now().toSec();
	}
}

void badgeRec(const ar_track_alvar_msgs::AlvarMarkers::ConstPtr& msg) {
	
	if (spottedPerson == true){
		if (ros::Time::now().toSec() - startTime < timeout){
			if ((msg->markers).size() > 0){
				ROS_INFO("FALSE ALARM");
				
				CURL *curl;
		  CURLcode res = CURLE_OK;
		  struct curl_slist *recipients = NULL;
		  struct upload_status upload_ctx;

		  upload_ctx.lines_read = 0;

		  curl = curl_easy_init();
		  if(curl) {
			curl_easy_setopt(curl, CURLOPT_USERNAME, "akaprojectstudios@gmail.com");
			curl_easy_setopt(curl, CURLOPT_PASSWORD, "hady'slumia");
			curl_easy_setopt(curl, CURLOPT_URL, "smtps://smtp.gmail.com:465"); //smtp-mail.outlook.com
			curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
			curl_easy_setopt(curl, CURLOPT_MAIL_FROM, FROM);
			recipients = curl_slist_append(recipients, TO);
			curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
			curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source_false);
			curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
			curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
			curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

			res = curl_easy_perform(curl);

			if(res != CURLE_OK)
			  fprintf(stderr, "curl_easy_perform() failed: %s\n",
					  curl_easy_strerror(res));

			curl_slist_free_all(recipients);

			curl_easy_cleanup(curl);
			}
				ros::shutdown();
				
			}
			
		} else {
			ROS_INFO("INTRUSION CONFIRMED");
			
		  CURL *curl;
		  CURLcode res = CURLE_OK;
		  struct curl_slist *recipients = NULL;
		  struct upload_status upload_ctx;

		  upload_ctx.lines_read = 0;

		  curl = curl_easy_init();
		  if(curl) {
			curl_easy_setopt(curl, CURLOPT_USERNAME, "akaprojectstudios@gmail.com");
			curl_easy_setopt(curl, CURLOPT_PASSWORD, "hady'slumia");
			curl_easy_setopt(curl, CURLOPT_URL, "smtps://smtp.gmail.com:465"); //smtp-mail.outlook.com
			curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
			curl_easy_setopt(curl, CURLOPT_MAIL_FROM, FROM);
			recipients = curl_slist_append(recipients, TO);
			curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
			curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
			curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
			curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
			curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
			res = curl_easy_perform(curl);

			if(res != CURLE_OK)
			  fprintf(stderr, "curl_easy_perform() failed: %s\n",
					  curl_easy_strerror(res));

			curl_slist_free_all(recipients);

			curl_easy_cleanup(curl);
					
		}
		ros::shutdown();
	}
  }
   
}

int main(int argc, char **argv){

  ros::init(argc, argv, "badgeRecognition");

  ros::NodeHandle n;
  
  ros::Subscriber sub = n.subscribe<std_msgs::String>("spottedPerson", 1000, checkPerson);
  
  ros::Subscriber sub1 = n.subscribe<ar_track_alvar_msgs::AlvarMarkers>("ar_pose_marker", 1000, badgeRec);
  
  ros::spin();

  return 0;
  
}

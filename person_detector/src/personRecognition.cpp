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

#include "opencv2/core.hpp"
#include "opencv2/face.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>

using namespace cv;
using namespace cv::face;
using namespace std;

bool spottedPerson = false;
double startTime;
double timeout = 20;
Ptr<LBPHFaceRecognizer> model;
double lim = 10;

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

static void read_csv(const string& filename, vector<Mat>& images, vector<int>& labels, char separator = ';') {
        std::ifstream file(filename.c_str(), ifstream::in);
        if (!file) {
                string error_message = "No valid input file was given, please check the given filename.";
                CV_Error(Error::StsBadArg, error_message);
        }
        string line, path, classlabel;
        while (getline(file, line)) {
                stringstream liness(line);
                getline(liness, path, separator);
                getline(liness, classlabel);
                if(!path.empty() && !classlabel.empty()) {
                        images.push_back(imread(path, 0));
                        labels.push_back(atoi(classlabel.c_str()));
                }
        }
}


void checkPerson(const std_msgs::String::ConstPtr& msg){
        if ((spottedPerson == false) && (strcmp((msg->data).c_str(), "spotted") == 0)) {
                spottedPerson = true;
                startTime = ros::Time::now().toSec();
        }
}




void faceRec(const sensor_msgs::ImageConstPtr& msg) {

        if (spottedPerson == true) {
                if (ros::Time::now().toSec() - startTime < timeout) {

                        cv_bridge::CvImageConstPtr cv_ptr;

                        try {
                                cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
                        }
                        catch (cv_bridge::Exception& ex) {

                                ROS_ERROR("cv_bridge exception: %s", ex.what());
                                exit(EXIT_FAILURE);
                        }

                        Mat gray_image;

                        cvtColor(cv_ptr->image, gray_image, CV_BGR2GRAY);

                        int predictedLabel = -1;
                        double confidence = 0.0;
                        model->predict(gray_image, predictedLabel, confidence);

                        if (confidence < lim) {
                                string result_message = format("Predicted class = %d / confidence = %f", predictedLabel, confidence);
                                cout << result_message << endl;
                                ROS_INFO("USER RECOGNIZED. SYSTEM DEACTIVATED");
                                CURL *curl;
                                CURLcode res = CURLE_OK;
                                struct curl_slist *recipients = NULL;
                                struct upload_status upload_ctx;

                                upload_ctx.lines_read = 0;

                                curl = curl_easy_init();
                                if(curl) {
                                        /* Set username and password */
                                        curl_easy_setopt(curl, CURLOPT_USERNAME, "akaprojectstudios@gmail.com");
                                        curl_easy_setopt(curl, CURLOPT_PASSWORD, "hady'slumia");

                                        curl_easy_setopt(curl, CURLOPT_URL, "smtps://smtp.gmail.com:465"); //smtp-mail.outlook.com
                                        //curl_easy_setopt(curl, CURLOPT_URL, "smtps://smtp-mail.outlook.com:587"); //

                                        curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);


                                        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, FROM);

                                        /* Add two recipients, in this particular case they correspond to the
                                         * To: and Cc: addressees in the header, but they could be any kind of
                                         * recipient. */
                                        recipients = curl_slist_append(recipients, TO);
                                        //recipients = curl_slist_append(recipients, CC);
                                        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

                                        /* We're using a callback function to specify the payload (the headers and
                                         * body of the message). You could just use the CURLOPT_READDATA option to
                                         * specify a FILE pointer to read from. */
                                        curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source_false);
                                        curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
                                        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

                                        /* Since the traffic will be encrypted, it is very useful to turn on debug
                                         * information within libcurl to see what is happening during the transfer.
                                         */
                                        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

                                        /* Send the message */
                                        res = curl_easy_perform(curl);

                                        /* Check for errors */
                                        if(res != CURLE_OK)
                                                fprintf(stderr, "curl_easy_perform() failed: %s\n",
                                                        curl_easy_strerror(res));

                                        /* Free the list of recipients */
                                        curl_slist_free_all(recipients);

                                        /* Always cleanup */
                                        curl_easy_cleanup(curl);
                                }
                                ros::shutdown();
                        }
                        else {
                                string result_message = format("Not recognized, confidence  = % f", confidence);
                                cout << result_message << endl;
                                
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
                                        /* Set username and password */
                                        curl_easy_setopt(curl, CURLOPT_USERNAME, "akaprojectstudios@gmail.com");
                                        curl_easy_setopt(curl, CURLOPT_PASSWORD, "hady'slumia");

                                        curl_easy_setopt(curl, CURLOPT_URL, "smtps://smtp.gmail.com:465"); //smtp-mail.outlook.com
                                        //curl_easy_setopt(curl, CURLOPT_URL, "smtps://smtp-mail.outlook.com:587"); //

                                        curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);


                                        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, FROM);

                                        /* Add two recipients, in this particular case they correspond to the
                                         * To: and Cc: addressees in the header, but they could be any kind of
                                         * recipient. */
                                        recipients = curl_slist_append(recipients, TO);
                                        //recipients = curl_slist_append(recipients, CC);
                                        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

                                        /* We're using a callback function to specify the payload (the headers and
                                         * body of the message). You could just use the CURLOPT_READDATA option to
                                         * specify a FILE pointer to read from. */
                                        curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
                                        curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
                                        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

                                        /* Since the traffic will be encrypted, it is very useful to turn on debug
                                         * information within libcurl to see what is happening during the transfer.
                                         */
                                        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

                                        /* Send the message */
                                        res = curl_easy_perform(curl);

                                        /* Check for errors */
                                        if(res != CURLE_OK)
                                                fprintf(stderr, "curl_easy_perform() failed: %s\n",
                                                        curl_easy_strerror(res));

                                        /* Free the list of recipients */
                                        curl_slist_free_all(recipients);

                                        /* Always cleanup */
                                        curl_easy_cleanup(curl);

                                }
                        ros::shutdown();
                }
        }
}

int main(int argc, char **argv){

        string fn_csv = "/home/mastro/ros_packages_src/src/person_detector/config/images.csv";

        vector<Mat> images;
        vector<int> labels;

        try {
                read_csv(fn_csv, images, labels);
        } catch (cv::Exception& e) {
                cerr << "Error opening file \"" << fn_csv << "\". Reason: " << e.msg << endl;
            
                exit(1);
        }


        model = createLBPHFaceRecognizer();
        model->train(images, labels);

        ros::init(argc, argv, "personRecognition");

        ros::NodeHandle n;

        ros::Subscriber sub = n.subscribe<std_msgs::String>("spottedPerson", 1000, checkPerson);

        ros::Subscriber sub1 = n.subscribe<sensor_msgs::Image>("camera/image_raw", 1000, faceRec);

        ros::spin();

        return 0;
}

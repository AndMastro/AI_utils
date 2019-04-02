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
//#include <conio.h>
#include <curl/curl.h>
#ifdef _DEBUG
        #pragma comment(lib, "libcurl_a_debug.lib") //cURL Debug build
#else
        #pragma comment(lib, "libcurl_a.lib")
#endif

#pragma warning(disable: 4996)  //Disable Function or Variable may be unsafe warning

#define FROM    "<akaprojectstudios@gmail.com>"
#define TO      "<andrea.mastropietro@gmail.com>"
#define FILENAME "/home/mastro/shots/snapshot0.png"

static const int CHARS= 76;   //76  //Sending 54 chararcters at a time with \r , \n and \0 it becomes 57
static const int ADD_SIZE= 14;   // ADD_SIZE for TO,FROM,SUBJECT,CONTENT-TYPE,CONTENT-TRANSFER-ENCODING,CONETNT-DISPOSITION and \r\n
static const int SEND_BUF_SIZE= 54;
static char (*fileBuf)[CHARS] = NULL;
static const char cb64[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

using namespace std;


typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

MoveBaseClient* ac;
bool spottedLegs = 0;
bool spottedFace = 0;
int num = 0;


void checkLegs(const geometry_msgs::PoseArray::ConstPtr& msg){
  geometry_msgs::PoseArray spot = *msg;
  if (spottedLegs == false){
    if (spot.poses.size() > 0 && !std::isnan(spot.poses[0].position.x) && spot.poses[0].position.x <= 3.0){

  	  spottedLegs = true;

      ROS_INFO("LEGS DETECTED");


      //wait for the action server to come up
      /*while(!ac->waitForServer(ros::Duration(5.0))) {
              ROS_INFO("Waiting for the move_base action server to come up");
      }*/

      /*move_base_msgs::MoveBaseGoal stopGoal;
      stopGoal.target_pose.header.frame_id = "base_link";
      stopGoal.target_pose.header.stamp = ros::Time::now();
      stopGoal.target_pose.pose.position.x = 0;
      stopGoal.target_pose.pose.position.y = 0;
      stopGoal.target_pose.pose.orientation.w = 0;*/

      //(*ac).cancelAllGoals();


      //(ac->sendGoal)(stopGoal);
      //ac->waitForResult();

      //AGGIUNGERE CANCEL GOALS

  }
}



}

void checkFace(const std_msgs::Int32MultiArray::ConstPtr& msg)
{
  //ROS_INFO("I heard: [%d]", msg->data.c_str());
  std_msgs::Int32MultiArray facePos = *msg;

  if (spottedFace == false){
    if (facePos.data.size() == 10){
      spottedFace = true;
      ROS_INFO("FACE DETECTED");
    }


  }

  //ROS_INFO("I heard: ");
}

bool LARGEFILE = false; /*For Percent*/
int status = 0;   /*For Percent*/
int percent2 = 0; /*For Percent*/
int percent3 = 0; /*For Percent*/
int percent4 = 0; /*For Percent*/
int percent5 = 0; /*For Percent*/
void LargeFilePercent(int rowcount, string filename)
{
        //This is for displaying the percentage
        //while encoding larger files
        int percent = rowcount/100;

        if(LARGEFILE == true) {
                status++;
                percent2++;
                if(percent2 == 18) {
                        percent3++;
                        percent2 = 0;
                }
                if(percent3 == percent){
                        percent4++;
                        percent3 = 0;
                }
                if(percent4 == 10){
                        system("cls");
                        cout << "Larger Files take longer to encode, Please be patient." << endl
                             << "Otherwise push X to exit program now." << endl << endl
                             << "(Push Anykey to Continue)" << endl
                             << endl << "Encoding " << filename << " please be patient..." << endl;
                        cout << percent5 << "%";
                        percent5 += 10;
                        percent4 = 0;
                }
                if(status == 10000) {
                        if(percent5 == 0){cout << " 0%"; percent5 = 10;}
                        cout << ".";
                        status = 0;
                }
        }
}

void encodeblock(unsigned char in[3], unsigned char out[4], int len)
{
    out[0] = cb64[ in[0] >> 2 ];
    out[1] = cb64[ ((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4) ];
    out[2] = (unsigned char) (len > 1 ? cb64[ ((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6) ] : '=');
    out[3] = (unsigned char) (len > 2 ? cb64[ in[2] & 0x3f ] : '=');
}

void encode(FILE *infile, unsigned char *output_buf, int rowcount, string filename/*For Percent*/)
{
    unsigned char in[3], out[4];
        int i, len;
        *output_buf = 0;

    while(!feof(infile)) {
        len = 0;
        for(i = 0; i < 3; i++) {
            in[i] = (unsigned char) getc(infile);
            if(!feof(infile) ) {
                len++;
            }
            else {
                in[i] = 0;
            }
        }
        if(len) {
            encodeblock(in, out, len);
            strncat((char*)output_buf, (char*)out, 4);
        }
            LargeFilePercent(rowcount, filename); //Display encoded file percent /*For Percent*/
        }
}


struct fileBuf_upload_status
{
  int lines_read;
};

/*size_t*/size_t read_file()
{
        FILE* hFile=NULL;
        size_t fileSize(0),len(0),buffer_size(0);
        char key = ' ';

        //Open the file and make sure it exsits
        hFile = fopen(FILENAME,"rb");
        if(!hFile) {
                cout << "File not found!!!" << endl;
                getchar();
                exit (EXIT_FAILURE);
        }

        //Get filesize
        fseek(hFile,0,SEEK_END);
        fileSize = ftell(hFile);
        fseek(hFile,0,SEEK_SET);

        //Check Filesize, if larger then 256kb prompt user
        if(fileSize > 256000/*bytes*/){
                cout << "Larger Files take longer to encode, Please be patient." << endl
                     << "Otherwise push X to exit program now." << endl << endl
                     << "(Push Anykey to Continue)" << endl;
                //key = getchar();
                //if(tolower(key) == 'x')
                //        exit (EXIT_SUCCESS);
                LARGEFILE = true; /*For Percent*/
        }
        cout << endl << "Encoding " FILENAME " please be patient..." << endl;

        int no_of_rows = fileSize/SEND_BUF_SIZE + 1;
        int charsize = (no_of_rows*72)+(no_of_rows*2);
        unsigned char* b64encode = new unsigned char[charsize];
        *b64encode = 0;

        encode(hFile, b64encode, no_of_rows, FILENAME /*For Percent*/);
        string encoded_buf = (char*)b64encode;

        if(LARGEFILE == true) cout << endl << endl; /*For Percent*/
        ///PROVAAAA






        //Create structure of email to be sent
        fileBuf = new char[ADD_SIZE + no_of_rows][CHARS];  //ADD_SIZE for TO,FROM,SUBJECT,CONTENT-TYPE,CONTENT-TRANSFER-
                                                           //ENCODING,CONETNT-DISPOSITION and \r\n
         strcpy(fileBuf[len++],"To: " TO "\r\n");
         buffer_size += strlen(fileBuf[len-1]);
         strcpy(fileBuf[len++],"From: " FROM "(Home-Intrusion Detection Robot)\r\n");
         buffer_size += strlen(fileBuf[len-1]);
         strcpy(fileBuf[len++],"Subject: POSSIBLE INTRUSION DETECTED\r\n");
         buffer_size += strlen(fileBuf[len-1]);
         strcpy(fileBuf[len++],"Content-Type: application/x-msdownload; name=\"" "Foto1" " \"\r\n"); //image/jpeg
         buffer_size += strlen(fileBuf[len-1]);
         strcpy(fileBuf[len++],"Content-Transfer-Encoding: base64\r\n");
         buffer_size += strlen(fileBuf[len-1]);
         strcpy(fileBuf[len++],"Content-Disposition: attachment; filename=\"" "/home/mastro/shots/snapshot0.png" "\"\r\n");
         buffer_size += strlen(fileBuf[len-1]);
         strcpy(fileBuf[len++],"\r\n");
         buffer_size += strlen(fileBuf[len-1]);
         /*strcpy(fileBuf[len++],"Content-Type: text/plain; charset=utf-8; format=flowed\r\n");
         buffer_size += strlen(fileBuf[len-1]);
         strcpy(fileBuf[len++],"Prova body\r\n");
         buffer_size += strlen(fileBuf[len-1]);*/
         strcpy(fileBuf[len++],"\r\n");
         buffer_size += strlen(fileBuf[len-1]);

        //This part attaches the Base64 encoded string and
        //sets the Base64 linesize to 72 characters + \r\n
        int pos = 0;
        string sub_encoded_buf;
        printf("Prova1\n");
        for(int i = 0; i <= no_of_rows-1; i++)
        {
                sub_encoded_buf = encoded_buf.substr(pos*72,72);  //Reads 72 characters at a time
                sub_encoded_buf += "\r\n";                        //and appends \r\n at the end
                strcpy(fileBuf[len++], sub_encoded_buf.c_str());  //copy the 72 characters & \r\n to email
                buffer_size += sub_encoded_buf.size();            //now increase the buffer_size
                pos++;                                            //finally increase pos by 1
        }


//////////////////////////////////////////////////////////////////////
// The following code prints the structure of the email being sent.//
//    printf("Images Base64 encoded string:\n\n\t\t\t std::string" //
//         "encoded_buf\n%s\n",encoded_buf.c_str());               //
//   printf("\n\t\t\tStructure of Email being sent:\n");          //
//     for (int i=0;i<no_of_rows + ADD_SIZE;++i){                 //
//           printf("%s",fileBuf[i]); }                           //
//     printf("\nbuffer size: %d\n",buffer_size);                 //
/////////////////////////////////////////////////////////////////////
//    /* Push X to Exit the program after viewing                  //
//       the structure of the email */                             //
//    printf("Push X to exit program now, otherwise continue\n");  //
//       key = getchar();                                           //
//       if(tolower(key) == 'x')                                   //
//              exit (EXIT_SUCCESS);                              //
/////////////////////////////////////////////////////////////////////

        delete[] b64encode;
        printf("Prova4\n");
        //buffer_size = 0;
        printf("\nbuffer size: %d\n",buffer_size);
        return buffer_size;
        //return 1;
}

 /*
  The fileBuf_source() is a function which CURL calls when it need to obtain data that will be uploaded to the server.
  Imagine that fileBuf_source() is something similar to fread(). When called it performs any voodoo-mumbo-jumbo that is needed,
  but in the end uploadable data must be stored in *ptr buffer, which is curl's internal buffer. For your in-memory buffer
  memcpy() will do just fine as body of fileBuf_source(), so you don't need real fread() at all.

  size * nmemb tells you how big buffer curl has reserved for a single chunk of data. The last void* is a pointer which was
  set by CURLOPT_READDATA option - it's a do-what-ever-you-need-with-it kind of pointer, so it can point to a structure
  containing data which you're uploading and a some additional info e.g. current progress.
  */
static size_t fileBuf_source(void *ptr, size_t size, size_t nmemb, void *userp)
{
        struct fileBuf_upload_status *upload_ctx = (struct fileBuf_upload_status *)userp;
        const char *fdata;

        if((size == 0) || (nmemb == 0) || ((size*nmemb) < 1))
        {
                return 0;
        }

        fdata = fileBuf[upload_ctx->lines_read];

        if(strcmp(fdata,""))
        {
                size_t len = strlen(fdata);
                memcpy(ptr, fdata, len);
                upload_ctx->lines_read++;
                return len;
        }
        return 0;
}

void takeSnapshot(const sensor_msgs::ImageConstPtr& msg){


	if (spottedLegs == true && spottedFace == true){
    ROS_INFO("TUTTO TRUE");
		cv_bridge::CvImageConstPtr cv_ptr;

		try {
			cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
		}
		catch (cv_bridge::Exception& ex){

			ROS_ERROR("cv_bridge exception: %s", ex.what());
			exit(EXIT_FAILURE);
		}
		
		ROS_INFO("INTRUSION DETECTED");
		std::stringstream ss;
		ss<<"/home/mastro/shots/snapshot"<<num<<".png";
		cv::imwrite(ss.str(),cv_ptr->image);
		num++;
		
		ROS_INFO("A snaphsot has been saved.");
		
	//wait for the action server to come up
      while(!ac->waitForServer(ros::Duration(5.0))) {
              ROS_INFO("Waiting for the move_base action server to come up");
      }

     /* move_base_msgs::MoveBaseGoal stopGoal;
      stopGoal.target_pose.header.frame_id = "base_link";
      stopGoal.target_pose.header.stamp = ros::Time::now();
      stopGoal.target_pose.pose.position.x = 0;
      stopGoal.target_pose.pose.position.y = 0;
      stopGoal.target_pose.pose.orientation.w = 1;*/

      


      (ac->sendGoal)(stopGoal);
       ac->waitForResult();
      (*ac).cancelAllGoals();
      
    printf("ProvaA\n");
    CURL *curl;
    CURLcode res = CURLE_OK;
    struct curl_slist *recipients = NULL;
    struct fileBuf_upload_status file_upload_ctx;
    size_t file_size(0);

    file_upload_ctx.lines_read = 0;

    curl = curl_easy_init();
    printf("ProvaA1\n");
    file_size = read_file();
    //file_size = 1355008;
    printf("ProvaA2\n");
    if(curl)
    {
      curl_easy_setopt(curl, CURLOPT_USERNAME, "akaprojectstudios@gmail.com");
      curl_easy_setopt(curl, CURLOPT_PASSWORD, "hady'slumia");
      curl_easy_setopt(curl, CURLOPT_URL, "smtp://smtp.gmail.com:587");
      curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
      //curl_easy_setopt(curl, CURLOPT_CAINFO, "google.pem");
      curl_easy_setopt(curl, CURLOPT_MAIL_FROM, FROM);
      recipients = curl_slist_append(recipients, TO);
      curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
      curl_easy_setopt(curl, CURLOPT_INFILESIZE, file_size);
      curl_easy_setopt(curl, CURLOPT_READFUNCTION, fileBuf_source);
    //  curl_easy_setopt(curl, CURLOPT_READFUNCTION, fileBuf_source);
      curl_easy_setopt(curl, CURLOPT_READDATA, &file_upload_ctx);
      curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
      curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); //Dont display Curl Connection data Change 1L to 0

      res = curl_easy_perform(curl);

      if(res != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
      curl_slist_free_all(recipients);
      curl_easy_cleanup(curl);
    }
    printf("ProvaB\n");
    printf("%d\n", file_upload_ctx.lines_read);
    delete[] fileBuf;
    //getchar();
    //return (int)res;
    printf("ProvaC\n");

		spottedFace = false;
		spottedLegs = false;
	   //cv::waitKey(30); //!!!!!!
	   
	
    ros::shutdown();
	}
}







int main(int argc, char **argv){

  ros::init(argc, argv, "spotPerson");

  ros::NodeHandle n;

  //ros::Publisher pub;

  //pub = n.advertise<std_msgs::String>("minDist", 1000);

  ac = new MoveBaseClient("move_base", true);

  ros::Subscriber sub = n.subscribe<geometry_msgs::PoseArray>("edge_leg_detector", 1000, checkLegs);

  ros::Subscriber sub1 = n.subscribe<sensor_msgs::Image>("camera/image_raw", 1000, takeSnapshot);

  ros::Subscriber sub2 = n.subscribe<std_msgs::Int32MultiArray>("faceCoord", 1000, checkFace);

  ros::spin();

  return 0;
}

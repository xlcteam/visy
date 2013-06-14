#include "cv.h"
#include "highgui.h"
#include "cxcore.h"

#include <visy/visy.h>

int Hmax = 30,Smax = 255,Vmax = 255,Hmin = 20,Smin = 100,Vmin = 100;
//int minH = 50;
IplImage *img, *imgHSV, *imgThreshed;

CvFileStorage* fs;


// stores profile name
char* profile_name;


void onTrack(int a)
{

	visy_img_filter(&img, cvScalar(Hmin, Smin, Vmin, 0),
			cvScalar(Hmax, Smax, Vmax, 0), &imgThreshed);



	fs = cvOpenFileStorage(profile_name, 0, CV_STORAGE_WRITE, NULL);
	cvWriteInt(fs, "Hmax", Hmax);
	cvWriteInt(fs, "Smax", Smax);
	cvWriteInt(fs, "Vmax", Vmax);
	cvWriteInt(fs, "Hmin", Hmin);
	cvWriteInt(fs, "Smin", Smin);
	cvWriteInt(fs, "Vmin", Vmin);
	//cvWriteInt(fs, "minH", minH);

	cvReleaseFileStorage(&fs);

	unsigned int *graph;
	unsigned int average, mode;
	unsigned int lsum, rsum;

	graph = (unsigned int *) malloc(img->width * sizeof(unsigned int));
	visy_img_graph(&imgThreshed, &graph, &lsum, &rsum, &average, &mode);


	printf("%d %d %d %d\n", lsum, rsum, average, mode);

	unsigned int first, last;
	IplImage *colored;
	colored = cvCreateImage(cvGetSize(img), 8, 3);

	IplImage *imgGraph;
	imgGraph = cvCreateImage(cvGetSize(img), 8, 3);
	cvCopyImage(img, colored);

	visy_graph_first_last(&graph, mode, &first, &last, &colored);
	visy_graph_draw(&graph, average, mode, &imgGraph);

	float pix = 12.0/average;
	printf("pix: %f\n", pix);

	int goal_center = (last+first) / 2;
	float X = ((img->width/2) - goal_center)*pix;
	printf("X: %fcm\n", X);

        float Y = (100 * 52) / (mode);
        printf("Y: %fcm\n", Y);


        printf("Action: ");
	/* decide wether to kick or move to the left or to the right */
	if (visy_graph_center_space(&graph, img->width,
				cvSize(10, 10), pix)){
		printf("K\n");
	} else {
		printf("%s\n", ((lsum > rsum) ? "L" : "R"));
	}



	cvShowImage("origin", colored);
	cvShowImage("treshed", imgThreshed);
	cvShowImage("graph", imgGraph);
	cvReleaseImage(&imgGraph);
	cvReleaseImage(&colored);

//	printf("test %d | %d | %d -> %d | %d | %d\n",Hmin,Smin,Vmin,Hmax,Smax,Vmax);
}


int main(int argc, char** argv)
{
	if (argc == 1) {
		printf("Usage: %s <input> <config>\n", argv[0]);
		return 0;
	}

	char* filename = (argc > 1 ? argv[1] : (char*)"input.jpg");

	profile_name = (argc > 2 ? argv[2] : (char*)"blue_goal.yml");


	fs = cvOpenFileStorage(profile_name, 0, CV_STORAGE_READ, NULL);
	Hmax = cvReadIntByName(fs, NULL, "Hmax", Hmax);
	Smax = cvReadIntByName(fs, NULL, "Smax", Smax);
	Vmax = cvReadIntByName(fs, NULL, "Vmax", Vmax);
	Hmin = cvReadIntByName(fs, NULL, "Hmin", Hmin);
	Smin = cvReadIntByName(fs, NULL, "Smin", Smin);
	Vmin = cvReadIntByName(fs, NULL, "Vmin", Vmin);
	//minH = cvReadIntByName(fs, NULL, "minH", minH);

	printf("%s\n", filename);
	img = cvLoadImage(filename, 1);

	visy_img_alloc(&img, cvGetSize(img), 8, 1);
	imgThreshed = cvCreateImage(cvGetSize(img), 8, 1);

	//cvSmooth( imgHSV, imgHSV, CV_GAUSSIAN, 7, 7 );
	//cvInRangeS(imgHSV, cvScalar(20, 100, 100), cvScalar(30, 255, 255), imgThreshed);

	//cvInRangeS(imgHSV, cvScalar(100, 100, 100), cvScalar(120, 255, 255), imgThreshed);

	//cvInRangeS(imgHSV, cvScalar(0.11*256, 0.60*256, 0.20*256, 0),
	//                cvScalar(0.14*256, 1.00*256, 1.00*256, 0), imgThreshed);

	cvNamedWindow("origin", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("treshed", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("graph", CV_WINDOW_AUTOSIZE);


	cvCreateTrackbar("Hmin", "treshed", &Hmin, 180, onTrack);
	cvCreateTrackbar("Smin", "treshed", &Smin, 255, onTrack);
	cvCreateTrackbar("Vmin", "treshed", &Vmin, 255, onTrack);

	cvCreateTrackbar("Hmax", "treshed", &Hmax, 180, onTrack);
	cvCreateTrackbar("Smax", "treshed", &Smax, 255, onTrack);
	cvCreateTrackbar("Vmax", "treshed", &Vmax, 255, onTrack);
	//cvCreateTrackbar("minH", "treshed", &minH, 255, onTrack);


	onTrack(0);

	//cvSaveImage("hsv.jpg", imgHSV);

	while(cvWaitKey(0) != 97);

	return 0;
}


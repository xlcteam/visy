#include <cv.h>
#include <highgui.h>
#include <cxcore.h>

#include <stdio.h>
#include <stdlib.h>

#include <visy/visy.h>

int Hmax = 30,Smax = 255,Vmax = 255,Hmin = 20,Smin = 100,Vmin = 100;
int minH = 50;
IplImage *img, *imgThreshed;
IplImage *imgGraph;

CvFileStorage* fs;



// stores profile name
char* profile_name;

void onTrack(int a)
{

	fs = cvOpenFileStorage(profile_name, 0, CV_STORAGE_WRITE, NULL);
	cvWriteInt(fs, "Hmax", Hmax);
	cvWriteInt(fs, "Smax", Smax);
	cvWriteInt(fs, "Vmax", Vmax);
	cvWriteInt(fs, "Hmin", Hmin);
	cvWriteInt(fs, "Smin", Smin);
	cvWriteInt(fs, "Vmin", Vmin);

	cvReleaseFileStorage(&fs);


//	printf("test %d | %d | %d -> %d | %d | %d\n",Hmin,Smin,Vmin,Hmax,Smax,Vmax);
}

time_t tstart;

int main(int argc, char** argv)
{

	profile_name = (argc > 1 ? argv[1] : (char*)"blue_goal.yml");

	int cam = (argc > 2 ? atoi(argv[2]) : 0);

	// value loading
	fs = cvOpenFileStorage(profile_name, 0, CV_STORAGE_READ, NULL);
	Hmax = cvReadIntByName(fs, NULL, "Hmax", Hmax);
	Smax = cvReadIntByName(fs, NULL, "Smax", Smax);
	Vmax = cvReadIntByName(fs, NULL, "Vmax", Vmax);
	Hmin = cvReadIntByName(fs, NULL, "Hmin", Hmin);
	Smin = cvReadIntByName(fs, NULL, "Smin", Smin);
	Vmin = cvReadIntByName(fs, NULL, "Vmin", Vmin);
	minH = cvReadIntByName(fs, NULL, "minH", minH);


	cvNamedWindow("img", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("treshed", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("graph", CV_WINDOW_AUTOSIZE);


	cvCreateTrackbar("Hmin", "treshed", &Hmin, 255, onTrack);
	cvCreateTrackbar("Smin", "treshed", &Smin, 255, onTrack);
	cvCreateTrackbar("Vmin", "treshed", &Vmin, 255, onTrack);

	cvCreateTrackbar("Hmax", "treshed", &Hmax, 255, onTrack);
	cvCreateTrackbar("Smax", "treshed", &Smax, 255, onTrack);
	cvCreateTrackbar("Vmax", "treshed", &Vmax, 255, onTrack);


	onTrack(0);

	CvCapture* camera = cvCaptureFromCAM(cam);
	cvSetCaptureProperty( camera, CV_CAP_PROP_FRAME_WIDTH, 640 );
	cvSetCaptureProperty( camera, CV_CAP_PROP_FRAME_HEIGHT, 480 );


	unsigned int average = 0, mode = 0;
	unsigned int lsum = 0, rsum = 0;


	while(1) {
		img = cvQueryFrame(camera);
		if (img == NULL)
			continue;

		visy_img_filter(&img, cvScalar(Hmin, Smin, Vmin, 0),
				cvScalar(Hmax, Smax, Vmax, 0), &imgThreshed);

		unsigned int *graph;
		average = lsum = rsum = 0;

		graph = (unsigned int *) malloc(img->width * sizeof(unsigned int));
		visy_img_graph(&imgThreshed, &graph, &lsum, &rsum, &average, &mode);

		int x = 0;

		/*
		for(x = 0;x<img->width;x++){
			printf("%d %d \n", x, graph[x]);
		}
		*/

		printf("%d %d %d %d\n", lsum, rsum, average, mode);

		visy_img_alloc(&imgGraph, cvGetSize(img), 8, 3);
		cvSet(imgGraph, cvScalar(0, 0, 0, 0), NULL);

		visy_graph_draw(&graph, average, mode, &imgGraph);

		float pix = 12.0/average;
		//printf("pix: %f\n", pix);

		unsigned int first, last;
		visy_graph_first_last(&graph, average, &first, &last, &img);


		int goal_center = (last+first) / 2;
		float X = ((img->width/2) - goal_center)*pix;
		printf("X: %fcm\n", X);

                float Y = (100 * 52) / (mode);

                printf("Y: %fcm\n", Y);


		/* decide wether to kick or move to the left or to the right */
		if (visy_graph_center_space(&graph, img->width,
					cvSize(10, 10), pix)){
			printf("K\n");
		} else {
			printf("%s\n", ((lsum > rsum) ? "L" : "R"));
		}

		cvShowImage("img", img);
		cvShowImage("treshed", imgThreshed);
		cvShowImage("graph", imgGraph);

		cvWaitKey(10);
		// releases memory of the graph

		free(graph);
	}


}


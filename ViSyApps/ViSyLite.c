#include <cv.h>
#include <highgui.h>
#include <cxcore.h>
#include <stdio.h>

int Hmax = 30,Smax = 255,Vmax = 255,Hmin = 20,Smin = 100,Vmin = 100;
int minH = 50;
IplImage *img, *imgHSV, *imgThreshed;

CvFileStorage* fs;


// stores profile name
char* profile_name;

inline static void allocateCvImage(IplImage **img, CvSize size,
					int depth, int channels)
{
	if(*img != NULL)
		return;

	*img  = cvCreateImage(size, depth, channels);

	if(*img == NULL){
		fprintf(stderr,"Out of memory!\n");
		exit(-1);
	}


}

void onTrack(int a)
{

	fs = cvOpenFileStorage(profile_name, 0, CV_STORAGE_WRITE, NULL);
	cvWriteInt(fs, "Hmax", Hmax);
	cvWriteInt(fs, "Smax", Smax);
	cvWriteInt(fs, "Vmax", Vmax);
	cvWriteInt(fs, "Hmin", Hmin);
	cvWriteInt(fs, "Smin", Smin);
	cvWriteInt(fs, "Vmin", Vmin);
	cvWriteInt(fs, "minH", minH);

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


	cvCreateTrackbar("Hmin", "treshed", &Hmin, 360, onTrack);
	cvCreateTrackbar("Smin", "treshed", &Smin, 255, onTrack);
	cvCreateTrackbar("Vmin", "treshed", &Vmin, 255, onTrack);

	cvCreateTrackbar("Hmax", "treshed", &Hmax, 360, onTrack);
	cvCreateTrackbar("Smax", "treshed", &Smax, 255, onTrack);
	cvCreateTrackbar("Vmax", "treshed", &Vmax, 255, onTrack);
	cvCreateTrackbar("minH", "treshed", &minH, 255, onTrack);


	onTrack(0);

	CvCapture* camera = cvCaptureFromCAM(cam);


	while(1){
		img = cvQueryFrame(camera);

		allocateCvImage(&imgHSV, cvGetSize(img), 8, 3);
		cvCvtColor(img, imgHSV, CV_BGR2HSV);

		allocateCvImage(&imgThreshed, cvGetSize(img), 8, 1);
		cvInRangeS(imgHSV, cvScalar(Hmin, Smin, Vmin, 0), cvScalar(Hmax,
			Smax, Vmax, 0), imgThreshed);

		cvErode(imgThreshed, imgThreshed, 0, 2);


		int width = imgThreshed->width;
		int height = imgThreshed->height;
		int nchannels = imgThreshed->nChannels;
		int step = imgThreshed->widthStep;

		uchar* data = (uchar *)imgThreshed->imageData;
		unsigned int graph[width];

		int x,y;
		for(x = 0; x < width ; x++)
			graph[x] = 0;

		int sum = 0, notnull = 0;
		for(x = 0; x < width; x++){
			for( y = 0 ; y < height ; y++ ) {
				if(data[y*step + x*nchannels] == 255){
					graph[x]++;
				}
			}
			sum += graph[x];
			if(graph[x] != 0)
				notnull += 1;

	//		printf("%d\t%d\n", x, graph[x]);
		}
		if(notnull == 0)
			notnull = 1;
		int average = sum/notnull;
		if(average == 0)
			average = 1;
		float pix = 12.0/average;

		printf("\n sum: %d average: %d\n",sum,average);


		int first = 0, last = 0;
		// looking for goal
		for(x = 0;x < width; x++){
			if(graph[x] >= average && graph[x-1] < average){
				cvLine(img, cvPoint(x, 0), cvPoint(x, height),
					cvScalar(255, 255, 0, 0), 1, 0, 0);
				if(first == 0)
					first = x;
			}
			if(graph[x] >= average && graph[x+1] < average){
				cvLine(img, cvPoint(x, 0), cvPoint(x, height),
					cvScalar(255, 255, 0, 0), 1, 0, 0);
				last = x;
			}
		}

		float goal = pix*(last-first);
		float error = (goal-60.0)/60.0*100.0;

		printf("Pix: %f; Goal in cm: %f; Error: %f%\n", pix, goal, error);

		// image center
		cvLine(img, cvPoint(width/2, 0), cvPoint(width/2, height),
			cvScalar(0, 255, 255, 0), 1, 0, 0);

		int gCenter = (last+first) / 2;

		// X
		float X = ((width/2) - gCenter)*pix;

		printf("X: %f +- %f\n",X, abs(error)/100.0*X);

		// goal center
		cvLine(img,cvPoint(gCenter, 0),cvPoint(gCenter, height),
			cvScalar(0, 0, 255, 0), 1, 0, 0);



		cvShowImage("img", img);
		cvShowImage("treshed", imgThreshed);
		//cvShowImage("graph", imgGraph);

		cvWaitKey(10);
	}


}


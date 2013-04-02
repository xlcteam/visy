#include <cv.h>
#include <highgui.h>
#include <cxcore.h>

#include <stdio.h>


// allocates memory for a new image (good for memory leaks)
inline void visy_img_alloc(IplImage **img, CvSize size,
					int depth, int channels)
{
	if (*img != NULL)
		return;

	*img  = cvCreateImage(size, depth, channels);

	if (*img == NULL) {
		fprintf(stderr,"Out of memory!\n");
		exit(-1);
	}
}

IplImage *hsv;

inline void visy_img_filter(IplImage **img, CvScalar hsv_min,
					CvScalar hsv_max, IplImage **out)
{
	visy_img_alloc(&hsv, cvGetSize(*img), 8, 3);
	cvCvtColor(*img, hsv, CV_BGR2HSV);

	visy_img_alloc(&(*out), cvGetSize(*img), 8, 1);
	cvInRangeS(hsv, hsv_min, hsv_max, *out);

	cvErode(*out, *out, 0, 1);

}

int cmp(const void * a, const void * b)
{
  return ( *(int*)a - *(int*)b );
}

inline void visy_img_graph(IplImage **img, unsigned int *graph[],
				unsigned int *lsum, unsigned int *rsum,
				unsigned int *avg, unsigned int *mode)
{

	int width = (*img)->width;
	int height = (*img)->height;
	int nchannels = (*img)->nChannels;
	int step = (*img)->widthStep;

	uchar* data = (uchar *)(*img)->imageData;

	int x,y;
	*lsum = *rsum = 0;
	int notnull = 1;

        int array[width];
	for(x = 0; x < width; x++){
		(*graph)[x] = 0;
		for( y = 0 ; y < height ; y++ ) {
			if(data[y*step + x*nchannels] == 255){
				(*graph)[x]++;
			}
		}

		if(x < (width/2))
			*lsum += (*graph)[x];
		else
			*rsum += (*graph)[x];

		if((*graph)[x] != 0)
			notnull += 1;

                array[x] = (*graph)[x];

/*		printf("%d\t%d\n", x, (*graph)[x]); */
	}

        qsort(array, width, sizeof(int), cmp);
        int max_streak = 0;
        *mode = -1;
        int streak = 0;
        for(x = 1; x < width; x++) {
                if (array[x] == array[x-1]) {
                        streak++;
                } else {
                        if (streak > max_streak && array[x-1] != 0) {
                                max_streak = streak;
                                *mode = array[x-1];
                        }
                        streak = 0;
                }
        }
	*avg = (int) (*lsum+*rsum)/notnull;

}




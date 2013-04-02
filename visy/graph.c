#include <cv.h>
#include <highgui.h>
#include <cxcore.h>

#include <stdio.h>


inline void visy_graph_first_last(unsigned int **graph, unsigned int avg,
					unsigned int *first, unsigned int *last,
					IplImage **img)
{

	int width = (*img)->width;
	int height = (*img)->height;

	*first = *last = 0;
	int x;

	for(x = 0;x < width; x++){
		if((*graph)[x] >= avg && (*graph)[x-1] < avg){
			cvLine(*img, cvPoint(x, 0), cvPoint(x, height),
				cvScalar(255, 255 , 0, 0), 1, 8, 0);
			if(*first == 0)
				*first = x;
		}
		if((*graph)[x] >= avg && (*graph)[x+1] < avg){
			cvLine(*img, cvPoint(x, 0), cvPoint(x, height),
				cvScalar(255, 255, 0, 0), 1, 8, 0);
			*last = x;
		}
	}


}


inline void visy_graph_draw(unsigned int **graph, unsigned int avg,
				unsigned int mode, IplImage **img)
{

	int width = (*img)->width;
	int height = (*img)->height;

	int x = 0;
	for(x = 0; x < width; x++){
		cvLine(*img, cvPoint(x, height-(*graph)[x]), cvPoint(x, height),
			cvScalar(255, 255, 255, 0), 1, 8, 0);
	}

	cvLine(*img, cvPoint(0, height-avg), cvPoint(width, height-avg),
		cvScalar(255, 0, 0, 0), 1, 8, 0);

////////cvLine(*img, cvPoint(0, height-mode), cvPoint(width, height-mode),
////////	cvScalar(0, 255, 0, 0), 1, 8, 0);

}

int visy_graph_center_space(unsigned int **graph, unsigned int width,
				CvSize size, float pix)
{
	int height = size.height * pix;
	int i = (width/2.0) - (size.width/pix);

	for(; i < (int)((width/2.0) + (size.width/pix)); i++){
		if((*graph)[i] < height){
			return 0;
		}
	}

	return 1;

}


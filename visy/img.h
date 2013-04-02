#ifndef _IMG_H
#define _IMG_H

#include <cv.h>
#include <highgui.h>
#include <cxcore.h>

#include <stdio.h>

inline void visy_img_alloc(IplImage **img, CvSize size,
					int depth, int channels);

inline void visy_img_filter(IplImage **img, CvScalar hsv_min,
					CvScalar hsv_max, IplImage **out);

inline void visy_img_graph(IplImage **img, unsigned int *graph[],
				unsigned int *lsum, unsigned int *rsum,
				unsigned int *avg, unsigned int *mode);
#endif


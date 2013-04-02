#ifndef _GRAPH_H
#define _GRAPH_H

inline void visy_graph_first_last(unsigned int **graph, unsigned int avg,
					unsigned int *first,unsigned int *last,
					IplImage **img);

inline void visy_graph_draw (unsigned int **graph, unsigned int avg,
				unsigned int mode, IplImage **img);

int visy_graph_center_space(unsigned int **graph, unsigned int width,
				CvSize size, float pix);


#endif

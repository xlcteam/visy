#include <gtk/gtk.h>
#include <cv.h>
#include <highgui.h>


IplImage *imgTemp;
IplImage *hsv;
IplImage *threshed;
IplImage *threshedRGB;

// converts Opencv IplImage to GTK pixbuf
GdkPixbuf *convertOpenCv2Gtk(IplImage *image)
{
        cvCvtColor(image, image, CV_BGR2RGB);
        return gdk_pixbuf_new_from_data((guchar*)image->imageData,
                              GDK_COLORSPACE_RGB,
                              FALSE,image->depth,
                              image->width,image->height,(image->widthStep),NULL,NULL);
}
/*
GdkPixbuf *convertOpenCv2Gtk(IplImage *image)
{
  	cvCvtColor(image, image, CV_BGR2RGB);
	GdkPixbuf *pix;
	pix=gdk_pixbuf_new_from_data((guchar*)image->imageData,
                              GDK_COLORSPACE_RGB,
                              FALSE,image->depth,
                              image->width,image->height,(image->widthStep),NULL,NULL);
	return pix;
}
*/


/*
static gboolean heart(GtkWidget *window)
{
	gtk_image_set_from_pixbuf(GTK_IMAGE(video), convertOpenCv2Gtk(imgTemp));
}
*/

GtkBuilder *builder;
GError *error = NULL;


GtkScale *wHmin, *wSmin, *wVmin, *wHmax, *wSmax, *wVmax;
int Hmax = 30,Smax = 255,Vmax = 255,Hmin = 20,Smin = 100,Vmin = 100;

inline void load_values()
{
	Hmin = gtk_range_get_value(GTK_RANGE(gtk_builder_get_object(builder, "Hmin")));
	Smin = gtk_range_get_value(GTK_RANGE(gtk_builder_get_object(builder, "Smin")));
	Vmin = gtk_range_get_value(GTK_RANGE(gtk_builder_get_object(builder, "Vmin")));
	Hmax = gtk_range_get_value(GTK_RANGE(gtk_builder_get_object(builder, "Hmax")));
	Smax = gtk_range_get_value(GTK_RANGE(gtk_builder_get_object(builder, "Smax")));
	Vmax = gtk_range_get_value(GTK_RANGE(gtk_builder_get_object(builder, "Vmax")));
}


CvFileStorage* fs;
char* profile_name;

inline void config_file_load()
{
	fs = cvOpenFileStorage(profile_name, 0, CV_STORAGE_READ);
	Hmax = cvReadIntByName(fs, NULL, "Hmax", Hmax);
	Smax = cvReadIntByName(fs, NULL, "Smax", Smax);
	Vmax = cvReadIntByName(fs, NULL, "Vmax", Vmax);
	Hmin = cvReadIntByName(fs, NULL, "Hmin", Hmin);
	Smin = cvReadIntByName(fs, NULL, "Smin", Smin);
	Vmin = cvReadIntByName(fs, NULL, "Vmin", Vmin);

	cvReleaseFileStorage(&fs);	
	
	printf("load config\n");
	printf("%d %d %d %d %d %d \n",Hmin, Smin, Vmin, Hmax, Smax, Vmax);

	wHmax = GTK_SCALE(gtk_builder_get_object(builder, "Hmax"));
	
	gtk_range_set_value(GTK_RANGE(wHmax), Hmax);

	gtk_range_set_value((GtkRange*)gtk_builder_get_object(builder, "Hmin"), Hmin);
	gtk_range_set_value(GTK_RANGE(gtk_builder_get_object(builder, "Smin")), Smin);
	gtk_range_set_value(GTK_RANGE(gtk_builder_get_object(builder, "Vmin")), Vmin);
	gtk_range_set_value(GTK_RANGE(gtk_builder_get_object(builder, "Hmax")), Hmax);
	gtk_range_set_value(GTK_RANGE(gtk_builder_get_object(builder, "Smax")), Smax);
	gtk_range_set_value(GTK_RANGE(gtk_builder_get_object(builder, "Vmax")), Vmax);


}

inline void config_file_save()
{
	fs = cvOpenFileStorage(profile_name, 0, CV_STORAGE_WRITE);
	cvWriteInt(fs, "Hmax", Hmax);
	cvWriteInt(fs, "Smax", Smax);
	cvWriteInt(fs, "Vmax", Vmax);
	cvWriteInt(fs, "Hmin", Hmin);
	cvWriteInt(fs, "Smin", Smin);
	cvWriteInt(fs, "Vmin", Vmin);

	cvReleaseFileStorage(&fs);	
}

char* profile_yellow;
char* profile_blue;

GtkWidget *dialog;

void on_yellow_profile_clicked(GtkWidget *widget, gpointer data)
{

	dialog = gtk_file_chooser_dialog_new("Choose Yellow profile", 
						GTK_WINDOW(widget),
						GTK_FILE_CHOOSER_ACTION_OPEN,
						GTK_STOCK_CANCEL,
						GTK_RESPONSE_CANCEL,
						GTK_STOCK_OPEN,
						GTK_RESPONSE_ACCEPT, NULL);

	if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT){
		profile_yellow = (char *)gtk_file_chooser_get_filename(
					GTK_FILE_CHOOSER(dialog));

		printf("Yellow: %s\n", profile_yellow);
	}

	gtk_widget_destroy(dialog);
	
}

void on_blue_profile_clicked(GtkWidget *widget, gpointer data)
{
	dialog = gtk_file_chooser_dialog_new("Choose Blue profile", 
						GTK_WINDOW(widget),
						GTK_FILE_CHOOSER_ACTION_OPEN,
						GTK_STOCK_CANCEL,
						GTK_RESPONSE_CANCEL,
						GTK_STOCK_OPEN,
						GTK_RESPONSE_ACCEPT, NULL);

	if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT){
		profile_blue = (char *)gtk_file_chooser_get_filename(
					GTK_FILE_CHOOSER(dialog));

		printf("Blue: %s\n", profile_blue);
	}

	gtk_widget_destroy(dialog);
	
}

void on_load_click(GtkWidget *widget, gpointer data)
{

	config_file_load();

}


CvCapture* camera;

GtkImage *image1, *image2;
GtkWidget *button_blue, *button_yellow;

static gboolean
settings_loader(GtkWidget *widget)
{
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button_blue))){
		profile_name = profile_blue;
	}else{
		profile_name = profile_yellow;
	}
	config_file_save();
	
	load_values();
	//printf("%d %d %d %d %d %d \n",Hmin, Smin, Vmin, Hmax, Smax, Vmax);

}

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

static gboolean
time_handler(GtkWidget *widget)
{
	if (widget->window == NULL) return FALSE;
	

	imgTemp = (IplImage *) cvQueryFrame(camera);
	GdkPixbuf *img = convertOpenCv2Gtk(imgTemp);
	gtk_image_set_from_pixbuf(GTK_IMAGE(image1), img);

	//gtk_image_set_from_pixbuf(GTK_IMAGE(image1), convertOpenCv2Gtk(imgTemp));
	//gtk_image_set_from_pixbuf(GTK_IMAGE(image2), convertOpenCv2Gtk(imgTemp));
	

	allocateCvImage(&hsv, cvGetSize(imgTemp), 8, 3);
	allocateCvImage(&threshed, cvGetSize(imgTemp), 8, 1);
	allocateCvImage(&threshedRGB, cvGetSize(imgTemp), 8, 3);

	cvCvtColor(imgTemp, hsv, CV_BGR2HSV);

	load_values();
	cvInRangeS(hsv, cvScalar(Hmin, Smin, Vmin, 0), cvScalar(Hmax, Smax, Vmax, 0), threshed);
	cvErode(threshed, threshed, 0, 2);
	
	// one blue 3channeled image from one channeled
	cvMerge(threshed, threshed, threshed, NULL, threshedRGB);


	GdkPixbuf *img2 = convertOpenCv2Gtk(threshedRGB);
	gtk_image_set_from_pixbuf(GTK_IMAGE(image2), img2);
	

	return TRUE;
}


int main(int argc, char *argv[])
{

	GtkWidget *window, *table;
	GtkWidget *hbox;
	
	camera = cvCaptureFromCAM(0);

	gtk_init(&argc, &argv);
	
	builder = gtk_builder_new();

	profile_blue = "blue_goal.yml";
	profile_yellow = "yellow_goal.yml";
	profile_name = profile_blue;
	
	config_file_load();

	if(!gtk_builder_add_from_file(builder, "ViSy.glade", &error)){
		g_warning("! %s", error->message);
		g_free(error);
		return 1;

	}
	gtk_builder_connect_signals(builder, NULL);
	

	window = GTK_WIDGET(gtk_builder_get_object(builder, "window1"));
	image1 = GTK_IMAGE(gtk_builder_get_object(builder, "image1"));
	image2 = GTK_IMAGE(gtk_builder_get_object(builder, "image2"));
	
	button_blue = GTK_WIDGET(gtk_builder_get_object(builder, "radiobutton1"));
	button_yellow = GTK_WIDGET(gtk_builder_get_object(builder, "radiobutton2"));

	g_timeout_add(138, (GSourceFunc) time_handler, (gpointer) window);
	//g_timeout_add(66, (GSourceFunc) time_handler, (gpointer) window);
	//g_timeout_add(63, (GSourceFunc) time_handler, (gpointer) window);
	
	g_timeout_add(1000, (GSourceFunc) settings_loader, (gpointer) window);


	gtk_widget_show_all(window);
	gtk_main();

	return 0;
}

#include "opencv2/objdetect.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

/** Function Headers */
void detectAndDisplay(Mat frame);

/** Global variables */

String window_name = "Capture - Face detection";

class CascadeWrapper {
	String cascade_name;
	CascadeClassifier cascade;
	bool working = false;
public:
	CascadeWrapper(String name) : cascade_name(name)
	{
		if (!cascade.load(cascade_name))
			printf(" %s --(!)Error loading face cascade\n", cascade_name);
		else
			working = true;
	};
	void detectAndDisplay(const Mat &frame) {
		cout << "detcting " << cascade_name << endl;

		std::vector<Rect> faces;
		Mat frame_gray;

		cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
		equalizeHist(frame_gray, frame_gray);

		cout << "--Detect " << cascade_name << endl;
		cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));

		for (size_t i = 0; i < faces.size(); i++)
		{
			cout << "got " << cascade_name << endl;
			Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);
			ellipse(frame, center, Size(faces[i].width / 2, faces[i].height / 2), 0, 0, 360, Scalar(255, 0, 255), 4, 8, 0);

			Mat faceROI = frame_gray(faces[i]);
			std::vector<Rect> eyes;

		}

		imshow(window_name, frame);
	}
};

/** @function main */
int main(int argc, char ** argv) {

	Mat src;

	if (argc > 1)
		src = imread(argv[1], 1);
	else {
		cout << "ERROR: no image provided" << endl;
		return -1;
	}

	if (src.empty()) {
		cout << "ERROR: could not open " << argv[1] << endl;
		return -1;
	}

	//CascadeWrapper frontalface_cascade("C:\\TheTest\\Cascades\\lbpcascade_frontalface.xml");
	//CascadeWrapper profileface_cascade("C:\\TheTest\\Cascades\\lbpcascade_profileface.xml");
	//CascadeWrapper silverware_cascade("C:\\TheTest\\Cascades\\lbpcascade_silverware.xml");

	//frontalface_cascade.detectAndDisplay(src);
	//profileface_cascade.detectAndDisplay(src);
	//silverware_cascade.detectAndDisplay(src);
	
	CascadeWrapper face_cascade("C:\\TheTest\\Cascades\\haarcascade_frontalface_alt.xml");
	CascadeWrapper fullbody_cascade("C:\\TheTest\\Cascades\\haarcascade_fullbody.xml");
	CascadeWrapper lowerbody_cascade("C:\\TheTest\\Cascades\\haarcascade_lowerbody.xml");
	CascadeWrapper upperdoy_cascade("C:\\TheTest\\Cascades\\haarcascade_upperbody.xml");

	face_cascade.detectAndDisplay(src);
	fullbody_cascade.detectAndDisplay(src);
	lowerbody_cascade.detectAndDisplay(src);
	upperdoy_cascade.detectAndDisplay(src);

	waitKey(0);

	return 0;
}

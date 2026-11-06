
#include "Utilities.h"

// Basic table size and ball setup.
const int   TABLE_ROWS = 683;
const int   TABLE_COLUMNS = 1434;
const float BALL_DIAMETER_MM = 43.0f;
const float BALL_RADIUS_MM = BALL_DIAMETER_MM / 2.0f;

// Each image has 16 ball slots: balls 1-15 plus the white ball.
const int NUMBER_OF_BALL_SLOTS = 16;
const int WHITE_BALL_INDEX = 15;

// One ball in the known-good data.
struct GroundTruthBall{
	bool  present;
	float row;
	float column;

	Point2f centre() const { return Point2f(column, row); }
};


// A full image worth of known ball positions.
struct GroundTruthImage
{
	const char* filename;
	GroundTruthBall balls[NUMBER_OF_BALL_SLOTS];
};

// Handy little constructors for the reference data.
GroundTruthBall onTable(float row, float column) { return GroundTruthBall{ true, row, column }; }
GroundTruthBall potted() { return GroundTruthBall{ false, 0.0f, 0.0f }; }

// The reference ball positions used to check the detector.
const GroundTruthImage GROUND_TRUTH[] =
{
	{ "AllBalls.jpeg", {
		onTable(253, 908),  onTable(250, 959),  onTable(255, 1013), onTable(256, 1072),
		onTable(254, 1132), onTable(262, 1184), onTable(256, 1244), onTable(253, 1307),
		onTable(329, 901),  onTable(330, 961),  onTable(335, 1013), onTable(336, 1067),
		onTable(334, 1130), onTable(345, 1187), onTable(342, 1245), onTable(293, 797) } },
	{ "Table1.jpeg", {
		onTable(309, 1092), onTable(429, 1212), onTable(383, 1131), onTable(335, 1050),
		onTable(285, 1211), onTable(261, 1170), onTable(352, 1173), onTable(331, 1132),
		onTable(240, 1210), onTable(310, 1170), onTable(407, 1169), onTable(281, 1135),
		onTable(374, 1212), onTable(360, 1091), onTable(331, 1213), onTable(288, 29) } },
	{ "Table2.jpeg", {
		onTable(282, 1103), onTable(620, 1224), onTable(391, 1121), onTable(315, 1046),
		onTable(310, 1305), onTable(202, 621),  onTable(361, 1173), onTable(331, 1132),
		onTable(177, 725),  onTable(185, 1269), onTable(665, 798),  onTable(296, 438),
		onTable(53, 1235),  onTable(362, 1090), onTable(399, 1205), onTable(507, 1169) } },
	{ "Table3.jpeg", {
		onTable(23, 908),   onTable(616, 1224), onTable(447, 1199), onTable(336, 29),
		onTable(305, 1406), onTable(210, 621),  onTable(260, 1294), onTable(226, 1252),
		onTable(177, 725),  onTable(171, 1270), onTable(665, 798),  onTable(249, 351),
		onTable(53, 1233),  onTable(169, 901),  onTable(399, 1201), onTable(434, 29) } },
	{ "Table4.jpeg", {
		onTable(23, 908),   onTable(546, 1407), onTable(444, 1204), potted(),
		onTable(305, 1406), onTable(210, 621),  onTable(260, 1294), onTable(226, 1252),
		onTable(181, 724),  onTable(171, 1270), onTable(665, 798),  onTable(249, 351),
		onTable(58, 1231),  onTable(169, 901),  onTable(399, 1201), onTable(88, 1392) } },
	{ "Table5.jpeg", {
		onTable(535, 23),   onTable(546, 1407), onTable(444, 1204), potted(),
		onTable(305, 1406), onTable(131, 25),   onTable(260, 1294), onTable(226, 1252),
		onTable(538, 72),   onTable(171, 1270), onTable(665, 798),  onTable(249, 351),
		potted(),           onTable(169, 901),  onTable(399, 1201), onTable(608, 26) } },
	{ "Table6.jpeg", {
		onTable(535, 23),   onTable(477, 1364), onTable(444, 1204), potted(),
		onTable(305, 1406), onTable(131, 25),   onTable(260, 1294), onTable(226, 1252),
		onTable(538, 72),   onTable(171, 1270), potted(),           onTable(249, 351),
		potted(),           onTable(169, 901),  onTable(399, 1201), onTable(629, 947) } }
};


// Labels for the printed ball names.
string ballLabel(int ball_index)
{
	if (ball_index == WHITE_BALL_INDEX) return "White";
	return to_string(ball_index + 1);
}

// Count how many balls are actually on the table in a given image.
int countPresentBalls(const GroundTruthImage& image)
{
	int count = 0;
	for (int ball_index = 0; ball_index < NUMBER_OF_BALL_SLOTS; ball_index++)
	{
		if (image.balls[ball_index].present) count++;
	}
	return count;
}


bool centreIsPhysicallyPossible(const GroundTruthBall& ball)
{
	return (ball.row >= BALL_RADIUS_MM) && (ball.row <= TABLE_ROWS - BALL_RADIUS_MM) &&
		(ball.column >= BALL_RADIUS_MM) && (ball.column <= TABLE_COLUMNS - BALL_RADIUS_MM);
}


bool centreIsInsideImage(const GroundTruthBall& ball)
{
	return (ball.row >= 0) && (ball.row < TABLE_ROWS) &&
		(ball.column >= 0) && (ball.column < TABLE_COLUMNS);
}


void validateGroundTruth()
{
	cout << "=== Ground truth validation ===" << endl;
	for (const GroundTruthImage& image : GROUND_TRUTH)
	{
		cout << image.filename << ": " << countPresentBalls(image) << " balls present" << endl;
		for (int ball_index = 0; ball_index < NUMBER_OF_BALL_SLOTS; ball_index++)
		{
			const GroundTruthBall& ball = image.balls[ball_index];
			if (ball.present && !centreIsPhysicallyPossible(ball))
			{
				cout << "  WARNING: ball " << ballLabel(ball_index) << " at (row "
					<< ball.row << ", column " << ball.column << ") is outside the allowed range" << endl;
			}
		}
	}
	cout << endl;
}






// Quick helper so the maths uses the same (x, y) layout as OpenCV.
Point2f rowColumnToPoint(float row, float column)
{
	return Point2f(column, row);
}

// This is the mapping that turns the skewed photo into a straight top-down table view.
Mat computeTableHomography()
{

	vector<Point2f> source_corners =
	{
		rowColumnToPoint(35, 337),
		rowColumnToPoint(21, 1570),
		rowColumnToPoint(451, 1888),
		rowColumnToPoint(472, 38)
	};


	vector<Point2f> destination_corners =
	{
		Point2f(0, 0),
		Point2f(TABLE_COLUMNS - 1, 0),
		Point2f(TABLE_COLUMNS - 1, TABLE_ROWS - 1),
		Point2f(0, TABLE_ROWS - 1)
	};

	return getPerspectiveTransform(source_corners, destination_corners);
}


// Apply the table transform to the original image.
Mat warpToTable(const Mat& original_image, const Mat& homography)
{
	Mat warped_image;
	warpPerspective(original_image, warped_image, homography,
		Size(TABLE_COLUMNS, TABLE_ROWS), INTER_LINEAR);
	return warped_image;
}

// Mark the corners on the source image so we can sanity-check the setup.
Mat drawTableCorners(const Mat& original_image)
{
	Mat marked_image = original_image.clone();
	vector<Point> corners =
	{
		Point(337, 35), Point(1570, 21), Point(1888, 451), Point(38, 472)
	};
	polylines(marked_image, corners, true, Scalar(0, 0, 255), 2);
	for (const Point& corner : corners)
	{
		circle(marked_image, corner, 8, Scalar(0, 255, 255), 2);
	}
	return marked_image;
}


// Pull the matching reference data for a given file.
const GroundTruthImage* findGroundTruth(const string& filename)
{
	for (const GroundTruthImage& image : GROUND_TRUTH)
	{
		if (filename == image.filename) return &image;
	}
	return nullptr;
}

// Draw the known ball positions onto the transformed image.
void drawGroundTruth(Mat& transformed_image, const GroundTruthImage& ground_truth)
{
	for (int ball_index = 0; ball_index < NUMBER_OF_BALL_SLOTS; ball_index++)
	{
		const GroundTruthBall& ball = ground_truth.balls[ball_index];
		if (!ball.present || !centreIsInsideImage(ball))
		{
			continue;
		}
		Point centre_pixel(cvRound(ball.centre().x), cvRound(ball.centre().y));
		circle(transformed_image, centre_pixel, cvRound(BALL_RADIUS_MM), Scalar(0, 0, 255), 2);
		drawMarker(transformed_image, centre_pixel, Scalar(0, 0, 255), MARKER_CROSS, 10, 1);
		putText(transformed_image, ballLabel(ball_index),
			Point(centre_pixel.x + 4, centre_pixel.y - 24),
			FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 255, 255), 2);
	}
}


// Shrink the image a bit so it fits on screen without a headache.
void showScaled(const string& window_name, const Mat& image, double scale, int window_x, int window_y)
{
	Mat reduced_image;
	resize(image, reduced_image, Size(), scale, scale, INTER_AREA);
	imshow(window_name, reduced_image);
	moveWindow(window_name, window_x, window_y);
}

// Sampling window used when checking ball colours.
const int COLOUR_PATCH_HALF_WIDTH = 4;


const int HUE_BINS = 180;



Scalar meanHls(const Mat& hls_image, Rect region)
{
	region = region & Rect(0, 0, hls_image.cols, hls_image.rows);
	return mean(hls_image(region));
}


Scalar meanHlsAroundCentre(const Mat& hls_image, Point2f centre)
{
	int patch_size = 2 * COLOUR_PATCH_HALF_WIDTH + 1;
	Rect patch(cvRound(centre.x) - COLOUR_PATCH_HALF_WIDTH,
		cvRound(centre.y) - COLOUR_PATCH_HALF_WIDTH,
		patch_size, patch_size);
	return meanHls(hls_image, patch);
}


void printHlsMean(const string& label, const Scalar& hls_mean)
{
	cout << label << ":  H = " << cvRound(hls_mean[0] * 10) / 10.0
		<< "   L = " << cvRound(hls_mean[1] * 10) / 10.0
		<< "   S = " << cvRound(hls_mean[2] * 10) / 10.0 << endl;
}



void printHueHistogram(const Mat& hue_channel, Rect region)
{
	Mat hue_patch = hue_channel(region);

	int channel_numbers[] = { 0 };
	int number_bins = HUE_BINS;
	float hue_range[] = { 0.0f, 180.0f };
	const float* ranges[] = { hue_range };

	Mat histogram;
	calcHist(&hue_patch, 1, channel_numbers, Mat(), histogram, 1, &number_bins, ranges);


	double peak_count = 0.0;
	Point peak_location;
	minMaxLoc(histogram, nullptr, &peak_count, nullptr, &peak_location);

	int lowest_hue = -1;
	int highest_hue = -1;
	for (int hue = 0; hue < HUE_BINS; hue++)
	{
		if (histogram.at<float>(hue) >= 0.1 * peak_count)
		{
			if (lowest_hue < 0) lowest_hue = hue;
			highest_hue = hue;
		}
	}

	cout << "Cloth hue histogram: peak at hue " << peak_location.y
		<< " (" << peak_count << " of " << hue_patch.total() << " pixels); "
		<< "bins with >= 10% of the peak span hue " << lowest_hue
		<< " to " << highest_hue << endl;
}





// Quick colour inspection for the sample image and cloth.
void analyseColours(const Mat& transformed_image)
{
	Mat hls_image;
	cvtColor(transformed_image, hls_image, COLOR_BGR2HLS);

	vector<Mat> channels;
	split(hls_image, channels);

	imwrite("result_AllBalls_H.png", channels[0]);
	imwrite("result_AllBalls_L.png", channels[1]);
	imwrite("result_AllBalls_S.png", channels[2]);

	Mat side_by_side;
	hconcat(channels, side_by_side);
	showScaled("HLS channels: H | L | S", side_by_side, 0.3, 0, 780);

	cout << "=== HLS colour analysis (AllBalls) ===" << endl;


	Rect cloth_region(300, 450, 400, 150);
	printHlsMean("Cloth", meanHls(hls_image, cloth_region));

	const GroundTruthImage* ground_truth = findGroundTruth("AllBalls.jpeg");
	if (ground_truth != nullptr)
	{
		printHlsMean("Ball 6 (dark green)", meanHlsAroundCentre(hls_image, ground_truth->balls[5].centre()));
		printHlsMean("White ball", meanHlsAroundCentre(hls_image, ground_truth->balls[WHITE_BALL_INDEX].centre()));
		printHlsMean("Ball 8 (black)", meanHlsAroundCentre(hls_image, ground_truth->balls[7].centre()));
	}

	printHueHistogram(channels[0], cloth_region);
	cout << endl;
}

// Main app flow: validate the data, warp each image, and show the results.
void MyApplication()
{
	validateGroundTruth();

	const string file_location = "Media/Pool/";
	const char* pool_files[] = {
		"AllBalls.jpeg",
		"Table1.jpeg",
		"Table2.jpeg",
		"Table3.jpeg",
		"Table4.jpeg",
		"Table5.jpeg",
		"Table6.jpeg"
	};
	const int number_of_images = sizeof(pool_files) / sizeof(pool_files[0]);

	Mat homography = computeTableHomography();
	cout << "Homography (original -> table):" << endl << homography << endl << endl;

	for (int file_no = 0; file_no < number_of_images; file_no++)
	{
		const string image_name = pool_files[file_no];
		const string full_path = file_location + image_name;

		Mat original_image = imread(full_path, IMREAD_COLOR);
		if (original_image.empty())
		{
			cout << "Could not open " << full_path << endl;
			continue;
		}

		Mat transformed_image = warpToTable(original_image, homography);
		if (image_name == "AllBalls.jpeg")
		{
			analyseColours(transformed_image);
		}
		Mat original_with_corners = drawTableCorners(original_image);


		Mat transformed_with_truth = transformed_image.clone();
		const GroundTruthImage* ground_truth = findGroundTruth(image_name);
		if (ground_truth != nullptr)
		{
			drawGroundTruth(transformed_with_truth, *ground_truth);
		}
		else
		{
			cout << "No ground truth for " << image_name << endl;
		}


		const string image_stem = image_name.substr(0, image_name.find_last_of('.'));
		imwrite("result_" + image_stem + "_corners.png", original_with_corners);
		imwrite("result_" + image_stem + "_transformed.png", transformed_image);
		imwrite("result_" + image_stem + "_truth.png", transformed_with_truth);

		showScaled(image_name + " - original with corners", original_with_corners, 0.5, 0, 0);
		showScaled(image_name + " - transformed with ground truth", transformed_with_truth, 0.7, 0, 290);
		cout << "Showing " << image_name << " (press any key for next, Esc to stop)" << endl;
		int key = waitKey();
		destroyAllWindows();
		if (key == 27) break;
	}

}


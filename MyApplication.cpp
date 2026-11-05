
#include "Utilities.h"

// ---------------------------------------------------------------------------
// Table geometry. The transformed (top-down) image is 683 rows x 1434 columns
// and covers a 683mm x 1434mm table, so 1 pixel == 1 mm in that image.
// ---------------------------------------------------------------------------
const int   TABLE_ROWS = 683;
const int   TABLE_COLUMNS = 1434;
const float BALL_DIAMETER_MM = 43.0f;
const float BALL_RADIUS_MM = BALL_DIAMETER_MM / 2.0f;

// Each image has 16 ball "slots": balls 1-15 at indices 0-14, white at index 15.
const int NUMBER_OF_BALL_SLOTS = 16;
const int WHITE_BALL_INDEX = 15;

// One ball's ground-truth position. Values are stored exactly as in the
// course spreadsheet, i.e. (row, column). Conversion to OpenCV's (x, y)
// order happens ONLY in centre().
struct GroundTruthBall
{
	bool  present;   // false = ball is not on the table (potted)
	float row;       // y in OpenCV terms
	float column;    // x in OpenCV terms

	Point2f centre() const { return Point2f(column, row); }   // (x = column, y = row)
};

// Ground truth for one image.
struct GroundTruthImage
{
	const char* filename;
	GroundTruthBall balls[NUMBER_OF_BALL_SLOTS];
};

// Helpers that keep the data table readable.
GroundTruthBall onTable(float row, float column) { return GroundTruthBall{ true, row, column }; }
GroundTruthBall potted() { return GroundTruthBall{ false, 0.0f, 0.0f }; }

// ---------------------------------------------------------------------------
// Ground truth: ball centres in transformed-image coordinates (row, column).
// Source: course-supplied "Ball Locations In Transformed Image" spreadsheet.
// Order per image: balls 1-8 (first line), balls 9-15 then white (second line).
// SUSPECT (unverified): Table1 balls 2, 3, 4, 7, 11 (rows above 683 are
// impossible; balls 3 and 7 are 1mm apart). To be checked visually in Phase 2.
// ---------------------------------------------------------------------------
const GroundTruthImage GROUND_TRUTH[] =
{
	{ "AllBalls.jpeg", {
		onTable(253, 908),  onTable(250, 959),  onTable(255, 1013), onTable(256, 1072),
		onTable(254, 1132), onTable(262, 1184), onTable(256, 1244), onTable(253, 1307),
		onTable(329, 901),  onTable(330, 961),  onTable(334, 1130), onTable(335, 1013),
		onTable(336, 1067), onTable(342, 1245), onTable(345, 1187), onTable(293, 797) } },
	{ "Table1.jpeg", {
		onTable(309, 1092), onTable(1212, 429), onTable(353, 1174), onTable(1050, 335),
		onTable(285, 1211), onTable(261, 1170), onTable(352, 1173), onTable(331, 1132),
		onTable(240, 1210), onTable(310, 1170), onTable(1169, 407), onTable(281, 1135),
		onTable(374, 1212), onTable(360, 1091), onTable(331, 1213), onTable(288, 29) } },
	{ "Table2.jpeg", {
		onTable(282, 1103), onTable(620, 1224), onTable(391, 1121), onTable(310, 1305),
		onTable(315, 1046), onTable(202, 621),  onTable(361, 1173), onTable(331, 1132),
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

// Text label for a ball slot: "1".."15" or "White".
string ballLabel(int ball_index)
{
	if (ball_index == WHITE_BALL_INDEX) return "White";
	return to_string(ball_index + 1);
}

// Number of balls recorded as present (not potted) in one image.
int countPresentBalls(const GroundTruthImage& image)
{
	int count = 0;
	for (int ball_index = 0; ball_index < NUMBER_OF_BALL_SLOTS; ball_index++)
	{
		if (image.balls[ball_index].present) count++;
	}
	return count;
}

// A ball centre cannot be closer to a cushion than one ball radius.
bool centreIsPhysicallyPossible(const GroundTruthBall& ball)
{
	return (ball.row >= BALL_RADIUS_MM) && (ball.row <= TABLE_ROWS - BALL_RADIUS_MM) &&
		(ball.column >= BALL_RADIUS_MM) && (ball.column <= TABLE_COLUMNS - BALL_RADIUS_MM);
}

// True if the centre can be drawn at all, i.e. lies inside the transformed image.
bool centreIsInsideImage(const GroundTruthBall& ball)
{
	return (ball.row >= 0) && (ball.row < TABLE_ROWS) &&
		(ball.column >= 0) && (ball.column < TABLE_COLUMNS);
}

// Prints ball counts per image and flags impossible ground-truth positions.
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

// ---------------------------------------------------------------------------
// Table corners in the ORIGINAL image, as supplied with the assignment.
// The assignment gives them as (row, column); rowColumnToPoint() converts
// them to OpenCV's (x = column, y = row) order in one place.
// ---------------------------------------------------------------------------
Point2f rowColumnToPoint(float row, float column)
{
	return Point2f(column, row);
}

// Builds the 3x3 homography that maps the original (oblique) image onto a
// top-down view of the table where 1 pixel == 1 mm.
Mat computeTableHomography()
{
	// Source corners in a fixed order: top-left, top-right, bottom-right, bottom-left.
	vector<Point2f> source_corners =
	{
		rowColumnToPoint(35, 337),     // top-left
		rowColumnToPoint(21, 1570),    // top-right
		rowColumnToPoint(451, 1888),   // bottom-right
		rowColumnToPoint(472, 38)      // bottom-left
	};

	// Destination corners in the SAME order: the corners of the output image.
	vector<Point2f> destination_corners =
	{
		Point2f(0, 0),
		Point2f(TABLE_COLUMNS - 1, 0),
		Point2f(TABLE_COLUMNS - 1, TABLE_ROWS - 1),
		Point2f(0, TABLE_ROWS - 1)
	};

	return getPerspectiveTransform(source_corners, destination_corners);
}

// Produces the top-down view (TABLE_ROWS x TABLE_COLUMNS pixels).
Mat warpToTable(const Mat& original_image, const Mat& homography)
{
	Mat warped_image;
	warpPerspective(original_image, warped_image, homography,
		Size(TABLE_COLUMNS, TABLE_ROWS), INTER_LINEAR);
	return warped_image;
}

// Returns a copy of the original image with the four supplied corners marked
// and joined by lines 
Mat drawTableCorners(const Mat& original_image)
{
	Mat marked_image = original_image.clone();
	vector<Point> corners =
	{
		Point(337, 35), Point(1570, 21), Point(1888, 451), Point(38, 472)   // (x, y)
	};
	polylines(marked_image, corners, true, Scalar(0, 0, 255), 2);
	for (const Point& corner : corners)
	{
		circle(marked_image, corner, 8, Scalar(0, 255, 255), 2);
	}
	return marked_image;
}

// Returns the ground truth entry for a filename, or nullptr if there is none.
const GroundTruthImage* findGroundTruth(const string& filename)
{
	for (const GroundTruthImage& image : GROUND_TRUTH)
	{
		if (filename == image.filename) return &image;
	}
	return nullptr;
}

// Draws each ground-truth ball 
// onto the transformed image. Impossible positions cannot be drawn and are skipped.
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

// Shows an image reduced in size so that it fits on screen.
void showScaled(const string& window_name, const Mat& image, double scale, int window_x, int window_y)
{
	Mat reduced_image;
	resize(image, reduced_image, Size(), scale, scale, INTER_AREA);
	imshow(window_name, reduced_image);
	moveWindow(window_name, window_x, window_y);
}

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
		Mat original_with_corners = drawTableCorners(original_image);

		// Overlay the ground truth
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

		// Save figures for the report
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


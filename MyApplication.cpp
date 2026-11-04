
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

void MyApplication()
{
	validateGroundTruth();
	char* file_location = "Media/Pool/";
	char* pool_files[] = {
		"AllBalls.jpeg",
		"Table1.jpeg",
		"Table2.jpeg",
		"Table3.jpeg",
		"Table4.jpeg",
		"Table5.jpeg",
		"Table6.jpeg"
	};

	// Load images
	int number_of_images = sizeof(pool_files) / sizeof(pool_files[0]);
	Mat* pool_image = new Mat[number_of_images];
	//Mat* new_pool_image = new Mat[number_of_images];
	for (int file_no = 0; (file_no < number_of_images); file_no++)
	{
		cout << pool_files[file_no] << endl;
		string filename(file_location);
		filename.append(pool_files[file_no]);
		pool_image[file_no] = imread(filename, -1);

		if (pool_image[file_no].empty())
		{
			cout << "Could not open " << pool_image[file_no] << endl;
		}
		imshow(pool_files[file_no], pool_image[file_no]);

		char pause = cv::waitKey();  // This introduces a delay between frame.  The length of the delay/number can be reduced.
	}

}


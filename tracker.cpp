#include <opencv2/opencv.hpp>

using namespace cv;

int main() {
    VideoCapture cap("D:/qvad.mp4");

    if (!cap.isOpened()) {
        std::cerr << "Error\n";
        return -1;
    }

    int frame_width = static_cast<int>(cap.get(CAP_PROP_FRAME_WIDTH));
    int frame_height = static_cast<int>(cap.get(CAP_PROP_FRAME_HEIGHT));

    namedWindow("Video", WINDOW_NORMAL);
    resizeWindow("Video", frame_width, frame_height);

    namedWindow("HSV", WINDOW_NORMAL);
    resizeWindow("HSV", frame_width, frame_height);

    //int low_H = 0, low_S = 0, low_V = 255;
    //int high_H = 255, high_S = 4, high_V = 255;

    int low_H = 95, low_S = 12, low_V = 138;
    int high_H = 255, high_S = 50, high_V = 255;
    createTrackbar("Low H", "HSV", &low_H, 255);
    createTrackbar("High H", "HSV", &high_H, 255);
    createTrackbar("Low S", "HSV", &low_S, 255);
    createTrackbar("High S", "HSV", &high_S, 255);
    createTrackbar("Low V", "HSV", &low_V, 255);
    createTrackbar("High V", "HSV", &high_V, 255);

    int codec = VideoWriter::fourcc('X', 'V', 'I', 'D');
    VideoWriter video("D:/output.avi", codec, 30, Size(frame_width, frame_height));

    while (true) {
        Mat frame;
        cap >> frame;

        if (frame.empty())
            break;

        GaussianBlur(frame, frame, Size(5, 5), 0);

        Mat hsv;
        cvtColor(frame, hsv, COLOR_BGR2HSV);

        Scalar lower_white = Scalar(low_H, low_S, low_V);
        Scalar upper_white = Scalar(high_H, high_S, high_V);

        Mat mask;
        inRange(hsv, lower_white, upper_white, mask);

        Mat edges;
        Canny(mask, edges, 30, 90);

        dilate(edges, edges, Mat(), Point(-1, -1), 2);
        erode(edges, edges, Mat(), Point(-1, -1), 2);

        std::vector<std::vector<Point>> contours;
        findContours(edges, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        for (size_t i = 0; i < contours.size(); ++i) {
            double area = contourArea(contours[i]);
            if (area > 1000) {
                RotatedRect rotatedRect = minAreaRect(contours[i]);
                Point2f vertices[4];
                rotatedRect.points(vertices);
                for (int j = 0; j < 4; ++j) {
                    line(frame, vertices[j], vertices[(j + 1) % 4], Scalar(0, 255, 0), 2);
                }
            }
        }

        imshow("Video", frame);
        imshow("HSV", mask);

        video.write(frame);

        if (waitKey(33) == 'q')
            break;
    }

    cap.release();
    destroyAllWindows();

    video.release();

    return 0;
}

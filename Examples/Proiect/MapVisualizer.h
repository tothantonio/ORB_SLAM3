#pragma once
#include <vector>
#include <opencv2/opencv.hpp>
#include "SLAMManager.h"

class MapVisualizer {
public:
    MapVisualizer(int width = 800, int height = 800, float scale = 10.0f)
        : mW(width), mH(height), mScale(scale) {
        mOrigin = cv::Point2i(mW / 2, mH / 2);
        mAccumulated = cv::Mat::zeros(mH, mW, CV_32F);
    }

    cv::Mat render(const SLAMState& state) {
        // Acumuleaza map points noi
        accumulatePoints(state.mapPoints);

        // Construieste canvas
        cv::Mat canvas(mH, mW, CV_8UC3, cv::Scalar(255, 255, 255));

        drawGrid(canvas);
        drawDensityMap(canvas);

        if (state.trackingState == 2) {
            cv::Point2i px = worldToCanvas(state.position.x, state.position.z);
            mTrajectory.push_back(px);
        }
        drawTrajectory(canvas);

        return canvas;
    }

private:
    int mW, mH;
    float mScale;
    cv::Point2i mOrigin;
    std::vector<cv::Point2i> mTrajectory;
    cv::Mat mAccumulated;  // density map acumulat

    // Converteste coordonate din lume (x,z) in pixeli
    cv::Point2i worldToCanvas(float x, float z) {
        int px = mOrigin.x + (int)(x * mScale);
        int py = mOrigin.y - (int)(z * mScale);
        return cv::Point2i(px, py);
    }

    // Acumuleaza puncte noi in density map
    void accumulatePoints(const std::vector<cv::Point3f>& points) {
        for (size_t i = 0; i < points.size(); i += 50) {  // doar 1 din 50
            cv::Point2i px = worldToCanvas(points[i].x, points[i].z);
            if (px.x >= 1 && px.x < mW-1 && px.y >= 1 && px.y < mH-1)
                mAccumulated.at<float>(px.y, px.x) += 1.0f;
        }
    }

    // Dilatare manuala pentru a face punctele mai vizibile
    cv::Mat dilatare(const cv::Mat& src, int kernelSize) {
        cv::Mat dst = src.clone();
        int half = kernelSize / 2;
        for (int i = half; i < src.rows - half; i++) {
            for (int j = half; j < src.cols - half; j++) {
                float maxVal = 0;
                for (int u = -half; u <= half; u++)
                    for (int v = -half; v <= half; v++)
                        maxVal = std::max(maxVal,
                                 src.at<float>(i+u, j+v));
                dst.at<float>(i, j) = maxVal;
            }
        }
        return dst;
    }

    void drawDensityMap(cv::Mat& canvas) {
        cv::Mat dilated = dilatare(mAccumulated, 3);

        double maxVal;
        cv::minMaxLoc(dilated, nullptr, &maxVal);
        if (maxVal == 0) return;

        for (int i = 0; i < mH; i++) {
            for (int j = 0; j < mW; j++) {
                float val = dilated.at<float>(i, j);
                if (val == 0) continue;
                canvas.at<cv::Vec3b>(i, j) = {0, 0, 255};
            }
        }
    }

    void drawGrid(cv::Mat& canvas) {
        cv::Scalar gridColor(200, 200, 200);
        int step = 50;
        for (int x = 0; x < mW; x += step)
            cv::line(canvas, cv::Point(x, 0),
                     cv::Point(x, mH), gridColor, 1);
        for (int y = 0; y < mH; y += step)
            cv::line(canvas, cv::Point(0, y),
                     cv::Point(mW, y), gridColor, 1);
        cv::line(canvas, cv::Point(mOrigin.x, 0),
                 cv::Point(mOrigin.x, mH), cv::Scalar(150,150,150), 1);
        cv::line(canvas, cv::Point(0, mOrigin.y),
                 cv::Point(mW, mOrigin.y), cv::Scalar(150,150,150), 1);
    }

    void drawTrajectory(cv::Mat& canvas) {
        if (mTrajectory.size() < 2) return;
        for (size_t i = 1; i < mTrajectory.size(); i++)
            cv::line(canvas, mTrajectory[i-1], mTrajectory[i], cv::Scalar(255, 0, 0), 2);
            
        cv::circle(canvas, mTrajectory.front(), 8,
                   cv::Scalar(0, 200, 0), -1);
        cv::circle(canvas, mTrajectory.back(), 5,
                   cv::Scalar(0, 255, 0), -1);
    }
};
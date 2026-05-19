
#pragma once
#include <vector>
#include <opencv2/opencv.hpp>
#include "System.h"
#include "Tracking.h"

struct SLAMState {
    std::vector<cv::Point3f> mapPoints;  // Punctele din harta
    std::vector<cv::KeyPoint> keypoints; // Keypoints urmarite
    cv::Point3f position;                // Pozitia camerei
    int trackingState;                   // Starea tracking-ului
};

class SLAMManager {
public:
    SLAMManager(ORB_SLAM3::System* slam) : mpSLAM(slam) {}

    SLAMState update(const cv::Mat& frame, double timestamp) {
        SLAMState state;

        // Pasul principal: TrackMonocular
        mpSLAM->TrackMonocular(frame, timestamp);
        state.trackingState = mpSLAM->GetTrackingState();
        state.keypoints = mpSLAM->GetTrackedKeyPointsUn();

        // Extragem punctele 3D din scena
        // std::vector<ORB_SLAM3::MapPoint*> pts = mpSLAM->GetTrackedMapPoints();
        // state.mapPoints.clear();

        // for (auto* mp : pts) {
        //     if (!mp || mp->isBad()) continue;
        //     Eigen::Matrix<float,3,1> pos = mp->GetWorldPos();
        //     state.mapPoints.push_back(cv::Point3f(pos(0), pos(1), pos(2)));
        // }

        // Calculam pozitia camerei in coordonate globale (Lume)
        ORB_SLAM3::Tracking* tracker = mpSLAM->GetTracker();
        if (tracker && state.trackingState == 2
            && !tracker->mlRelativeFramePoses.empty()
            && !tracker->mlpReferences.empty()) {
            
            Sophus::SE3f Trel = tracker->mlRelativeFramePoses.back();
            ORB_SLAM3::KeyFrame* pRef = tracker->mlpReferences.back();
            
            // Daca avem o referinta valida, calculam pozitia camerei in lume
            if (pRef && !pRef->isBad()) {
                Sophus::SE3f Tref = pRef->GetPose();
                Sophus::SE3f Tcw = Trel * Tref; // World to Camera
                Sophus::SE3f Twc = Tcw.inverse(); // Camera to World
                Eigen::Vector3f t = Twc.translation(); // vectorul de translatie reprezinta pozitia camerei in lume
                state.position = cv::Point3f(t(0), t(1), t(2)); // salvam pozitia camerei in structura de stare
            }
        }

        // Extragem si filtram punctele
        std::vector<ORB_SLAM3::MapPoint*> pts = mpSLAM->GetTrackedMapPoints();
        state.mapPoints.clear();

        // Definim raza de relevanta
        const float maxRadius = 25.0f; 

        for (auto* mp : pts) {
            if (!mp || mp->isBad()) continue;

            // verificam daca punctul a fost vazut in cel putin 3 cadre
            if (mp->Observations() < 3) continue; 

            Eigen::Matrix<float,3,1> pos = mp->GetWorldPos();
            cv::Point3f pt(pos(0), pos(1), pos(2));

            float dist = std::sqrt(std::pow(pt.x - state.position.x, 2) + 
                                std::pow(pt.y - state.position.y, 2) + 
                                std::pow(pt.z - state.position.z, 2));

            if (dist < maxRadius) {
                state.mapPoints.push_back(pt);
            }
        }

        return state;
    }

private:
    ORB_SLAM3::System* mpSLAM;
};
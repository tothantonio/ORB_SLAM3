#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>
#include "System.h"
#include "FrameDrawer.h"
#include "SLAMManager.h"
#include "MapVisualizer.h"

// Functie pentru a incarca timestamp-urile din fisierul text
std::vector<double> loadTimestamps(const std::string& path) {
    std::vector<double> times;
    std::ifstream f(path);
    double t;
    while (f >> t) times.push_back(t);
    return times;
}

int main(int argc, char** argv) {
    // Verificam argumentele: Vocabular, Config, folder Date
    if (argc != 4) {
        return 1;
    }

    // Incarcam timpii pentru sincronizare
    auto timestamps = loadTimestamps(std::string(argv[3]) + "/times.txt");
    if (timestamps.empty()) {
        std::cerr << "Nu s-au putut citi timestamps!" << std::endl;
        return 1;
    }

    // Initializare ORB_SLAM3 (Monocular, fara Pangolin)
    ORB_SLAM3::System SLAM(argv[1], argv[2],
                           ORB_SLAM3::System::MONOCULAR, false);

    // Manager pentru gestionarea starii SLAM
    SLAMManager manager(&SLAM);

    // Vizualizator pentru harta 2D 
    MapVisualizer viz(800, 800, 10.0f);

    for (size_t i = 0; i < timestamps.size(); i++) {
        // Generam numele imaginii (ex: 000000.png)
        char fname[50];
        sprintf(fname, "%06zu.png", i);
        std::string imgPath = std::string(argv[3]) + "/image_0/" + fname;

        // Citim imaginea in mod Grayscale
        cv::Mat frame = cv::imread(imgPath, cv::IMREAD_GRAYSCALE);
        if (frame.empty()) break;

        // Procesam cadrul si obtinem starea (puncte harta, pozitie)
        SLAMState state = manager.update(frame, timestamps[i]);

        // Randam vizualizarea 2D
        cv::Mat mapView = viz.render(state);
        cv::imshow("Map", mapView);

        // Desenam trasaturile (features) detectate pe imaginea originala
        cv::Mat frameWithFeatures = SLAM.GetFrameDrawer()->DrawFrame();

        cv::imshow("ORB-SLAM3 Features", frameWithFeatures);
        cv::waitKey(1); // Necesar pentru afisare OpenCV

        std::cout << "Frame " << i << "/" << timestamps.size() - 1 << "\r" << std::flush;
    }

    std::cout << std::endl;
    SLAM.Shutdown();
    std::cout << "Gata!" << std::endl;

    return 0;
}

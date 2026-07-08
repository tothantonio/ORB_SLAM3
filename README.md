# ORB-SLAM3 Project

This repository contains a working ORB-SLAM3 setup with a small example application under the Examples/Proiect folder.

## What this project is about

This project is about visual SLAM, which means estimating where a camera is moving while simultaneously building a map of the environment. In other words, it helps solve the problem of understanding motion and surroundings from camera images alone.

## What problem it solves

A robot, drone, or AR device needs to know:

- where it is,
- where it has been,
- and what the surrounding scene looks like.

ORB-SLAM3 helps with this by tracking visual features in images, matching them over time, and using them to estimate camera motion and reconstruct a map.

## Key concepts

- SLAM: Simultaneous Localization and Mapping.
- Visual features: distinctive points detected in images, such as corners or textured regions.
- ORB descriptors: compact feature descriptions used for matching points between frames.
- Tracking: following the camera motion frame by frame.
- Mapping: building a sparse map of observed landmarks.
- Loop closing: recognizing a previously visited place and correcting accumulated drift.
- Bundle adjustment: optimizing camera poses and map points for a more accurate reconstruction.

## What this project does

- Builds the ORB-SLAM3 core libraries and example executables.
- Includes a sample application that can run SLAM on a dataset.
- Uses a pre-trained ORB vocabulary file for feature matching and mapping.

## Requirements

This project is intended for Ubuntu 20.04 or a WSL Ubuntu environment.

You need the following tools and libraries installed:

- Git
- CMake
- Make
- G++ / build-essential
- OpenCV 4.x
- Eigen3
- Boost
- pkg-config

The helper script build_prerequisites.sh installs most of these automatically.

## Quick start

1. Clone the repository:

```bash
git clone https://github.com/tothantonio/ORB_SLAM3.git
cd ORB_SLAM3
```

2. Make the setup scripts executable:

```bash
chmod +x build_prerequisites.sh build.sh
```

3. Install dependencies and build the project:

```bash
./build_prerequisites.sh
```

This script will:

- install required system packages,
- build the bundled third-party libraries,
- compile the main ORB-SLAM3 project,
- generate the example executable in the Examples/Proiect folder.

## Rebuild after code changes

If you modify the source code, rebuild with:

```bash
./build.sh
```

## Run the example

After the build finishes, you can run the sample executable with a dataset.

Example:

```bash
./Examples/Proiect/slam_kitti Vocabulary/ORBvoc.txt Examples/Monocular/settings.yaml /path/to/dataset/
```
<<<<<<< HEAD
./Examples/Proiect/slam_kitti Vocabulary/ORBvoc.txt Examples/Monocular/settings.yaml dataset/
```
=======

Notes:

- Replace /path/to/dataset/ with the real path to your dataset.
- If you are using a different dataset, make sure the YAML configuration file matches it.
- The vocabulary file must exist in the Vocabulary folder.

## Important files

- build_prerequisites.sh: installs dependencies and builds the project.
- build.sh: rebuilds the project quickly after modifications.
- Examples/Proiect/main.cpp: example entry point.
- Examples/Proiect/settings.yaml: example configuration.
- Vocabulary/ORBvoc.txt: ORB vocabulary used by the SLAM system.

## Troubleshooting

If the build fails, check the following:

- Make sure you are in the project root when running the scripts.
- Confirm that OpenCV and Eigen are installed correctly.
- If ORBvoc.txt is missing, check whether Vocabulary/ORBvoc.txt.tar.gz exists and extract it manually.
- If the executable is not created, inspect the build output for CMake or compiler errors.

## Project structure

- Thirdparty/: third-party libraries used by ORB-SLAM3
- Examples/: example applications and configurations
- include/: public headers
- src/: implementation files
- Vocabulary/: ORB vocabulary data
>>>>>>> add readme

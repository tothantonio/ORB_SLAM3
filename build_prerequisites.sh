#!/bin/bash

echo "################################################################"
echo ">>> {Instalare Completă ORB-SLAM3}"
echo "################################################################"

# --- Funcție pentru animație ---
show_loading() {
    local pid=$1
    local delay=0.1
    local spinstr='|/-\'
    while ps -p $pid > /dev/null 2>&1; do
        local temp=${spinstr#?}
        printf " [%c]  " "$spinstr"
        spinstr=$temp${spinstr%"$temp"}
        sleep $delay
        printf "\b\b\b\b\b\b"
    done
    printf "    \b\b\b\b"
}

# --- Pasul 1: Dependințe de bază ---
echo ">> Se instalează dependințele sistem (Eigen, Boost, OpenCV deps)..."
sudo apt update
sudo apt install -y build-essential cmake git pkg-config \
    libjpeg-dev libpng-dev libtiff-dev libgoogle-glog-dev \
    libgflags-dev libboost-serialization-dev libssl-dev \
    libatlas-base-dev libeigen3-dev > /dev/null 2>&1
echo ">> Dependințe instalate."

# --- Pasul 2: OpenCV 4.4 ---
if pkg-config --modversion opencv4 > /dev/null 2>&1; then
    echo ">> OpenCV 4.x este deja prezent."
else
    echo ">> Se instalează OpenCV 4.4.0 (Build de la zero)..."
    mkdir -p ~/dev && cd ~/dev
    git clone https://github.com/opencv/opencv.git --branch 4.4.0 --single-branch
    mkdir -p opencv/build && cd opencv/build
    cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local -D BUILD_TESTS=OFF -D BUILD_PERF_TESTS=OFF ..
    make -j$(nproc)
    sudo make install
    sudo ldconfig
fi

# --- Pasul 3: Descărcare și Organizare Proiect ---
echo ">> Se pregătește folderul proiectului..."
mkdir -p ~/dev && cd ~/dev

# Dacă folderul nu există, îl clonăm (înlocuiește URL-ul cu cel al repo-ului tău dacă e diferit)
if [ ! -d "ORB_SLAM3" ]; then
    echo ">> Se clonează ORB-SLAM3..."
    git clone https://github.com/tothantonio/ORB_SLAM3.git
fi

cd ORB_SLAM3
    
# --- Pasul 4: Compilare Thirdparty (Critic pentru CMake-ul tău) ---

# 4.1 DBoW2 - Fișierul tău caută libDBoW2.so în folderul lib
echo ">> Se compilează DBoW2..."
cd Thirdparty/DBoW2
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
# Creăm folderul lib dacă nu există și mutăm biblioteca acolo (așa cum cere CMake-ul tău)
mkdir -p ../lib
cp lib/libDBoW2.so ../lib/
cd ../../..

# 4.2 g2o - Deși ai add_subdirectory, e mai sigur să-l pregătim
echo ">> Se pregătește g2o..."
cd Thirdparty/g2o
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
# Mutăm în folderul lib cerut de CMakeLists.txt din rădăcină
mkdir -p ../lib
cp lib/libg2o.so ../lib/
cd ../../..

# --- Pasul 5: Compilare Proiect Principal ---
echo ">> Se compilează proiectul principal (SLAM Core + slam_kitti)..."
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
( make -j$(nproc) ) &
pid=$!
show_loading $pid

# Verificare finală
if [ -f "../Examples/Proiect/slam_kitti" ]; then
    echo ""
    echo "################################################################"
    echo ">>> Succes! Executabilul a fost creat în:"
    echo ">>> ~/dev/ORB_SLAM3/Examples/Proiect/slam_kitti"
    echo "################################################################"
else
    echo ">> Eroare la compilare. Verifică log-urile de mai sus."
fi
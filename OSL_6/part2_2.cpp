#include<bits/stdc++.h>
#include<sys/shm.h>
#include<sys/ipc.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>

using namespace std;

int height, width, maxColor;
int *sharedData;
sem_t* sem;
int *grayPixel;
key_t key1, key2;

int* get(int i, int j, int k) {
    return &sharedData[i * width * 3 + j * 3 + k];
}


void readImage(char* filename) {

    ifstream file(filename, ios::binary);

    if(!file.is_open()) {
        cerr <<  "Error opening file: " << filename << endl;
        return;
    }

    string image_format;
    string line;
    // file >> image_format;
    getline(file, image_format);
    assert(image_format == "P3");

    getline(file, line);
    while(line[0] == '#') {
        getline(file, line);
    }
    
    stringstream ss(line);
    ss >> width >> height;
    size_t alot = sizeof(int) * height * width * 3;
    key1 = ftok(filename, 1);
    int shmid = shmget(key1, alot, 0666 | IPC_CREAT);
    assert(shmid != -1);
    sharedData = (int*)shmat(shmid, NULL, 0);

    file >> maxColor;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            for(int k = 0; k < 3; k++) {
                file >> *get(i, j, k);
            }
        }
    }
}

void writeImage(char* outPath) {

    ofstream file(outPath, ios::binary);
    
    file << "P3\n" << width  << " " << height << "\n" << maxColor << "\n";

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            file << *get(i, j, 0) << " " << *get(i, j, 1) << " " << *get(i, j, 2) << " ";
        }
        file << "\n";
    } 
}


void makeGrayScale() {

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {

            sem_wait(sem);

            int gray = 0.2126 * (*get(i, j, 0)) + 0.7152 * (*get(i, j, 1)) + 0.0722 * (*get(i, j, 2));
            *get(i, j, 0) = *get(i, j, 1) = *get(i, j, 2) = gray;

            grayPixel[0] = i;
            grayPixel[1] = j;

            sem_post(sem);
        }
    }
}

void makeBlur() {
    for(int itr = 0; itr < 4; itr++) {
        for (int i = 1; i < height - 1; i++) {
            for (int j = 1; j < width - 1; j++) {
                
                while(true) {
                    sem_wait(sem);
                    if(grayPixel[0] > i + 1 || (grayPixel[0] == i + 1 && grayPixel[1] >= j + 1)) break;
                    else sem_post(sem);
                }

                for(int k = 0; k < 3; k++) {

                    int val = *get(i-1,j-1,k) + 2 * (*get(i-1,j,k)) + *get(i-1, j+1, k) +
                            2 * (*get(i,j-1, k)) + 4 * (*get(i, j, k)) +   2 * (*get(i, j+1, k)) +
                            *get(i+1, j-1, k) + 2 * (*get(i+1, j, k)) + *get(i+1, j+1, k);
                    val /= 16;

                    *get(i, j, k) = val;

                }

                sem_post(sem);
            }
        }
    }
}

void invertImage() {

    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            while(true) {
                sem_wait(sem);
                if(grayPixel[0] > i || (grayPixel[0] == i && grayPixel[1] >= j)) break;
                else sem_post(sem);
            }
        
            for(int k = 0; k < 3; k++) {
                *get(i, j, k) = maxColor - *get(i, j, k);
            }
            sem_post(sem);
        }
    }
}





int main(int argc, char* argv[]) {
    
    readImage(argv[1]);
    key2 = ftok(argv[1], 2);
    int shmid = shmget(key2, 8, 0666 | IPC_CREAT);
    grayPixel = (int*)shmat(shmid, NULL, 0);
    grayPixel[0] = grayPixel[1] = -1;
    sem = sem_open("/semaphore", O_CREAT | O_EXCL, 0666, 1);;

    auto start = chrono::high_resolution_clock::now();
    if(fork() == 0) {
        makeGrayScale();
        exit(0);
    } else {
        invertImage();
        wait(NULL);
    }
    auto end = chrono::high_resolution_clock::now();

    sem_close(sem);
    sem_unlink("/semaphore");

    auto duration = end - start;
    cout << "Time Taken: " << chrono::duration_cast<chrono::milliseconds>(duration).count() << " ms" << endl;

    writeImage(argv[2]);
}
#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<cassert>
#include<thread>
#include<semaphore.h>
#include<chrono>
#include<sstream>


using namespace std;
using Pixel = vector<int>;

sem_t sem;
int grayPixel[2] = {-1, -1};

struct Image {
    int height, width, maxColor;
    vector<vector<Pixel>> data;
};


Image* readImage(char* filename) {

    ifstream file(filename, ios::binary);

    if(!file.is_open()) {
        cerr <<  "Error opening file: " << filename << endl;
        return NULL;
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
    
    Image* image = new Image;
    stringstream ss(line);
    ss >> image->width >> image->height;
    image->data.resize(image->height, vector<Pixel>(image->width, vector<int>(3)));

    file >> image->maxColor;
    for (int i = 0; i < image->height; i++) {
        for (int j = 0; j < image->width; j++) {
            file >> image->data[i][j][0] >> image->data[i][j][1] >> image->data[i][j][2];
        }
    }

    return image;
}

void writeImage(Image* image, char* outPath) {

    ofstream file(outPath, ios::binary);
    
    file << "P3\n" << image->width  << " " << image->height << "\n" << image->maxColor << "\n";

    for (int i = 0; i < image->height; i++) {
        for (int j = 0; j < image->width; j++) {
            file << image->data[i][j][0] << " " << image->data[i][j][1] << " " << image->data[i][j][2] << " ";
        }
        file << "\n";
    } 
}


void makeGrayScale(Image* image) {

    for (int i = 0; i < image->height; i++) {
        for (int j = 0; j < image->width; j++) {
            
            sem_wait(&sem);

            int gray = 0.2126 * image->data[i][j][0] + 0.7152 * image->data[i][j][1] + 0.0722 * image->data[i][j][2];
            image->data[i][j][0] = image->data[i][j][1] = image->data[i][j][2] = gray;

            grayPixel[0] = i;
            grayPixel[1] = j;

            sem_post(&sem);

        }
    }
}

void makeBlur(Image* image) {

    for(int itr = 0; itr < 4; itr++) {
        for (int i = 1; i < image->height - 1; i++) {
            for (int j = 1; j < image->width - 1; j++) {
                
                while(true) {
                    sem_wait(&sem);
                    if(grayPixel[0] > i || (grayPixel[0] == i && grayPixel[1] >= j)) break;
                    else sem_post(&sem);
                }

                for(int k = 0; k < 3; k++) {

                    int val = image->data[i-1][j-1][k] + 2 * image->data[i-1][j][k] + image->data[i-1][j+1][k] +
                            2 * image->data[i][j-1][k] + 4 * image->data[i][j][k] +   2 * image->data[i][j+1][k] +
                            image->data[i+1][j-1][k] + 2 * image->data[i+1][j][k] + image->data[i+1][j+1][k];
                    val /= 16;

                    image->data[i][j][k] = val;

                }

                sem_post(&sem);
            
            }
        }
    }
}


void invertImage(Image* image) {

    for(int i = 0; i < image->height; i++) {
        for(int j = 0; j < image->width; j++) {
            while(true) {
                sem_wait(&sem);
                if(grayPixel[0] > i || (grayPixel[0] == i && grayPixel[1] >= j)) break;
                else sem_post(&sem);
            }
        
            for(int k = 0; k < 3; k++) {
                image->data[i][j][k] = image->maxColor - image->data[i][j][k];
            }
            sem_post(&sem);
        }
    }
}




int main(int argc, char* argv[]) {

    Image* image = readImage(argv[1]);
    sem_init(&sem, 0, 1);
    auto start = chrono::high_resolution_clock::now();

    thread t1(makeGrayScale, image);
    thread t2(invertImage, image);

    t1.join();
    t2.join();

    auto end = chrono::high_resolution_clock::now();
    auto duration = end - start;
    cout << "Time Taken: " << chrono::duration_cast<chrono::milliseconds>(duration).count() << " ms" << endl;

    sem_destroy(&sem);
    writeImage(image, argv[2]);
}
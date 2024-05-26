#include<bits/stdc++.h>
#include<sys/types.h>
#include<sys/wait.h>


using namespace std;
using Pixel = vector<int>;
int fds[2];

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

    close(fds[0]);
    for (int i = 0; i < image->height; i++) {
        for (int j = 0; j < image->width; j++) {
            int gray = 0.2126 * image->data[i][j][0] + 0.7152 * image->data[i][j][1] + 0.0722 * image->data[i][j][2];
            image->data[i][j][0] = image->data[i][j][1] = image->data[i][j][2] = gray;
            write(fds[1], &gray, sizeof(int));
        }
    }
}

void makeBlur(Image* image) {

    close(fds[1]);

    for(int itr = 0; itr < 4; itr++) {
        for (int i = 1; i < image->height - 1; i++) {
            for (int j = 1; j < image->width - 1; j++) {
                for(int k = 0; k < 3; k++) {

                    int val = image->data[i-1][j-1][k] + 2 * image->data[i-1][j][k] + image->data[i-1][j+1][k] +
                            2 * image->data[i][j-1][k] + 4 * image->data[i][j][k] +   2 * image->data[i][j+1][k] +
                            image->data[i+1][j-1][k] + 2 * image->data[i+1][j][k] + image->data[i+1][j+1][k];
                    val /= 16;

                    image->data[i][j][k] = val;

                }
            
            }
        }
    }
}

void invertImage(Image* image) {
    close(fds[1]);
    int done = 0;
    int val;
    while(done < image->height * image->width) {
        read(fds[0], &val, sizeof(int));
        int i = done / image->width;
        int j = done % image->width;
        for(int k = 0; k < 3; k++) {
            image->data[i][j][k] = image->maxColor - val;
        }
        done++;
    }
}


int main(int argc, char* argv[]) {
    
    Image* image = readImage(argv[1]);

    pipe(fds);
    
    auto start = chrono::high_resolution_clock::now();
    if(fork() == 0) {
        makeGrayScale(image);
        exit(0);
    } else {
        invertImage(image);
        wait(NULL);
    }

    auto end = chrono::high_resolution_clock::now();
    auto duration = end - start;
    cout << "Time Taken: " << chrono::duration_cast<chrono::milliseconds>(duration).count() << " ms" << endl;

    writeImage(image, argv[2]);
}
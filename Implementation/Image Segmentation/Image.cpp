#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <filesystem>
using namespace std;
typedef vector<int> vi;
#define pb push_back

const long double sigma = 2;		// Control parameter (Lower sigma => More sensitive towards neighboring pixel difference)
const int cMAX = 1000;              // Maximum edge capacity

void error(const string &msg){
	cout<<"\nError: "<<msg<<endl;
	exit(1);
}

vector<vector<vi>> inputImage(const string &path){
	cv::Mat img = cv::imread(path);
    if(img.empty()) error("Could not open image");

	int height = img.rows, width  = img.cols;
    vector<vector<vi>> image(height, vector<vi>(width, vi(3)));

	for(int i=0;i<height;i++){
        for(int j=0;j<width;j++){
            cv::Vec3b pixel = img.at<cv::Vec3b>(i,j);
            int B = pixel[0], G = pixel[1], R = pixel[2];
            image[i][j][0] = R;
            image[i][j][1] = G;
            image[i][j][2] = B;
        }
    }
    return image;
}

int capacity(vi &a, vi &b){
	long double c = 0;
	int n = a.size();
	for(int i=0;i<n;i++) c += (a[i]-b[i])*(a[i]-b[i]);
	c /= n*sigma*sigma;
	return cMAX*exp(-c);
}

vector<vector<pair<int, long long>>> ImageToGraph(vector<vector<vi>> &image, bool diagonal_edges=false){
	int n = image.size(), m = image[0].size();
	vector<vector<pair<int, long long>>> graph(n*m);
	for(int i=0, cur=0;i<n;i++){
		for(int j=0;j<m;j++, cur++){
			if(i<n-1){
				int V = capacity(image[i][j], image[i+1][j]);
				graph[cur].pb({cur+m,V});
				graph[cur+m].pb({cur,V});
				if(diagonal_edges){
					if(j>0){
						int DL = capacity(image[i][j], image[i+1][j-1]);
						graph[cur].pb({cur+m-1,DL});
						graph[cur+m-1].pb({cur,DL});
					}
					if(j<m-1){
						int DR = capacity(image[i][j], image[i+1][j+1]);
						graph[cur].pb({cur+m+1,DR});
						graph[cur+m+1].pb({cur,DR});
					}
				}
			}
			if(j<m-1){
				int H = capacity(image[i][j], image[i][j+1]);
				graph[cur].pb({cur+1,H});
				graph[cur+1].pb({cur,H});
			}
		}
	}
	return graph;
}

cv::Scalar optimalBackground(vector<vector<vi>> &image, vi &cluster){
	static const int MAX = 255;
	long long background[2][image[0][0].size()];
	memset(background, 0, sizeof(background));
	int m=image[0].size(), channels=image[0][0].size();
	for(int node:cluster){
		int i=node/m, j=node%m;
		vi &pixel = image[i][j];
		for(int k=0;k<channels;k++){
			background[0][k]+=pixel[k]*pixel[k];
			background[1][k]+=(MAX-pixel[k])*(MAX-pixel[k]);
		}
	}
	int R=((background[0][0]>background[1][0])? 0:MAX);
	int G=((background[0][1]>background[1][1])? 0:MAX);
	int B=((background[0][2]>background[1][2])? 0:MAX);
	
	R = G = B = 255; // White
	
 	return cv::Scalar(B, G, R);
}

void GraphToImage(string path, vector<vi> &clusters, vector<vector<vi>> &image){
	string folder = path.substr(0, path.find_last_of('.'));
    try{
        filesystem::create_directories(folder);
    }
	catch(const filesystem::filesystem_error& e) {
        cout<<"Filesystem error creating directory - "<<folder<<" : "<<e.what()<<endl;
    }
    folder.pb('/');
    
    
    int n = image.size(), m = image[0].size(), c=0;
    for(vi &cluster:clusters){
    	cv::Scalar background = optimalBackground(image, cluster);
	    cv::Mat clusterImage(n, m, CV_8UC3, background);
	    
    	for(int pixel:cluster){
    		int i=pixel/m, j=pixel%m;
	        auto &pix = clusterImage.at<cv::Vec3b>(i,j);
	        pix[0] = image[i][j][2];
	        pix[1] = image[i][j][1];
	        pix[2] = image[i][j][0];
	    }

	    cv::imwrite(folder+"cluster_"+to_string(++c)+".png", clusterImage);
	}
}

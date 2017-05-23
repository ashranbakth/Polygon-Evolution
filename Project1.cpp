#include "draw.h" // renderPolyImage(), score()
#include <opencv2/core/core.hpp> // Mat, Point, Scalar
#include <opencv2/highgui/highgui.hpp> // imread(), imwrite(), imshow()
#include <random> //default_random_engine, uniform_int_distribution<>, uniform_real_distribution<>
#include <iostream> // cerr, cout
#include <sstream> // ostringstream
#include <string> // string
#include <stdlib.h> //rand()
#include <vector> //std::vector
#include <algorithm>//std::sort
using namespace cv;
using namespace std;
//you may want to wrap vertexCounts, polyArr, polygons, and colors

struct Image{

	Point polyArr[100][6];
	const Point* polygons[100];
	Scalar colors[100];
	double score; 

	Image(const Image& im);
	Image(Point polyArr1[][6], const Point* polygons1[], Scalar colors1[], double score1);
};


Image::Image(const Image& im) {
	for (int i = 0; i < 100; ++i){
		for (int j = 0; j < 3; ++j){
			polyArr[i][j] = im.polyArr[i][j]; 
		}
	}
	for (int i = 0; i < 100; ++i){
		polygons[i] = &polyArr[i][0];
		colors[i] = im.colors[i];
	}
	score = im.score; 
}

Image::Image(Point polyArr1[][6], const Point* polygons1[], Scalar colors1[], double score1){
	
	for (int i = 0; i < 100; ++i){
		for (int j = 0; j < 3; ++j){
			polyArr[i][j] = polyArr1[i][j]; 
		}
	}
	for (int i = 0; i < 100; ++i){
		polygons[i] = &polyArr[i][0];
		colors[i] = colors1[i];
	}
	score = score1; 
}

struct ScoreSort{
	bool operator()(const Image a, const Image b){
		return a.score > b.score; 
	}
}; 



int main(int argc, char** argv){
	int N = 0;
	int K = 0;
	int E = 0; 
	cout << "Type the value of N: ";
	cin >> N;
	cout << "Type the value of K: ";
	cin >> K;
	cout << "Type the value of E: "; 
	cin >> E; 

	if (argc < 2) {
		cerr << "Not enough arguments\n";
		return 0;
	}

	// 1.) read in the file that you have as an argument into the imread function. 
	Mat referenceImage = imread(argv[1]);

	// 2.) Make a pseudo random number generator with the seed 1. 
	default_random_engine randEngine(1);

	// vertexCounts is an array of integers that has the numbr of vertices of each polygon
	int vertexCounts[100];
	for (int i = 0; i < 100; i++) vertexCounts[i] = 3; //In this case each polygon only has 3 vertices (all of the polygons are triangles)



	// 3.) Uniformly distribute random x and y values of points within the image bounds. 
	uniform_int_distribution<int> xValGen(0, referenceImage.cols - 1);
	uniform_int_distribution<int> yValGen(0, referenceImage.rows - 1);

	// 4.) Create distributions for generating color and opacity values
	uniform_int_distribution<int> BGRgen(0, 255);
	uniform_real_distribution<double> alphaGen(0, 1);


	vector <Image> ParentPictures;//Container for N polygon images stored in.  
	for (int i = 0; i < N; ++i){

		Point polyArr[100][6];
		for (int i = 0; i < 100; i++) {
			for (int j = 0; j < vertexCounts[i]; j++) {
				//Initialize each point randomly
				polyArr[i][j] = Point(xValGen(randEngine), yValGen(randEngine));
			}
		}

		const Point* polygons[100];
		//Array of constant pointers to arrays of Points
		for (int i = 0; i < 100; i++) {
			// Initialize each pointer to point to a polygon
			polygons[i] = &polyArr[i][0];
		}

		Scalar colors[100];
		for (int i = 0; i < 100; i++) {
			// Randomly initialize each color value
			colors[i] = Scalar(BGRgen(randEngine), BGRgen(randEngine),
				BGRgen(randEngine), alphaGen(randEngine));
		}

		/*Mat polyImage = renderPolyImage(
		referenceImage.cols,
		referenceImage.rows,
		100,
		polygons,
		vertexCounts,
		colors
		);*/
		Image pic(polyArr, polygons, colors, 0);
		ParentPictures.push_back(pic);

	}
	
	vector <double> scores; 
	vector <Image> KidPictures; 
	uniform_int_distribution<int> Parent1(0, N - 1);
	uniform_int_distribution<int> Parent2(0, N - 1);
	uniform_int_distribution<int> kidtomutate(0, K - 1);
	uniform_int_distribution<int> cutOff(0, 99);
	uniform_int_distribution<int> mutationtype(1, 4);
	//uniform_int_distribution<int> howmanymutatedpolygons(1, 2);
	uniform_int_distribution<int> whichcolor(0, 99); 
	uniform_int_distribution<int> swap1(0,99); 
	uniform_int_distribution<int> swap2(0,99); 
	uniform_int_distribution<int> verticechange(0,2); 
	uniform_int_distribution<int> verticechange1(0,99); 
	
	int C = 0;
	int p1 = 0;
	int p2 = 0;
	int mutation = 0; 
	int kidmutate = 0; 
	int howmany = 0; 
	int pixel = 0;
	int swp1 = 0; 
	int swp2 = 0;
	int vertice1 = 0; 
	int vertice2 = 0; 
	//Randomly Pick K pairs of parents and perform cross-over, get K children
	for(int generation = 0; generation < ((E - N) / K) - 1; ++generation){
		if (N > 1){ 
			for (int pair = 0; pair < K; ++pair){
			
				Point polyArrChild[100][6];
				const Point* polygonsChild[100];
				Scalar colorsChild[100];

				C = cutOff(randEngine);
				p1 = Parent1(randEngine);

				//cout << "Parent1 before " << p1 << endl; 
				p2 = Parent2(randEngine);
				//cout << "Parent2 before: " << p2 << endl; 
				
				//Get the first C points from parent 1 and put it  in the child. 
				for (int i = 0; i < C; ++i){
					colorsChild[i] = ParentPictures[p1].colors[i];
					for (int j = 0; j < 3; ++j){
						polyArrChild[i][j] = ParentPictures[p1].polyArr[i][j];
					}
				}

				//Get the C-99 points from parent 2 and put it in the child. 
				for (int i = C; i < 100; ++i){ //FIX THIS
					colorsChild[i] = ParentPictures[p2].colors[i];
					for (int j = 0; j < 3; ++j){
						polyArrChild[i][j] = ParentPictures[p2].polyArr[i][j];
					}
				}

				for (int i = 0; i < 100; ++i){
					polygonsChild[i] = &polyArrChild[i][0];
				}
				Image kid(polyArrChild, polygonsChild, colorsChild, 0);
				KidPictures.push_back(kid); 
			}
			
		} //END N > 1
		else{
			for(int pair = 0; pair < K; ++pair){
				Point polyArrChild[100][6];
				const Point* polygonsChild[100];
				Scalar colorsChild[100];
				
				for (int i = 0; i < 100; ++i){
					for (int j = 0; j < 3; ++j){
						polyArrChild[i][j] = ParentPictures[0].polyArr[i][j]; 
					}
				}
				
				for(int i = 0; i < 100; ++i){
					colorsChild[i] = ParentPictures[0].colors[i]; 
					polygonsChild[i] = &polyArrChild[i][0]; 
				}
				
				
				Image kid(polyArrChild, polygonsChild, colorsChild,0); 
				KidPictures.push_back(kid); 
		
			}
		}
	
	
		mutation = mutationtype(randEngine); 
		
	 
		//double percentchange = 0; 
	//SOMEWHERE BETWEEN AFTER THIS
	
		if (mutation == 1){//Swap the positions of two polygons in its ordered list.
		//cout << "MUTATE 1" << endl; 
			//howmany = howmanymutatedpolygons(randEngine); 
			//for(int i = 0; i < howmany; ++i){
			Point temp[1][3];
			kidmutate = kidtomutate(randEngine);  
			swp1 = swap1(randEngine); 
			swp2 = swap2(randEngine); 
			//cout << "swp1: " << swp1 << " swp2: " << swp2 << endl; //HELP!
			for(int i = 0; i < 3; ++i){
				temp[0][i] = KidPictures[kidmutate].polyArr[swp1][i]; 
			}
			for(int i = 0; i < 3; ++i){
				KidPictures[kidmutate].polyArr[swp1][i] = KidPictures[kidmutate].polyArr[swp2][i];  
			}
			for(int i = 0; i < 3; ++i){
				KidPictures[kidmutate].polyArr[swp2][i] = temp[0][i]; 
			}
			//cout << "MUTATE 1 end" << endl; 
			//}
		}
		else if (mutation == 2){ //Change position of polygon's vertice position NEED HELP WITH THIS
			//cout << "MUTATE 2" << endl; 
		 
			kidmutate = kidtomutate(randEngine);
			vertice1 = verticechange1(randEngine); 
			vertice2 = verticechange(randEngine); 
			//polyArr[i][j] = Point(xValGen(randEngine), yValGen(randEngine));
			//verticechange1
			KidPictures[kidmutate].polyArr[vertice1][vertice2] = Point(xValGen(randEngine), yValGen(randEngine));
			
		}
		else if (mutation == 3){ //Change the opacity
		//cout << "MUTATE 3" << endl; 
			//howmany = howmanymutatedpolygons(randEngine); 
			//for(int i = 0; i < howmany; ++i){
				pixel = whichcolor(randEngine); //MIGHT HAVE TO USE DIFFERENT RANDOM ENGINE 
				kidmutate = kidtomutate(randEngine); 
				KidPictures[kidmutate].colors[pixel].val[3] = alphaGen(randEngine);
			//} 
		}
		else{ //Change the RGB 
		//cout << "MUTATE 4" << endl; 
			//howmany = howmanymutatedpolygons(randEngine);
			//for (int i = 0; i < howmany; ++i){
				pixel = whichcolor(randEngine); 
				kidmutate = kidtomutate(randEngine); 
				KidPictures[kidmutate].colors[pixel].val[0] = BGRgen(randEngine);
				KidPictures[kidmutate].colors[pixel].val[1] = BGRgen(randEngine); 
				KidPictures[kidmutate].colors[pixel].val[2] = BGRgen(randEngine); 
			//}
		}
		//SOMEWHERE END OF THIS

		//Add the children to the population. 
		for(int i = 0; i < static_cast<int>(KidPictures.size()); ++i){
			ParentPictures.push_back(KidPictures[i]); 
		}
		//Since the children were added to the main population, get rid of children from the KidPictures container vector. 
		KidPictures.clear(); 

		//Calculating the fitnessscore for each picture in the ParentPictures vector. 
		for(int i = 0; i < static_cast<int>(ParentPictures.size()); ++i){
		
			Mat polyImage = renderPolyImage(
				referenceImage.cols, 
				referenceImage.rows, 
				100, 
				ParentPictures[i].polygons, 
				vertexCounts, 
				ParentPictures[i].colors
				);
			double polyImScore = score(polyImage, referenceImage);
			ParentPictures[i].score = polyImScore; 
		} 
	
		//Sort the Population fitness scores by highest to lowest.
		sort(ParentPictures.begin(), ParentPictures.end(), ScoreSort()); 
	
		//for(int i = 0; i < static_cast<int>(ParentPictures.size()); ++i){
			//cout << "[" << i << "]begin GENERATION " << generation << ": " << ParentPictures[i].score << endl; 
		//}
	
		//Erase the last K lowest fitness scored Images.
		for(int i = 0; i < K; ++i){
			ParentPictures.pop_back();  
		}
		
		if(generation == 50000){
			scores.push_back(ParentPictures[0].score); 
		}
		else if(generation == 100000){
			scores.push_back(ParentPictures[0].score); 
		}
		else if(generation == 150000){
			scores.push_back(ParentPictures[0].score); 
		}
		else if(generation == 200000){
			scores.push_back(ParentPictures[0].score); 
		}
		else if(generation == 250000){
			scores.push_back(ParentPictures[0].score); 
		}
		
		/*cout << endl; 
		for(int i = 0; i < static_cast<int>(ParentPictures.size()); ++i){
			cout << "[" << i << "]end GENERATION " << generation << ": " << ParentPictures[i].score << endl; 
		}
		cout << endl;*/ 
		cout << "Generation " << generation << ": " << ParentPictures[0].score << endl; 
		/*if(generation % 1000 == 0){
			double polyImScore = ParentPictures[0].score;
			Mat polyImage = renderPolyImage(
				referenceImage.cols, 
				referenceImage.rows, 
				100, 
				ParentPictures[0].polygons, 
				vertexCounts, 
				ParentPictures[0].colors
			); 
			ostringstream fileOut;
			fileOut << "PolyImage_" << polyImScore << ".jpg";
			string filename = fileOut.str();
	

			// Use the OpenCV imwrite function to write the generated image to a file
			imwrite(filename, polyImage); //Extension determines write format
			cout << "Wrote image file " << filename << '\n';
		}*/
	}
	
	
	for(int i = 0; i < static_cast<int>(scores.size()); ++i){
		cout << "Fitness: " << scores[i] << endl; 
	} 
	
	double polyImScore = ParentPictures[0].score;
	Mat polyImage = renderPolyImage(
			referenceImage.cols, 
			referenceImage.rows, 
			100, 
			ParentPictures[0].polygons, 
			vertexCounts, 
			ParentPictures[0].colors
			); 
	ostringstream fileOut;
	fileOut << "PolyImage_" << polyImScore << ".jpg";
	string filename = fileOut.str();
	

	// Use the OpenCV imwrite function to write the generated image to a file
	imwrite(filename, polyImage); //Extension determines write format
	cout << "Wrote image file " << filename << '\n';

	return 0;
}

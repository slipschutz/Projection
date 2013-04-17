

#include <iostream>
#include "TH2F.h"
#include "TH1D.h"

#include "TGraphErrors.h"

#include <vector>
using namespace std;

typedef struct{
  Double_t x;
  Double_t y;
  Double_t ex;
  Double_t ey;
}Point;

class Projection{

public:
  Projection();


  inline void SetData(TH2F* h){theData=h;}
  inline void SetNumOfProjections(int n){numOfProjections=n;}
  inline void SetRange(Double_t low,Double_t high){range_start=low;
    range_end=high;}

  inline vector<TH1D*> getProjections(){return theProjections;}

  void MakeProjections();

  vector <double> GetGraphCoef();

  inline TGraphErrors* getGraph(){return theGraph;}
  
  void Reset();

private:
  TH2F * theData;

  int numOfProjections;
  Double_t range_start;//lower y bound
  Double_t range_end;//upper y bound

  Double_t fitWidth;

  TGraphErrors * theGraph;
  
  vector <Point> thePoints;

  vector<TH1D*> theProjections;
};

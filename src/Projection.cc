



#include "Projection.hh"
#include <sstream>
#include "TF1.h"
#include "TFitResult.h"
          
Projection::Projection(){

  //set some defualts
  numOfProjections =100;

  //range is the range of y values to consider
  range_start=0;
  range_end=1000;
  fitWidth=0.8;
  
}

void Projection::MakeProjections(){

  thePoints.clear();
  theProjections.clear();
  stringstream name;

  double step = (range_end-range_start)/numOfProjections;

  for (int i=0;i<numOfProjections;i++){
    name.str("");//clear the stream
    Int_t bin1 = theData->GetYaxis()->FindBin(range_start +i*step);
    Int_t bin2 = theData->GetYaxis()->FindBin(range_start +(i+1)*step);
    
    name<<theData->GetName()<<"_px_"<<i<<"_"<<i+1;
    theProjections.push_back(theData->ProjectionX(name.str().c_str(),bin1,bin2));

    Double_t maxBinValue=theProjections[i]->
      GetXaxis()->GetBinCenter((theProjections[i]->GetMaximumBin()));

    theProjections[i]->SetTitle(name.str().c_str());
    TF1 f("f1","gaus",maxBinValue-fitWidth,maxBinValue+fitWidth);

    TFitResultPtr theFitResult;
    theFitResult = theProjections[i]->Fit("f1","QSNR");//do the fit
    
    Int_t status = theFitResult;//get fit status
    if (status == 0 ){ //A good fit
      Point p;
      p.x=range_start +i*step +(step/2.0);
      p.y=theFitResult->Value(1);
      p.ex=0;
      p.ey=theFitResult->UpperError(1);
      thePoints.push_back(p);
    } else{
      Point p;
      p.x=0;
      p.y=0;
      p.ex=0;
      p.ey=0;
      thePoints.push_back(p);


    }
    
  }
  GetGraphCoef();
}

vector <double> Projection::GetGraphCoef(){

  Double_t x[thePoints.size()];
  Double_t y[thePoints.size()];
  Double_t ey[thePoints.size()];
  Double_t ex[thePoints.size()];
  
  for (int i=0;i<thePoints.size();i++){
    x[i]=thePoints[i].x;
    y[i]=thePoints[i].y;
    ex[i]=thePoints[i].ex;
    ey[i]=thePoints[i].ey;
  }

  theGraph =new TGraphErrors(thePoints.size(),x,y,ex,ey);


  TF1 f("f2","pol3",range_start,range_end);
  TFitResultPtr result=theGraph->Fit("f2","QSNR");
  
  Int_t status = result;
  vector<double>v;
  if (status==0){
    for (int i=0;i<3;i++)
      v.push_back(result->Value(i));
  }else 
    v.resize(3,0);

  return v;
}

void Projection::Reset(){

  delete theData;
  delete theGraph;
  thePoints.clear();
  theProjections.clear();


}

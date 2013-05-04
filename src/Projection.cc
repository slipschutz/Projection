



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

  //how far in each direction to fit the projecions
  //in order to find centriod value
  fitWidth=0.8;



  theData=0;
  theGraph=0;
  
}

void Projection::MakeResProjections(){

  thePoints.clear();
  theProjections.clear();
  stringstream name;

  double step = (range_end-range_start)/numOfProjections;

  for (int i=0;i<numOfProjections;i++){
    name.str("");//clear the stream
    Int_t bin1 = theData->GetYaxis()->FindBin(range_start +i*step);
    Int_t bin2=  theData->GetYaxis()->FindBin(range_end);

    
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
      p.y=theFitResult->Value(2);
      p.ex=0;
      p.ey=theFitResult->UpperError(2);
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



}


void Projection::MakeProjections(){



  thePoints.clear();
  theProjections.clear();
  stringstream name;




  if (theData->GetEntries()<5000){// if there is very little data 
    //don't try and make prjections 
    //make zero points

    for (int i=0;i<numOfProjections;i++){
      Point p;
      p.x=0;
      p.y=0;
      p.ex=0;
      p.ey=0;
      thePoints.push_back(p);
    }
    return;
  }


  


  double step = (range_end-range_start)/numOfProjections;

  for (int i=0;i<numOfProjections;i++){
    name.str("");//clear the stream
    Int_t bin1 = theData->GetYaxis()->FindBin(range_start +i*step);
    Int_t bin2= theData->GetYaxis()->FindBin(range_start +(i+1)*step);;

    
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

}

vector <double> Projection::GetGraphCoef(){



  Double_t x[thePoints.size()];
  Double_t y[thePoints.size()];
  Double_t ey[thePoints.size()];
  Double_t ex[thePoints.size()];

  int count =0;

  //TGRaph erros need things to be in
  //c arrays
  for (int i=0;i<thePoints.size();i++){
    x[i]=thePoints[i].x;
    y[i]=thePoints[i].y;
    ex[i]=thePoints[i].ex;
    ey[i]=thePoints[i].ey;
    
    if (x[i]==0 && y[i]==0 && ex[i]==0&&ey[i]==0)
      count++;

  }
  vector<double>v;
  theGraph =new TGraphErrors(thePoints.size(),x,y,ex,ey);

  if (count >= thePoints.size()*0.25 ){
    //Too many zeros in points
    //from low count histogram default to 0s
    v.resize(3,0);
    return v;
  }
    
  

  
  stringstream s;
  s<<theData->GetName()<<"_Graph";
  theGraph->SetName(s.str().c_str());

  TF1 f("f2","pol3",range_start,range_end);
  TFitResultPtr result=theGraph->Fit("f2","QSNR");
  
  Int_t status = result;

  if (status==0){
    for (int i=1;i<=3;i++)
      v.push_back(result->Value(i));
  }else 
    v.resize(3,0);

  return v;
}

void Projection::Reset(){

  if (theData != 0){
    delete theData;
    theData=0;
  }
    if (theGraph != 0){
   delete theGraph;
   theGraph=0;
    }

    thePoints.clear();
    theProjections.clear();
}

void Projection::autoRange(){


  TH1D * _temp;
  //of the TOF

  Double_t frac=0.05;//precentage threshold

  //_temp=theData->ProjectionY();//Project all bins to _temp

  _temp=theData->ProjectionY("Temp__");//Project all bins to te

  Int_t maxBin = _temp->GetMaximumBin();
  Double_t maxVal = _temp->GetBinContent(maxBin);  

  //root histos have number of bins +2
  //1 at index 0 for underflow
  //1 at index number of bins +1 for overflow
  for (int i=maxBin;i<_temp->GetSize()-2;++i){
    if (_temp->GetBinContent(i)<=frac*maxVal){
      range_end=_temp->GetXaxis()->GetBinCenter(i);
      break;
    }
  }
  for (int i=0;i<maxBin;++i){
      if (_temp->GetBinContent(i)>=frac*maxVal){
	range_start=_temp->GetXaxis()->GetBinCenter(i);
	break;
      }
    }
 
  //  cout<<"Range start "<<range_start<<" range end "<<range_end<<endl;

       
}

void Projection::SetRange(string s){
  if(s=="auto"){
    autoRange();				\
  }else{
    cout<<"Invalid"<<endl;
  }
}

#include "RecoLocalCalo/HcalRecAlgos/interface/NNInference.h"
#include <iostream>
#include <cmath>


//--------------------------------------------------------------------------------------------------
NNInference::NNInference()
{
  NNvectorVar_.clear();
}

//--------------------------------------------------------------------------------------------------
NNInference::~NNInference() {
  tensorflow::closeSession(session);
  delete graphDef;
}

void NNInference::initialize(const std::string iWeightFile){
  std::string cmssw_base_src = getenv("CMSSW_BASE");
  graphDef= tensorflow::loadGraphDef((cmssw_base_src+"/src/"+iWeightFile).c_str());
  session = tensorflow::createSession(graphDef);
}

void NNInference::SetNNVectorVar(){

    NNvectorVar_.clear();

    NNvectorVar_.push_back(fIeta) ;
    NNvectorVar_.push_back(fIphi) ;
    NNvectorVar_.push_back(fDepth) ;
    NNvectorVar_.push_back(fGain) ;
    NNvectorVar_.push_back(fRaw0) ;
    NNvectorVar_.push_back(fRaw1) ;
    NNvectorVar_.push_back(fRaw2) ;
    NNvectorVar_.push_back(fRaw3) ;
    NNvectorVar_.push_back(fRaw4) ;
    NNvectorVar_.push_back(fRaw5) ;
    NNvectorVar_.push_back(fRaw6) ;
    NNvectorVar_.push_back(fRaw7) ;
    NNvectorVar_.push_back(fPed0) ;
    NNvectorVar_.push_back(fPed1) ;
    NNvectorVar_.push_back(fPed2) ;
    NNvectorVar_.push_back(fPed3) ;
    NNvectorVar_.push_back(fPed4) ;
    NNvectorVar_.push_back(fPed5) ;
    NNvectorVar_.push_back(fPed6) ;
    NNvectorVar_.push_back(fPed7) ;

}

float NNInference::EvaluateNN(){
    tensorflow::Tensor input(tensorflow::DT_FLOAT, {1,(unsigned int)NNvectorVar_.size()});
    for (unsigned int i = 0; i < NNvectorVar_.size(); i++){
        input.matrix<float>()(0,i) =  float(NNvectorVar_[i]);
    }
    std::vector<tensorflow::Tensor> outputs;
    tensorflow::run(session, { { "input",input } }, { "output" }, &outputs);
    float inference = outputs[0].matrix<float>()(0, 0);
    return inference;

}


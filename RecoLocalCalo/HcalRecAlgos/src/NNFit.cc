#include "RecoLocalCalo/HcalRecAlgos/interface/NNFit.h"

NNFit::NNFit(){
}

NNFit::~NNFit(){
}

void NNFit::init() {
}

void NNFit::phase1Apply(const HBHEChannelInfo& channelData,
			float& reconstructedEnergy, NNInference* fNN, const HcalDetId& cell) const
{


  std::vector<float> inputCharge;
  std::vector<float> inputPedestal; 
  float ieta = (float)cell.ieta();
  float iphi = (float)cell.iphi();
  float depth = (float)cell.depth();
  float gainCorr = (float)channelData.tsGain(1);
  for(unsigned int ip=0; ip<channelData.nSamples(); ip++){

    float charge = (float)channelData.tsRawCharge(ip);
    float ped = (float)channelData.tsPedestal(ip); 
    //double noise = channelData.tsPedestalWidth(ip);
    //double gain = channelData.tsGain(ip);

    inputCharge.push_back(charge);
    inputPedestal.push_back(ped);
    //inputNoise.push_back(noise);
  } 

  fNN->fIeta = ieta;
  fNN->fIphi = iphi; 
  fNN->fDepth = depth;
  fNN->fGain = gainCorr;

  fNN->fRaw0 = inputCharge[0];
  fNN->fRaw1 = inputCharge[1];
  fNN->fRaw2 = inputCharge[2];
  fNN->fRaw3 = inputCharge[3];
  fNN->fRaw4 = inputCharge[4];
  fNN->fRaw5 = inputCharge[5];
  fNN->fRaw6 = inputCharge[6];
  fNN->fRaw7 = inputCharge[7];

  fNN->fPed0 = inputPedestal[0];
  fNN->fPed1 = inputPedestal[1];
  fNN->fPed2 = inputPedestal[2];
  fNN->fPed3 = inputPedestal[3];
  fNN->fPed4 = inputPedestal[4];
  fNN->fPed5 = inputPedestal[5];
  fNN->fPed6 = inputPedestal[6];
  fNN->fPed7 = inputPedestal[7];

  fNN->SetNNVectorVar();

  reconstructedEnergy = fNN->EvaluateNN();
}

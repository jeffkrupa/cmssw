#ifndef RecoLocalCalo_HcalRecAlgos_NNInference_hh
#define RecoLocalCalo_HcalRecAlgos_NNInference_hh

#include <string>
#include "PhysicsTools/TensorFlow/interface/TensorFlow.h"
#include "DataFormats/HcalDetId/interface/HcalDetId.h"
#include "DataFormats/HcalRecHit/interface/HBHEChannelInfo.h"

class NNInference  {
    public:
      NNInference();
      ~NNInference();

      void initialize(const std::string iWeightFile);
      void SetNNVectorVar();
      float EvaluateNN();

      float fIeta;
      float fIphi;
      float fDepth;
      float fGain;
      float fRaw0;
      float fRaw1;
      float fRaw2;
      float fRaw3;
      float fRaw4;
      float fRaw5;
      float fRaw6;
      float fRaw7;
      float fPed0;
      float fPed1;
      float fPed2;
      float fPed3;
      float fPed4;
      float fPed5;
      float fPed6;
      float fPed7;

    private:
      tensorflow::Session* session;
      tensorflow::GraphDef* graphDef;
      std::vector<float> NNvectorVar_;
};

#endif

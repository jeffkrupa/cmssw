#ifndef RecoLocalCalo_HcalRecAlgos_NNFit_H
#define RecoLocalCalo_HcalRecAlgos_NNFit_H

#include "PhysicsTools/TensorFlow/interface/TensorFlow.h"
#include "DataFormats/HcalDetId/interface/HcalDetId.h"
#include "DataFormats/HcalRecHit/interface/HBHEChannelInfo.h"
#include "RecoLocalCalo/HcalRecAlgos/interface/NNInference.h"
class NNFit
{

 public:
 NNFit();
 ~NNFit();

   void phase1Apply(const HBHEChannelInfo& channelData,  
		    float& reconstructedEnergy, NNInference* fNN, const HcalDetId& cell) const;

   void init();
};

#endif

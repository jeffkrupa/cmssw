#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "Geometry/CaloGeometry/interface/CaloSubdetectorGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloCellGeometry.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"
#include "Geometry/CaloTopology/interface/HcalTopology.h"
#include "Geometry/HcalCommonData/interface/HcalHitRelabeller.h"
#include "DataFormats/HcalRecHit/interface/HcalRecHitCollections.h"
#include "HeterogeneousCore/SonicTriton/interface/TritonClient.h"
#include "HeterogeneousCore/SonicCore/interface/SonicEDProducer.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "DataFormats/HcalRecHit/interface/HBHERecHit.h"

//#include "RecoLocalCalo/HcalRecProducers/src/HcalPhase1Reconstructor_FACILE.h"
class HcalReconstructor : public SonicEDProducer<TritonClient>
{
public:
    explicit HcalReconstructor(edm::ParameterSet const& cfg) : 
        SonicEDProducer<TritonClient>(cfg),
        fChannelInfoName_(cfg.getParameter<edm::InputTag>("ChannelInfoName")),
        fTokChannelInfo(this->consumes<HBHEChannelInfoCollection>(fChannelInfoName_))
     
    {
        this->produces<HBHERecHitCollection>();
        this->setDebugName("HcalReconstructor");
    }

    void acquire(edm::Event const& iEvent, edm::EventSetup const& iSetup, Input& iInput) override {

	const HBHEChannelInfoCollection *channelInfo   = 0;
	edm::Handle<HBHEChannelInfoCollection> hChannelInfo;
	iEvent.getByToken(fTokChannelInfo_, hChannelInfo); 
	channelInfo = hChannelInfo.product();

	auto& input1 = iInput.begin()->second;
	auto data1 = std::make_shared<TritonInput<float>>();
	data1->reserve(std::distance(channelInfo->begin(), channelInfo->end()));

	hcalIds_.clear(); 
	
	for(HBHEChannelInfoCollection::const_iterator itC = channelInfo->begin(); itC != channelInfo->end(); itC++){

	    std::vector<float> input;
	    const HBHEChannelInfo& pChannel(*itC);
  	    const HcalDetId        pDetId = pChannel.id();
 	    hcalIds_.push_back(pDetId);    

	    //FACILE uses iphi as a continuous variable
	    input.push_back((float)pDetId.iphi());
            input.push_back((float)pChannel.tsGain(0.));
	    for (unsigned int itTS=0; itTS < pChannel.nSamples(); ++itTS) {
		input.push_back((float)pChannel.tsRawCharge(itTS));
	    }
  
	    //FACILE considers 7 Hcal depths as binary variables
            for (int itDepth=1; itDepth < 8; itDepth++){
		if (pDetId.depth() == itDepth)  input.push_back(1.f);
		else				input.push_back(0.f);
	    }

	    //ieta is also encoded as a binary variable
	    for (int itIeta = 0; itIeta < 30; itIeta++){
		if (std::abs(pDetId.ieta()) == itIeta)  input.push_back(1.f);
		else					input.push_back(0.f);
	    }

	    data1->push_back(input);
	}

	//set batch at maximum RHcollsize; pad the remainder
        if(std::distance(channelInfo->begin(), channelInfo->end()) < input1.batchSize()){
	    std::vector<float> pad(47,0.f);
 	    for(unsigned int iP = last; iP != input1.batchSize(); iP++){
                data1->push_back(pad);
	    }
        } 

	input1.toServer(data1);
    }

    void produce(edm::Event& iEvent, edm::EventSetup const& iSetup, Output const& iOutput) override {
	std::unique_ptr<HBHERecHitCollection> out;
	out = std::make_unique<HBHERecHitCollection>();
	out->reserve(hcalIds_.size());

	const auto& output1 = iOutput.begin()->second;
	const auto& outputs = output1.fromServer<float>();

	for(std::size_t iB = 0; iB < hcalIds_.size(); iB++){

	    float rhE = outputs[iB][0];
	    if(rhE < 0.) rhE = 0.;//shouldn't be necessary, relu activation function
	    //exception?
	    if(std::isnan(rhE)) rhE = 0; 
	    if(std::isinf(rhE)) rhE = 0; 

	    //FACILE does no time reco 
	    HBHERecHit rh = HBHERecHit(hcalIds_[iB],rhE,0.f,0.f);
	    out->push_back(rh); 
	}
	iEvent.put(std::move(out));	
    }

    /*static void fillDescriptions(edm::ConfigurationDescriptions & descriptions) {
        edm::ParameterSetDescription desc;
        TritonClient::fillPSetDescription(desc);
        //add producer-specific parameters
 	desc.add<edm::InputTag>("ChannelInfoName","hbheprereco");
        descriptions.add("HcalPhase1Reconstructor_FACILE",desc);
    }*/


private:
    edm::InputTag fChannelInfoName_;   
    edm::EDGetTokenT<HBHEChannelInfoCollection> fTokChannelInfo_;
    std::vector<HcalDetId> hcalIds_;
};

DEFINE_FWK_MODULE(HcalReconstructor);

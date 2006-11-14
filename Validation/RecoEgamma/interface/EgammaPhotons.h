#ifndef Validation_RecoEgamma_EgammaPhotons_h
#define Validation_RecoEgamma_EgammaPhotons_h

/**\class EgammaPhotons

 Description: SVSuite Photon Validation

 Implementation:
     \\\author: Michael A. Balazs, Nov 2006
*/
//
// $Id: EgammaPhotons.h,v 1.1 2006/11/13 19:20:39 mabalazs Exp $
//
#include <string>

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DQMServices/Core/interface/DaqMonitorBEInterface.h"
#include "DQMServices/Daemon/interface/MonitorDaemon.h"

class EgammaPhotons : public edm::EDAnalyzer
{
	public:
      explicit EgammaPhotons( const edm::ParameterSet& );
      ~EgammaPhotons();

      virtual void analyze( const edm::Event&, const edm::EventSetup& );
      virtual void beginJob(edm::EventSetup const&);
      virtual void endJob();

	private:
			std::string outputFile_;
			std::string CMSSW_Version_;

			bool verboseDBE_;
			DaqMonitorBEInterface* dbe_;

      edm::InputTag PhotonCollection_;
 
      MonitorElement* hist_Photon_Size_;

      double hist_min_Size_;
      double hist_max_Size_;
      int    hist_bins_Size_;

      MonitorElement* hist_Photon_ET_;

      double hist_min_ET_;
      double hist_max_ET_;
      int    hist_bins_ET_;

      MonitorElement* hist_Photon_Eta_;

      double hist_min_Eta_;
      double hist_max_Eta_;
      int    hist_bins_Eta_;

      MonitorElement* hist_Photon_Phi_;

      double hist_min_Phi_;
      double hist_max_Phi_;
      int    hist_bins_Phi_;
};
#endif

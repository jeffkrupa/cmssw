#include <iostream>
//
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
//
#include "Validation/RecoEgamma/interface/PhotonValidator.h"

//
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Utilities/interface/Exception.h"
//
#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "SimDataFormats/Track/interface/SimTrack.h"
#include "SimDataFormats/Track/interface/SimTrackContainer.h"
#include "SimDataFormats/Vertex/interface/SimVertex.h"
#include "SimDataFormats/Vertex/interface/SimVertexContainer.h"
//
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticleFwd.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingVertexContainer.h"
//
#include "SimTracker/TrackerHitAssociation/interface/TrackerHitAssociator.h"
#include "SimTracker/Records/interface/TrackAssociatorRecord.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "Geometry/CommonDetUnit/interface/GeomDet.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/CommonDetUnit/interface/GeomDetUnit.h"
//
#include "TrackingTools/TrajectoryState/interface/FreeTrajectoryState.h"
#include "TrackingTools/PatternTools/interface/TwoTrackMinimumDistance.h"
#include "TrackingTools/TransientTrack/interface/TrackTransientTrack.h"
//
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "CLHEP/Units/GlobalPhysicalConstants.h"
#include "CommonTools/Statistics/interface/ChiSquaredProbability.h"

//
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackExtra.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/EgammaCandidates/interface/Conversion.h"
#include "DataFormats/EgammaCandidates/interface/ConversionFwd.h"
#include "DataFormats/EgammaCandidates/interface/Photon.h"
#include "DataFormats/EgammaCandidates/interface/PhotonFwd.h"
#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "DataFormats/TrackCandidate/interface/TrackCandidateCollection.h"
#include "DataFormats/TrackingRecHit/interface/TrackingRecHit.h"
#include "DataFormats/EgammaReco/interface/BasicCluster.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/EgammaReco/interface/BasicCluster.h"
#include "DataFormats/EgammaReco/interface/BasicClusterFwd.h"
#include "DataFormats/CaloRecHit/interface/CaloCluster.h"
#include "DataFormats/CaloRecHit/interface/CaloClusterFwd.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

//
#include "RecoEgamma/EgammaMCTools/interface/PhotonMCTruthFinder.h"
#include "RecoEgamma/EgammaMCTools/interface/PhotonMCTruth.h"
#include "RecoEgamma/EgammaMCTools/interface/ElectronMCTruth.h"
#include "RecoEcal/EgammaCoreTools/interface/EcalClusterTools.h"
#include "RecoCaloTools/MetaCollections/interface/CaloRecHitMetaCollections.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"
#include "Geometry/CaloTopology/interface/CaloTopology.h"
#include "Geometry/CaloEventSetup/interface/CaloTopologyRecord.h"
//
//#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
//
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"
#include "TVector3.h"
#include "TProfile.h"
// 
/** \class PhotonValidator
 **  
 **
 **  $Id: PhotonValidator
 **  $Date: 2010/09/24 16:30:14 $ 
 **  $Revision: 1.62 $
 **  \author Nancy Marinelli, U. of Notre Dame, US
 **
 ***/

using namespace std;

 
PhotonValidator::PhotonValidator( const edm::ParameterSet& pset )
  {

    fName_     = pset.getUntrackedParameter<std::string>("Name");
    verbosity_ = pset.getUntrackedParameter<int>("Verbosity");
    parameters_ = pset;

    
    photonCollectionProducer_ = pset.getParameter<std::string>("phoProducer");
    photonCollection_ = pset.getParameter<std::string>("photonCollection");

   
    label_tp_   = pset.getParameter<edm::InputTag>("label_tp");

    barrelEcalHits_   = pset.getParameter<edm::InputTag>("barrelEcalHits");
    endcapEcalHits_   = pset.getParameter<edm::InputTag>("endcapEcalHits");

    conversionOITrackProducer_ = pset.getParameter<std::string>("conversionOITrackProducer");
    conversionIOTrackProducer_ = pset.getParameter<std::string>("conversionIOTrackProducer");

   
    minPhoEtCut_ = pset.getParameter<double>("minPhoEtCut");   
    convTrackMinPtCut_ = pset.getParameter<double>("convTrackMinPtCut");   
    likelihoodCut_ = pset.getParameter<double>("likelihoodCut");

    trkIsolExtRadius_ = pset.getParameter<double>("trkIsolExtR");   
    trkIsolInnRadius_ = pset.getParameter<double>("trkIsolInnR");   
    trkPtLow_     = pset.getParameter<double>("minTrackPtCut");   
    lip_       = pset.getParameter<double>("lipCut");   
    ecalIsolRadius_ = pset.getParameter<double>("ecalIsolR");   
    bcEtLow_     = pset.getParameter<double>("minBcEtCut");   
    hcalIsolExtRadius_ = pset.getParameter<double>("hcalIsolExtR");   
    hcalIsolInnRadius_ = pset.getParameter<double>("hcalIsolInnR");   
    hcalHitEtLow_     = pset.getParameter<double>("minHcalHitEtCut");   

    numOfTracksInCone_ = pset.getParameter<int>("maxNumOfTracksInCone");   
    trkPtSumCut_  = pset.getParameter<double>("trkPtSumCut");   
    ecalEtSumCut_ = pset.getParameter<double>("ecalEtSumCut");   
    hcalEtSumCut_ = pset.getParameter<double>("hcalEtSumCut");   
    dCotCutOn_ = pset.getParameter<bool>("dCotCutOn");   
    dCotCutValue_ = pset.getParameter<double>("dCotCutValue");   
    dCotHardCutValue_ = pset.getParameter<double>("dCotHardCutValue");   
    signal_ = pset.getParameter<bool>("signal");
    fastSim_ =   pset.getParameter<bool>("fastSim");    
   

  }





PhotonValidator::~PhotonValidator() {}




void  PhotonValidator::beginJob() {

  nEvt_=0;
  nEntry_=0;
  nRecConv_=0;
  nRecConvAss_=0;
  nRecConvAssWithEcal_=0;
   
  nInvalidPCA_=0;
  
  dbe_ = 0;
  dbe_ = edm::Service<DQMStore>().operator->();
  



  double resMin = parameters_.getParameter<double>("resMin");
  double resMax = parameters_.getParameter<double>("resMax");
  int resBin = parameters_.getParameter<int>("resBin");

  double eMin = parameters_.getParameter<double>("eMin");
  double eMax = parameters_.getParameter<double>("eMax");
  int eBin = parameters_.getParameter<int>("eBin");

  double etMin = parameters_.getParameter<double>("etMin");
  double etMax = parameters_.getParameter<double>("etMax");
  int etBin = parameters_.getParameter<int>("etBin");

  double etScale = parameters_.getParameter<double>("etScale");

  double etaMin = parameters_.getParameter<double>("etaMin");
  double etaMax = parameters_.getParameter<double>("etaMax");
  int etaBin = parameters_.getParameter<int>("etaBin");
  int etaBin2 = parameters_.getParameter<int>("etaBin2");

  double dEtaMin = parameters_.getParameter<double>("dEtaMin");
  double dEtaMax = parameters_.getParameter<double>("dEtaMax");
  int dEtaBin = parameters_.getParameter<int>("dEtaBin");
 
  double phiMin = parameters_.getParameter<double>("phiMin");
  double phiMax = parameters_.getParameter<double>("phiMax");
  int    phiBin = parameters_.getParameter<int>("phiBin");

  double dPhiMin = parameters_.getParameter<double>("dPhiMin");
  double dPhiMax = parameters_.getParameter<double>("dPhiMax");
  int    dPhiBin = parameters_.getParameter<int>("dPhiBin");

  double rMin = parameters_.getParameter<double>("rMin");
  double rMax = parameters_.getParameter<double>("rMax");
  int    rBin = parameters_.getParameter<int>("rBin");

  double zMin = parameters_.getParameter<double>("zMin");
  double zMax = parameters_.getParameter<double>("zMax");
  int    zBin = parameters_.getParameter<int>("zBin");
 

 
  double r9Min = parameters_.getParameter<double>("r9Min"); 
  double r9Max = parameters_.getParameter<double>("r9Max"); 
  int r9Bin = parameters_.getParameter<int>("r9Bin");

  double dPhiTracksMin = parameters_.getParameter<double>("dPhiTracksMin"); 
  double dPhiTracksMax = parameters_.getParameter<double>("dPhiTracksMax"); 
  int dPhiTracksBin = parameters_.getParameter<int>("dPhiTracksBin"); 
  
  double dEtaTracksMin = parameters_.getParameter<double>("dEtaTracksMin"); 
  double dEtaTracksMax = parameters_.getParameter<double>("dEtaTracksMax"); 
  int    dEtaTracksBin = parameters_.getParameter<int>("dEtaTracksBin"); 

  double dCotTracksMin = parameters_.getParameter<double>("dCotTracksMin"); 
  double dCotTracksMax = parameters_.getParameter<double>("dCotTracksMax"); 
  int    dCotTracksBin = parameters_.getParameter<int>("dCotTracksBin"); 


  double povereMin = parameters_.getParameter<double>("povereMin");
  double povereMax = parameters_.getParameter<double>("povereMax");
  int povereBin = parameters_.getParameter<int>("povereBin");

  double eoverpMin = parameters_.getParameter<double>("eoverpMin"); 
  double eoverpMax = parameters_.getParameter<double>("eoverpMax"); 
  int    eoverpBin = parameters_.getParameter<int>("eoverpBin"); 

  double chi2Min = parameters_.getParameter<double>("chi2Min"); 
  double chi2Max = parameters_.getParameter<double>("chi2Max"); 

  int    ggMassBin = parameters_.getParameter<int>("ggMassBin");   
  double ggMassMin = parameters_.getParameter<double>("ggMassMin"); 
  double ggMassMax = parameters_.getParameter<double>("ggMassMax"); 


  double rMinForXray = parameters_.getParameter<double>("rMinForXray");
  double rMaxForXray = parameters_.getParameter<double>("rMaxForXray");
  int    rBinForXray = parameters_.getParameter<int>("rBinForXray");
  double zMinForXray = parameters_.getParameter<double>("zMinForXray");
  double zMaxForXray = parameters_.getParameter<double>("zMaxForXray");
  int    zBinForXray = parameters_.getParameter<int>("zBinForXray");
  int    zBin2ForXray = parameters_.getParameter<int>("zBin2ForXray");



  if (dbe_) {  
    //// All MC photons
    // SC from reco photons

    dbe_->setCurrentFolder("EgammaV/PhotonValidator/SimulationInfo");
    //
    // simulation information about all MC photons found
    std::string histname = "nOfSimPhotons";    
    h_nSimPho_[0] = dbe_->book1D(histname,"# of Sim photons per event ",20,-0.5,19.5); 
    histname = "SimPhoMotherEt";    
    h_SimPhoMotherEt_[0] = dbe_->book1D(histname,"Sim photon Mother tranverse energy spectrum",etBin,etMin,etMax);
    h_SimPhoMotherEta_[0] = dbe_->book1D("SimPhoMotherEta"," Sim Photon Mother Eta ",etaBin,etaMin, etaMax) ;
    histname = "SimPhoMotherEtMatched";    
    h_SimPhoMotherEt_[1] = dbe_->book1D(histname,"Sim photon  matched by a reco Photon: Mother tranverse energy spectrum",etBin,etMin,etMax);
    h_SimPhoMotherEta_[1] = dbe_->book1D("SimPhoMotherEtaMatched"," Sim Photon matched by a reco Photon:  Mother Eta ",etaBin,etaMin, etaMax) ;
    //
    histname = "h_SimPhoEta";
    h_SimPho_[0] =  dbe_->book1D(histname," All photons simulated #eta",etaBin,etaMin, etaMax);
    histname = "h_SimPhoPhi";
    h_SimPho_[1] =  dbe_->book1D(histname," All photons simulated #phi",phiBin,phiMin, phiMax);
    histname = "h_SimPhoEt";
    h_SimPho_[2] =  dbe_->book1D(histname," All photons simulated Et",etBin,etMin, etMax);
    // Numerators
    histname = "nOfSimPhotonsMatched";    
    h_nSimPho_[1] = dbe_->book1D(histname,"# of Sim photons matched by a reco Photon per event ",20,-0.5,19.5);
    histname = "h_MatchedSimPhoEta";
    h_MatchedSimPho_[0] =  dbe_->book1D(histname," Matching photons simulated #eta",etaBin,etaMin, etaMax);
    histname = "h_MatchedSimPhoPhi";
    h_MatchedSimPho_[1] =  dbe_->book1D(histname," Matching photons simulated #phi",phiBin,phiMin, phiMax);
    histname = "h_MatchedSimPhoEt";
    h_MatchedSimPho_[2] =  dbe_->book1D(histname," Matching photons simulated Et",etBin,etMin, etMax);
    //
    histname = "h_MatchedSimPhoBadChEta";
    h_MatchedSimPhoBadCh_[0] =  dbe_->book1D(histname," Matching photons simulated #eta",etaBin,etaMin, etaMax);
    histname = "h_MatchedSimPhoBadChPhi";
    h_MatchedSimPhoBadCh_[1] =  dbe_->book1D(histname," Matching photons simulated #phi",phiBin,phiMin, phiMax);
    histname = "h_MatchedSimPhoBadChEt";
    h_MatchedSimPhoBadCh_[2] =  dbe_->book1D(histname," Matching photons simulated Et",etBin,etMin, etMax);


    /// Histograms for efficiencies
    histname = "nOfSimConversions";    
    h_nSimConv_[0] = dbe_->book1D(histname,"# of Sim conversions per event ",20,-0.5,19.5);
    /// Denominators
    histname = "h_AllSimConvEta";
    h_AllSimConv_[0] =  dbe_->book1D(histname," All conversions: simulated #eta",etaBin2,etaMin,etaMax);
    histname = "h_AllSimConvPhi";
    h_AllSimConv_[1] =  dbe_->book1D(histname," All conversions: simulated #phi",phiBin,phiMin,phiMax);
    histname = "h_AllSimConvR";
    h_AllSimConv_[2] =  dbe_->book1D(histname," All conversions: simulated R",rBin,rMin,rMax);
    histname = "h_AllSimConvZ";
    h_AllSimConv_[3] =  dbe_->book1D(histname," All conversions: simulated Z",zBin,zMin,zMax);
    histname = "h_AllSimConvEt";
    h_AllSimConv_[4] =  dbe_->book1D(histname," All conversions: simulated Et",etBin,etMin,etMax);
    //
    histname = "nOfVisSimConversions";    
    h_nSimConv_[1] = dbe_->book1D(histname,"# of Sim conversions per event ",20,-0.5,19.5);
    histname = "h_VisSimConvEta";
    h_VisSimConv_[0] =  dbe_->book1D(histname," All vis conversions: simulated #eta",etaBin2,etaMin, etaMax);
    histname = "h_VisSimConvPhi";
    h_VisSimConv_[1] =  dbe_->book1D(histname," All vis conversions: simulated #phi",phiBin,phiMin, phiMax);
    histname = "h_VisSimConvR";
    h_VisSimConv_[2] =  dbe_->book1D(histname," All vis conversions: simulated R",rBin,rMin,rMax);
    histname = "h_VisSimConvZ";
    h_VisSimConv_[3] =  dbe_->book1D(histname," All vis conversions: simulated Z",zBin,zMin, zMax);
    histname = "h_VisSimConvEt";
    h_VisSimConv_[4] =  dbe_->book1D(histname," All vis conversions: simulated Et",etBin,etMin, etMax);
    /// Numerators 
    histname = "h_SimConvOneTracksEta";
    h_SimConvOneTracks_[0] =  dbe_->book1D(histname," All vis conversions with 1 reco  tracks: simulated #eta",etaBin2,etaMin, etaMax);
    histname = "h_SimConvOneTracksPhi";
    h_SimConvOneTracks_[1] =  dbe_->book1D(histname," All vis conversions with 1 reco  tracks: simulated #phi",phiBin,phiMin, phiMax);
    histname = "h_SimConvOneTracksR";
    h_SimConvOneTracks_[2] =  dbe_->book1D(histname," All vis conversions with 1 reco  tracks: simulated R",rBin,rMin, rMax);
    histname = "h_SimConvOneTracksZ";
    h_SimConvOneTracks_[3] =  dbe_->book1D(histname," All vis conversions with 1 reco  tracks: simulated Z",zBin,zMin, zMax);
    histname = "h_SimConvOneTracksEt";
    h_SimConvOneTracks_[4] =  dbe_->book1D(histname," All vis conversions with 1 reco  tracks: simulated Et",etBin,etMin, etMax);
    //
    histname = "h_SimConvTwoMTracksEta";
    h_SimConvTwoMTracks_[0] =  dbe_->book1D(histname," All vis conversions with 2 reco-matching tracks: simulated #eta",etaBin2,etaMin, etaMax);
    histname = "h_SimConvTwoMTracksPhi";
    h_SimConvTwoMTracks_[1] =  dbe_->book1D(histname," All vis conversions with 2 reco-matching tracks: simulated #phi",phiBin,phiMin, phiMax);
    histname = "h_SimConvTwoMTracksR";
    h_SimConvTwoMTracks_[2] =  dbe_->book1D(histname," All vis conversions with 2 reco-matching tracks: simulated R",rBin,rMin, rMax);
    histname = "h_SimConvTwoMTracksZ";
    h_SimConvTwoMTracks_[3] =  dbe_->book1D(histname," All vis conversions with 2 reco-matching tracks: simulated Z",zBin,zMin, zMax);
    histname = "h_SimConvTwoMTracksEt";
    h_SimConvTwoMTracks_[4] =  dbe_->book1D(histname," All vis conversions with 2 reco-matching tracks: simulated Et",etBin,etMin, etMax);
    //
    histname = "h_SimConvTwoTracksEta";
    h_SimConvTwoTracks_[0] =  dbe_->book1D(histname," All vis conversions with 2 reco  tracks: simulated #eta",etaBin2,etaMin, etaMax);
    histname = "h_SimConvTwoTracksPhi";
    h_SimConvTwoTracks_[1] =  dbe_->book1D(histname," All vis conversions with 2 reco tracks: simulated #phi",phiBin,phiMin, phiMax);
    histname = "h_SimConvTwoTracksR";
    h_SimConvTwoTracks_[2] =  dbe_->book1D(histname," All vis conversions with 2 reco tracks: simulated R",rBin,rMin, rMax);
    histname = "h_SimConvTwoTracksZ";
    h_SimConvTwoTracks_[3] =  dbe_->book1D(histname," All vis conversions with 2 reco tracks: simulated Z",zBin,zMin, zMax);
    histname = "h_SimConvTwoTracksEt";
    h_SimConvTwoTracks_[4] =  dbe_->book1D(histname," All vis conversions with 2 reco tracks: simulated Et",etBin,etMin, etMax);
    // 
    histname = "h_SimConvOneMTracksEta";
    h_SimConvOneMTracks_[0] =  dbe_->book1D(histname," All vis conversions with 1 reco-matching tracks: simulated #eta",etaBin2,etaMin, etaMax);
    histname = "h_SimConvOneMTracksPhi";
    h_SimConvOneMTracks_[1] =  dbe_->book1D(histname," All vis conversions with 1 reco-matching tracks: simulated #phi",phiBin,phiMin, phiMax);
    histname = "h_SimConvOneMTracksR";
    h_SimConvOneMTracks_[2] =  dbe_->book1D(histname," All vis conversions with 1 reco-matching tracks: simulated R",rBin,rMin, rMax);
    histname = "h_SimConvOneMTracksZ";
    h_SimConvOneMTracks_[3] =  dbe_->book1D(histname," All vis conversions with 1 reco-matching tracks: simulated Z",zBin,zMin, zMax);
    histname = "h_SimConvOneMTracksEt";
    h_SimConvOneMTracks_[4] =  dbe_->book1D(histname," All vis conversions with 1 reco-matching tracks: simulated Et",etBin,etMin, etMax);
    //
    histname = "h_SimConvTwoMTracksEtaAndVtxPGT0";
    h_SimConvTwoMTracksAndVtxPGT0_[0] =  dbe_->book1D(histname," All vis conversions with 2 reco-matching tracks + vertex: simulated #eta",etaBin2,etaMin, etaMax);
    histname = "h_SimConvTwoMTracksPhiAndVtxPGT0";
    h_SimConvTwoMTracksAndVtxPGT0_[1] =  dbe_->book1D(histname," All vis conversions with 2 reco-matching tracks + vertex: simulated #phi",phiBin,phiMin, phiMax);
    histname = "h_SimConvTwoMTracksRAndVtxPGT0";
    h_SimConvTwoMTracksAndVtxPGT0_[2] =  dbe_->book1D(histname," All vis conversions with 2 reco-matching tracks + vertex: simulated R",rBin,rMin, rMax);
    histname = "h_SimConvTwoMTracksZAndVtxPGT0";
    h_SimConvTwoMTracksAndVtxPGT0_[3] =  dbe_->book1D(histname," All vis conversions with 2 reco-matching tracks + vertex: simulated Z",zBin,zMin, zMax);
    histname = "h_SimConvTwoMTracksEtAndVtxPGT0";
    h_SimConvTwoMTracksAndVtxPGT0_[4] =  dbe_->book1D(histname," All vis conversions with 2 reco-matching tracks + vertex: simulated Et",etBin,etMin, etMax);
    // 
    histname = "h_SimConvTwoMTracksEtaAndVtxPGT0005";
    h_SimConvTwoMTracksAndVtxPGT0005_[0] =  dbe_->book1D(histname," All vis conversions with 2 reco-matching tracks + vertex: simulated #eta",etaBin2,etaMin, etaMax);
    histname = "h_SimConvTwoMTracksPhiAndVtxPGT0005";
    h_SimConvTwoMTracksAndVtxPGT0005_[1] =  dbe_->book1D(histname," All vis conversions with 2 reco-matching tracks + vertex: simulated #phi",phiBin,phiMin, phiMax);
    histname = "h_SimConvTwoMTracksRAndVtxPGT0005";
    h_SimConvTwoMTracksAndVtxPGT0005_[2] =  dbe_->book1D(histname," All vis conversions with 2 reco-matching tracks + vertex: simulated R",rBin,rMin, rMax);
    histname = "h_SimConvTwoMTracksZAndVtxPGT0005";
    h_SimConvTwoMTracksAndVtxPGT0005_[3] =  dbe_->book1D(histname," All vis conversions with 2 reco-matching tracks + vertex: simulated Z",zBin,zMin, zMax);
    histname = "h_SimConvTwoMTracksEtAndVtxPGT0005";
    h_SimConvTwoMTracksAndVtxPGT0005_[4] =  dbe_->book1D(histname," All vis conversions with 2 reco-matching tracks + vertex: simulated Et",etBin,etMin, etMax);


    h_SimConvEtaPix_[0] = dbe_->book1D("simConvEtaPix"," sim converted Photon Eta: Pix ",etaBin,etaMin, etaMax) ;
    h_simTkPt_ = dbe_->book1D("simTkPt","Sim conversion tracks pt ",etBin*3,0.,etMax);
    h_simTkEta_ = dbe_->book1D("simTkEta","Sim conversion tracks eta ",etaBin,etaMin,etaMax);

    h_simConvVtxRvsZ_[0] =   dbe_->book2D("simConvVtxRvsZAll"," Photon Sim conversion vtx position",zBinForXray, zMinForXray, zMaxForXray, rBinForXray, rMinForXray, rMaxForXray); 
    h_simConvVtxRvsZ_[1] =   dbe_->book2D("simConvVtxRvsZBarrel"," Photon Sim conversion vtx position",zBinForXray, zMinForXray, zMaxForXray, rBinForXray, rMinForXray, rMaxForXray); 
    h_simConvVtxRvsZ_[2] =   dbe_->book2D("simConvVtxRvsZEndcap"," Photon Sim conversion vtx position",zBin2ForXray, zMinForXray, zMaxForXray, rBinForXray, rMinForXray, rMaxForXray); 
    h_simConvVtxRvsZ_[3] =   dbe_->book2D("simConvVtxRvsZBarrel2"," Photon Sim conversion vtx position when reco R<4cm",zBinForXray, zMinForXray, zMaxForXray, rBinForXray, rMinForXray, rMaxForXray); 
    h_simConvVtxYvsX_ =   dbe_->book2D("simConvVtxYvsXTrkBarrel"," Photon Sim conversion vtx position, (x,y) eta<1 ",100, -80., 80., 100, -80., 80.); 

    //// histograms for bkg
    histname = "h_SimJetEta";
    h_SimJet_[0] =  dbe_->book1D(histname," Jet bkg simulated #eta",etaBin,etaMin, etaMax);
    histname = "h_SimJetPhi";
    h_SimJet_[1] =  dbe_->book1D(histname," Jet bkg simulated #phi",phiBin,phiMin, phiMax);
    histname = "h_SimJetEt";
    h_SimJet_[2] =  dbe_->book1D(histname," Jet bkg simulated Et",etBin,etMin, etMax);
    //
    histname = "h_MatchedSimJetEta";
    h_MatchedSimJet_[0] =  dbe_->book1D(histname," Matching jet simulated #eta",etaBin,etaMin, etaMax);
    histname = "h_MatchedSimJetPhi";
    h_MatchedSimJet_[1] =  dbe_->book1D(histname," Matching jet simulated #phi",phiBin,phiMin, phiMax);
    histname = "h_MatchedSimJetEt";
    h_MatchedSimJet_[2] =  dbe_->book1D(histname," Matching jet simulated Et",etBin,etMin, etMax);
    //
    histname = "h_MatchedSimJetBadChEta";
    h_MatchedSimJetBadCh_[0] =  dbe_->book1D(histname," Matching jet simulated #eta",etaBin,etaMin, etaMax);
    histname = "h_MatchedSimJetBadChPhi";
    h_MatchedSimJetBadCh_[1] =  dbe_->book1D(histname," Matching jet simulated #phi",phiBin,phiMin, phiMax);
    histname = "h_MatchedSimJetBadChEt";
    h_MatchedSimJetBadCh_[2] =  dbe_->book1D(histname," Matching jet simulated Et",etBin,etMin, etMax);
   

    dbe_->setCurrentFolder("EgammaV/PhotonValidator/Background");

    histname = "nOfPhotons";    
    h_nPho_ = dbe_->book1D(histname,"# of Reco photons per event ",20,-0.5,19.5); 

    h_scBkgEta_ = dbe_->book1D("scBkgEta"," SC Bkg Eta ",etaBin,etaMin, etaMax) ;
    h_scBkgPhi_ = dbe_->book1D("scBkgPhi"," SC Bkg  Phi ",phiBin,phiMin,phiMax) ;
    //
    h_phoBkgEta_ = dbe_->book1D("phoBkgEta"," Photon Bkg Eta ",etaBin,etaMin, etaMax) ;
    h_phoBkgPhi_ = dbe_->book1D("phoBkgPhi"," Photon Bkg Phi ",phiBin,phiMin,phiMax) ;
    //
    h_phoBkgDEta_ = dbe_->book1D("phoBkgDEta"," Photon Eta(rec)-Eta(true) ",dEtaBin,dEtaMin, dEtaMax) ;
    h_phoBkgDPhi_ = dbe_->book1D("phoBkgDPhi"," Photon  Phi(rec)-Phi(true) ",dPhiBin,dPhiMin,dPhiMax) ;
    //
    histname = "phoBkgE";
    h_phoBkgE_[0]=dbe_->book1D(histname+"All"," Photon Bkg Energy: All ecal ", eBin,eMin, eMax);
    h_phoBkgE_[1]=dbe_->book1D(histname+"Barrel"," Photon Bkg Energy: barrel ",eBin,eMin, eMax);
    h_phoBkgE_[2]=dbe_->book1D(histname+"Endcap"," Photon Bkg Energy: Endcap ",eBin,eMin, eMax);
    //
    histname = "phoBkgEt";
    h_phoBkgEt_[0] = dbe_->book1D(histname+"All"," Photon Bkg Transverse Energy: All ecal ", etBin,etMin, etMax);
    h_phoBkgEt_[1] = dbe_->book1D(histname+"Barrel"," Photon Bkg Transverse Energy: Barrel ",etBin,etMin, etMax);
    h_phoBkgEt_[2] = dbe_->book1D(histname+"Endcap"," Photon BkgTransverse Energy: Endcap ",etBin,etMin, etMax);

    //
    histname = "scBkgE";
    h_scBkgE_[0] = dbe_->book1D(histname+"All","    SC bkg Energy: All Ecal  ",eBin,eMin, eMax);
    h_scBkgE_[1] = dbe_->book1D(histname+"Barrel"," SC bkg Energy: Barrel ",eBin,eMin, eMax);
    h_scBkgE_[2] = dbe_->book1D(histname+"Endcap"," SC bkg Energy: Endcap ",eBin,eMin, eMax);
    histname = "scBkgEt";
    h_scBkgEt_[0] = dbe_->book1D(histname+"All","    SC bkg Et: All Ecal  ",eBin,eMin, eMax);
    h_scBkgEt_[1] = dbe_->book1D(histname+"Barrel"," SC bkg Et: Barrel ",eBin,eMin, eMax);
    h_scBkgEt_[2] = dbe_->book1D(histname+"Endcap"," SC bkg Et: Endcap ",eBin,eMin, eMax);
    //
    histname = "r9Bkg";
    h_r9Bkg_[0] = dbe_->book1D(histname+"All",   " r9 bkg: All Ecal",r9Bin,r9Min, r9Max) ;
    h_r9Bkg_[1] = dbe_->book1D(histname+"Barrel"," r9 bkg: Barrel ",r9Bin,r9Min, r9Max) ;
    h_r9Bkg_[2] = dbe_->book1D(histname+"Endcap"," r9 bkg: Endcap ",r9Bin,r9Min, r9Max) ;
    //
    histname="R9VsEtaBkg";
    h2_r9VsEtaBkg_ = dbe_->book2D(histname+"All"," Bkg r9 vs #eta: all Ecal ",etaBin2,etaMin, etaMax,100, 0.,1.1);
    //
    histname="R9VsEtBkg";
    h2_r9VsEtBkg_ = dbe_->book2D(histname+"All"," Bkg photons r9 vs Et: all Ecal ",etBin,etMin, etMax,100, 0.,1.1);
    //
    histname = "r1Bkg";
    h_r1Bkg_[0] = dbe_->book1D(histname+"All",   " Bkg photon e1x5/e5x5: All Ecal",r9Bin,r9Min, r9Max) ;
    h_r1Bkg_[1] = dbe_->book1D(histname+"Barrel"," Bkg photon e1x5/e5x5: Barrel ",r9Bin,r9Min, r9Max) ;
    h_r1Bkg_[2] = dbe_->book1D(histname+"Endcap"," Bkg photon e1x5/e5x5: Endcap ",r9Bin,r9Min, r9Max) ;
    //
    histname="R1VsEtaBkg";
    h2_r1VsEtaBkg_ = dbe_->book2D(histname+"All"," Bkg photons e1x5/e5x5 vs #eta: all Ecal ",etaBin2,etaMin, etaMax,100, 0.,1.1);
    histname="pR1VsEtaBkg";
    p_r1VsEtaBkg_ = dbe_->bookProfile(histname+"All"," Bkg photons e1x5/e5x5 vs #eta: all Ecal ",etaBin2,etaMin, etaMax,100, 0.,1.1);
    //
    histname="R1VsEtBkg";
    h2_r1VsEtBkg_ = dbe_->book2D(histname+"All"," Bkg photons e1x5/e5x5 vs Et: all Ecal ",etBin,etMin, etMax,100, 0.,1.1);
    histname="pR1VsEtBkg";
    p_r1VsEtBkg_ = dbe_->bookProfile(histname+"All"," Bkg photons e2x5/e5x5 vs Et: all Ecal ",etBin,etMin, etMax,100, 0.,1.1);
    //
    histname = "r2Bkg";
    h_r2Bkg_[0] = dbe_->book1D(histname+"All",   " Bkg photon e2x5/e5x5: All Ecal",r9Bin,r9Min, r9Max) ;
    h_r2Bkg_[1] = dbe_->book1D(histname+"Barrel"," Bkg photon e2x5/e5x5: Barrel ",r9Bin,r9Min, r9Max) ;
    h_r2Bkg_[2] = dbe_->book1D(histname+"Endcap"," Bkg photon e2x5/e5x5: Endcap ",r9Bin,r9Min, r9Max) ;
    //
    histname="R2VsEtaBkg";
    h2_r2VsEtaBkg_ = dbe_->book2D(histname+"All"," Bkg photons e2x5/e5x5 vs #eta: all Ecal ",etaBin2,etaMin, etaMax,100, 0.,1.1);
    histname="pR2VsEtaBkg";
    p_r2VsEtaBkg_ = dbe_->bookProfile(histname+"All"," Bkg photons e2x5/e5x5 vs #eta: all Ecal ",etaBin2,etaMin, etaMax,100, 0.,1.1);
    //
    histname="R2VsEtBkg";
    h2_r2VsEtBkg_ = dbe_->book2D(histname+"All"," Bkg photons e2x5/e5x5 vs Et: all Ecal ",etBin,etMin, etMax,100, 0.,1.1);
    histname="pR2VsEtBkg";
    p_r2VsEtBkg_ = dbe_->bookProfile(histname+"All"," Bkg photons e2x5/e5x5 vs Et: all Ecal ",etBin,etMin, etMax,100, 0.,1.1);


    histname = "sigmaIetaIetaBkg";
    h_sigmaIetaIetaBkg_[0] = dbe_->book1D(histname+"All",   "Bkg sigmaIetaIeta: All Ecal",100,0., 0.1) ;
    h_sigmaIetaIetaBkg_[1] = dbe_->book1D(histname+"Barrel","Bkg sigmaIetaIeta: Barrel ", 100,0., 0.05) ;
    h_sigmaIetaIetaBkg_[2] = dbe_->book1D(histname+"Endcap","Bkg sigmaIetaIeta: Endcap ", 100,0., 0.1) ;
    //
    histname="sigmaIetaIetaVsEtaBkg";
    h2_sigmaIetaIetaVsEtaBkg_ = dbe_->book2D(histname+"All"," Bkg photons sigmaIetaIeta vs #eta: all Ecal ",  etaBin2,etaMin, etaMax,100, 0.,0.1);
    histname="pSigmaIetaIetaVsEtaBkg";
    p_sigmaIetaIetaVsEtaBkg_ = dbe_->bookProfile(histname+"All"," Bkg photons sigmaIetaIeta vs #eta: all Ecal ",  etaBin2,etaMin, etaMax,100, 0.,0.1);
    //
    histname="sigmaIetaIetaVsEtBkg";
    h2_sigmaIetaIetaVsEtBkg_[0] = dbe_->book2D(histname+"All"," Bkg photons sigmaIetaIeta vs Et: all Ecal ",etBin,etMin, etMax,100, 0.,0.1);
    h2_sigmaIetaIetaVsEtBkg_[1] = dbe_->book2D(histname+"Barrel"," Bkg photons sigmaIetaIeta vs Et: all Ecal ",etBin,etMin, etMax,100, 0.,0.1);
    h2_sigmaIetaIetaVsEtBkg_[2] = dbe_->book2D(histname+"Endcap"," Bkg photons sigmaIetaIeta vs Et: all Ecal ",etBin,etMin, etMax,100, 0.,0.1);
    //
    histname="pSigmaIetaIetaVsEtBkg";
    p_sigmaIetaIetaVsEtBkg_[0] = dbe_->bookProfile(histname+"All"," Bkg photons sigmaIetaIeta vs Et: all Ecal ",etBin,etMin, etMax,100, 0.,0.1);
    p_sigmaIetaIetaVsEtBkg_[1] = dbe_->bookProfile(histname+"Barrel"," Bkg photons sigmaIetaIeta vs Et: all Ecal ",etBin,etMin, etMax,100, 0.,0.1);
    p_sigmaIetaIetaVsEtBkg_[2] = dbe_->bookProfile(histname+"Endcap"," Bkg photons sigmaIetaIeta vs Et: all Ecal ",etBin,etMin, etMax,100, 0.,0.1);
    //
    histname = "hOverEBkg";
    h_hOverEBkg_[0] = dbe_->book1D(histname+"All",   "H/E bkg: All Ecal",100,0., 1.) ;
    h_hOverEBkg_[1] = dbe_->book1D(histname+"Barrel","H/E bkg: Barrel ", 100,0., 1.) ;
    h_hOverEBkg_[2] = dbe_->book1D(histname+"Endcap","H/E bkg: Endcap ", 100,0., 1.) ;
    //
    histname="hOverEVsEtaBkg";
    h2_hOverEVsEtaBkg_ = dbe_->book2D(histname+"All"," Bkg H/E vs #eta: all Ecal ",  etaBin2,etaMin, etaMax,100, 0.,0.1);
    histname="pHOverEVsEtaBkg";
    p_hOverEVsEtaBkg_ = dbe_->bookProfile(histname+"All"," Bkg H/E vs #eta: all Ecal ",  etaBin2,etaMin, etaMax,100, 0.,0.1);
    //
    histname="hOverEVsEtBkg";
    h2_hOverEVsEtBkg_ = dbe_->book2D(histname+"All"," Bkg photons H/E vs Et: all Ecal ",etBin,etMin, etMax,100, 0.,0.1);
    histname="pHOverEVsEtBkg";
    p_hOverEVsEtBkg_ = dbe_->bookProfile(histname+"All"," Bkg photons H/E vs Et: all Ecal ",etBin,etMin, etMax,100, 0.,0.1);
    //
    histname = "ecalRecHitSumEtConeDR04Bkg";
    h_ecalRecHitSumEtConeDR04Bkg_[0] = dbe_->book1D(histname+"All",   "bkg ecalRecHitSumEtDR04: All Ecal",etBin,etMin,50.);
    h_ecalRecHitSumEtConeDR04Bkg_[1] = dbe_->book1D(histname+"Barrel","bkg ecalRecHitSumEtDR04: Barrel ", etBin,etMin,50.);
    h_ecalRecHitSumEtConeDR04Bkg_[2] = dbe_->book1D(histname+"Endcap","bkg ecalRecHitSumEtDR04: Endcap ", etBin,etMin,50.);
    //
    histname="ecalRecHitSumEtConeDR04VsEtaBkg";
    h2_ecalRecHitSumEtConeDR04VsEtaBkg_ = dbe_->book2D(histname+"All"," bkg ecalRecHitSumEtDR04 vs #eta: all Ecal ",  etaBin2,etaMin, etaMax, etBin,etMin,etMax*etScale);
    histname="pEcalRecHitSumEtConeDR04VsEtaBkg";
    p_ecalRecHitSumEtConeDR04VsEtaBkg_ = dbe_->bookProfile(histname+"All","bkg photons ecalRecHitSumEtDR04 vs #eta: all Ecal ",  etaBin2,etaMin, etaMax, etBin,etMin,etMax*etScale, "");
    //
    histname="ecalRecHitSumEtConeDR04VsEtBkg";
    h2_ecalRecHitSumEtConeDR04VsEtBkg_[0] = dbe_->book2D(histname+"All"," Bkg ecalRecHitSumEtDR04 vs Et: all Ecal ",etBin,etMin, etMax, etBin,etMin,etMax*etScale);
    h2_ecalRecHitSumEtConeDR04VsEtBkg_[1] = dbe_->book2D(histname+"Barrel"," Bkg ecalRecHitSumEtDR04 vs Et: Barrel ",etBin,etMin, etMax, etBin,etMin,etMax*etScale);
    h2_ecalRecHitSumEtConeDR04VsEtBkg_[2] = dbe_->book2D(histname+"Endcap"," Bkg ecalRecHitSumEtDR04 vs Et: Endcap ",etBin,etMin, etMax, etBin,etMin,etMax*etScale);
    histname="pEcalRecHitSumEtConeDR04VsEtBkg";
    p_ecalRecHitSumEtConeDR04VsEtBkg_[0] = dbe_->bookProfile(histname+"All","Bkg ecalRecHitSumEtDR04 vs Et: all Ecal ",  etBin,etMin, etMax, etBin,etMin,etMax*etScale, "");
    p_ecalRecHitSumEtConeDR04VsEtBkg_[1] = dbe_->bookProfile(histname+"Barrel","Bkg ecalRecHitSumEtDR04 vs Et: all Ecal ",  etBin,etMin, etMax, etBin,etMin,etMax*etScale, "");
    p_ecalRecHitSumEtConeDR04VsEtBkg_[2] = dbe_->bookProfile(histname+"Endcap","Bkg ecalRecHitSumEtDR04 vs Et: all Ecal ",  etBin,etMin, etMax, etBin,etMin,etMax*etScale, "");
    //
    histname = "hcalTowerSumEtConeDR04Bkg";
    h_hcalTowerSumEtConeDR04Bkg_[0] = dbe_->book1D(histname+"All",   "bkg hcalTowerSumEtDR04: All Ecal",etBin,etMin,20.);
    h_hcalTowerSumEtConeDR04Bkg_[1] = dbe_->book1D(histname+"Barrel","bkg hcalTowerSumEtDR04: Barrel ", etBin,etMin,20.);
    h_hcalTowerSumEtConeDR04Bkg_[2] = dbe_->book1D(histname+"Endcap","bkg hcalTowerSumEtDR04: Endcap ", etBin,etMin,20.);
    //
    histname="hcalTowerSumEtConeDR04VsEtaBkg";
    h2_hcalTowerSumEtConeDR04VsEtaBkg_ = dbe_->book2D(histname+"All"," bkg hcalTowerSumEtDR04 vs #eta: all Ecal ",  etaBin2,etaMin, etaMax, etBin,etMin,etMax*etScale);
    histname="pHcalTowerSumEtConeDR04VsEtaBkg";
    p_hcalTowerSumEtConeDR04VsEtaBkg_ = dbe_->bookProfile(histname+"All","bkg photons hcalTowerSumEtDR04 vs #eta: all Ecal ",  etaBin2,etaMin, etaMax, etBin,etMin,etMax*etScale, "");
    //
    histname="hcalTowerSumEtConeDR04VsEtBkg";
    h2_hcalTowerSumEtConeDR04VsEtBkg_[0] = dbe_->book2D(histname+"All"," Bkg hcalTowerSumEtDR04 vs Et: all Ecal ",etBin,etMin, etMax, etBin,etMin,etMax*etScale);
    h2_hcalTowerSumEtConeDR04VsEtBkg_[1] = dbe_->book2D(histname+"Barrel"," Bkg hcalTowerSumEtDR04 vs Et: Barrel ",etBin,etMin, etMax, etBin,etMin,etMax*etScale);
    h2_hcalTowerSumEtConeDR04VsEtBkg_[2] = dbe_->book2D(histname+"Endcap"," Bkg hcalTowerSumEtDR04 vs Et: Endcap ",etBin,etMin, etMax, etBin,etMin,etMax*etScale);
    histname="pHcalTowerSumEtConeDR04VsEtBkg";
    p_hcalTowerSumEtConeDR04VsEtBkg_[0] = dbe_->bookProfile(histname+"All","Bkg hcalTowerSumEtDR04 vs Et: all Ecal ",  etBin,etMin, etMax, etBin,etMin,etMax*etScale, "");
    p_hcalTowerSumEtConeDR04VsEtBkg_[1] = dbe_->bookProfile(histname+"Barrel","Bkg hcalTowerSumEtDR04 vs Et: all Ecal ",  etBin,etMin, etMax, etBin,etMin,etMax*etScale, "");
    p_hcalTowerSumEtConeDR04VsEtBkg_[2] = dbe_->bookProfile(histname+"Endcap","Bkg hcalTowerSumEtDR04 vs Et: all Ecal ",  etBin,etMin, etMax, etBin,etMin,etMax*etScale, "");
    //
    histname = "isoTrkSolidConeDR04Bkg";
    h_isoTrkSolidConeDR04Bkg_[0] = dbe_->book1D(histname+"All",   "isoTrkSolidConeDR04 Bkg: All Ecal",etBin,etMin,etMax*0.1);
    h_isoTrkSolidConeDR04Bkg_[1] = dbe_->book1D(histname+"Barrel","isoTrkSolidConeDR04 Bkg: Barrel ", etBin,etMin,etMax*0.1);
    h_isoTrkSolidConeDR04Bkg_[2] = dbe_->book1D(histname+"Endcap","isoTrkSolidConeDR04 Bkg: Endcap ", etBin,etMin,etMax*0.1);
    //
    histname="isoTrkSolidConeDR04VsEtaBkg";
    h2_isoTrkSolidConeDR04VsEtaBkg_ = dbe_->book2D(histname+"All"," Bkg photons isoTrkSolidConeDR04 vs #eta: all Ecal ",  etaBin2,etaMin, etaMax, etBin,etMin,etMax*0.1);
    histname="pIsoTrkSolidConeDR04VsEtaBkg";
    p_isoTrkSolidConeDR04VsEtaBkg_ = dbe_->bookProfile(histname+"All"," Bkg photons isoTrkSolidConeDR04 vs #eta: all Ecal ",  etaBin2,etaMin, etaMax, etBin,etMin,etMax*0.1);
    //
    histname="isoTrkSolidConeDR04VsEtBkg";
    h2_isoTrkSolidConeDR04VsEtBkg_[0] = dbe_->book2D(histname+"All"," Bkg photons isoTrkSolidConeDR04 vs Et: all Ecal ",etBin,etMin, etMax, etBin,etMin,etMax*0.1);
    h2_isoTrkSolidConeDR04VsEtBkg_[1] = dbe_->book2D(histname+"Barrel"," Bkg photons isoTrkSolidConeDR04 vs Et: all Ecal ",etBin,etMin, etMax, etBin,etMin,etMax*0.1);
    h2_isoTrkSolidConeDR04VsEtBkg_[2] = dbe_->book2D(histname+"Endcap"," Bkg photons isoTrkSolidConeDR04 vs Et: all Ecal ",etBin,etMin, etMax, etBin,etMin,etMax*0.1);
    histname="pIsoTrkSolidConeDR04VsEtBkg";
    p_isoTrkSolidConeDR04VsEtBkg_[0] = dbe_->bookProfile(histname+"All"," Bkg photons isoTrkSolidConeDR04 vs Et: all Ecal ",etBin,etMin, etMax, etBin,etMin,etMax*0.1);
    p_isoTrkSolidConeDR04VsEtBkg_[1] = dbe_->bookProfile(histname+"Barrel"," Bkg photons isoTrkSolidConeDR04 vs Et: all Ecal ",etBin,etMin, etMax, etBin,etMin,etMax*0.1);
    p_isoTrkSolidConeDR04VsEtBkg_[2] = dbe_->bookProfile(histname+"Endcap"," Bkg photons isoTrkSolidConeDR04 vs Et: all Ecal ",etBin,etMin, etMax, etBin,etMin,etMax*0.1);
    //
    histname = "nTrkSolidConeDR04Bkg";
    h_nTrkSolidConeDR04Bkg_[0] = dbe_->book1D(histname+"All",   "Bkg nTrkSolidConeDR04: All Ecal",20,0., 20) ;
    h_nTrkSolidConeDR04Bkg_[1] = dbe_->book1D(histname+"Barrel","Bkg nTrkSolidConeDR04: Barrel ", 20,0., 20) ;
    h_nTrkSolidConeDR04Bkg_[2] = dbe_->book1D(histname+"Endcap","Bkg nTrkSolidConeDR04: Endcap ", 20,0., 20) ;
    //
    histname="nTrkSolidConeDR04VsEtaBkg";
    h2_nTrkSolidConeDR04VsEtaBkg_ = dbe_->book2D(histname+"All"," Bkg photons nTrkSolidConeDR04 vs #eta: all Ecal ",  etaBin2,etaMin, etaMax, 20,0., 20) ;
    histname="p_nTrkSolidConeDR04VsEtaBkg";
    p_nTrkSolidConeDR04VsEtaBkg_ = dbe_->bookProfile(histname+"All"," Bkg photons nTrkSolidConeDR04 vs #eta: all Ecal ",  etaBin2,etaMin, etaMax, 20,0., 20) ;
    //
    histname="nTrkSolidConeDR04VsEtBkg";
    h2_nTrkSolidConeDR04VsEtBkg_[0] = dbe_->book2D(histname+"All","Bkg photons nTrkSolidConeDR04 vs Et: all Ecal ",etBin,etMin, etMax, 20,0., 20) ;
    h2_nTrkSolidConeDR04VsEtBkg_[1] = dbe_->book2D(histname+"Barrel","Bkg photons nTrkSolidConeDR04 vs Et: all Ecal ",etBin,etMin, etMax, 20,0., 20) ;
    h2_nTrkSolidConeDR04VsEtBkg_[2] = dbe_->book2D(histname+"Endcap","Bkg photons nTrkSolidConeDR04 vs Et: all Ecal ",etBin,etMin, etMax, 20,0., 20) ;
    //
    histname="pnTrkSolidConeDR04VsEtBkg";
    p_nTrkSolidConeDR04VsEtBkg_[0] = dbe_->bookProfile(histname+"All","Bkg photons nTrkSolidConeDR04 vs Et: all Ecal ",etBin,etMin, etMax, 20,0., 20) ;
    p_nTrkSolidConeDR04VsEtBkg_[1] = dbe_->bookProfile(histname+"Barrel","Bkg photons nTrkSolidConeDR04 vs Et: all Ecal ",etBin,etMin, etMax, 20,0., 20) ;
    p_nTrkSolidConeDR04VsEtBkg_[2] = dbe_->bookProfile(histname+"Endcap","Bkg photons nTrkSolidConeDR04 vs Et: all Ecal ",etBin,etMin, etMax, 20,0., 20) ;
    //
    h_convEtaBkg_ = dbe_->book1D("convEtaBkg"," converted Photon Bkg Eta 2 tracks",etaBin,etaMin, etaMax) ;
    h_convPhiBkg_ = dbe_->book1D("convPhiBkg"," converted Photon Bkg Phi ",phiBin,phiMin,phiMax) ;
    //
    histname="mvaOutBkg";
    h_mvaOutBkg_[0] = dbe_->book1D(histname+"All"," mvaOut  conversions bkg : All Ecal",100, 0., 1.);
    h_mvaOutBkg_[1] = dbe_->book1D(histname+"Barrel"," mvaOut conversions bkg: Barrel Ecal",100, 0., 1.);
    h_mvaOutBkg_[2] = dbe_->book1D(histname+"Endcap"," mvaOut  conversions bkg: Endcap Ecal",100, 0., 1.);

    histname="PoverEtracksBkg";
    h_PoverETracksBkg_[0] = dbe_->book1D(histname+"All"," bkg photons conversion p/E: all Ecal ",povereBin, povereMin, povereMax);
    h_PoverETracksBkg_[1] = dbe_->book1D(histname+"Barrel","bkg photons conversion p/E: Barrel Ecal",povereBin, povereMin, povereMax);
    h_PoverETracksBkg_[2] = dbe_->book1D(histname+"Endcap"," bkg photons conversion p/E: Endcap Ecal ",povereBin, povereMin, povereMax);

    histname="EoverPtracksBkg";
    h_EoverPTracksBkg_[0] = dbe_->book1D(histname+"All"," bkg photons conversion E/p: all Ecal ",eoverpBin, eoverpMin, eoverpMax);
    h_EoverPTracksBkg_[1] = dbe_->book1D(histname+"Barrel","bkg photons conversion E/p: Barrel Ecal",eoverpBin, eoverpMin, eoverpMax);
    h_EoverPTracksBkg_[2] = dbe_->book1D(histname+"Endcap"," bkg photons conversion E/p: Endcap Ecal ",eoverpBin, eoverpMin, eoverpMax);

    histname="hDCotTracksBkg";
    h_DCotTracksBkg_[0]= dbe_->book1D(histname+"All"," bkg Photons:Tracks from conversions #delta cotg(#Theta) Tracks: all Ecal ",dCotTracksBin,dCotTracksMin,dCotTracksMax); 
    h_DCotTracksBkg_[1]= dbe_->book1D(histname+"Barrel"," bkg Photons:Tracks from conversions #delta cotg(#Theta) Tracks: Barrel Ecal ",dCotTracksBin,dCotTracksMin,dCotTracksMax); 
    h_DCotTracksBkg_[2]= dbe_->book1D(histname+"Endcap"," bkg Photons:Tracks from conversions #delta cotg(#Theta) Tracks: Endcap Ecal ",dCotTracksBin,dCotTracksMin,dCotTracksMax); 

    histname="hDPhiTracksAtVtxBkg";
    h_DPhiTracksAtVtxBkg_[0] =dbe_->book1D(histname+"All", " Bkg Photons:Tracks from conversions: #delta#phi Tracks at vertex: all Ecal",dPhiTracksBin,dPhiTracksMin,dPhiTracksMax); 
    h_DPhiTracksAtVtxBkg_[1] =dbe_->book1D(histname+"Barrel", " Bkg Photons:Tracks from conversions: #delta#phi Tracks at vertex: Barrel Ecal",dPhiTracksBin,dPhiTracksMin,dPhiTracksMax); 
    h_DPhiTracksAtVtxBkg_[2] =dbe_->book1D(histname+"Endcap", " Bkg Photons:Tracks from conversions: #delta#phi Tracks at vertex: Endcap Ecal",dPhiTracksBin,dPhiTracksMin,dPhiTracksMax); 

    h_convVtxRvsZBkg_[0] =   dbe_->book2D("convVtxRvsZAllBkg"," Bkg Photon Reco conversion vtx position",zBinForXray, zMinForXray, zMaxForXray, rBinForXray, rMinForXray, rMaxForXray); 
    h_convVtxRvsZBkg_[1] =   dbe_->book2D("convVtxRvsZBarrelBkg"," Bkg Photon Reco conversion vtx position",zBinForXray, zMinForXray, zMaxForXray, rBinForXray, rMinForXray, rMaxForXray); 
    h_convVtxYvsXBkg_ =   dbe_->book2D("convVtxYvsXTrkBarrelBkg"," Bkg Photon Reco conversion vtx position, (x,y) eta<1 ",100, -80., 80., 100, -80., 80.); 


    //
    dbe_->setCurrentFolder("EgammaV/PhotonValidator/Photons");

    h_phoEta_[0] = dbe_->book1D("phoEta"," Photon Eta ",etaBin,etaMin, etaMax) ;
    h_phoPhi_[0] = dbe_->book1D("phoPhi"," Photon  Phi ",phiBin,phiMin,phiMax) ;

    h_phoDEta_[0] = dbe_->book1D("phoDEta"," Photon Eta(rec)-Eta(true) ",dEtaBin,dEtaMin, dEtaMax) ;
    h_phoDPhi_[0] = dbe_->book1D("phoDPhi"," Photon  Phi(rec)-Phi(true) ",dPhiBin,dPhiMin,dPhiMax) ;

    h_scEta_[0] =   dbe_->book1D("scEta"," SC Eta ",etaBin,etaMin, etaMax);
    h_scPhi_[0] =   dbe_->book1D("scPhi"," SC Phi ",phiBin,phiMin,phiMax);

    h_scEtaWidth_[0] =   dbe_->book1D("scEtaWidth"," SC Eta Width ",100,0., 0.1);
    h_scPhiWidth_[0] =   dbe_->book1D("scPhiWidth"," SC Phi Width ",100,0., 1.);


    histname = "scE";
    h_scE_[0][0] = dbe_->book1D(histname+"All"," SC Energy: All Ecal  ",eBin,eMin, eMax);
    h_scE_[0][1] = dbe_->book1D(histname+"Barrel"," SC Energy: Barrel ",eBin,eMin, eMax);
    h_scE_[0][2] = dbe_->book1D(histname+"Endcap"," SC Energy: Endcap ",eBin,eMin, eMax);

    histname = "psE";
    h_psE_ = dbe_->book1D(histname+"Endcap"," ES Energy  ",eBin,eMin, 50.);


    histname = "scEt";
    h_scEt_[0][0] = dbe_->book1D(histname+"All"," SC Et: All Ecal ",etBin,etMin, etMax) ;
    h_scEt_[0][1] = dbe_->book1D(histname+"Barrel"," SC Et: Barrel",etBin,etMin, etMax) ;
    h_scEt_[0][2] = dbe_->book1D(histname+"Endcap"," SC Et: Endcap",etBin,etMin, etMax) ;

    histname = "r9";
    h_r9_[0][0] = dbe_->book1D(histname+"All",   " r9: All Ecal",r9Bin,r9Min, r9Max) ;
    h_r9_[0][1] = dbe_->book1D(histname+"Barrel"," r9: Barrel ",r9Bin,r9Min, r9Max) ;
    h_r9_[0][2] = dbe_->book1D(histname+"Endcap"," r9: Endcap ",r9Bin,r9Min, r9Max) ;
    //
    histname = "r9ConvFromMC";
    h_r9_[1][0] = dbe_->book1D(histname+"All",   " r9: All Ecal",r9Bin,r9Min, r9Max) ;
    h_r9_[1][1] = dbe_->book1D(histname+"Barrel"," r9: Barrel ",r9Bin,r9Min, r9Max) ;
    h_r9_[1][2] = dbe_->book1D(histname+"Endcap"," r9: Endcap ",r9Bin,r9Min, r9Max) ;
    //
    histname = "r9ConvFromReco";
    h_r9_[2][0] = dbe_->book1D(histname+"All",   " r9: All Ecal",r9Bin,r9Min, r9Max) ;
    h_r9_[2][1] = dbe_->book1D(histname+"Barrel"," r9: Barrel ",r9Bin,r9Min, r9Max) ;
    h_r9_[2][2] = dbe_->book1D(histname+"Endcap"," r9: Endcap ",r9Bin,r9Min, r9Max) ;
    //
    histname="R9VsEta";
    h2_r9VsEta_[0] = dbe_->book2D(histname+"All"," All photons r9 vs #eta: all Ecal ",etaBin2,etaMin, etaMax,100, 0.,1.1);
    h2_r9VsEta_[1] = dbe_->book2D(histname+"Unconv"," All photons r9 vs #eta: all Ecal ",etaBin2,etaMin, etaMax,100, 0.,1.1);
    //
    histname="R9VsEt";
    h2_r9VsEt_[0] = dbe_->book2D(histname+"All"," All photons r9 vs Et: all Ecal ",etBin,etMin, etMax,100, 0.,1.1);
    h2_r9VsEt_[1] = dbe_->book2D(histname+"Unconv"," All photons r9 vs Et: all Ecal ",etBin,etMin, etMax,100, 0.,1.1);
    //
    histname = "r1";
    h_r1_[0][0] = dbe_->book1D(histname+"All",   " e1x5/e5x5: All Ecal",r9Bin,r9Min, r9Max) ;
    h_r1_[0][1] = dbe_->book1D(histname+"Barrel"," e1x5/e5x5: Barrel ",r9Bin,r9Min, r9Max) ;
    h_r1_[0][2] = dbe_->book1D(histname+"Endcap"," e1x5/e5x5: Endcap ",r9Bin,r9Min, r9Max) ;
    //
    histname="R1VsEta";
    h2_r1VsEta_[0] = dbe_->book2D(histname+"All"," All photons e1x5/e5x5 vs #eta: all Ecal ",etaBin2,etaMin, etaMax,100, 0.,1.1);
    h2_r1VsEta_[1] = dbe_->book2D(histname+"Unconv"," All photons e1x5/e5x5 vs #eta: all Ecal ",etaBin2,etaMin, etaMax,100, 0.,1.1);
    //
    histname="R1VsEt";
    h2_r1VsEt_[0] = dbe_->book2D(histname+"All"," All photons e1x5/e5x5 vs Et: all Ecal ",etBin,etMin, etMax,100, 0.,1.1);
    h2_r1VsEt_[1] = dbe_->book2D(histname+"Unconv"," All photons e1x5/e5x5 vs Et: all Ecal ",etBin,etMin, etMax,100, 0.,1.1);
    //
    histname = "r2";
    h_r2_[0][0] = dbe_->book1D(histname+"All",   " e2x5/e5x5: All Ecal",r9Bin,r9Min, r9Max) ;
    h_r2_[0][1] = dbe_->book1D(histname+"Barrel"," e2x5/e5x5: Barrel ",r9Bin,r9Min, r9Max) ;
    h_r2_[0][2] = dbe_->book1D(histname+"Endcap"," e2x5/e5x5: Endcap ",r9Bin,r9Min, r9Max) ;
    //
    histname="R2VsEta";
    h2_r2VsEta_[0] = dbe_->book2D(histname+"All"," All photons e2x5/e5x5 vs #eta: all Ecal ",etaBin2,etaMin, etaMax,100, 0.,1.1);
    h2_r2VsEta_[1] = dbe_->book2D(histname+"Unconv"," All photons e2x5/e5x5 vs #eta: all Ecal ",etaBin2,etaMin, etaMax,100, 0.,1.1);
    //
    histname="R2VsEt";
    h2_r2VsEt_[0] = dbe_->book2D(histname+"All"," All photons e2x5/e5x5 vs Et: all Ecal ",etBin,etMin, etMax,100, 0.,1.1);
    h2_r2VsEt_[1] = dbe_->book2D(histname+"Unconv"," All photons e2x5/e5x5 vs Et: all Ecal ",etBin,etMin, etMax,100, 0.,1.1);
    //
    histname = "sigmaIetaIeta";
    h_sigmaIetaIeta_[0][0] = dbe_->book1D(histname+"All",   "sigmaIetaIeta: All Ecal",100,0., 0.1) ;
    h_sigmaIetaIeta_[0][1] = dbe_->book1D(histname+"Barrel","sigmaIetaIeta: Barrel ", 100,0., 0.05) ;
    h_sigmaIetaIeta_[0][2] = dbe_->book1D(histname+"Endcap","sigmaIetaIeta: Endcap ", 100,0., 0.1) ;
    //
    histname="sigmaIetaIetaVsEta";
    h2_sigmaIetaIetaVsEta_[0] = dbe_->book2D(histname+"All"," All photons sigmaIetaIeta vs #eta: all Ecal ",  etaBin2,etaMin, etaMax,100, 0.,0.1);
    h2_sigmaIetaIetaVsEta_[1] = dbe_->book2D(histname+"Unconv"," All photons sigmaIetaIeta vs #eta: all Ecal ",etaBin2,etaMin, etaMax,100, 0.,0.1);
    //
    histname="sigmaIetaIetaVsEt";
    h2_sigmaIetaIetaVsEt_[0] = dbe_->book2D(histname+"All"," All photons sigmaIetaIeta vs Et: all Ecal ",etBin,etMin, etMax,100, 0.,0.1);
    h2_sigmaIetaIetaVsEt_[1] = dbe_->book2D(histname+"Unconv"," All photons sigmaIetaIeta vs Et: all Ecal ",etBin,etMin, etMax,100, 0.,0.1);
    //
    histname = "hOverE";
    h_hOverE_[0][0] = dbe_->book1D(histname+"All",   "H/E: All Ecal",100,0., 0.1) ;
    h_hOverE_[0][1] = dbe_->book1D(histname+"Barrel","H/E: Barrel ", 100,0., 0.1) ;
    h_hOverE_[0][2] = dbe_->book1D(histname+"Endcap","H/E: Endcap ", 100,0., 0.1) ;
    //
    histname="hOverEVsEta";
    h2_hOverEVsEta_[0] = dbe_->book2D(histname+"All"," All photons H/E vs #eta: all Ecal ",  etaBin2,etaMin, etaMax,100, 0.,0.1);
    h2_hOverEVsEta_[1] = dbe_->book2D(histname+"Unconv"," All photons H/E vs #eta: all Ecal ",etaBin2,etaMin, etaMax,100, 0.,0.1);
    //
    histname="hOverEVsEt";
    h2_hOverEVsEt_[0] = dbe_->book2D(histname+"All"," All photons H/E vs Et: all Ecal ",etBin,etMin, etMax,100, 0.,0.1);
    h2_hOverEVsEt_[1] = dbe_->book2D(histname+"Unconv"," All photons H/E vs Et: all Ecal ",etBin,etMin, etMax,100, 0.,0.1);
    //
    histname="pHoverEVsEta";
    p_hOverEVsEta_[0] = dbe_->bookProfile(histname+"All"," All photons H/E vs #eta: all Ecal ",  etaBin2,etaMin, etaMax,100, 0.,0.1);
    p_hOverEVsEta_[1] = dbe_->bookProfile(histname+"Unconv"," All photons H/E vs #eta: all Ecal ",etaBin2,etaMin, etaMax,100, 0.,0.1);
    //
    histname="pHoverEVsEt";
    p_hOverEVsEt_[0] = dbe_->bookProfile(histname+"All"," All photons H/E vs Et: all Ecal ",etBin,etMin, etMax,100, 0.,0.1);
    p_hOverEVsEt_[1] = dbe_->bookProfile(histname+"Unconv"," All photons H/E vs Et: all Ecal ",etBin,etMin, etMax,100, 0.,0.1);

    //
    histname = "ecalRecHitSumEtConeDR04";
    h_ecalRecHitSumEtConeDR04_[0][0] = dbe_->book1D(histname+"All",   "ecalRecHitSumEtDR04: All Ecal",etBin,etMin,20.);
    h_ecalRecHitSumEtConeDR04_[0][1] = dbe_->book1D(histname+"Barrel","ecalRecHitSumEtDR04: Barrel ", etBin,etMin,20.);
    h_ecalRecHitSumEtConeDR04_[0][2] = dbe_->book1D(histname+"Endcap","ecalRecHitSumEtDR04: Endcap ", etBin,etMin,20.);
    //
    histname="ecalRecHitSumEtConeDR04VsEta";
    h2_ecalRecHitSumEtConeDR04VsEta_[0] = dbe_->book2D(histname+"All"," All photons ecalRecHitSumEtDR04 vs #eta: all Ecal ",  etaBin2,etaMin, etaMax, etBin,etMin,etMax*etScale);
    h2_ecalRecHitSumEtConeDR04VsEta_[1] = dbe_->book2D(histname+"Unconv"," All photons ecalRecHitSumEtDR04 vs #eta: all Ecal ",etaBin2,etaMin, etaMax,etBin,etMin,etMax*etScale);
    histname="pEcalRecHitSumEtConeDR04VsEta";
    p_ecalRecHitSumEtConeDR04VsEta_[0] = dbe_->bookProfile(histname+"All","All photons ecalRecHitSumEtDR04 vs #eta: all Ecal ",  etaBin2,etaMin, etaMax, etBin,etMin,etMax*etScale, "");
    p_ecalRecHitSumEtConeDR04VsEta_[1] = dbe_->bookProfile(histname+"Unconv","All photons ecalRecHitSumEtDR04 vs #eta: all Ecal ",  etaBin2,etaMin, etaMax, etBin,etMin,etMax*etScale, "");
    //
    histname="ecalRecHitSumEtConeDR04VsEt";
    h2_ecalRecHitSumEtConeDR04VsEt_[0] = dbe_->book2D(histname+"All"," All photons ecalRecHitSumEtDR04 vs Et: all Ecal ",etBin,etMin, etMax, etBin,etMin,etMax*etScale);
    h2_ecalRecHitSumEtConeDR04VsEt_[1] = dbe_->book2D(histname+"Barrel"," All photons ecalRecHitSumEtDR04 vs Et: Barrel ",etBin,etMin, etMax, etBin,etMin,etMax*etScale);
    h2_ecalRecHitSumEtConeDR04VsEt_[2] = dbe_->book2D(histname+"Endcap"," All photons ecalRecHitSumEtDR04 vs Et: Endcap ",etBin,etMin, etMax, etBin,etMin,etMax*etScale);
    histname="pEcalRecHitSumEtConeDR04VsEt";
    p_ecalRecHitSumEtConeDR04VsEt_[0] = dbe_->bookProfile(histname+"All","All photons ecalRecHitSumEtDR04 vs Et: all Ecal ",  etBin,etMin, etMax, etBin,etMin,etMax*etScale, "");
    p_ecalRecHitSumEtConeDR04VsEt_[1] = dbe_->bookProfile(histname+"Barrel","All photons ecalRecHitSumEtDR04 vs Et: all Ecal ",  etBin,etMin, etMax, etBin,etMin,etMax*etScale, "");
    p_ecalRecHitSumEtConeDR04VsEt_[2] = dbe_->bookProfile(histname+"Endcap","All photons ecalRecHitSumEtDR04 vs Et: all Ecal ",  etBin,etMin, etMax, etBin,etMin,etMax*etScale, "");
    //
    histname = "hcalTowerSumEtConeDR04";
    h_hcalTowerSumEtConeDR04_[0][0] = dbe_->book1D(histname+"All",   "hcalTowerSumEtConeDR04: All Ecal",etBin,etMin,20.);
    h_hcalTowerSumEtConeDR04_[0][1] = dbe_->book1D(histname+"Barrel","hcalTowerSumEtConeDR04: Barrel ", etBin,etMin,20.);
    h_hcalTowerSumEtConeDR04_[0][2] = dbe_->book1D(histname+"Endcap","hcalTowerSumEtConeDR04: Endcap ", etBin,etMin,20.);

    //
    histname="hcalTowerSumEtConeDR04VsEta";
    h2_hcalTowerSumEtConeDR04VsEta_[0] = dbe_->book2D(histname+"All"," All photons hcalTowerSumEtConeDR04 vs #eta: all Ecal ",  etaBin2,etaMin, etaMax, etBin,etMin,etMax*0.1);
    h2_hcalTowerSumEtConeDR04VsEta_[1] = dbe_->book2D(histname+"Unconv"," All photons hcalTowerSumEtConeDR04 vs #eta: all Ecal ",etaBin2,etaMin, etaMax,etBin,etMin,etMax*0.1);
    histname="pHcalTowerSumEtConeDR04VsEta";
    p_hcalTowerSumEtConeDR04VsEta_[0] = dbe_->bookProfile(histname+"All","All photons hcalTowerSumEtDR04 vs #eta: all Ecal ",  etaBin2,etaMin, etaMax, etBin,etMin,etMax*0.1, "");
    p_hcalTowerSumEtConeDR04VsEta_[1] = dbe_->bookProfile(histname+"Unconv","All photons hcalTowerSumEtDR04 vs #eta: all Ecal ",  etaBin2,etaMin, etaMax, etBin,etMin,etMax*0.1, "");
    //
    histname="hcalTowerSumEtConeDR04VsEt";
    h2_hcalTowerSumEtConeDR04VsEt_[0] = dbe_->book2D(histname+"All"," All photons hcalTowerSumEtConeDR04 vs Et: all Ecal ",etBin,etMin, etMax, etBin,etMin,etMax*0.1);
    h2_hcalTowerSumEtConeDR04VsEt_[1] = dbe_->book2D(histname+"Barrel"," All photons hcalTowerSumEtConeDR04 vs Et: Barrel ",etBin,etMin, etMax,etBin,etMin,etMax*0.1);
    h2_hcalTowerSumEtConeDR04VsEt_[2] = dbe_->book2D(histname+"Endcap"," All photons hcalTowerSumEtConeDR04 vs Et: Endcap ",etBin,etMin, etMax,etBin,etMin,etMax*0.1);
    histname="pHcalTowerSumEtConeDR04VsEt";
    p_hcalTowerSumEtConeDR04VsEt_[0] = dbe_->bookProfile(histname+"All","All photons hcalTowerSumEtDR04 vs Et: all Ecal ",  etBin,etMin, etMax, etBin,etMin,etMax*etScale, "");
    p_hcalTowerSumEtConeDR04VsEt_[1] = dbe_->bookProfile(histname+"Barrel","All photons hcalTowerSumEtDR04 vs Et: all Ecal ",  etBin,etMin, etMax, etBin,etMin,etMax*etScale, "");
    p_hcalTowerSumEtConeDR04VsEt_[2] = dbe_->bookProfile(histname+"Endcap","All photons hcalTowerSumEtDR04 vs Et: all Ecal ",  etBin,etMin, etMax, etBin,etMin,etMax*etScale, "");

    //
    histname = "isoTrkSolidConeDR04";
    h_isoTrkSolidConeDR04_[0][0] = dbe_->book1D(histname+"All",   "isoTrkSolidConeDR04: All Ecal",etBin,etMin,etMax*0.1);
    h_isoTrkSolidConeDR04_[0][1] = dbe_->book1D(histname+"Barrel","isoTrkSolidConeDR04: Barrel ", etBin,etMin,etMax*0.1);
    h_isoTrkSolidConeDR04_[0][2] = dbe_->book1D(histname+"Endcap","isoTrkSolidConeDR04: Endcap ", etBin,etMin,etMax*0.1);
    //
    histname="isoTrkSolidConeDR04VsEta";
    h2_isoTrkSolidConeDR04VsEta_[0] = dbe_->book2D(histname+"All"," All photons isoTrkSolidConeDR04 vs #eta: all Ecal ",  etaBin2,etaMin, etaMax, etBin,etMin,etMax*0.1);
    h2_isoTrkSolidConeDR04VsEta_[1] = dbe_->book2D(histname+"Unconv"," All photons isoTrkSolidConeDR04 vs #eta: all Ecal ",etaBin2,etaMin, etaMax,etBin,etMin,etMax*0.1);
    //
    histname="isoTrkSolidConeDR04VsEt";
    h2_isoTrkSolidConeDR04VsEt_[0] = dbe_->book2D(histname+"All"," All photons isoTrkSolidConeDR04 vs Et: all Ecal ",etBin,etMin, etMax, etBin,etMin,etMax*0.1);
    h2_isoTrkSolidConeDR04VsEt_[1] = dbe_->book2D(histname+"Unconv"," All photons isoTrkSolidConeDR04 vs Et: all Ecal ",etBin,etMin, etMax, etBin,etMin,etMax*0.1);
    //
    histname = "nTrkSolidConeDR04";
    h_nTrkSolidConeDR04_[0][0] = dbe_->book1D(histname+"All",   "nTrkSolidConeDR04: All Ecal",20,0., 20) ;
    h_nTrkSolidConeDR04_[0][1] = dbe_->book1D(histname+"Barrel","nTrkSolidConeDR04: Barrel ", 20,0., 20) ;
    h_nTrkSolidConeDR04_[0][2] = dbe_->book1D(histname+"Endcap","nTrkSolidConeDR04: Endcap ", 20,0., 20) ;
    //
    histname="nTrkSolidConeDR04VsEta";
    h2_nTrkSolidConeDR04VsEta_[0] = dbe_->book2D(histname+"All"," All photons nTrkSolidConeDR04 vs #eta: all Ecal ",  etaBin2,etaMin, etaMax, 20,0., 20) ;
    h2_nTrkSolidConeDR04VsEta_[1] = dbe_->book2D(histname+"Unconv"," All photons nTrkSolidConeDR04 vs #eta: all Ecal ",etaBin2,etaMin, etaMax,20,0., 20) ;
    //
    histname="nTrkSolidConeDR04VsEt";
    h2_nTrkSolidConeDR04VsEt_[0] = dbe_->book2D(histname+"All"," All photons nTrkSolidConeDR04 vs Et: all Ecal ",etBin,etMin, etMax, 20,0., 20) ;
    h2_nTrkSolidConeDR04VsEt_[1] = dbe_->book2D(histname+"Unconv"," All photons nTrkSolidConeDR04 vs Et: all Ecal ",etBin,etMin, etMax,20,0., 20) ;
    //
    histname = "phoE";
    h_phoE_[0][0]=dbe_->book1D(histname+"All"," Photon Energy: All ecal ", eBin,eMin, eMax);
    h_phoE_[0][1]=dbe_->book1D(histname+"Barrel"," Photon Energy: barrel ",eBin,eMin, eMax);
    h_phoE_[0][2]=dbe_->book1D(histname+"Endcap"," Photon Energy: Endcap ",eBin,eMin, eMax);

    histname = "phoEt";
    h_phoEt_[0][0] = dbe_->book1D(histname+"All"," Photon Transverse Energy: All ecal ", etBin,etMin, etMax);
    h_phoEt_[0][1] = dbe_->book1D(histname+"Barrel"," Photon Transverse Energy: Barrel ",etBin,etMin, etMax);
    h_phoEt_[0][2] = dbe_->book1D(histname+"Endcap"," Photon Transverse Energy: Endcap ",etBin,etMin, etMax);

    histname = "eRes";
    h_phoERes_[0][0] = dbe_->book1D(histname+"All"," Photon rec/true Energy: All ecal ", resBin,resMin, resMax);
    h_phoERes_[0][1] = dbe_->book1D(histname+"Barrel"," Photon rec/true Energy: Barrel ",resBin,resMin, resMax);
    h_phoERes_[0][2] = dbe_->book1D(histname+"Endcap"," Photon rec/true Energy: Endcap ",resBin,resMin, resMax);

    h_phoERes_[1][0] = dbe_->book1D(histname+"unconvAll"," Photon rec/true Energy if r9>0.93: All ecal ", resBin,resMin, resMax);
    h_phoERes_[1][1] = dbe_->book1D(histname+"unconvBarrel"," Photon rec/true Energy if r9>0.93: Barrel ",resBin,resMin, resMax);
    h_phoERes_[1][2] = dbe_->book1D(histname+"unconvEndcap"," Photon rec/true Energyif r9>0.93: Endcap ",resBin,resMin, resMax);

    h_phoERes_[2][0] = dbe_->book1D(histname+"convAll"," Photon rec/true Energy if r9<0.93: All ecal ", resBin,resMin, resMax);
    h_phoERes_[2][1] = dbe_->book1D(histname+"convBarrel"," Photon rec/true Energyif r9<0.93: Barrel ",resBin,resMin, resMax);
    h_phoERes_[2][2] = dbe_->book1D(histname+"convEndcap"," Photon rec/true Energyif r9<0.93: Endcap ",resBin,resMin, resMax);
  

    histname="eResVsEta";
    h2_eResVsEta_[0] = dbe_->book2D(histname+"All"," All photons E/Etrue vs #eta: all Ecal ",etaBin2,etaMin, etaMax,100, 0., 2.5);
    h2_eResVsEta_[1] = dbe_->book2D(histname+"Unconv"," Unconv photons E/Etrue vs #eta: all Ecal ",etaBin2,etaMin, etaMax,100, 0., 2.5);

    histname="pEResVsEta";
    p_eResVsEta_[0] = dbe_->bookProfile(histname+"All","All photons  E/Etrue vs #eta: all Ecal ",etaBin2,etaMin,etaMax,resBin,resMin, resMax,"");
    p_eResVsEta_[1] = dbe_->bookProfile(histname+"Unconv","Unconv photons  E/Etrue vs #eta: all Ecal",etaBin2,etaMin,etaMax,resBin,resMin, resMax,"");
    p_eResVsEta_[2] = dbe_->bookProfile(histname+"Conv","Conv photons  E/Etrue vs #eta: all Ecal",etaBin2,etaMin,etaMax,resBin,resMin, resMax,"");


    histname="eResVsEt";
    h2_eResVsEt_[0][0] = dbe_->book2D(histname+"All"," All photons E/Etrue vs true Et: all Ecal ",etBin,etMin, etMax,100, 0.9, 1.1);
    h2_eResVsEt_[0][1] = dbe_->book2D(histname+"unconv"," All photons E/Etrue vs true Et: all Ecal ",etBin,etMin, etMax,100, 0.9, 1.1);
    h2_eResVsEt_[0][2] = dbe_->book2D(histname+"conv"," All photons E/Etrue vs true Et: all Ecal ",etBin,etMin, etMax,100, 0.9, 1.1);
    h2_eResVsEt_[1][0] = dbe_->book2D(histname+"Barrel"," All photons E/Etrue vs true Et: Barrel ",etBin,etMin, etMax,100, 0.9, 1.1);
    h2_eResVsEt_[1][1] = dbe_->book2D(histname+"unconvBarrel"," All photons E/Etrue vs true Et: Barrel ",etBin,etMin, etMax,100, 0.9, 1.1);
    h2_eResVsEt_[1][2] = dbe_->book2D(histname+"convBarrel"," All photons E/Etrue vs true Et: Barrel ",etBin,etMin, etMax,100, 0.9, 1.1);
    h2_eResVsEt_[2][0] = dbe_->book2D(histname+"Endcap"," All photons E/Etrue vs true Et: Endcap ",etBin,etMin, etMax,100, 0.9, 1.1);
    h2_eResVsEt_[2][1] = dbe_->book2D(histname+"unconvEndcap"," All photons E/Etrue vs true Et: Endcap ",etBin,etMin, etMax,100, 0.9, 1.1);
    h2_eResVsEt_[2][2] = dbe_->book2D(histname+"convEndcap"," All photons E/Etrue vs true Et: Endcap ",etBin,etMin, etMax,100, 0.9, 1.1);
    histname="pEResVsEt";
    p_eResVsEt_[0][0] = dbe_->bookProfile(histname+"All","All photons  E/Etrue vs Et: all Ecal ",etBin,etMin,etMax,resBin,resMin, resMax,"");
    p_eResVsEt_[0][1] = dbe_->bookProfile(histname+"unconv","All photons  E/Etrue vs Et: all Ecal ",etBin,etMin,etMax,resBin,resMin, resMax,"");
    p_eResVsEt_[0][2] = dbe_->bookProfile(histname+"conv","All photons  E/Etrue vs Et: all Ecal ",etBin,etMin,etMax,resBin,resMin, resMax,"");
    p_eResVsEt_[1][0] = dbe_->bookProfile(histname+"Barrel","All photons  E/Etrue vs Et: Barrel ",etBin,etMin,etMax,resBin,resMin, resMax,"");
    p_eResVsEt_[1][1] = dbe_->bookProfile(histname+"unconvBarrel","All photons  E/Etrue vs Et: Barrel ",etBin,etMin,etMax,resBin,resMin, resMax,"");
    p_eResVsEt_[1][2] = dbe_->bookProfile(histname+"convBarrel","All photons  E/Etrue vs Et: Barrel ",etBin,etMin,etMax,resBin,resMin, resMax,"");
    p_eResVsEt_[2][0] = dbe_->bookProfile(histname+"Endcap","All photons  E/Etrue vs Et: Endcap ",etBin,etMin,etMax,resBin,resMin, resMax,"");
    p_eResVsEt_[2][1] = dbe_->bookProfile(histname+"unconvEndcap","All photons  E/Etrue vs Et: Endcap ",etBin,etMin,etMax,resBin,resMin, resMax,"");
    p_eResVsEt_[2][2] = dbe_->bookProfile(histname+"convEndcap","All photons  E/Etrue vs Et: Endcap ",etBin,etMin,etMax,resBin,resMin, resMax,"");


    histname="eResVsR9";
    h2_eResVsR9_[0] = dbe_->book2D(histname+"All"," All photons E/Etrue vs R9: all Ecal ",r9Bin*2,r9Min, r9Max,100, 0., 2.5);
    h2_eResVsR9_[1] = dbe_->book2D(histname+"Barrel"," All photons E/Etrue vs R9: Barrel ",  r9Bin*2,r9Min, r9Max,100, 0.,2.5);
    h2_eResVsR9_[2] = dbe_->book2D(histname+"Endcap"," All photons E/Etrue vs R9: Endcap ",  r9Bin*2,r9Min, r9Max,100, 0., 2.5);
    histname="pEResVsR9";
    p_eResVsR9_[0] = dbe_->bookProfile(histname+"All"," All photons  E/Etrue vs R9: all Ecal ",r9Bin*2,r9Min, r9Max,resBin,resMin, resMax,"");
    p_eResVsR9_[1] = dbe_->bookProfile(histname+"Barrel"," All photons  E/Etrue vs R9: Barrel ",  r9Bin*2,r9Min, r9Max,resBin,resMin, resMax,"");
    p_eResVsR9_[2] = dbe_->bookProfile(histname+"Endcap"," All photons  E/Etrue vs R9: Endcap ",  r9Bin*2,r9Min, r9Max,resBin,resMin, resMax,"");
    histname="sceResVsR9";
    h2_sceResVsR9_[0] = dbe_->book2D(histname+"All"," All photons scE/Etrue vs R9: all Ecal ",r9Bin*2,r9Min, r9Max,100, 0., 2.5);
    h2_sceResVsR9_[1] = dbe_->book2D(histname+"Barrel"," All photons scE/Etrue vs R9: Barrel ",  r9Bin*2,r9Min, r9Max,100, 0.,2.5);
    h2_sceResVsR9_[2] = dbe_->book2D(histname+"Endcap"," All photons scE/Etrue vs R9: Endcap ",  r9Bin*2,r9Min, r9Max,100, 0., 2.5);
    histname="scpEResVsR9";
    p_sceResVsR9_[0] = dbe_->bookProfile(histname+"All"," All photons  scE/Etrue vs R9: all Ecal ",r9Bin*2,r9Min, r9Max,resBin,resMin, resMax,""); 
    p_sceResVsR9_[1] = dbe_->bookProfile(histname+"Barrel"," All photons  scE/Etrue vs R9: Barrel ",  r9Bin*2,r9Min, r9Max,resBin,resMin, resMax,"");
    p_sceResVsR9_[2] = dbe_->bookProfile(histname+"Endcap"," All photons  scE/Etrue vs R9: Endcap ",  r9Bin*2,r9Min, r9Max,resBin,resMin, resMax,"");
 


    // Photon pair invariant mass
    histname = "gamgamMass"; 
    h_gamgamMass_[0][0] = dbe_->book1D(histname+"All","2 photons invariant mass: All ecal ", ggMassBin, ggMassMin, ggMassMax);
    h_gamgamMass_[0][1] = dbe_->book1D(histname+"Barrel","2 photons invariant mass:  Barrel ",ggMassBin, ggMassMin, ggMassMax);
    h_gamgamMass_[0][2] = dbe_->book1D(histname+"Endcap","2 photons invariant mass:  Endcap ",ggMassBin, ggMassMin, ggMassMax);
    //
    histname = "gamgamMassNoConv";
    h_gamgamMass_[1][0] = dbe_->book1D(histname+"All","2 photons with no conversion invariant mass: All ecal ",ggMassBin, ggMassMin, ggMassMax); 
    h_gamgamMass_[1][1] = dbe_->book1D(histname+"Barrel","2 photons with no conversion  invariant mass:  Barrel ",ggMassBin, ggMassMin, ggMassMax);
    h_gamgamMass_[1][2] = dbe_->book1D(histname+"Endcap","2 photons with no conversion  invariant mass:  Endcap ",ggMassBin, ggMassMin, ggMassMax);
    //
    histname = "gamgamMassConv";
    h_gamgamMass_[2][0] = dbe_->book1D(histname+"All","2 photons with conversion invariant mass: All ecal ", ggMassBin, ggMassMin, ggMassMax);
    h_gamgamMass_[2][1] = dbe_->book1D(histname+"Barrel","2 photons with  conversion  invariant mass:  Barrel ",ggMassBin, ggMassMin, ggMassMax);
    h_gamgamMass_[2][2] = dbe_->book1D(histname+"Endcap","2 photons with  conversion  invariant mass:  Endcap ",ggMassBin, ggMassMin, ggMassMax);


    dbe_->setCurrentFolder("EgammaV/PhotonValidator/ConversionInfo");

    histname="nConv";
    h_nConv_[0][0] = dbe_->book1D(histname+"All","Number Of Conversions per isolated candidates per events: All Ecal  ",10,-0.5, 9.5);
    h_nConv_[0][1] = dbe_->book1D(histname+"Barrel","Number Of Conversions per isolated candidates per events: Ecal Barrel  ",10,-0.5, 9.5);
    h_nConv_[0][2] = dbe_->book1D(histname+"Endcap","Number Of Conversions per isolated candidates per events: Ecal Endcap ",10,-0.5, 9.5);

    h_convEta_[0] = dbe_->book1D("convEta1"," converted Photon Eta >1 track",etaBin,etaMin, etaMax) ;
    h_convEta_[1] = dbe_->book1D("convEta2"," converted Photon Eta =2 tracks ",etaBin,etaMin, etaMax) ;
    h_convEta_[2] = dbe_->book1D("convEta2ass"," converted Photon Eta =2 tracks, both ass ",etaBin,etaMin, etaMax) ;
    h_convPhi_[0] = dbe_->book1D("convPhi"," converted Photon  Phi ",phiBin,phiMin,phiMax) ;


    histname = "convERes";
    h_convERes_[0][0] = dbe_->book1D(histname+"All"," Conversion rec/true Energy: All ecal ", resBin,resMin, resMax);
    h_convERes_[0][1] = dbe_->book1D(histname+"Barrel"," Conversion rec/true Energy: Barrel ",resBin,resMin, resMax);
    h_convERes_[0][2] = dbe_->book1D(histname+"Endcap"," Conversion rec/true Energy: Endcap ",resBin,resMin, resMax);
    histname = "convPRes";
    h_convPRes_[1][0] = dbe_->book1D(histname+"All"," Conversion rec/true Momentum from tracks : All ecal ", resBin,0.,1.5);
    h_convPRes_[1][1] = dbe_->book1D(histname+"Barrel"," Conversion rec/true Momentum from tracks: Barrel ",resBin,0., 1.5);
    h_convPRes_[1][2] = dbe_->book1D(histname+"Endcap"," Conversion rec/true Momentum from tracks: Endcap ",resBin,0., 1.5);



    histname="r9VsTracks";
    h_r9VsNofTracks_[0][0] = dbe_->book2D(histname+"All"," photons r9 vs nTracks from conversions: All Ecal",r9Bin,r9Min, r9Max, 3, -0.5, 2.5) ;
    h_r9VsNofTracks_[0][1] = dbe_->book2D(histname+"Barrel"," photons r9 vs nTracks from conversions: Barrel Ecal",r9Bin,r9Min, r9Max, 3, -0.5, 2.5) ;
    h_r9VsNofTracks_[0][2] = dbe_->book2D(histname+"Endcap"," photons r9 vs nTracks from conversions: Endcap Ecal",r9Bin,r9Min, r9Max, 3, -0.5, 2.5) ;

    histname="mvaOut";
    h_mvaOut_[0] = dbe_->book1D(histname+"All"," mvaOut for all conversions : All Ecal",100, 0., 1.);
    h_mvaOut_[1] = dbe_->book1D(histname+"Barrel"," mvaOut for all conversions : Barrel Ecal",100, 0., 1.);
    h_mvaOut_[2] = dbe_->book1D(histname+"Endcap"," mvaOut for all conversions : Endcap Ecal",100, 0., 1.);



    histname="EoverPtracks";
    h_EoverPTracks_[0][0] = dbe_->book1D(histname+"BarrelPix"," photons conversion E/p: barrel pix",eoverpBin, eoverpMin,eoverpMax);
    h_EoverPTracks_[0][1] = dbe_->book1D(histname+"BarrelTib"," photons conversion E/p: barrel tib",eoverpBin, eoverpMin,eoverpMax);
    h_EoverPTracks_[0][2] = dbe_->book1D(histname+"BarrelTob"," photons conversion E/p: barrel tob ",eoverpBin, eoverpMin,eoverpMax);

    h_EoverPTracks_[1][0] = dbe_->book1D(histname+"All"," photons conversion E/p: all Ecal ",100, 0., 5.);
    h_EoverPTracks_[1][1] = dbe_->book1D(histname+"Barrel"," photons conversion E/p: Barrel Ecal",100, 0., 5.);
    h_EoverPTracks_[1][2] = dbe_->book1D(histname+"Endcap"," photons conversion E/p: Endcap Ecal ",100, 0., 5.);

    histname="PoverEtracks";
    h_PoverETracks_[1][0] = dbe_->book1D(histname+"All"," photons conversion p/E: all Ecal ",povereBin, povereMin, povereMax);
    h_PoverETracks_[1][1] = dbe_->book1D(histname+"Barrel"," photons conversion p/E: Barrel Ecal",povereBin, povereMin, povereMax);
    h_PoverETracks_[1][2] = dbe_->book1D(histname+"Endcap"," photons conversion p/E: Endcap Ecal ",povereBin, povereMin, povereMax);



    histname="EoverEtrueVsEoverP";
    h2_EoverEtrueVsEoverP_[0] = dbe_->book2D(histname+"All"," photons conversion E/Etrue vs E/P: all Ecal ",100, 0., 5., 100, 0.5, 1.5);
    h2_EoverEtrueVsEoverP_[1] = dbe_->book2D(histname+"Barrel"," photons conversion  E/Etrue vs E/: Barrel Ecal",100, 0., 5.,100, 0.5, 1.5);
    h2_EoverEtrueVsEoverP_[2] = dbe_->book2D(histname+"Endcap"," photons conversion  E/Etrue vs E/: Endcap Ecal ",100, 0., 5., 100, 0.5, 1.5);
    histname="PoverPtrueVsEoverP";
    h2_PoverPtrueVsEoverP_[0] = dbe_->book2D(histname+"All"," photons conversion P/Ptrue vs E/P: all Ecal ",100, 0., 5., 100, 0., 2.5);
    h2_PoverPtrueVsEoverP_[1] = dbe_->book2D(histname+"Barrel"," photons conversion  P/Ptrue vs E/: Barrel Ecal",100, 0., 5.,100, 0., 2.5);
    h2_PoverPtrueVsEoverP_[2] = dbe_->book2D(histname+"Endcap"," photons conversion  P/Ptrue vs E/: Endcap Ecal ",100, 0., 5., 100, 0., 2.5);

    histname="EoverEtrueVsEta";
    h2_EoverEtrueVsEta_[0] = dbe_->book2D(histname+"All"," photons conversion with 2 (associated) reco tracks  E/Etrue vs #eta: all Ecal ",etaBin2,etaMin, etaMax,100, 0., 2.5);
    histname="pEoverEtrueVsEta";
    p_EoverEtrueVsEta_[0] = dbe_->bookProfile(histname+"All"," photons conversion with 2 (associated) reco tracks E/Etrue vs #eta: all Ecal ",etaBin2,etaMin, etaMax, 100,0.,2.5,"");


    histname="EoverEtrueVsEta";
    h2_EoverEtrueVsEta_[1] = dbe_->book2D(histname+"All2"," photons conversion  2 reco tracks  E/Etrue vs #eta: all Ecal ",etaBin2,etaMin, etaMax,100, 0., 2.5);
    histname="pEoverEtrueVsEta";
    p_EoverEtrueVsEta_[1] = dbe_->bookProfile(histname+"All2"," photons conversion  2 reco tracks  E/Etrue vs #eta: all Ecal ",etaBin2,etaMin, etaMax,100, 0., 2.5,"");


    histname="EoverEtrueVsR";
    h2_EoverEtrueVsR_[0] = dbe_->book2D(histname+"All"," photons conversion E/Etrue vs R: all Ecal ",rBin,rMin, rMax,100, 0., 2.5);
    histname="pEoverEtrueVsR";
    p_EoverEtrueVsR_[0] = dbe_->bookProfile(histname+"All"," photons conversion E/Etrue vs R: all Ecal ",rBin,rMin,rMax, 100, 0., 2.5, "");



    histname="PoverPtrueVsEta";
    h2_PoverPtrueVsEta_[0] = dbe_->book2D(histname+"All"," photons conversion P/Ptrue vs #eta: all Ecal ",etaBin2,etaMin, etaMax,100, 0., 5.);
    histname="pPoverPtrueVsEta";
    p_PoverPtrueVsEta_[0] = dbe_->bookProfile(histname+"All"," photons conversion P/Ptrue vs #eta: all Ecal ",etaBin2,etaMin, etaMax,100, 0., 5.,""); 

    histname="EoverPVsEta";
    h2_EoverPVsEta_[0] = dbe_->book2D(histname+"All"," photons conversion E/P vs #eta: all Ecal ",etaBin2,etaMin, etaMax,100, 0., 5.);
    histname="pEoverPVsEta";
    p_EoverPVsEta_[0] = dbe_->bookProfile(histname+"All"," photons conversion E/P vs #eta: all Ecal ",etaBin2,etaMin, etaMax, 100, 0., 5.,"");

    histname="EoverPVsR";
    h2_EoverPVsR_[0] = dbe_->book2D(histname+"All"," photons conversion E/P vs R: all Ecal ",rBin,rMin, rMax,100, 0., 5.);
    histname="pEoverPVsR";
    p_EoverPVsR_[0] = dbe_->bookProfile(histname+"All"," photons conversion E/P vs R: all Ecal ",rBin,rMin,rMax, 100, 0., 5.,"");

    histname="etaVsRsim";
    h2_etaVsRsim_[0] = dbe_->book2D(histname+"All"," eta(sim) vs R (sim) for associated conversions: all Ecal ",etaBin, etaMin, etaMax,rBin,rMin, rMax);
    histname="etaVsRreco";
    h2_etaVsRreco_[0] = dbe_->book2D(histname+"All"," eta(reco) vs R (reco) for associated conversions: all Ecal ",etaBin, etaMin, etaMax,rBin,rMin, rMax);
    

    histname="hInvMass";
    h_invMass_[0][0]= dbe_->book1D(histname+"All_AllTracks"," Photons:Tracks from conversion: Pair invariant mass: all Ecal ",100, 0., 1.5);
    h_invMass_[0][1]= dbe_->book1D(histname+"Barrel_AllTracks"," Photons:Tracks from conversion: Pair invariant mass: Barrel Ecal ",100, 0., 1.5);
    h_invMass_[0][2]= dbe_->book1D(histname+"Endcap_AllTracks"," Photons:Tracks from conversion: Pair invariant mass: Endcap Ecal ",100, 0., 1.5);
    histname="hInvMass";
    h_invMass_[1][0]= dbe_->book1D(histname+"All_AssTracks"," Photons:Tracks from conversion: Pair invariant mass: all Ecal ",100, 0., 1.5);
    h_invMass_[1][1]= dbe_->book1D(histname+"Barrel_AssTracks"," Photons:Tracks from conversion: Pair invariant mass: Barrel Ecal ",100, 0., 1.5);
    h_invMass_[1][2]= dbe_->book1D(histname+"Endcap_AssTracks"," Photons:Tracks from conversion: Pair invariant mass: Endcap Ecal ",100, 0., 1.5);


    histname="hDPhiTracksAtVtx";
    h_DPhiTracksAtVtx_[1][0] =dbe_->book1D(histname+"All", " Photons:Tracks from conversions: #delta#phi Tracks at vertex: all Ecal",dPhiTracksBin,dPhiTracksMin,dPhiTracksMax); 
    h_DPhiTracksAtVtx_[1][1] =dbe_->book1D(histname+"Barrel", " Photons:Tracks from conversions: #delta#phi Tracks at vertex: Barrel Ecal",dPhiTracksBin,dPhiTracksMin,dPhiTracksMax); 
    h_DPhiTracksAtVtx_[1][2] =dbe_->book1D(histname+"Endcap", " Photons:Tracks from conversions: #delta#phi Tracks at vertex: Endcap Ecal",dPhiTracksBin,dPhiTracksMin,dPhiTracksMax); 

    histname="hDPhiTracksAtVtxVsEta";
    h2_DPhiTracksAtVtxVsEta_ = dbe_->book2D(histname+"All","  Photons:Tracks from conversions: #delta#phi Tracks at vertex vs #eta",etaBin2,etaMin, etaMax,100, -0.5, 0.5);
    histname="pDPhiTracksAtVtxVsEta";
    p_DPhiTracksAtVtxVsEta_ = dbe_->bookProfile(histname+"All"," Photons:Tracks from conversions: #delta#phi Tracks at vertex vs #eta ",etaBin2,etaMin, etaMax, 100, -0.5, 0.5,"");

    histname="hDPhiTracksAtVtxVsR";
    h2_DPhiTracksAtVtxVsR_ = dbe_->book2D(histname+"All","  Photons:Tracks from conversions: #delta#phi Tracks at vertex vs R",rBin,rMin, rMax,100, -0.5, 0.5);
    histname="pDPhiTracksAtVtxVsR";
    p_DPhiTracksAtVtxVsR_ = dbe_->bookProfile(histname+"All"," Photons:Tracks from conversions: #delta#phi Tracks at vertex vs R ",rBin,rMin, rMax,100, -0.5, 0.5,""); 


    histname="hDCotTracks";
    h_DCotTracks_[1][0]= dbe_->book1D(histname+"All"," Photons:Tracks from conversions #delta cotg(#Theta) Tracks: all Ecal ",dCotTracksBin,dCotTracksMin,dCotTracksMax); 
    h_DCotTracks_[1][1]= dbe_->book1D(histname+"Barrel"," Photons:Tracks from conversions #delta cotg(#Theta) Tracks: Barrel Ecal ",dCotTracksBin,dCotTracksMin,dCotTracksMax); 
    h_DCotTracks_[1][2]= dbe_->book1D(histname+"Endcap"," Photons:Tracks from conversions #delta cotg(#Theta) Tracks: Endcap Ecal ",dCotTracksBin,dCotTracksMin,dCotTracksMax); 


    histname="hDCotTracksVsEta";
    h2_DCotTracksVsEta_ = dbe_->book2D(histname+"All","  Photons:Tracks from conversions:  #delta cotg(#Theta) Tracks vs #eta",etaBin2,etaMin, etaMax,100, -0.2, 0.2);
    histname="pDCotTracksVsEta";
    p_DCotTracksVsEta_ = dbe_->bookProfile(histname+"All"," Photons:Tracks from conversions:  #delta cotg(#Theta) Tracks vs #eta ",etaBin2,etaMin, etaMax, 100, -0.2, 0.2,"");

    histname="hDCotTracksVsR";
    h2_DCotTracksVsR_ = dbe_->book2D(histname+"All","  Photons:Tracks from conversions:  #delta cotg(#Theta)  Tracks at vertex vs R",rBin,rMin, rMax,100, -0.2, 0.2);
    histname="pDCotTracksVsR";
    p_DCotTracksVsR_ = dbe_->bookProfile(histname+"All"," Photons:Tracks from conversions:  #delta cotg(#Theta) Tracks at vertex vs R ",rBin,rMin, rMax,100, -0.2, 0.2,""); 


    histname="hDistMinAppTracks";
    h_distMinAppTracks_[1][0]= dbe_->book1D(histname+"All"," Photons:Tracks from conversions Min Approach Dist Tracks: all Ecal ",dEtaTracksBin,-0.1,0.6); 
    h_distMinAppTracks_[1][1]= dbe_->book1D(histname+"Barrel"," Photons:Tracks from conversions Min Approach Dist Tracks: Barrel Ecal ",dEtaTracksBin,-0.1,0.6); 
    h_distMinAppTracks_[1][2]= dbe_->book1D(histname+"Endcap"," Photons:Tracks from conversions Min Approach Dist Tracks: Endcap Ecal ",dEtaTracksBin,-0.1,0.6); 

    histname="hDPhiTracksAtEcal";
    h_DPhiTracksAtEcal_[1][0]= dbe_->book1D(histname+"All"," Photons:Tracks from conversions:  #delta#phi at Ecal : all Ecal ",dPhiTracksBin,0.,dPhiTracksMax); 
    h_DPhiTracksAtEcal_[1][1]= dbe_->book1D(histname+"Barrel"," Photons:Tracks from conversions:  #delta#phi at Ecal : Barrel Ecal ",dPhiTracksBin,0.,dPhiTracksMax); 
    h_DPhiTracksAtEcal_[1][2]= dbe_->book1D(histname+"Endcap"," Photons:Tracks from conversions:  #delta#phi at Ecal : Endcap Ecal ",dPhiTracksBin,0.,dPhiTracksMax); 
    histname="h2_DPhiTracksAtEcalVsR";
    h2_DPhiTracksAtEcalVsR_= dbe_->book2D(histname+"All"," Photons:Tracks from conversions:  #delta#phi at Ecal vs R : all Ecal ",rBin,rMin, rMax, dPhiTracksBin,0.,dPhiTracksMax);
    histname="pDPhiTracksAtEcalVsR";
    p_DPhiTracksAtEcalVsR_ = dbe_->bookProfile(histname+"All"," Photons:Tracks from conversions:  #delta#phi at Ecal  vs R ",rBin,rMin, rMax, dPhiTracksBin,0.,dPhiTracksMax,"");

    histname="h2_DPhiTracksAtEcalVsEta";
    h2_DPhiTracksAtEcalVsEta_= dbe_->book2D(histname+"All"," Photons:Tracks from conversions:  #delta#phi at Ecal vs #eta : all Ecal ",etaBin2,etaMin, etaMax, dPhiTracksBin,0.,dPhiTracksMax);
    histname="pDPhiTracksAtEcalVsEta";
    p_DPhiTracksAtEcalVsEta_ = dbe_->bookProfile(histname+"All"," Photons:Tracks from conversions:  #delta#phi at Ecal  vs #eta ",etaBin2,etaMin, etaMax,dPhiTracksBin,0.,dPhiTracksMax,""); 



    histname="hDEtaTracksAtEcal";
    h_DEtaTracksAtEcal_[1][0]= dbe_->book1D(histname+"All"," Photons:Tracks from conversions:  #delta#eta at Ecal : all Ecal ",dEtaTracksBin,dEtaTracksMin,dEtaTracksMax); 
    h_DEtaTracksAtEcal_[1][1]= dbe_->book1D(histname+"Barrel"," Photons:Tracks from conversions:  #delta#eta at Ecal : Barrel Ecal ",dEtaTracksBin,dEtaTracksMin,dEtaTracksMax); 
    h_DEtaTracksAtEcal_[1][2]= dbe_->book1D(histname+"Endcap"," Photons:Tracks from conversions:  #delta#eta at Ecal : Endcap Ecal ",dEtaTracksBin,dEtaTracksMin,dEtaTracksMax); 
   

    h_convVtxRvsZ_[0] =   dbe_->book2D("convVtxRvsZAll"," Photon Reco conversion vtx position",zBinForXray, zMinForXray, zMaxForXray, rBinForXray, rMinForXray, rMaxForXray); 
    h_convVtxRvsZ_[1] =   dbe_->book2D("convVtxRvsZBarrel"," Photon Reco conversion vtx position",zBinForXray, zMinForXray, zMaxForXray, rBinForXray, rMinForXray, rMaxForXray); 
    h_convVtxRvsZ_[2] =   dbe_->book2D("convVtxRvsZEndcap"," Photon Reco conversion vtx position",zBin2ForXray, zMinForXray, zMaxForXray, rBinForXray, rMinForXray, rMaxForXray); 
    h_convVtxYvsX_ =   dbe_->book2D("convVtxYvsXTrkBarrel"," Photon Reco conversion vtx position, (x,y) eta<1 ",100, -80., 80., 100, -80., 80.); 
    /// zooms
    h_convVtxRvsZ_zoom_[0] =  dbe_->book2D("convVtxRvsZBarrelZoom1"," Photon Reco conversion vtx position",zBinForXray, zMinForXray, zMaxForXray, rBinForXray, -10., 40.); 
    h_convVtxRvsZ_zoom_[1] =  dbe_->book2D("convVtxRvsZBarrelZoom2"," Photon Reco conversion vtx position",zBinForXray, zMinForXray, zMaxForXray, rBinForXray, -10., 20.); 
    h_convVtxYvsX_zoom_[0] =   dbe_->book2D("convVtxYvsXTrkBarrelZoom1"," Photon Reco conversion vtx position, (x,y) eta<1 ",100, -40., 40., 100, -40., 40.); 
    h_convVtxYvsX_zoom_[1] =   dbe_->book2D("convVtxYvsXTrkBarrelZoom2"," Photon Reco conversion vtx position, (x,y) eta<1 ",100, -20., 20., 100, -20., 20.); 

    h_convVtxdX_ =   dbe_->book1D("convVtxdX"," Photon Reco conversion vtx dX",100, -20.,20.);
    h_convVtxdY_ =   dbe_->book1D("convVtxdY"," Photon Reco conversion vtx dY",100, -20.,20.);
    h_convVtxdZ_ =   dbe_->book1D("convVtxdZ"," Photon Reco conversion vtx dZ",100, -20.,20.);
    h_convVtxdR_ =   dbe_->book1D("convVtxdR"," Photon Reco conversion vtx dR",100, -20.,20.);

    h_convVtxdX_barrel_ =   dbe_->book1D("convVtxdX_barrel"," Photon Reco conversion vtx dX, |eta|<=1.2",100, -20.,20.);
    h_convVtxdY_barrel_ =   dbe_->book1D("convVtxdY_barrel"," Photon Reco conversion vtx dY, |eta|<=1.2 ",100, -20.,20.);
    h_convVtxdZ_barrel_ =   dbe_->book1D("convVtxdZ_barrel"," Photon Reco conversion vtx dZ, |eta|<=1.2,",100, -20.,20.);
    h_convVtxdR_barrel_ =   dbe_->book1D("convVtxdR_barrel"," Photon Reco conversion vtx dR, |eta|<=1.2",100, -20.,20.);
    h_convVtxdX_endcap_ =   dbe_->book1D("convVtxdX_endcap"," Photon Reco conversion vtx dX,  |eta|>1.2",100, -20.,20.);
    h_convVtxdY_endcap_ =   dbe_->book1D("convVtxdY_endcap"," Photon Reco conversion vtx dY,  |eta|>1.2",100, -20.,20.);
    h_convVtxdZ_endcap_ =   dbe_->book1D("convVtxdZ_endcap"," Photon Reco conversion vtx dZ,  |eta|>1.2",100, -20.,20.);
    h_convVtxdR_endcap_ =   dbe_->book1D("convVtxdR_endcap"," Photon Reco conversion vtx dR,  |eta|>1.2 ",100, -20.,20.);


    h_convVtxdPhi_ =   dbe_->book1D("convVtxdPhi"," Photon Reco conversion vtx dPhi",100, -0.005,0.005);
    h_convVtxdEta_ =   dbe_->book1D("convVtxdEta"," Photon Reco conversion vtx dEta",100, -0.5,0.5);



    h2_convVtxdRVsR_ =  dbe_->book2D("h2ConvVtxdRVsR","Photon Reco conversion vtx dR vsR" ,rBin,rMin, rMax,100, -20.,20.);
    p_convVtxdRVsR_ =  dbe_->bookProfile("pConvVtxdRVsR","Photon Reco conversion vtx dR vsR" ,rBin,rMin, rMax ,100, -20.,20., "");
    h2_convVtxdRVsEta_ =  dbe_->book2D("h2ConvVtxdRVsEta","Photon Reco conversion vtx dR vs Eta" ,etaBin2,etaMin, etaMax,100, -20.,20.);
    p_convVtxdRVsEta_ =  dbe_->bookProfile("pConvVtxdRVsEta","Photon Reco conversion vtx dR vs Eta" ,etaBin2,etaMin, etaMax, 100, -20.,20., "");
    
    h2_convVtxRrecVsTrue_ =  dbe_->book2D("h2ConvVtxRrecVsTrue","Photon Reco conversion vtx R rec vs true" ,rBin,rMin, rMax,rBin,rMin, rMax);

    histname="vtxChi2";
    h_vtxChi2_[0] = dbe_->book1D(histname+"All","vertex #chi^{2} all", 100, chi2Min, chi2Max); 
    h_vtxChi2_[1] = dbe_->book1D(histname+"Barrel","vertex #chi^{2} barrel", 100, chi2Min, chi2Max); 
    h_vtxChi2_[2] = dbe_->book1D(histname+"Endcap","vertex #chi^{2} endcap", 100, chi2Min, chi2Max); 
    histname="vtxChi2Prob";
    h_vtxChi2Prob_[0] = dbe_->book1D(histname+"All","vertex #chi^{2} all", 100, 0., 1.);
    h_vtxChi2Prob_[1] = dbe_->book1D(histname+"Barrel","vertex #chi^{2} barrel", 100, 0., 1.);
    h_vtxChi2Prob_[2] = dbe_->book1D(histname+"Endcap","vertex #chi^{2} endcap", 100, 0., 1.);

    h_zPVFromTracks_[1] =  dbe_->book1D("zPVFromTracks"," Photons: PV z from conversion tracks",100, -25., 25.);
    h_dzPVFromTracks_[1] =  dbe_->book1D("dzPVFromTracks"," Photons: PV Z_rec - Z_true from conversion tracks",100, -5., 5.);
    h2_dzPVVsR_ =  dbe_->book2D("h2dzPVVsR","Photon Reco conversions: dz(PV) vs R" ,rBin,rMin, rMax,100, -3.,3.);
    p_dzPVVsR_ =  dbe_->bookProfile("pdzPVVsR","Photon Reco conversions: dz(PV) vs R" ,rBin,rMin, rMax, 100, -3.,3.,"");


    //////////////////// plots per track
    histname="nHitsVsEta";
    nHitsVsEta_[0] =  dbe_->book2D(histname+"AllTracks","Photons:Tracks from conversions: # of hits vs #eta all tracks",etaBin,etaMin, etaMax,25,0., 25.);
    histname="h_nHitsVsEta";
    p_nHitsVsEta_[0] =  dbe_->bookProfile(histname+"AllTracks","Photons:Tracks from conversions: # of hits vs #eta all tracks",etaBin,etaMin, etaMax, 25,-0.5, 24.5,"");

    histname="nHitsVsEta";
    nHitsVsEta_[1] =  dbe_->book2D(histname+"AssTracks","Photons:Tracks from conversions: # of hits vs #eta associated tracks",etaBin,etaMin, etaMax,25,0., 25.);
    histname="h_nHitsVsEta";
    p_nHitsVsEta_[1] =  dbe_->bookProfile(histname+"AssTracks","Photons:Tracks from conversions: # of hits vs #eta associated tracks",etaBin,etaMin, etaMax, 25,-0.5, 24.5,"");


    histname="nHitsVsR";
    nHitsVsR_[0] =  dbe_->book2D(histname+"AllTracks","Photons:Tracks from conversions: # of hits vs radius all tracks" ,rBin,rMin, rMax,25,0.,25);
    histname="h_nHitsVsR";
    p_nHitsVsR_[0] =  dbe_->bookProfile(histname+"AllTracks","Photons:Tracks from conversions: # of hits vs radius all tracks",rBin,rMin, rMax, 25,-0.5, 24.5,"");
    histname="tkChi2";
    h_tkChi2_[0] = dbe_->book1D(histname+"AllTracks","Photons:Tracks from conversions: #chi^{2} of all tracks", 100, chi2Min, chi2Max);  
    histname="tkChi2Large";
    h_tkChi2Large_[0] = dbe_->book1D(histname+"AllTracks","Photons:Tracks from conversions: #chi^{2} of all tracks", 1000, 0., 5000.0); 

    histname="nHitsVsR";
    nHitsVsR_[1] =  dbe_->book2D(histname+"AssTracks","Photons:Tracks from conversions: # of hits vs radius associated tracks" ,rBin,rMin, rMax,25,0.,25);
    histname="h_nHitsVsR";
    p_nHitsVsR_[1] =  dbe_->bookProfile(histname+"AssTracks","Photons:Tracks from conversions: # of hits vs radius associated tracks",rBin,rMin, rMax, 25,-0.5, 24.5,"");

    histname="tkChi2";
    h_tkChi2_[1] = dbe_->book1D(histname+"AssTracks","Photons:Tracks from conversions: #chi^{2} of associated  tracks", 100, chi2Min, chi2Max); 
    histname="tkChi2Large";
    h_tkChi2Large_[1] = dbe_->book1D(histname+"AssTracks","Photons:Tracks from conversions: #chi^{2} of associated  tracks", 1000, 0., 5000.0); 

    histname="h2Chi2VsEta";
    h2_Chi2VsEta_[0]=dbe_->book2D(histname+"All"," Reco Track  #chi^{2} vs #eta: All ",etaBin2,etaMin, etaMax,100, chi2Min, chi2Max); 
    histname="pChi2VsEta";
    p_Chi2VsEta_[0]=dbe_->bookProfile(histname+"All"," Reco Track #chi^{2} vs #eta : All ",etaBin2,etaMin, etaMax, 100, chi2Min, chi2Max,""); 


    histname="h2Chi2VsR";
    h2_Chi2VsR_[0]=dbe_->book2D(histname+"All"," Reco Track  #chi^{2} vs R: All ",rBin,rMin, rMax,100,chi2Min, chi2Max);
    histname="pChi2VsR";
    p_Chi2VsR_[0]=dbe_->bookProfile(histname+"All"," Reco Track #chi^{2} vas R : All ",rBin,rMin,rMax, 100,chi2Min, chi2Max,"");



    histname="hTkD0";
    h_TkD0_[0]=dbe_->book1D(histname+"All"," Reco Track D0*q: All ",100,-0.1,0.6);
    h_TkD0_[1]=dbe_->book1D(histname+"Barrel"," Reco Track D0*q: Barrel ",100,-0.1,0.6);
    h_TkD0_[2]=dbe_->book1D(histname+"Endcap"," Reco Track D0*q: Endcap ",100,-0.1,0.6);



    histname="hTkPtPull";
    h_TkPtPull_[0]=dbe_->book1D(histname+"All"," Reco Track Pt pull: All ",100, -10., 10.);
    histname="hTkPtPull";
    h_TkPtPull_[1]=dbe_->book1D(histname+"Barrel"," Reco Track Pt pull: Barrel ",100, -10., 10.);
    histname="hTkPtPull";
    h_TkPtPull_[2]=dbe_->book1D(histname+"Endcap"," Reco Track Pt pull: Endcap ",100, -10., 10.);

    histname="h2TkPtPullEta";
    h2_TkPtPull_[0]=dbe_->book2D(histname+"All"," Reco Track Pt pull: All ",etaBin2,etaMin, etaMax,100, -10., 10.);
    histname="pTkPtPullEta";
    p_TkPtPull_[0]=dbe_->bookProfile(histname+"All"," Reco Track Pt pull: All ",etaBin2,etaMin, etaMax, 100, -10., 10., " ");


    histname="PtRecVsPtSim";
    h2_PtRecVsPtSim_[0]=dbe_->book2D(histname+"All", "Pt Rec vs Pt sim: All ", etBin,etMin,etMax,etBin,etMin, etMax);
    h2_PtRecVsPtSim_[1]=dbe_->book2D(histname+"Barrel", "Pt Rec vs Pt sim: Barrel ", etBin,etMin,etMax,etBin,etMin, etMax);
    h2_PtRecVsPtSim_[2]=dbe_->book2D(histname+"Endcap", "Pt Rec vs Pt sim: Endcap ", etBin,etMin,etMax,etBin,etMin, etMax);
    histname="PtRecVsPtSimMixProv";
    h2_PtRecVsPtSimMixProv_ =dbe_->book2D(histname+"All", "Pt Rec vs Pt sim All for mix with general tracks ", etBin,etMin,etMax,etBin,etMin, etMax);


    histname="eBcOverTkPout";
    hBCEnergyOverTrackPout_[0] = dbe_->book1D(histname+"All","Matrching BC E/P_out: all Ecal ",100, 0., 5.);
    hBCEnergyOverTrackPout_[1] = dbe_->book1D(histname+"Barrel","Matrching BC E/P_out: Barrel ",100, 0., 5.);
    hBCEnergyOverTrackPout_[2] = dbe_->book1D(histname+"Endcap","Matrching BC E/P_out: Endcap ",100, 0., 5.);
    

    ////////////// test on OutIn tracks
    h_OIinnermostHitR_ = dbe_->book1D("OIinnermostHitR"," R innermost hit for OI tracks ",50, 0., 25);
    h_IOinnermostHitR_ = dbe_->book1D("IOinnermostHitR"," R innermost hit for IO tracks ",50, 0., 25);

    /// test track provenance
    h_trkProv_[0] = dbe_->book1D("allTrkProv"," Track pair provenance ",4, 0., 4.);
    h_trkProv_[1] = dbe_->book1D("assTrkProv"," Track pair provenance ",4, 0., 4.);

    // histos for fake rate
    histname = "h_RecoConvTwoTracksEta";
    h_RecoConvTwoTracks_[0] =  dbe_->book1D(histname," All reco conversions with 2 reco  tracks: simulated #eta",etaBin2,etaMin, etaMax);
    histname = "h_RecoConvTwoTracksPhi";
    h_RecoConvTwoTracks_[1] =  dbe_->book1D(histname," All reco conversions with 2 reco tracks: simulated #phi",phiBin,phiMin, phiMax);
    histname = "h_RecoConvTwoTracksR";
    h_RecoConvTwoTracks_[2] =  dbe_->book1D(histname," All reco conversions with 2 reco tracks: simulated R",rBin,rMin, rMax);
    histname = "h_RecoConvTwoTracksZ";
    h_RecoConvTwoTracks_[3] =  dbe_->book1D(histname," All reco conversions with 2 reco tracks: simulated Z",zBin,zMin, zMax);
    histname = "h_RecoConvTwoTracksEt";
    h_RecoConvTwoTracks_[4] =  dbe_->book1D(histname," All reco conversions with 2 reco tracks: simulated Et",etBin,etMin, etMax);
    //
    histname = "h_RecoConvTwoMTracksEta";
    h_RecoConvTwoMTracks_[0] =  dbe_->book1D(histname," All reco conversions with 2 reco-ass tracks: simulated #eta",etaBin2,etaMin, etaMax);
    histname = "h_RecoConvTwoMTracksPhi";
    h_RecoConvTwoMTracks_[1] =  dbe_->book1D(histname," All reco conversions with 2 reco-ass tracks: simulated #phi",phiBin,phiMin, phiMax);
    histname = "h_RecoConvTwoMTracksR";
    h_RecoConvTwoMTracks_[2] =  dbe_->book1D(histname," All reco conversions with 2 reco-ass tracks: simulated R",rBin,rMin, rMax);
    histname = "h_RecoConvTwoMTracksZ";
    h_RecoConvTwoMTracks_[3] =  dbe_->book1D(histname," All reco conversions with 2 reco-ass tracks: simulated Z",zBin,zMin, zMax);
    histname = "h_RecoConvTwoMTracksEt";
    h_RecoConvTwoMTracks_[4] =  dbe_->book1D(histname," All reco conversions with 2 reco-ass tracks: simulated Et",etBin,etMin, etMax);




  } // if DQM 



}



 void  PhotonValidator::beginRun (edm::Run const & r, edm::EventSetup const & theEventSetup) {
   
   //get magnetic field
  edm::LogInfo("ConvertedPhotonProducer") << " get magnetic field" << "\n";
  theEventSetup.get<IdealMagneticFieldRecord>().get(theMF_);  


  edm::ESHandle<TrackAssociatorBase> theHitsAssociator;
  theEventSetup.get<TrackAssociatorRecord>().get("TrackAssociatorByHits",theHitsAssociator);
  theTrackAssociator_ = (TrackAssociatorBase *) theHitsAssociator.product();

  thePhotonMCTruthFinder_ = new PhotonMCTruthFinder();  

}

void  PhotonValidator::endRun (edm::Run& r, edm::EventSetup const & theEventSetup) {

  delete thePhotonMCTruthFinder_;

}



void PhotonValidator::analyze( const edm::Event& e, const edm::EventSetup& esup ) {
  
  
  using namespace edm;
  //  const float etaPhiDistance=0.01;
  // Fiducial region
  // const float TRK_BARL =0.9;
  const float BARL = 1.4442; // DAQ TDR p.290
  const float END_LO = 1.566;
  const float END_HI = 2.5;
  // Electron mass
  //const Float_t mElec= 0.000511;


  nEvt_++;  
  LogInfo("PhotonValidator") << "PhotonValidator Analyzing event number: " << e.id() << " Global Counter " << nEvt_ <<"\n";
  //  std::cout << "PhotonValidator Analyzing event number: "  << e.id() << " Global Counter " << nEvt_ <<"\n";


  // get the geometry from the event setup:
  esup.get<CaloGeometryRecord>().get(theCaloGeom_);


  // Transform Track into TransientTrack (needed by the Vertex fitter)
  edm::ESHandle<TransientTrackBuilder> theTTB;
  esup.get<TransientTrackRecord>().get("TransientTrackBuilder",theTTB);


  ///// Get the recontructed  photons
  Handle<reco::PhotonCollection> photonHandle; 
  e.getByLabel(photonCollectionProducer_, photonCollection_ , photonHandle);
  const reco::PhotonCollection photonCollection = *(photonHandle.product());
  if (!photonHandle.isValid()) {
    edm::LogError("PhotonProducer") << "Error! Can't get the Photon collection "<< std::endl;
    return; 
  }

  Handle< edm::View<reco::Track> > outInTrkHandle;
  Handle< edm::View<reco::Track> > inOutTrkHandle;
  if ( !fastSim_) {
    //// Get the Out In CKF tracks from conversions 
    e.getByLabel(conversionOITrackProducer_,  outInTrkHandle);
    //std::cout << "ConvPhoAnalyzerWithOfficialAssociation  outInTrack collection size " << (*outInTrkHandle).size() << "\n";
    
    //// Get the In Out  CKF tracks from conversions 
    e.getByLabel(conversionIOTrackProducer_, inOutTrkHandle);
    //std::cout  << " ConvPhoAnalyzerWithOfficialAssociation inOutTrack collection size " << (*inOutTrkHandle).size() << "\n";

    // Loop over Out In Tracks 
    int iTrk=0;
    int nHits=0;
    for( View<reco::Track>::const_iterator    iTk =  (*outInTrkHandle).begin(); iTk !=  (*outInTrkHandle).end(); iTk++) {
      //    std::cout  << " Barrel  Out In Track charge " << iTk->charge() << " Num of RecHits " << iTk->recHitsSize() << " inner momentum " << sqrt( iTk->innerMomentum().Mag2() ) << "\n";  
      // std::cout  << " Barrel Out In Track Extra inner momentum  " << sqrt(iTk->extra()->innerMomentum().Mag2()) <<  " inner position R " <<  sqrt( iTk->innerPosition().Perp2() ) << "\n";  
      h_OIinnermostHitR_ ->Fill ( sqrt( iTk->innerPosition().Perp2() ) );
      for (  trackingRecHit_iterator itHits=iTk->extra()->recHitsBegin();  itHits!=iTk->extra()->recHitsEnd(); ++itHits ) {
	if ( (*itHits)->isValid() ) {
	  nHits++;
	  //	cout <<nHits <<") RecHit in GP " <<  trackerGeom->idToDet((*itHits)->geographicalId())->surface().toGlobal((*itHits)->localPosition()) << " R "<< trackerGeom->idToDet((*itHits)->geographicalId())->surface().toGlobal((*itHits)->localPosition()).perp() << " Z " << trackerGeom->idToDet((*itHits)->geographicalId())->surface().toGlobal((*itHits)->localPosition()).z() << "\n";
	}
	
	
      }
      
      iTrk++;
      
      
    }
    
    // Loop over In Out Tracks Barrel
    iTrk=0;
    for( View<reco::Track>::const_iterator    iTk =  (*inOutTrkHandle).begin(); iTk !=  (*inOutTrkHandle).end(); iTk++) {
      //std::cout  << " Barrel In Out Track charge " << iTk->charge() << " Num of RecHits " << iTk->recHitsSize() << " inner momentum " << sqrt( iTk->innerMomentum().Mag2())  << "\n";  
      // std::cout   << " Barrel In Out  Track Extra inner momentum  " << sqrt(iTk->extra()->innerMomentum().Mag2()) << "\n"; 
      h_IOinnermostHitR_ ->Fill ( sqrt( iTk->innerPosition().Perp2() ) );  
      nHits=0;
      for (  trackingRecHit_iterator itHits=iTk->extra()->recHitsBegin();  itHits!=iTk->extra()->recHitsEnd(); ++itHits ) {
	if ( (*itHits)->isValid() ) {
	  nHits++;
	  //cout <<nHits <<") RecHit in GP " << trackerGeom->idToDet((*itHits)->geographicalId())->surface().toGlobal((*itHits)->localPosition())  << " R "<< trackerGeom->idToDet((*itHits)->geographicalId())->surface().toGlobal((*itHits)->localPosition()).perp() << " Z " << trackerGeom->idToDet((*itHits)->geographicalId())->surface().toGlobal((*itHits)->localPosition()).z() << "\n";
	  
	}
      }
      
      
      
      iTrk++;
    }

  }  // if !fastSim


  //////////////////// Get the MC truth
  //get simtrack info
  std::vector<SimTrack> theSimTracks;
  std::vector<SimVertex> theSimVertices;
  edm::Handle<SimTrackContainer> SimTk;
  edm::Handle<SimVertexContainer> SimVtx;

  if ( ! fastSim_) {  
    e.getByLabel("g4SimHits",SimTk);
    e.getByLabel("g4SimHits",SimVtx);
  } else {
    e.getByLabel("famosSimHits",SimTk);
    e.getByLabel("famosSimHits",SimVtx);

  }
  


  theSimTracks.insert(theSimTracks.end(),SimTk->begin(),SimTk->end());
  theSimVertices.insert(theSimVertices.end(),SimVtx->begin(),SimVtx->end());
  std::vector<PhotonMCTruth> mcPhotons=thePhotonMCTruthFinder_->find (theSimTracks,  theSimVertices);  

  edm::Handle<edm::HepMCProduct> hepMC;
  e.getByLabel("generator",hepMC);
  const HepMC::GenEvent *myGenEvent = hepMC->GetEvent();


  // get generated jets
  Handle<reco::GenJetCollection> GenJetsHandle ;
  e.getByLabel("iterativeCone5GenJets","",GenJetsHandle);
  reco::GenJetCollection genJetCollection = *(GenJetsHandle.product());



  // Get electron tracking truth
  bool useTP= parameters_.getParameter<bool>("useTP"); 
  TrackingParticleCollection trackingParticles;
  edm::Handle<TrackingParticleCollection> ElectronTPHandle;
  if ( useTP) {
    if ( ! fastSim_) {
    e.getByLabel(label_tp_,ElectronTPHandle);
    //  e.getByLabel("mergedtruth","MergedTrackTruth",ElectronTPHandle);
    trackingParticles = *(ElectronTPHandle.product());
    }
  }

  //// Track association with TrackingParticles
  std::vector<reco::PhotonCollection::const_iterator> StoRMatchedConvertedPhotons;
  reco::SimToRecoCollection OISimToReco;
  reco::SimToRecoCollection IOSimToReco;
  // Reco to Sim
  reco::RecoToSimCollection OIRecoToSim;
  reco::RecoToSimCollection IORecoToSim;

  if ( useTP) {  
    if ( ! fastSim_) {
    // Sim to Reco
    OISimToReco = theTrackAssociator_->associateSimToReco(outInTrkHandle, ElectronTPHandle, &e);
    IOSimToReco = theTrackAssociator_->associateSimToReco(inOutTrkHandle, ElectronTPHandle, &e);
    // Reco to Sim
    OIRecoToSim = theTrackAssociator_->associateRecoToSim(outInTrkHandle, ElectronTPHandle, &e);
    IORecoToSim = theTrackAssociator_->associateRecoToSim(inOutTrkHandle, ElectronTPHandle, &e);
    }
  }
  //
  vector<reco::SimToRecoCollection*> StoRCollPtrs;
  StoRCollPtrs.push_back(&OISimToReco);
  StoRCollPtrs.push_back(&IOSimToReco);
  vector<reco::RecoToSimCollection*> RtoSCollPtrs;
  RtoSCollPtrs.push_back(&OIRecoToSim);
  RtoSCollPtrs.push_back(&IORecoToSim);
  //
  for (int i=0; i<2; i++)  
    nSimPho_[i]=0;
  for (int i=0; i<2; i++)  
    nSimConv_[i]=0;


  //////////////////////////////////////////////////////////////////////
  for( reco::PhotonCollection::const_iterator  iPho = photonCollection.begin(); iPho != photonCollection.end(); iPho++) {
    if ( iPho->pt() < minPhoEtCut_ ) continue;
    if ( ! (  fabs(iPho->eta() ) <= BARL || ( fabs(iPho->eta()) >= END_LO && fabs(iPho->eta() ) <=END_HI ) ) ) 
      continue;  // all ecal fiducial region

    bool  phoIsInBarrel=false;
    bool  phoIsInEndcap=false;
    if ( fabs(iPho->eta() ) < 1.479 ) {
      phoIsInBarrel=true;
    } else {
      phoIsInEndcap=true;
    }
    
    for (reco::PhotonCollection::const_iterator iPho2=iPho+1; iPho2!=photonCollection.end(); iPho2++){
      math::XYZTLorentzVector p12 = iPho->p4()+iPho2->p4();
      float gamgamMass2 = p12.Dot(p12);
      
      h_gamgamMass_[0][0] -> Fill(sqrt( gamgamMass2 ));
      if ( phoIsInBarrel ) h_gamgamMass_[0][1] -> Fill(sqrt( gamgamMass2 ));
      if ( phoIsInEndcap ) h_gamgamMass_[0][2] -> Fill(sqrt( gamgamMass2 ));
      
      if ( iPho->hasConversionTracks() ) {
	h_gamgamMass_[1][0] -> Fill(sqrt( gamgamMass2 ));
	if ( phoIsInBarrel ) h_gamgamMass_[1][1] -> Fill(sqrt( gamgamMass2 ));
	if ( phoIsInEndcap ) h_gamgamMass_[1][2] -> Fill(sqrt( gamgamMass2 ));
      } else {
	h_gamgamMass_[2][0] -> Fill(sqrt( gamgamMass2 ));
	if ( phoIsInBarrel ) h_gamgamMass_[2][1] -> Fill(sqrt( gamgamMass2 ));
	if ( phoIsInEndcap ) h_gamgamMass_[2][2] -> Fill(sqrt( gamgamMass2 ));
      }

    }
  }
  

  //  cout << " PhotonValidator mcPhotons.size() " << mcPhotons.size() << endl;
  for ( std::vector<PhotonMCTruth>::const_iterator mcPho=mcPhotons.begin(); mcPho !=mcPhotons.end(); mcPho++) {
    if ( (*mcPho).fourMomentum().et() < minPhoEtCut_ ) continue;
    //    if ( signal_ ) 
    // if ( (*mcPho).motherType() != -1 ) continue;
    
    for ( HepMC::GenEvent::particle_const_iterator mcIter=myGenEvent->particles_begin(); mcIter != myGenEvent->particles_end(); mcIter++ ) {   
      if ( (*mcIter)->pdg_id() != 22 ) continue;
      bool isTheSame= false;     
      HepMC::GenParticle* mother = 0;
      if ( (*mcIter)->production_vertex() )  {
	if ( (*mcIter)->production_vertex()->particles_begin(HepMC::parents) !=
	     (*mcIter)->production_vertex()->particles_end(HepMC::parents))
	  mother = *((*mcIter)->production_vertex()->particles_begin(HepMC::parents));
      }
      
      
      
      
      float mcPhi= (*mcPho).fourMomentum().phi();
      mcPhi_= phiNormalization(mcPhi);
      mcEta_= (*mcPho).fourMomentum().pseudoRapidity();   
      mcEta_ = etaTransformation(mcEta_, (*mcPho).primaryVertex().z() ); 
      mcConvR_= (*mcPho).vertex().perp();   
      mcConvX_= (*mcPho).vertex().x();    
      mcConvY_= (*mcPho).vertex().y();    
      mcConvZ_= (*mcPho).vertex().z();  
      mcConvEta_= (*mcPho).vertex().eta();    
      mcConvPhi_= (*mcPho).vertex().phi();  
  
      if ( fabs(mcEta_) > END_HI ) continue;
      
      
      
      if (  mother ==0 
            ||  (mother != 0  && mother->pdg_id() == 22)  
	    ||  (mother != 0  && mother->pdg_id() == 25) 
	    ||  (mother != 0  && mother->pdg_id() == 35) ) 
	{ 
	  //std::cout << " ZERO mother or Higgs or primary photon  " << std::endl;
	  //if (mother !=0) std::cout <<  mother->pdg_id() << std::endl; 
          double dPt =  fabs((*mcIter)->momentum().perp() - (*mcPho).fourMomentum().et());
          float phiMother=(*mcIter)->momentum().phi();
          double dPhi = phiNormalization(phiMother) - mcPhi_ ; 
          double dEta = fabs( (*mcIter)->momentum().eta() - (*mcPho).fourMomentum().pseudoRapidity() );
	  
	  //std::cout << " HEP photon pt " << (*mcIter)->momentum().perp() << " eta " << (*mcIter)->momentum().eta() << " phi " << (*mcIter)->momentum().phi() << std::endl;
	  //std::cout << "   (*mcPho).motherType() selection  " <<   (*mcPho).motherType() << " pt " <<  (*mcPho).fourMomentum().et()  << " eta " << (*mcPho).fourMomentum().pseudoRapidity() << " phi " << mcPhi_ << std::endl;
	  //std::cout << " dPt " << dPt << " dEta " << dEta << " dPhi " << dPhi << std::endl;
          if ( dEta <=0.0001 && dPhi <=0.0001 && dPt <=0.0001) 
	    isTheSame = true;

	} 
      if ( ! isTheSame ) continue; 
      //      std::cout << " HEP photon pt " << (*mcIter)->momentum().perp() << " eta " << (*mcIter)->momentum().eta() << " phi " << (*mcIter)->momentum().phi() << std::endl;
      //std::cout << "   (*mcPho).motherType() after  " <<   (*mcPho).motherType() << " pt " <<  (*mcPho).fourMomentum().et()  << " eta " << (*mcPho).fourMomentum().pseudoRapidity() << " phi " << mcPhi_ << std::endl;


    
      // if ( ! (  fabs(mcEta_) <= BARL || ( fabs(mcEta_) >= END_LO && fabs(mcEta_) <=END_HI ) ) ) 
      //	continue;  // all ecal fiducial region


      nSimPho_[0]++;
      h_SimPhoMotherEt_[0]->Fill(  (*mcPho).motherMomentum().et()  );
      h_SimPhoMotherEta_[0]->Fill(  (*mcPho).motherMomentum().pseudoRapidity());

      h_SimPho_[0]->Fill ( mcEta_);
      h_SimPho_[1]->Fill ( mcPhi_);
      h_SimPho_[2]->Fill ( (*mcPho).fourMomentum().et()   );


    

      ////////////////////////////////// extract info about simulated conversions 
    
      bool goodSimConversion=false;
      bool visibleConversion=false;
      bool visibleConversionsWithTwoSimTracks=false;
      if (  (*mcPho).isAConversion() == 1 ) {
	nSimConv_[0]++;
	h_AllSimConv_[0]->Fill( mcEta_ ) ;
	h_AllSimConv_[1]->Fill( mcPhi_ );
	h_AllSimConv_[2]->Fill( mcConvR_ );
	h_AllSimConv_[3]->Fill( mcConvZ_ );
	h_AllSimConv_[4]->Fill(  (*mcPho).fourMomentum().et());
      
	if ( mcConvR_ <15) h_SimConvEtaPix_[0]->Fill( mcEta_ ) ;

	if ( ( fabs(mcEta_) <= BARL && mcConvR_ <85 )  || 
	     ( fabs(mcEta_) > BARL && fabs(mcEta_) <=END_HI && fabs( (*mcPho).vertex().z() ) < 210 )  ) visibleConversion=true;
      


	theConvTP_.clear(); 
	//std::cout << " PhotonValidator TrackingParticles   TrackingParticleCollection size "<<  trackingParticles.size() <<  "\n";
	for(size_t i = 0; i < trackingParticles.size(); ++i){
	  TrackingParticleRef tp (ElectronTPHandle,i);
	  //std::cout << "  Electron pt " << tp -> pt() << " charge " << tp -> charge() << " pdgId " << tp->pdgId() << " Hits for this track: " << tp -> trackPSimHit().size() << std::endl;      
	  //std::cout << " track vertex position x " <<  tp->vertex().x() << " y " << tp->vertex().y() << " z " << tp->vertex().z() << std::endl;
	  //std::cout << " track vertex position x " <<  tp->vx() << " y " << tp->vy() << " z " << tp->vz() << std::endl;
	  //std::cout << " conversion vertex position x " <<  (*mcPho).vertex().x() << " y " << (*mcPho).vertex().y() << " z " << (*mcPho).vertex().z() << std::endl;
	  if ( fabs( tp->vx() - (*mcPho).vertex().x() ) < 0.001   &&
	       fabs( tp->vy() - (*mcPho).vertex().y() ) < 0.001   &&
	       fabs( tp->vz() - (*mcPho).vertex().z() ) < 0.001) {
	  
	    //std::cout << " From conversion Electron pt " << tp -> pt() << " charge " << tp -> charge() << " pdgId " << tp->pdgId() << " Hits for this track: " << tp -> trackPSimHit().size() << std::endl;      
	    //	std::cout << " track vertex position x " <<  tp->vertex().x() << " y " << tp->vertex().y() << " z " << tp->vertex().z() << std::endl;
	    //std::cout << " conversion vertex position x " <<  (*mcPho).vertex().x() << " y " << (*mcPho).vertex().y() << " z " << (*mcPho).vertex().z() << "  R " <<  (*mcPho).vertex().perp() << std::endl;
	    theConvTP_.push_back( tp );	
	  }
	}
	//      std::cout << " PhotonValidator  theConvTP_ size " <<   theConvTP_.size() << std::endl;	

	if ( theConvTP_.size() == 2 )   visibleConversionsWithTwoSimTracks=true;
	goodSimConversion=false;

	if (   visibleConversion && visibleConversionsWithTwoSimTracks )  goodSimConversion=true;
	if ( goodSimConversion ) {
	  nSimConv_[1]++;	
	  h_VisSimConv_[0]->Fill( mcEta_ ) ;
	  h_VisSimConv_[1]->Fill( mcPhi_ );
	  h_VisSimConv_[2]->Fill( mcConvR_ );
	  h_VisSimConv_[3]->Fill( mcConvZ_ );
	  h_VisSimConv_[4]->Fill(  (*mcPho).fourMomentum().et());
	
	
	  if ( useTP ) { 
	    for ( vector<TrackingParticleRef>::iterator iTrk=theConvTP_.begin(); iTrk!=theConvTP_.end(); ++iTrk) {
	      h_simTkPt_ -> Fill ( (*iTrk)->pt() );
	      h_simTkEta_ -> Fill ( (*iTrk)->eta() );
	    }
	  }
	}
      }  ////////////// End of info from sim conversions //////////////////////////////////////////////////


    
      float minDelta=10000.;
      std::vector<reco::Photon> thePhotons;
      int index=0;
      int iMatch=-1;
      bool matched=false;
    
      //std::cout << " Reco photon size " <<  photonCollection.size() << std::endl;
      for( reco::PhotonCollection::const_iterator  iPho = photonCollection.begin(); iPho != photonCollection.end(); iPho++) {
	reco::Photon aPho = reco::Photon(*iPho);
	thePhotons.push_back(aPho);
	float phiPho=aPho.phi();
	float etaPho=aPho.eta();
	float deltaPhi = phiPho-mcPhi_;
	float deltaEta = etaPho-mcEta_;
	if ( deltaPhi > pi )  deltaPhi -= twopi;
	if ( deltaPhi < -pi) deltaPhi += twopi;
	deltaPhi=pow(deltaPhi,2);
	deltaEta=pow(deltaEta,2);
	float delta = sqrt( deltaPhi+deltaEta); 
	if ( delta<0.1 && delta < minDelta ) {
	  minDelta=delta;
	  iMatch=index;
         
	}
	index++;
      }  // end loop over reco photons
      if ( iMatch>-1 ) matched=true; 

  
      if ( matched ) {
	nSimPho_[1]++;

	h_SimPhoMotherEt_[1]->Fill(  (*mcPho).motherMomentum().et()  );
	h_SimPhoMotherEta_[1]->Fill(  (*mcPho).motherMomentum().pseudoRapidity());

	h_MatchedSimPho_[0]->Fill( mcEta_ ) ;
	h_MatchedSimPho_[1]->Fill( mcPhi_ );
	h_MatchedSimPho_[2]->Fill(  (*mcPho).fourMomentum().et());

      }




      if ( ! matched) continue;

      bool  phoIsInBarrel=false;
      bool  phoIsInEndcap=false;

      reco::Photon matchingPho = thePhotons[iMatch];

      if ( fabs(matchingPho.superCluster()->position().eta() ) < 1.479 ) {
	phoIsInBarrel=true;
      } else {
	phoIsInEndcap=true;
      }
      edm::Handle<EcalRecHitCollection>   ecalRecHitHandle;
      if ( phoIsInBarrel ) {
	// Get handle to rec hits ecal barrel 
	e.getByLabel(barrelEcalHits_, ecalRecHitHandle);
	if (!ecalRecHitHandle.isValid()) {
	  edm::LogError("PhotonProducer") << "Error! Can't get the product "<<barrelEcalHits_.label();
	  return;
	}
      
      } else if ( phoIsInEndcap ) {    
      
	// Get handle to rec hits ecal encap 
	e.getByLabel(endcapEcalHits_, ecalRecHitHandle);
	if (!ecalRecHitHandle.isValid()) {
	  edm::LogError("PhotonProducer") << "Error! Can't get the product "<<endcapEcalHits_.label();
	  return;
	}
      
      }

   

      int type=0;
      const EcalRecHitCollection ecalRecHitCollection = *(ecalRecHitHandle.product());
      float photonE = matchingPho.energy();
      float photonEt= matchingPho.energy()/cosh( matchingPho.eta()) ;
      float r9 = matchingPho.r9();
      float r1 = matchingPho.r1x5();
      float r2 = matchingPho.r2x5();
      float sigmaIetaIeta =  matchingPho.sigmaIetaIeta();
      float hOverE = matchingPho.hadronicOverEm();
      float ecalIso = matchingPho.ecalRecHitSumEtConeDR04();
      float hcalIso = matchingPho.hcalTowerSumEtConeDR04();
      float trkIso =  matchingPho.trkSumPtSolidConeDR04();
      float nIsoTrk   =  matchingPho.nTrkSolidConeDR04();
      std::vector< std::pair<DetId, float> >::const_iterator rhIt;

      bool atLeastOneDeadChannel=false;
      for(reco::CaloCluster_iterator bcIt = matchingPho.superCluster()->clustersBegin();bcIt != matchingPho.superCluster()->clustersEnd(); ++bcIt) {
	for(rhIt = (*bcIt)->hitsAndFractions().begin();rhIt != (*bcIt)->hitsAndFractions().end(); ++rhIt) {

	  for(EcalRecHitCollection::const_iterator it =  ecalRecHitCollection.begin(); it !=  ecalRecHitCollection.end(); ++it) {
	    if  (rhIt->first ==  (*it).id() ) {
	      if (  (*it).recoFlag() == 9 ) {
                atLeastOneDeadChannel=true;
		break;
	      }
	    }
	  }
	} 
      }

      if (   atLeastOneDeadChannel ) {
	h_MatchedSimPhoBadCh_[0]->Fill( mcEta_ ) ;
	h_MatchedSimPhoBadCh_[1]->Fill( mcPhi_ );
	h_MatchedSimPhoBadCh_[2]->Fill(  (*mcPho).fourMomentum().et());
      
      }

  
      h_scEta_[type]->Fill( matchingPho.superCluster()->eta() );
      h_scPhi_[type]->Fill( matchingPho.superCluster()->phi() );
      h_scEtaWidth_[type]->Fill( matchingPho.superCluster()->etaWidth() );
      h_scPhiWidth_[type]->Fill( matchingPho.superCluster()->phiWidth() );
      h_scE_[type][0]->Fill( matchingPho.superCluster()->energy() );
      h_scEt_[type][0]->Fill( matchingPho.superCluster()->energy()/cosh( matchingPho.superCluster()->eta()) );
      if ( phoIsInEndcap ) h_psE_->Fill( matchingPho.superCluster()->preshowerEnergy() ) ;
      //
      h_r9_[type][0]->Fill( r9 );
      h2_r9VsEta_[0] -> Fill (mcEta_, r9);      
      h2_r9VsEt_[0] -> Fill ((*mcPho).fourMomentum().et(), r9);      
      //
      h_r1_[type][0]->Fill( r1 );
      h2_r1VsEta_[0] -> Fill (mcEta_, r1);      
      h2_r1VsEt_[0] -> Fill ((*mcPho).fourMomentum().et(), r1);      
      //
      h_r2_[type][0]->Fill( r2 );
      h2_r2VsEta_[0] -> Fill (mcEta_, r2);      
      h2_r2VsEt_[0] -> Fill ((*mcPho).fourMomentum().et(), r2);      
      //
      h_sigmaIetaIeta_[type][0]->Fill( sigmaIetaIeta );
      h2_sigmaIetaIetaVsEta_[0] -> Fill (mcEta_, sigmaIetaIeta );      
      h2_sigmaIetaIetaVsEt_[0] -> Fill ((*mcPho).fourMomentum().et(), sigmaIetaIeta);      
      //    
      h_hOverE_[type][0]->Fill( hOverE );
      h2_hOverEVsEta_[0] -> Fill (mcEta_, hOverE );      
      h2_hOverEVsEt_[0] -> Fill ((*mcPho).fourMomentum().et(), hOverE);      
      p_hOverEVsEta_[0] -> Fill (mcEta_, hOverE );      
      p_hOverEVsEt_[0] -> Fill ((*mcPho).fourMomentum().et(), hOverE);      
      //
      h_ecalRecHitSumEtConeDR04_[type][0]->Fill( ecalIso );
      h2_ecalRecHitSumEtConeDR04VsEta_[0] -> Fill (mcEta_, ecalIso );      
      h2_ecalRecHitSumEtConeDR04VsEt_[0] -> Fill ((*mcPho).fourMomentum().et(), ecalIso);      
      p_ecalRecHitSumEtConeDR04VsEta_[0] -> Fill (mcEta_, ecalIso );      
      p_ecalRecHitSumEtConeDR04VsEt_[0] -> Fill ((*mcPho).fourMomentum().et(), ecalIso);      
      //
      h_hcalTowerSumEtConeDR04_[type][0]->Fill( hcalIso );
      h2_hcalTowerSumEtConeDR04VsEta_[0] -> Fill (mcEta_, hcalIso );      
      h2_hcalTowerSumEtConeDR04VsEt_[0] -> Fill ((*mcPho).fourMomentum().et(), hcalIso);      
      p_hcalTowerSumEtConeDR04VsEta_[0] -> Fill (mcEta_, hcalIso );      
      p_hcalTowerSumEtConeDR04VsEt_[0] -> Fill ((*mcPho).fourMomentum().et(), hcalIso);      
      //
      h_isoTrkSolidConeDR04_[type][0]->Fill( trkIso );
      h2_isoTrkSolidConeDR04VsEta_[0] -> Fill (mcEta_, trkIso );      
      h2_isoTrkSolidConeDR04VsEt_[0] -> Fill ((*mcPho).fourMomentum().et(), trkIso);      
      //
      h_nTrkSolidConeDR04_[type][0]->Fill( nIsoTrk );
      h2_nTrkSolidConeDR04VsEta_[0] -> Fill (mcEta_, nIsoTrk );      
      h2_nTrkSolidConeDR04VsEt_[0] -> Fill ((*mcPho).fourMomentum().et(), nIsoTrk);      
      //
      h_phoEta_[type]->Fill( matchingPho.eta() );
      h_phoPhi_[type]->Fill( matchingPho.phi() );
      h_phoDEta_[0]->Fill (  matchingPho.eta() - (*mcPho).fourMomentum().eta() );
      h_phoDPhi_[0]->Fill (  matchingPho.phi() - mcPhi_ );
      h_phoE_[type][0]->Fill( photonE );
      h_phoEt_[type][0]->Fill( photonEt);
      //
      h_phoERes_[0][0]->Fill( photonE / (*mcPho).fourMomentum().e() );
      h2_eResVsEta_[0]->Fill (mcEta_, photonE/(*mcPho).fourMomentum().e()  ) ;
      p_eResVsEta_[0]->Fill (mcEta_, photonE/(*mcPho).fourMomentum().e()  ) ;

      h2_eResVsEt_[0][0]->Fill ((*mcPho).fourMomentum().et(), photonE/(*mcPho).fourMomentum().e()  ) ;
      p_eResVsEt_[0][0]->Fill ((*mcPho).fourMomentum().et(), photonE/(*mcPho).fourMomentum().e()  ) ;

      h2_eResVsR9_[0]->Fill (r9, photonE/(*mcPho).fourMomentum().e()  ) ;
      h2_sceResVsR9_[0]->Fill (r9,  matchingPho.superCluster()->energy()/(*mcPho).fourMomentum().e()  ) ;
      p_eResVsR9_[0]->Fill (r9, photonE/(*mcPho).fourMomentum().e()  ) ;
      p_sceResVsR9_[0]->Fill (r9,  matchingPho.superCluster()->energy()/(*mcPho).fourMomentum().e()  ) ;
      //
      if (  (*mcPho).isAConversion() == 0 ) {
	h2_eResVsEta_[1]->Fill (mcEta_, photonE/ (*mcPho).fourMomentum().e()  ) ;



	h2_r9VsEta_[1] -> Fill (mcEta_, r9);
	h2_r9VsEt_[1] -> Fill ((*mcPho).fourMomentum().et(), r9);     
	//
	h2_r1VsEta_[1] -> Fill (mcEta_, r1);      
	h2_r1VsEt_[1] -> Fill ((*mcPho).fourMomentum().et(), r1);      
	//
	h2_r2VsEta_[1] -> Fill (mcEta_, r2);      
	h2_r2VsEt_[1] -> Fill ((*mcPho).fourMomentum().et(), r2);      
	//
	h2_sigmaIetaIetaVsEta_[1] -> Fill (mcEta_, sigmaIetaIeta );      
	h2_sigmaIetaIetaVsEt_[1] -> Fill ((*mcPho).fourMomentum().et(), sigmaIetaIeta);
	//
	h2_hOverEVsEta_[1] -> Fill (mcEta_, hOverE );      
	h2_hOverEVsEt_[1] -> Fill ((*mcPho).fourMomentum().et(), hOverE);
	//
	h2_ecalRecHitSumEtConeDR04VsEta_[1] -> Fill (mcEta_, ecalIso );      
	p_ecalRecHitSumEtConeDR04VsEta_[1] -> Fill (mcEta_, ecalIso );      
	//
	h2_hcalTowerSumEtConeDR04VsEta_[1] -> Fill (mcEta_, hcalIso );      
	p_hcalTowerSumEtConeDR04VsEta_[1] -> Fill (mcEta_, hcalIso );      
	//
	h2_isoTrkSolidConeDR04VsEta_[1] -> Fill (mcEta_, trkIso );      
	h2_isoTrkSolidConeDR04VsEt_[1] -> Fill ((*mcPho).fourMomentum().et(), trkIso);      
	//
	h2_nTrkSolidConeDR04VsEta_[1] -> Fill (mcEta_, nIsoTrk );      
	h2_nTrkSolidConeDR04VsEt_[1] -> Fill ((*mcPho).fourMomentum().et(), nIsoTrk);      

      }


    
 
      if ( photonE/(*mcPho).fourMomentum().e()  < 0.3 &&   photonE/(*mcPho).fourMomentum().e() > 0.1 ) {
	//      std::cout << " Eta sim " << mcEta_ << " sc eta " << matchingPho.superCluster()->eta() << " pho eta " << matchingPho.eta() << std::endl;
      
      }


      if ( r9 > 0.93 )  {
	h_phoERes_[1][0]->Fill( photonE / (*mcPho).fourMomentum().e() );
	h2_eResVsEt_[0][1]->Fill ((*mcPho).fourMomentum().et(), photonE/(*mcPho).fourMomentum().e()  ) ;  
	p_eResVsEt_[0][1]->Fill ((*mcPho).fourMomentum().et(), photonE/(*mcPho).fourMomentum().e()  ) ; 
	p_eResVsEta_[1]->Fill (mcEta_,photonE/ (*mcPho).fourMomentum().e()  ) ;     

      } else if ( r9 <= 0.93 ) {  
	h_phoERes_[2][0]->Fill(photonE / (*mcPho).fourMomentum().e() );
	h2_eResVsEt_[0][2]->Fill ((*mcPho).fourMomentum().et(), photonE/(*mcPho).fourMomentum().e()  ) ;
	p_eResVsEt_[0][2]->Fill ((*mcPho).fourMomentum().et(), photonE/(*mcPho).fourMomentum().e()  ) ;
	p_eResVsEta_[2]->Fill (mcEta_,photonE/ (*mcPho).fourMomentum().e()  ) ;     
      }
	
    
          
      if ( phoIsInBarrel ) {
	h_scE_[type][1]->Fill( matchingPho.superCluster()->energy() );
	h_scEt_[type][1]->Fill( matchingPho.superCluster()->energy()/cosh( matchingPho.superCluster()->eta()) );
	h_r9_[type][1]->Fill( r9 );
	h_r1_[type][1]->Fill( r1 );
	h_r2_[type][1]->Fill( r2 );
	h_sigmaIetaIeta_[type][1]->Fill( sigmaIetaIeta );
	h_hOverE_[type][1]->Fill( hOverE );
	h_ecalRecHitSumEtConeDR04_[type][1]->Fill( ecalIso );
	h2_ecalRecHitSumEtConeDR04VsEt_[1] -> Fill ((*mcPho).fourMomentum().et(), ecalIso);      
	p_ecalRecHitSumEtConeDR04VsEt_[1] -> Fill ((*mcPho).fourMomentum().et(), ecalIso);      
	h_hcalTowerSumEtConeDR04_[type][1]->Fill( hcalIso );
	h2_hcalTowerSumEtConeDR04VsEt_[1] -> Fill ((*mcPho).fourMomentum().et(), hcalIso);      
	p_hcalTowerSumEtConeDR04VsEt_[1] -> Fill ((*mcPho).fourMomentum().et(), hcalIso);      
	h_isoTrkSolidConeDR04_[type][1]->Fill( trkIso );
	h_nTrkSolidConeDR04_[type][1]->Fill( nIsoTrk );

      
	h_phoE_[type][1]->Fill( photonE );
	h_phoEt_[type][1]->Fill( photonEt );
	h_nConv_[type][1]->Fill(float( matchingPho.conversions().size()));
      
      
	h_phoERes_[0][1]->Fill( photonE / (*mcPho).fourMomentum().e() );
	h2_eResVsR9_[1]->Fill (r9, photonE/(*mcPho).fourMomentum().e()  ) ;
	h2_sceResVsR9_[1]->Fill (r9,  matchingPho.superCluster()->energy()/(*mcPho).fourMomentum().e()  ) ;
	p_eResVsR9_[1]->Fill (r9, photonE/(*mcPho).fourMomentum().e()  ) ;
	p_sceResVsR9_[1]->Fill (r9,  matchingPho.superCluster()->energy()/(*mcPho).fourMomentum().e()  ) ;

	h2_eResVsEt_[1][0]->Fill ((*mcPho).fourMomentum().et(), photonE/(*mcPho).fourMomentum().e()  ) ;  
	p_eResVsEt_[1][0]->Fill ((*mcPho).fourMomentum().et(), photonE/(*mcPho).fourMomentum().e()  ) ;

      
	if ( r9 > 0.93 ) {  
	  h_phoERes_[1][1]->Fill(  photonE  / (*mcPho).fourMomentum().e() );
	  h2_eResVsEt_[1][1]->Fill ((*mcPho).fourMomentum().et(), photonE/(*mcPho).fourMomentum().e()  ) ;  
	  p_eResVsEt_[1][1]->Fill ((*mcPho).fourMomentum().et(), photonE/(*mcPho).fourMomentum().e()  ) ;
	}
	if ( r9 <= 0.93 )  { 
	  h_phoERes_[2][1]->Fill( photonE / (*mcPho).fourMomentum().e() );
	  h2_eResVsEt_[1][2]->Fill ((*mcPho).fourMomentum().et(), photonE/(*mcPho).fourMomentum().e()  ) ;  
	  p_eResVsEt_[1][2]->Fill ((*mcPho).fourMomentum().et(), photonE/(*mcPho).fourMomentum().e()  ) ;
	}
      }
      if ( phoIsInEndcap ) {
	h_scE_[type][2]->Fill( matchingPho.superCluster()->energy() );
	h_scEt_[type][2]->Fill( matchingPho.superCluster()->energy()/cosh( matchingPho.superCluster()->eta()) );
	h_r9_[type][2]->Fill( r9 );
	h_r1_[type][2]->Fill( r1 );
	h_r2_[type][2]->Fill( r2 );
	h_sigmaIetaIeta_[type][2]->Fill( sigmaIetaIeta );
	h_hOverE_[type][2]->Fill( hOverE );
	h_ecalRecHitSumEtConeDR04_[type][2]->Fill( ecalIso );
	h2_ecalRecHitSumEtConeDR04VsEt_[2] -> Fill ((*mcPho).fourMomentum().et(), ecalIso);      
	p_ecalRecHitSumEtConeDR04VsEt_[2] -> Fill ((*mcPho).fourMomentum().et(), ecalIso);      
	h_hcalTowerSumEtConeDR04_[type][2]->Fill( hcalIso );
	h2_hcalTowerSumEtConeDR04VsEt_[2] -> Fill ((*mcPho).fourMomentum().et(), hcalIso);      
        p_hcalTowerSumEtConeDR04VsEt_[2] -> Fill ((*mcPho).fourMomentum().et(), hcalIso);      
	h_isoTrkSolidConeDR04_[type][2]->Fill( trkIso );
	h_nTrkSolidConeDR04_[type][2]->Fill( nIsoTrk );
	h_phoE_[type][2]->Fill( photonE );
	h_phoEt_[type][2]->Fill( photonEt );
	h_nConv_[type][2]->Fill(float( matchingPho.conversions().size()));
	h_phoERes_[0][2]->Fill( photonE / (*mcPho).fourMomentum().e() );
	h2_eResVsR9_[2]->Fill (r9, photonE/(*mcPho).fourMomentum().e()  ) ;     
	h2_sceResVsR9_[2]->Fill (r9,  matchingPho.superCluster()->energy()/(*mcPho).fourMomentum().e()  ) ;
	p_eResVsR9_[2]->Fill (r9, photonE/(*mcPho).fourMomentum().e()  ) ;     
	p_sceResVsR9_[2]->Fill (r9,  matchingPho.superCluster()->energy()/(*mcPho).fourMomentum().e()  ) ;

	h2_eResVsEt_[2][0]->Fill ((*mcPho).fourMomentum().et(), photonE/(*mcPho).fourMomentum().e()  ) ;  
	p_eResVsEt_[2][0]->Fill ((*mcPho).fourMomentum().et(), photonE/(*mcPho).fourMomentum().e()  ) ;

	if ( r9 > 0.93 ) {  

	  h_phoERes_[1][2]->Fill( photonE / (*mcPho).fourMomentum().e() );
	  h2_eResVsEt_[2][1]->Fill ((*mcPho).fourMomentum().et(), photonE/(*mcPho).fourMomentum().e()  ) ;  
	  p_eResVsEt_[2][1]->Fill ((*mcPho).fourMomentum().et(), photonE/(*mcPho).fourMomentum().e()  ) ;
	}
	if ( r9 <= 0.93 ) {
	  h_phoERes_[2][2]->Fill( photonE / (*mcPho).fourMomentum().e() );
	  h2_eResVsEt_[2][2]->Fill ((*mcPho).fourMomentum().et(), photonE/(*mcPho).fourMomentum().e()  ) ;
	  p_eResVsEt_[2][2]->Fill ((*mcPho).fourMomentum().et(), photonE/(*mcPho).fourMomentum().e()  ) ;  
	}
      }
      


        
      if ( ! (visibleConversion &&  visibleConversionsWithTwoSimTracks ) ) continue;
      h_r9_[1][0]->Fill( r9 );
      if ( phoIsInBarrel ) h_r9_[1][1]->Fill( r9 );
      if ( phoIsInEndcap ) h_r9_[1][2]->Fill( r9 );

      h_simConvVtxRvsZ_[0] ->Fill ( fabs (mcConvZ_), mcConvR_  ) ;
      if ( fabs(mcEta_) <=1.) {
	h_simConvVtxRvsZ_[1] ->Fill ( fabs (mcConvZ_), mcConvR_  ) ;
	h_simConvVtxYvsX_ ->Fill ( mcConvX_, mcConvY_  ) ;
      }
      else 
	h_simConvVtxRvsZ_[2] ->Fill ( fabs (mcConvZ_), mcConvR_  ) ;

      if ( ! fastSim_) {    
	h_nConv_[type][0]->Fill(float( matchingPho.conversions().size()));          
	////////////////// plot quantities related to conversions
	reco::ConversionRefVector conversions = matchingPho.conversions();
	for (unsigned int iConv=0; iConv<conversions.size(); iConv++) {
	  reco::ConversionRef aConv=conversions[iConv];
	  double like = aConv->MVAout();
	  if ( like < likelihoodCut_ ) continue;      
	  
	  h2_EoverEtrueVsEta_[1]->Fill (mcEta_,matchingPho.superCluster()->energy()/ (*mcPho).fourMomentum().e()  ) ;
	  p_EoverEtrueVsEta_[1]->Fill (mcEta_,matchingPho.superCluster()->energy()/ (*mcPho).fourMomentum().e()  ) ;
	  
	  
	  //std::vector<reco::TrackRef> tracks = aConv->tracks();
	  const std::vector<edm::RefToBase<reco::Track> > tracks = aConv->tracks();
	  if (tracks.size() < 1 ) continue;
	  
	  
	  h_mvaOut_[0]-> Fill(like);
	  
	  if ( tracks.size()==2 ) {
	    if ( sqrt( aConv->tracksPin()[0].Perp2()) < convTrackMinPtCut_ || sqrt( aConv->tracksPin()[1].Perp2()) < convTrackMinPtCut_) continue;
	  } else {
	    if ( sqrt( aConv->tracksPin()[0].Perp2()) < convTrackMinPtCut_ ) continue;
	  }
	  
	  
	  if ( dCotCutOn_ ) {
	    if (  (fabs(mcEta_) > 1.1 && fabs (mcEta_)  < 1.4  )  &&
		  fabs( aConv->pairCotThetaSeparation() ) > dCotHardCutValue_ ) continue;
	    if ( fabs( aConv->pairCotThetaSeparation() ) > dCotCutValue_ ) continue;
	  }
	  
	  //std::cout << " PhotonValidator converison algo name " << aConv->algoName() << " " << aConv->algo() << std::endl;
	  
	  nRecConv_++;
	  
	  
	  if ( tracks.size()==2 ) {
	    h_convEta_[0]->Fill( aConv->caloCluster()[0]->eta() );
	    h_convPhi_[0]->Fill( aConv->caloCluster()[0]->phi() );
	    h_convERes_[0][0]->Fill( aConv->caloCluster()[0]->energy() / (*mcPho).fourMomentum().e() );
	    h_r9VsNofTracks_[0][0]->Fill( r9, aConv->nTracks() ) ; 
	    
	    if ( phoIsInBarrel )  {
	      h_convERes_[0][1]->Fill(aConv->caloCluster()[0]->energy() / (*mcPho).fourMomentum().e() );
	      h_r9VsNofTracks_[0][1]->Fill( r9, aConv->nTracks() ) ; 
	      h_mvaOut_[1]-> Fill(like);
	    }
	    if ( phoIsInEndcap ) {
	      h_convERes_[0][2]->Fill(aConv->caloCluster()[0]->energy() / (*mcPho).fourMomentum().e() );
	      h_r9VsNofTracks_[0][2]->Fill( r9, aConv->nTracks() ) ; 
	      h_mvaOut_[2]-> Fill(like);
	    }
	    
	  }
	  std::map<reco::TrackRef,TrackingParticleRef> myAss;
	  std::map<reco::TrackRef,TrackingParticleRef>::const_iterator itAss;
	  std::map<reco::TrackRef,TrackingParticleRef>::const_iterator itAssMin;
	  std::map<reco::TrackRef,TrackingParticleRef>::const_iterator itAssMax;
	  //     
	  
	  int nAssT2=0;
	  // unused      int nAssT1=0;
	  for (unsigned int i=0; i<tracks.size(); i++) {
	    reco::TrackRef track = tracks[i].castTo<reco::TrackRef>();
	    
	    type =0;
	    nHitsVsEta_[type] ->Fill (mcEta_,   float(tracks[i]->numberOfValidHits())-0.0001 );
	    nHitsVsR_[type] ->Fill (mcConvR_,   float(tracks[i]->numberOfValidHits())-0.0001 );
	    p_nHitsVsEta_[type] ->Fill (mcEta_,   float(tracks[i]->numberOfValidHits()) );
	    p_nHitsVsR_[type] ->Fill (mcConvR_,   float(tracks[i]->numberOfValidHits()) );
	    h_tkChi2_[type] ->Fill (tracks[i]->normalizedChi2() ); 
	  
	    /////////// fill my local track - trackingparticle association map
	    TrackingParticleRef myTP;
	    for (size_t j = 0; j < RtoSCollPtrs.size(); j++) {          
	      reco::RecoToSimCollection q = *(RtoSCollPtrs[j]);
	      
	      RefToBase<reco::Track> myTk( track );
	      
	      if( q.find(myTk ) != q.end() ) {
		std::vector<std::pair<TrackingParticleRef, double> > tp = q[myTk];
		for (unsigned int itp=0; itp<tp.size(); itp++) {
		  myTP=tp[itp].first;
		  //      std::cout << " associated with TP " << myTP->pdgId() << " pt " << sqrt(myTP->momentum().perp2()) << std::endl;
		  myAss.insert( std::make_pair ( track  , myTP) );
		  nAssT2++;
		}
	      }
	    }
	  }
	  
	  
	  
	  type=0;

	  //	  float totP = sqrt(aConv->pairMomentum().Mag2());
	  float refP =-99999.;
          if ( aConv->conversionVertex().isValid() )  refP=sqrt(aConv->refittedPairMomentum().Mag2());

          float invM = aConv->pairInvariantMass();

	  h_invMass_[type][0] ->Fill( invM);
	  if ( phoIsInBarrel ) h_invMass_[type][1] ->Fill(invM);
	  if ( phoIsInEndcap ) h_invMass_[type][2] ->Fill(invM);

	  	  
	  ////////// Numerators for conversion absolute efficiency 
	  if ( tracks.size() ==1  ) {
	    h_SimConvOneTracks_[0]->Fill( mcEta_ ) ;
	    h_SimConvOneTracks_[1]->Fill( mcPhi_ );
	    h_SimConvOneTracks_[2]->Fill( mcConvR_ );
	    h_SimConvOneTracks_[3]->Fill( mcConvZ_ );
	    h_SimConvOneTracks_[4]->Fill(  (*mcPho).fourMomentum().et());
	    
	    
	  } else if ( tracks.size() ==2 ) {
	    
	    h_SimConvTwoTracks_[0]->Fill( mcEta_ ) ;
	    h_SimConvTwoTracks_[1]->Fill( mcPhi_ );
	    h_SimConvTwoTracks_[2]->Fill( mcConvR_ );
	    h_SimConvTwoTracks_[3]->Fill( mcConvZ_ );
	    h_SimConvTwoTracks_[4]->Fill(  (*mcPho).fourMomentum().et());
	    
	    h_convEta_[1]->Fill( aConv->caloCluster()[0]->eta() );		
	    
	    float trkProvenance=3;
	    if ( tracks[0]->algoName() == "outInEcalSeededConv"  &&  tracks[1]->algoName() == "outInEcalSeededConv" ) trkProvenance=0;
	    if ( tracks[0]->algoName() == "inOutEcalSeededConv"  &&  tracks[1]->algoName() == "inOutEcalSeededConv" ) trkProvenance=1;
	    if ( ( tracks[0]->algoName() == "outInEcalSeededConv"  &&  tracks[1]->algoName() == "inOutEcalSeededConv") || 
		 ( tracks[1]->algoName() == "outInEcalSeededConv"  &&  tracks[0]->algoName() == "inOutEcalSeededConv") ) trkProvenance=2;
	    if ( trkProvenance==3 ) {
	      // std::cout << " PhotonValidator provenance of tracks is " << tracks[0]->algoName() << " and " << tracks[1]->algoName() << std::endl;
	    }
	    h_trkProv_[0]->Fill( trkProvenance );
	    
	    
	    
	    ////////// Numerators for conversion efficiencies: both tracks are associated
	    if ( nAssT2 ==2 ) {
	      
	      
	      
	      
	      h_r9_[2][0]->Fill( r9 );
	      if ( phoIsInBarrel ) h_r9_[2][1]->Fill( r9 );
	      if ( phoIsInEndcap ) h_r9_[2][2]->Fill( r9 );
	      
	      h_convEta_[2]->Fill( aConv->caloCluster()[0]->eta() );	
	      
	      nRecConvAss_++;
	      
	      
	      h_SimConvTwoMTracks_[0]->Fill( mcEta_ ) ;
	      h_SimConvTwoMTracks_[1]->Fill( mcPhi_ );
	      h_SimConvTwoMTracks_[2]->Fill( mcConvR_ );
	      h_SimConvTwoMTracks_[3]->Fill( mcConvZ_ );
	      h_SimConvTwoMTracks_[4]->Fill(  (*mcPho).fourMomentum().et());
	      
	      if ( aConv->conversionVertex().isValid() ) {
		if ( trkProvenance==3 ) std::cout << " PhotonValidator provenance of tracks is mixed and vertex is valid " << std::endl;
		float chi2Prob = ChiSquaredProbability( aConv->conversionVertex().chi2(),  aConv->conversionVertex().ndof() );
		if (   chi2Prob > 0) {
		  h_SimConvTwoMTracksAndVtxPGT0_[0]->Fill( mcEta_ ) ;
		  h_SimConvTwoMTracksAndVtxPGT0_[1]->Fill( mcPhi_ );
		  h_SimConvTwoMTracksAndVtxPGT0_[2]->Fill( mcConvR_ ); 
		  h_SimConvTwoMTracksAndVtxPGT0_[3]->Fill( mcConvZ_ );
		  h_SimConvTwoMTracksAndVtxPGT0_[4]->Fill(  (*mcPho).fourMomentum().et()); 
		}
		if (   chi2Prob > 0.0005) {
		  h_SimConvTwoMTracksAndVtxPGT0005_[0]->Fill( mcEta_ ) ;
		  h_SimConvTwoMTracksAndVtxPGT0005_[1]->Fill( mcPhi_ );
		  h_SimConvTwoMTracksAndVtxPGT0005_[2]->Fill( mcConvR_ ); 
		  h_SimConvTwoMTracksAndVtxPGT0005_[3]->Fill( mcConvZ_ );
		  h_SimConvTwoMTracksAndVtxPGT0005_[4]->Fill(  (*mcPho).fourMomentum().et()); 
		  
		}
	      }	
	      
	      ///////////  Quantities per conversion
	      type =1;
	      
	      h_trkProv_[1]->Fill( trkProvenance );
	      h_invMass_[type][0] ->Fill( invM);	      
	      
	      
	      
	      float eoverp= -99999.;
	   
	      if ( aConv->conversionVertex().isValid() ) {
		eoverp= aConv->EoverPrefittedTracks();
		h_convPRes_[type][0]->Fill( refP / (*mcPho).fourMomentum().e() );
		h_EoverPTracks_[type][0] ->Fill( eoverp ) ;
		h_PoverETracks_[type][0] ->Fill( 1./eoverp ) ;
		h2_EoverEtrueVsEoverP_[0] ->Fill( eoverp,matchingPho.superCluster()->energy()/ (*mcPho).fourMomentum().e()  ) ;
		h2_PoverPtrueVsEoverP_[0] ->Fill( eoverp, refP/ (*mcPho).fourMomentum().e()  ) ;
		h2_EoverPVsEta_[0]->Fill (mcEta_, eoverp);
		h2_EoverPVsR_[0]->Fill (mcConvR_, eoverp);
		p_EoverPVsEta_[0]->Fill (mcEta_, eoverp);
		p_EoverPVsR_[0]->Fill (mcConvR_, eoverp);
		h2_PoverPtrueVsEta_[0]->Fill (mcEta_,refP/ (*mcPho).fourMomentum().e()  ) ;
		p_PoverPtrueVsEta_[0]->Fill (mcEta_,refP/ (*mcPho).fourMomentum().e()  ) ;


	      }

	      	      
	      h2_EoverEtrueVsEta_[0]->Fill (mcEta_,matchingPho.superCluster()->energy()/ (*mcPho).fourMomentum().e()  ) ;
	      p_EoverEtrueVsEta_[0]->Fill (mcEta_,matchingPho.superCluster()->energy()/ (*mcPho).fourMomentum().e()  ) ;
	      h2_EoverEtrueVsR_[0]->Fill (mcConvR_,matchingPho.superCluster()->energy()/ (*mcPho).fourMomentum().e()  ) ;
	      p_EoverEtrueVsR_[0]->Fill (mcConvR_,matchingPho.superCluster()->energy()/ (*mcPho).fourMomentum().e()  ) ;
	      
	      
	      h2_etaVsRsim_[0]->Fill (mcEta_,mcConvR_);         
	      
	      
	      reco::TrackRef track1 = tracks[0].castTo<reco::TrackRef>();
	      reco::TrackRef track2 = tracks[1].castTo<reco::TrackRef>();
	      reco::TransientTrack tt1 = (*theTTB).build( &track1);
	      reco::TransientTrack tt2 = (*theTTB).build( &track2);
	      TwoTrackMinimumDistance md;
	      md.calculate  (  tt1.initialFreeState(),  tt2.initialFreeState() );
	      if (md.status() )  {
		//cout << " Min Dist " << md.distance() << std::endl;
		h_distMinAppTracks_[1][0]->Fill ( md.distance() );
	      }  else {
		nInvalidPCA_++;
		
	      }

	      	      
	      //  here original tracks and their inner momentum is considered
	      float  dPhiTracksAtVtx =  aConv->dPhiTracksAtVtx();
	      h_DPhiTracksAtVtx_[type][0]->Fill( dPhiTracksAtVtx);
	      h2_DPhiTracksAtVtxVsEta_->Fill( mcEta_, dPhiTracksAtVtx);
	      h2_DPhiTracksAtVtxVsR_->Fill( mcConvR_, dPhiTracksAtVtx);
	      p_DPhiTracksAtVtxVsEta_->Fill( mcEta_, dPhiTracksAtVtx);
	      p_DPhiTracksAtVtxVsR_->Fill( mcConvR_, dPhiTracksAtVtx);
	      	      
	      h_DCotTracks_[type][0] ->Fill ( aConv->pairCotThetaSeparation() );
	      h2_DCotTracksVsEta_->Fill( mcEta_, aConv->pairCotThetaSeparation() );
	      h2_DCotTracksVsR_->Fill( mcConvR_, aConv->pairCotThetaSeparation() );
	      p_DCotTracksVsEta_->Fill( mcEta_, aConv->pairCotThetaSeparation() );
	      p_DCotTracksVsR_->Fill( mcConvR_, aConv->pairCotThetaSeparation() );
	      
	      	      
	      if ( phoIsInBarrel ) {
		h_invMass_[type][1] ->Fill(invM);
		if ( aConv->conversionVertex().isValid() ) {
		  h_convPRes_[type][1]->Fill( refP / (*mcPho).fourMomentum().e() );
		  h_EoverPTracks_[type][1] ->Fill( eoverp ) ;
		  if (  mcConvR_ < 15 )                 h_EoverPTracks_[0][0] ->Fill( eoverp ) ;
		  if (  mcConvR_ > 15 && mcConvR_< 58 ) h_EoverPTracks_[0][1] ->Fill( eoverp ) ;
		  if (  mcConvR_ > 58 )                 h_EoverPTracks_[0][2] ->Fill( eoverp ) ;
		  h_PoverETracks_[type][1] ->Fill( 1./eoverp ) ;
		  h2_EoverEtrueVsEoverP_[1] ->Fill( eoverp,matchingPho.superCluster()->energy()/ (*mcPho).fourMomentum().e()  ) ;
		  h2_PoverPtrueVsEoverP_[1] ->Fill( eoverp, refP/ (*mcPho).fourMomentum().e()  ) ;
		}
		h_DPhiTracksAtVtx_[type][1]->Fill( dPhiTracksAtVtx);
		h_DCotTracks_[type][1] ->Fill ( aConv->pairCotThetaSeparation() );
		
		
	      }
	      
	     	      
	      if ( phoIsInEndcap ) {
		h_invMass_[type][2] ->Fill(invM);
		if ( aConv->conversionVertex().isValid() ) {
		  h_convPRes_[type][2]->Fill( refP / (*mcPho).fourMomentum().e() );
		  h_EoverPTracks_[type][2] ->Fill( eoverp ) ;
		  h_PoverETracks_[type][2] ->Fill( 1./eoverp ) ;
		  h2_EoverEtrueVsEoverP_[2] ->Fill( eoverp,matchingPho.superCluster()->energy()/ (*mcPho).fourMomentum().e()  ) ;
		  h2_PoverPtrueVsEoverP_[2] ->Fill( eoverp, refP/ (*mcPho).fourMomentum().e()  ) ;
		}
		h_DPhiTracksAtVtx_[type][2]->Fill( dPhiTracksAtVtx);
		h_DCotTracks_[type][2] ->Fill ( aConv->pairCotThetaSeparation() );
		 
	      }
	      
	      	      
	      if ( aConv->conversionVertex().isValid() ) {
		//	      float chi2Prob = ChiSquaredProbability( aConv->conversionVertex().chi2(),  aConv->conversionVertex().ndof() );
		
		//	      h2_etaVsRreco_[0]->Fill (aConv->caloCluster()[0]->eta(),sqrt(aConv->conversionVertex().position().perp2()) );         
		//h_convVtxRvsZ_[0] ->Fill ( fabs (aConv->conversionVertex().position().z() ),  sqrt(aConv->conversionVertex().position().perp2())  ) ;
		h_convVtxdX_ ->Fill ( aConv->conversionVertex().position().x() - mcConvX_);
		h_convVtxdY_ ->Fill ( aConv->conversionVertex().position().y() - mcConvY_);
		h_convVtxdZ_ ->Fill ( aConv->conversionVertex().position().z() - mcConvZ_);
		h_convVtxdR_ ->Fill ( sqrt(aConv->conversionVertex().position().perp2()) - mcConvR_);
		
		if ( fabs( mcConvEta_ ) <= 1.2 ) {
		  h_convVtxdX_barrel_ ->Fill ( aConv->conversionVertex().position().x() - mcConvX_);
		  h_convVtxdY_barrel_ ->Fill ( aConv->conversionVertex().position().y() - mcConvY_);
		  h_convVtxdZ_barrel_ ->Fill ( aConv->conversionVertex().position().z() - mcConvZ_);
		  h_convVtxdR_barrel_ ->Fill ( sqrt(aConv->conversionVertex().position().perp2()) - mcConvR_);
		} else {
		  h_convVtxdX_endcap_ ->Fill ( aConv->conversionVertex().position().x() - mcConvX_);
		  h_convVtxdY_endcap_ ->Fill ( aConv->conversionVertex().position().y() - mcConvY_);
		  h_convVtxdZ_endcap_ ->Fill ( aConv->conversionVertex().position().z() - mcConvZ_);
		  h_convVtxdR_endcap_ ->Fill ( sqrt(aConv->conversionVertex().position().perp2()) - mcConvR_);
		}
		
		
		h_convVtxdPhi_ ->Fill ( aConv->conversionVertex().position().phi() - mcConvPhi_);
		h_convVtxdEta_ ->Fill ( aConv->conversionVertex().position().eta() - mcConvEta_);
		h2_convVtxdRVsR_ ->Fill (mcConvR_, sqrt(aConv->conversionVertex().position().perp2()) - mcConvR_ );
		h2_convVtxdRVsEta_ ->Fill (mcEta_, sqrt(aConv->conversionVertex().position().perp2()) - mcConvR_ );
		p_convVtxdRVsR_ ->Fill (mcConvR_, sqrt(aConv->conversionVertex().position().perp2()) - mcConvR_ );
		p_convVtxdRVsEta_ ->Fill (mcEta_, sqrt(aConv->conversionVertex().position().perp2()) - mcConvR_ );
		h2_convVtxRrecVsTrue_ -> Fill (mcConvR_, sqrt(aConv->conversionVertex().position().perp2()) );
		
		
		if ( fabs(matchingPho.superCluster()->position().eta() ) <= 1.) {
		  if (  sqrt(aConv->conversionVertex().position().perp2()) <4 )   h_simConvVtxRvsZ_[3] ->Fill ( mcConvZ_, mcConvR_);
		  
		  //		h_convVtxYvsX_ ->Fill ( aConv->conversionVertex().position().y() , aConv->conversionVertex().position().x()  ) ;
		  //	h_convVtxRvsZ_[1] ->Fill ( fabs (aConv->conversionVertex().position().z() ),  sqrt(aConv->conversionVertex().position().perp2())  ) ;
		  
		  //	h_convVtxYvsX_zoom_[0] ->Fill ( aConv->conversionVertex().position().y() , aConv->conversionVertex().position().x()  ) ;
		  //h_convVtxYvsX_zoom_[1] ->Fill ( aConv->conversionVertex().position().y() , aConv->conversionVertex().position().x()  ) ;
		  //h_convVtxRvsZ_zoom_[0] ->Fill ( fabs (aConv->conversionVertex().position().z() ),  sqrt(aConv->conversionVertex().position().perp2())  ) ;
		  //h_convVtxRvsZ_zoom_[1] ->Fill ( fabs (aConv->conversionVertex().position().z() ),  sqrt(aConv->conversionVertex().position().perp2())  ) ;
		  
		  
		}
		//	      if ( fabs(matchingPho.superCluster()->position().eta() ) > 1.)      h_convVtxRvsZ_[2] ->Fill ( fabs (aConv->conversionVertex().position().z() ),  sqrt(aConv->conversionVertex().position().perp2())  ) ;
		
		
		
		
		//	      h_vtxChi2Prob_[0]->Fill( chi2Prob ); 
		// h_vtxChi2_[0]->Fill(  aConv->conversionVertex().normalizedChi2() );
		//if ( phoIsInBarrel ) {
		//	h_vtxChi2Prob_[1]->Fill( chi2Prob );
		//	h_vtxChi2_[1]->Fill( aConv->conversionVertex().normalizedChi2() ); 
		// }
		//if ( phoIsInEndcap ) {   
		//	h_vtxChi2Prob_[2]->Fill(  chi2Prob );
		//	h_vtxChi2_[2]->Fill( aConv->conversionVertex().normalizedChi2() );
		// }
		
	      } // end conversion vertex valid
	      
	      
	      
	      h_zPVFromTracks_[type]->Fill ( aConv->zOfPrimaryVertexFromTracks() );
	      h_dzPVFromTracks_[type]->Fill ( aConv->zOfPrimaryVertexFromTracks() - (*mcPho).primaryVertex().z() );
	      h2_dzPVVsR_ ->Fill(mcConvR_, aConv->zOfPrimaryVertexFromTracks() - (*mcPho).primaryVertex().z() );
	      p_dzPVVsR_ ->Fill(mcConvR_, aConv->zOfPrimaryVertexFromTracks() - (*mcPho).primaryVertex().z() );
	      

	      	      
	      float  dPhiTracksAtEcal=-99;
	      float  dEtaTracksAtEcal=-99;
	      if (aConv->bcMatchingWithTracks()[0].isNonnull() && aConv->bcMatchingWithTracks()[1].isNonnull() ) {
		nRecConvAssWithEcal_++;
		float recoPhi1 = aConv->ecalImpactPosition()[0].phi();
		float recoPhi2 = aConv->ecalImpactPosition()[1].phi();
		float recoEta1 = aConv->ecalImpactPosition()[0].eta();
		float recoEta2 = aConv->ecalImpactPosition()[1].eta();
		float bcPhi1 = aConv->bcMatchingWithTracks()[0]->phi();
		float bcPhi2 = aConv->bcMatchingWithTracks()[1]->phi();
		// unused	float bcEta1 = aConv->bcMatchingWithTracks()[0]->eta();
		// unused   float bcEta2 = aConv->bcMatchingWithTracks()[1]->eta();
		recoPhi1 = phiNormalization(recoPhi1);
		recoPhi2 = phiNormalization(recoPhi2);
		bcPhi1 = phiNormalization(bcPhi1);
		bcPhi2 = phiNormalization(bcPhi2);
		dPhiTracksAtEcal = recoPhi1 -recoPhi2;
		dPhiTracksAtEcal = phiNormalization( dPhiTracksAtEcal );
		dEtaTracksAtEcal = recoEta1 -recoEta2;
		
		
		h_DPhiTracksAtEcal_[type][0]->Fill( fabs(dPhiTracksAtEcal));
		h2_DPhiTracksAtEcalVsR_ ->Fill (mcConvR_, fabs(dPhiTracksAtEcal));
		h2_DPhiTracksAtEcalVsEta_ ->Fill (mcEta_, fabs(dPhiTracksAtEcal));
		p_DPhiTracksAtEcalVsR_ ->Fill (mcConvR_, fabs(dPhiTracksAtEcal));
		p_DPhiTracksAtEcalVsEta_ ->Fill (mcEta_, fabs(dPhiTracksAtEcal));
		
		h_DEtaTracksAtEcal_[type][0]->Fill( dEtaTracksAtEcal);
		
		if ( phoIsInBarrel ) {
		  h_DPhiTracksAtEcal_[type][1]->Fill( fabs(dPhiTracksAtEcal));
		  h_DEtaTracksAtEcal_[type][1]->Fill( dEtaTracksAtEcal);
		}
		if ( phoIsInEndcap ) {
		  h_DPhiTracksAtEcal_[type][2]->Fill( fabs(dPhiTracksAtEcal));
		  h_DEtaTracksAtEcal_[type][2]->Fill( dEtaTracksAtEcal);
		}
		
	      }
	      
	      
	      
	      
	      ///////////  Quantities per track
	      for (unsigned int i=0; i<tracks.size(); i++) {
		reco::TrackRef track = tracks[i].castTo<reco::TrackRef>();
		itAss= myAss.find(  track );
		if ( itAss == myAss.end()  ) continue;
		
		float trkProvenance=3;
		if ( tracks[0]->algoName() == "outInEcalSeededConv"  &&  tracks[1]->algoName() == "outInEcalSeededConv" ) trkProvenance=0;
		if ( tracks[0]->algoName() == "inOutEcalSeededConv"  &&  tracks[1]->algoName() == "inOutEcalSeededConv" ) trkProvenance=1;
		if ( ( tracks[0]->algoName() == "outInEcalSeededConv"  &&  tracks[1]->algoName() == "inOutEcalSeededConv") || 
		     ( tracks[1]->algoName() == "outInEcalSeededConv"  &&  tracks[0]->algoName() == "inOutEcalSeededConv") ) trkProvenance=2;

		
		nHitsVsEta_[type] ->Fill (mcEta_,   float(tracks[i]->numberOfValidHits()) );
		nHitsVsR_[type] ->Fill (mcConvR_,   float(tracks[i]->numberOfValidHits()) );
		p_nHitsVsEta_[type] ->Fill (mcEta_,   float(tracks[i]->numberOfValidHits()) -0.0001);
		p_nHitsVsR_[type] ->Fill (mcConvR_,   float(tracks[i]->numberOfValidHits()) -0.0001);
		h_tkChi2_[type] ->Fill (tracks[i]->normalizedChi2() ); 
		h_tkChi2Large_[type] ->Fill (tracks[i]->normalizedChi2() ); 
		h2_Chi2VsEta_[0] ->Fill(  mcEta_, tracks[i]->normalizedChi2() ); 
		h2_Chi2VsR_[0] ->Fill(  mcConvR_, tracks[i]->normalizedChi2() ); 
		p_Chi2VsEta_[0] ->Fill(  mcEta_, tracks[i]->normalizedChi2() ); 
		p_Chi2VsR_[0] ->Fill(  mcConvR_, tracks[i]->normalizedChi2() ); 
		

		float simPt = sqrt( ((*itAss).second)->momentum().perp2() );	
		//		float recPt =   sqrt( aConv->tracks()[i]->innerMomentum().Perp2() ) ;
		float refPt=-9999.;
		float px=0, py=0;
              
		if ( aConv->conversionVertex().isValid() ) {
		  reco::Track refTrack= aConv->conversionVertex().refittedTracks()[i];
		  px= refTrack.momentum().x() ;
		  py= refTrack.momentum().y() ;
		  refPt=sqrt (px*px + py*py );
		  
		  float ptres= refPt - simPt ;
		  // float pterror = aConv->tracks()[i]->ptError();
		  float pterror =  aConv->conversionVertex().refittedTracks()[i].ptError();
		  h2_PtRecVsPtSim_[0]->Fill ( simPt, refPt);
		  if ( trkProvenance ==3 ) h2_PtRecVsPtSimMixProv_->Fill ( simPt, refPt);
				  
		  h_TkPtPull_[0] ->Fill(ptres/pterror);
		  h2_TkPtPull_[0] ->Fill(mcEta_, ptres/pterror);
		  
		  h_TkD0_[0]->Fill ( tracks[i]->d0()* tracks[i]->charge() );
		  
		  
		  if ( aConv->bcMatchingWithTracks()[i].isNonnull() ) hBCEnergyOverTrackPout_[0]->Fill  ( aConv->bcMatchingWithTracks()[i]->energy()/sqrt(aConv->tracks()[i]->outerMomentum().Mag2())  );
		  
		  if ( phoIsInBarrel ) {
		    h_TkD0_[1]->Fill ( tracks[i]->d0()* tracks[i]->charge() );
		    h_TkPtPull_[1] ->Fill(ptres/pterror);
		    h2_PtRecVsPtSim_[1]->Fill ( simPt, refPt);
		    if ( aConv->bcMatchingWithTracks()[i].isNonnull() ) hBCEnergyOverTrackPout_[1]->Fill  ( aConv->bcMatchingWithTracks()[i]->energy()/sqrt(aConv->tracks()[i]->outerMomentum().Mag2())  );
		    
		  }
		  if ( phoIsInEndcap ) { 
		    h_TkD0_[2]->Fill ( tracks[i]->d0()* tracks[i]->charge() );
		    h_TkPtPull_[2] ->Fill(ptres/pterror);
		    h2_PtRecVsPtSim_[2]->Fill ( simPt, refPt);
		    if ( aConv->bcMatchingWithTracks()[i].isNonnull() ) hBCEnergyOverTrackPout_[2]->Fill  ( aConv->bcMatchingWithTracks()[i]->energy()/sqrt(aConv->tracks()[i]->outerMomentum().Mag2())  );
		  }
		  
		}
	  
	      } // end loop over track
	    } // end analysis of two associated tracks
	  } // end analysis of two  tracks
      
	} // loop over conversions
      }  // if !fastSim
    }  // End loop over generated particles
  } // End loop over simulated Photons


 
 
  h_nSimPho_[0]->Fill(float(nSimPho_[0]));
  h_nSimPho_[1]->Fill(float(nSimPho_[1]));
  h_nSimConv_[0]->Fill(float(nSimConv_[0]));
  h_nSimConv_[1]->Fill(float(nSimConv_[1]));
  
 
  if ( !fastSim_) {
    ///////////////////  Measure fake rate
    for( reco::PhotonCollection::const_iterator  iPho = photonCollection.begin(); iPho != photonCollection.end(); iPho++) {
      reco::Photon aPho = reco::Photon(*iPho);
      //    float et= aPho.superCluster()->energy()/cosh( aPho.superCluster()->eta()) ;    
      reco::ConversionRefVector conversions = aPho.conversions();
      for (unsigned int iConv=0; iConv<conversions.size(); iConv++) {
	reco::ConversionRef aConv=conversions[iConv];
	double like = aConv->MVAout();   
	if ( like < likelihoodCut_ ) continue;      
	//std::vector<reco::TrackRef> tracks = aConv->tracks();
	const std::vector<edm::RefToBase<reco::Track> > tracks = aConv->tracks();
	if (tracks.size() < 2 ) continue;

	bool  phoIsInBarrel=false;
	bool  phoIsInEndcap=false;
	if ( fabs(aConv->caloCluster()[0]->eta() ) < 1.479 ) {
	  phoIsInBarrel=true;
	} else {
	  phoIsInEndcap=true;
	}

      
	if ( dCotCutOn_ ) {
	  if ( ( fabs(mcEta_) > 1.1 && fabs (mcEta_)  < 1.4  )  &&
	       fabs( aConv->pairCotThetaSeparation() ) > dCotHardCutValue_ )  continue;
	  if ( fabs( aConv->pairCotThetaSeparation() ) > dCotCutValue_ ) continue;
	}
      
      
	h_RecoConvTwoTracks_[0]->Fill( aPho.eta() ) ;
	h_RecoConvTwoTracks_[1]->Fill( aPho.phi() );
	if (  aConv->conversionVertex().isValid() ) h_RecoConvTwoTracks_[2]->Fill(  aConv->conversionVertex().position().perp2() );
	h_RecoConvTwoTracks_[3]->Fill( aConv->conversionVertex().position().z() );
	h_RecoConvTwoTracks_[4]->Fill( aPho.et() ) ;
      
      
      
	int  nAssT2=0;

	std::map<reco::TrackRef,TrackingParticleRef> myAss;
	for (unsigned int i=0; i<tracks.size(); i++) {
	  reco::TrackRef track = tracks[i].castTo<reco::TrackRef>();
	
	  TrackingParticleRef myTP;
	  for (size_t j = 0; j < RtoSCollPtrs.size(); j++) {          
	    reco::RecoToSimCollection q = *(RtoSCollPtrs[j]);
	  
	    RefToBase<reco::Track> myTk( track );
	  
	    if( q.find(myTk ) != q.end() ) {
	      std::vector<std::pair<TrackingParticleRef, double> > tp = q[myTk];
	      for (unsigned int itp=0; itp<tp.size(); itp++) {
		myTP=tp[itp].first;
		//	      std::cout << " associated with TP " << myTP->pdgId() << " pt " << sqrt(myTP->momentum().perp2()) << std::endl;
		myAss.insert( std::make_pair ( track  , myTP) );
		nAssT2++;
	      }
	    }
	  }
	
	  if ( nAssT2 == 2) {
	  
	  
	    h_RecoConvTwoMTracks_[0]->Fill( aPho.eta() ) ;
	    h_RecoConvTwoMTracks_[1]->Fill( aPho.phi() );
	    if (  aConv->conversionVertex().isValid() ) h_RecoConvTwoMTracks_[2]->Fill(  aConv->conversionVertex().position().perp2() );
	    h_RecoConvTwoMTracks_[3]->Fill( aConv->conversionVertex().position().z() );
	    h_RecoConvTwoMTracks_[4]->Fill( aPho.et() ) ;
	  
	  }
	}

	///////////////////////////// xray
	if ( aConv->conversionVertex().isValid() ) {
	  float chi2Prob = ChiSquaredProbability( aConv->conversionVertex().chi2(),  aConv->conversionVertex().ndof() );
	
	  double convR= sqrt(aConv->conversionVertex().position().perp2());
	  double scalar = aConv->conversionVertex().position().x()*aConv->pairMomentum().x() + 
	    aConv->conversionVertex().position().y()*aConv->pairMomentum().y();
	  if ( scalar < 0 ) convR= -sqrt(aConv->conversionVertex().position().perp2());
	
	  h2_etaVsRreco_[0]->Fill (aConv->caloCluster()[0]->eta(),sqrt(aConv->conversionVertex().position().perp2()) );         
	  h_convVtxRvsZ_[0] ->Fill ( fabs (aConv->conversionVertex().position().z() ),  sqrt(aConv->conversionVertex().position().perp2())  ) ;
	  if ( fabs(aConv->caloCluster()[0]->eta() ) <= 1.) {
	  
	    h_convVtxYvsX_ ->Fill ( aConv->conversionVertex().position().y() , aConv->conversionVertex().position().x()  ) ;
	    h_convVtxRvsZ_[1] ->Fill ( fabs (aConv->conversionVertex().position().z() ),  convR  ) ;
	  
	    h_convVtxYvsX_zoom_[0] ->Fill ( aConv->conversionVertex().position().y() , aConv->conversionVertex().position().x()  ) ;
	    h_convVtxYvsX_zoom_[1] ->Fill ( aConv->conversionVertex().position().y() , aConv->conversionVertex().position().x()  ) ;
	    h_convVtxRvsZ_zoom_[0] ->Fill ( fabs (aConv->conversionVertex().position().z() ),  convR  ) ;
	    h_convVtxRvsZ_zoom_[1] ->Fill ( fabs (aConv->conversionVertex().position().z() ),  convR ) ;
	  
	  
	  }
	  if ( fabs(aConv->caloCluster()[0]->eta() ) > 1.)      h_convVtxRvsZ_[2] ->Fill ( fabs (aConv->conversionVertex().position().z() ),  convR  ) ;
	
	
	
	
	  h_vtxChi2Prob_[0]->Fill( chi2Prob ); 
	  h_vtxChi2_[0]->Fill(  aConv->conversionVertex().normalizedChi2() );
	  if ( phoIsInBarrel ) {
	    h_vtxChi2Prob_[1]->Fill( chi2Prob );
	    h_vtxChi2_[1]->Fill( aConv->conversionVertex().normalizedChi2() ); 
	  }
	  if ( phoIsInEndcap ) {   
	    h_vtxChi2Prob_[2]->Fill(  chi2Prob );
	    h_vtxChi2_[2]->Fill( aConv->conversionVertex().normalizedChi2() );
	  }
	
	} // end conversion vertex valid
      } // end loop over reco conversions
    } // end loop on all reco photons
  } // if !fastSim

  

  ///////////////// histograms for background
  float nPho=0;
  for (reco::GenJetCollection::const_iterator genJetIter = genJetCollection.begin();
       genJetIter != genJetCollection.end();	 ++genJetIter) {
    
    if ( genJetIter->pt() < minPhoEtCut_ ) continue;
    if ( fabs(genJetIter->eta())  > 2.5 ) continue;
 
    float mcJetPhi= genJetIter->phi();
    mcJetPhi_= phiNormalization(mcJetPhi);
    mcJetEta_= genJetIter->eta();
    float mcJetPt = genJetIter->pt() ;
    
    h_SimJet_[0]->Fill ( mcJetEta_);
    h_SimJet_[1]->Fill ( mcJetPhi_);
    h_SimJet_[2]->Fill ( mcJetPt );

    std::vector<reco::Photon> thePhotons;
    bool matched=false;
    
    reco::Photon matchingPho;    
    for( reco::PhotonCollection::const_iterator  iPho = photonCollection.begin(); iPho != photonCollection.end(); iPho++) {
      reco::Photon aPho = reco::Photon(*iPho);
      float phiPho=aPho.phi();
      float etaPho=aPho.eta();
      float deltaPhi = phiPho-mcJetPhi_;
      float deltaEta = etaPho-mcJetEta_;
      if ( deltaPhi > pi )  deltaPhi -= twopi;
      if ( deltaPhi < -pi) deltaPhi += twopi;
      deltaPhi=pow(deltaPhi,2);
      deltaEta=pow(deltaEta,2);
      float delta = sqrt( deltaPhi+deltaEta); 
      if ( delta<0.3 ) {
       	matchingPho = * iPho;
        matched = true;
      }
    }  // end loop over reco photons

    if (!  matched ) continue;
    nPho++;

    h_MatchedSimJet_[0]->Fill( mcJetEta_ ) ;
    h_MatchedSimJet_[1]->Fill( mcJetPhi_ );
    h_MatchedSimJet_[2]->Fill( mcJetPt );


    bool  phoIsInBarrel=false;
    bool  phoIsInEndcap=false;
    if ( fabs(matchingPho.superCluster()->position().eta() ) < 1.479 ) {
      phoIsInBarrel=true;
    } else {
      phoIsInEndcap=true;
    }
    edm::Handle<EcalRecHitCollection>   ecalRecHitHandle;
    if ( phoIsInBarrel ) {
      // Get handle to rec hits ecal barrel 
      e.getByLabel(barrelEcalHits_, ecalRecHitHandle);
      if (!ecalRecHitHandle.isValid()) {
	edm::LogError("PhotonProducer") << "Error! Can't get the product "<<barrelEcalHits_.label();
	return;
      }
      
    } else if ( phoIsInEndcap ) {    
      
      // Get handle to rec hits ecal encap 
      e.getByLabel(endcapEcalHits_, ecalRecHitHandle);
      if (!ecalRecHitHandle.isValid()) {
	edm::LogError("PhotonProducer") << "Error! Can't get the product "<<endcapEcalHits_.label();
	return;
      }
      
    }
    


    const EcalRecHitCollection ecalRecHitCollection = *(ecalRecHitHandle.product());
    float photonE = matchingPho.energy();
    float photonEt= matchingPho.et();
    float r9 = matchingPho.r9();
    float r1 = matchingPho.r1x5();
    float r2 = matchingPho.r2x5();
    float sigmaIetaIeta =  matchingPho.sigmaIetaIeta();
    float hOverE = matchingPho.hadronicOverEm();
    float ecalIso = matchingPho.ecalRecHitSumEtConeDR04();
    float hcalIso = matchingPho.hcalTowerSumEtConeDR04();
    float trkIso =  matchingPho.trkSumPtSolidConeDR04();
    float nIsoTrk   =  matchingPho.nTrkSolidConeDR04();
    std::vector< std::pair<DetId, float> >::const_iterator rhIt;
    
    bool atLeastOneDeadChannel=false;
      for(reco::CaloCluster_iterator bcIt = matchingPho.superCluster()->clustersBegin();bcIt != matchingPho.superCluster()->clustersEnd(); ++bcIt) {
	for(rhIt = (*bcIt)->hitsAndFractions().begin();rhIt != (*bcIt)->hitsAndFractions().end(); ++rhIt) {
	  
	  for(EcalRecHitCollection::const_iterator it =  ecalRecHitCollection.begin(); it !=  ecalRecHitCollection.end(); ++it) {
	    if  (rhIt->first ==  (*it).id() ) {
	      if (  (*it).recoFlag() == 9 ) {
                atLeastOneDeadChannel=true;
		break;
	      }
	    }
	  }
	} 
      }
      
      if (   atLeastOneDeadChannel ) {
	h_MatchedSimJetBadCh_[0]->Fill( mcJetEta_ ) ;
	h_MatchedSimJetBadCh_[1]->Fill( mcJetPhi_ );
	h_MatchedSimJetBadCh_[2]->Fill( mcJetPt );
	
      }

      h_scBkgEta_->Fill( matchingPho.superCluster()->eta() );
      h_scBkgPhi_->Fill( matchingPho.superCluster()->phi() );
      h_scBkgE_[0]->Fill( matchingPho.superCluster()->energy() );
      h_scBkgEt_[0]->Fill( matchingPho.superCluster()->energy()/cosh( matchingPho.superCluster()->eta()) );
      //
      h_phoBkgEta_->Fill( matchingPho.eta() );
      h_phoBkgPhi_->Fill( matchingPho.phi() );
      h_phoBkgE_[0]->Fill( photonE );
      h_phoBkgEt_[0]->Fill( photonEt);
      h_phoBkgDEta_->Fill (  matchingPho.eta() - mcJetEta_ );
      h_phoBkgDPhi_->Fill (  matchingPho.phi() - mcJetPhi_ );

      
      h_r9Bkg_[0]->Fill( r9 );
      h_r1Bkg_[0]->Fill( r1 );
      h_r2Bkg_[0]->Fill( r2 );
      h_sigmaIetaIetaBkg_[0]->Fill( sigmaIetaIeta );
      h_hOverEBkg_[0]->Fill( hOverE );
      h_ecalRecHitSumEtConeDR04Bkg_[0]->Fill( ecalIso );
      h_hcalTowerSumEtConeDR04Bkg_[0]->Fill( hcalIso );
      h_isoTrkSolidConeDR04Bkg_[0]->Fill( trkIso );
      h_nTrkSolidConeDR04Bkg_[0]->Fill( nIsoTrk );
      

      h2_r9VsEtaBkg_ -> Fill (mcJetEta_, r9);      
      h2_r9VsEtBkg_ -> Fill (mcJetPt, r9);      

      h2_r1VsEtaBkg_ -> Fill (mcJetEta_, r1);      
      h2_r1VsEtBkg_ -> Fill (mcJetPt, r1);      
      p_r1VsEtaBkg_ -> Fill (mcJetEta_, r1);      
      p_r1VsEtBkg_ -> Fill (mcJetPt, r1);      

      h2_r2VsEtaBkg_ -> Fill (mcJetEta_, r2);      
      h2_r2VsEtBkg_ -> Fill (mcJetPt, r2);  
      p_r2VsEtaBkg_ -> Fill (mcJetEta_, r2);      
      p_r2VsEtBkg_ -> Fill (mcJetPt, r2);  


      h2_sigmaIetaIetaVsEtaBkg_ -> Fill (mcJetEta_, sigmaIetaIeta );      
      p_sigmaIetaIetaVsEtaBkg_ -> Fill (mcJetEta_, sigmaIetaIeta );      
      h2_sigmaIetaIetaVsEtBkg_[0] -> Fill (mcJetPt, sigmaIetaIeta);
      p_sigmaIetaIetaVsEtBkg_[0] -> Fill (mcJetPt, sigmaIetaIeta);


      h2_hOverEVsEtaBkg_ -> Fill (mcJetEta_, hOverE );      
      h2_hOverEVsEtBkg_ -> Fill (mcJetPt, hOverE);
      p_hOverEVsEtaBkg_ -> Fill (mcJetEta_, hOverE );      
      p_hOverEVsEtBkg_ -> Fill (mcJetPt, hOverE);


      h2_ecalRecHitSumEtConeDR04VsEtaBkg_ -> Fill (mcJetEta_, ecalIso );      
      p_ecalRecHitSumEtConeDR04VsEtaBkg_ -> Fill (mcJetEta_, ecalIso );      
      h2_ecalRecHitSumEtConeDR04VsEtBkg_[0] -> Fill ( mcJetPt, ecalIso);      
      p_ecalRecHitSumEtConeDR04VsEtBkg_[0] -> Fill ( mcJetPt, ecalIso);


      h2_hcalTowerSumEtConeDR04VsEtaBkg_ -> Fill (mcJetEta_, hcalIso );      
      p_hcalTowerSumEtConeDR04VsEtaBkg_ -> Fill (mcJetEta_, hcalIso );      
      h2_hcalTowerSumEtConeDR04VsEtBkg_[0] -> Fill ( mcJetPt, hcalIso);      
      p_hcalTowerSumEtConeDR04VsEtBkg_[0] -> Fill ( mcJetPt, hcalIso);

      h2_isoTrkSolidConeDR04VsEtaBkg_ -> Fill (mcJetEta_, trkIso );      
      p_isoTrkSolidConeDR04VsEtaBkg_ -> Fill (mcJetEta_, trkIso );      
      h2_isoTrkSolidConeDR04VsEtBkg_[0] -> Fill (mcJetPt, trkIso); 
      p_isoTrkSolidConeDR04VsEtBkg_[0] -> Fill (mcJetPt, trkIso); 


      h2_nTrkSolidConeDR04VsEtaBkg_ -> Fill (mcJetEta_, nIsoTrk );      
      p_nTrkSolidConeDR04VsEtaBkg_ -> Fill (mcJetEta_, nIsoTrk );      
      h2_nTrkSolidConeDR04VsEtBkg_[0] -> Fill (mcJetPt, nIsoTrk); 
      p_nTrkSolidConeDR04VsEtBkg_[0] -> Fill (mcJetPt, nIsoTrk); 




      if ( phoIsInBarrel ) {

	h_r9Bkg_[1]->Fill( r9 );
	h_r1Bkg_[1]->Fill( r1 );
	h_r2Bkg_[1]->Fill( r2 );


	h_sigmaIetaIetaBkg_[1]->Fill( sigmaIetaIeta );
	h_hOverEBkg_[1]->Fill( hOverE );
	h_ecalRecHitSumEtConeDR04Bkg_[1]->Fill( ecalIso );
	h_hcalTowerSumEtConeDR04Bkg_[1]->Fill( hcalIso );
	h_isoTrkSolidConeDR04Bkg_[1]->Fill( trkIso );
	h_nTrkSolidConeDR04Bkg_[1]->Fill( nIsoTrk );

	h2_sigmaIetaIetaVsEtBkg_[1] -> Fill (mcJetPt, sigmaIetaIeta);
	p_sigmaIetaIetaVsEtBkg_[1] -> Fill (mcJetPt, sigmaIetaIeta);

	h2_ecalRecHitSumEtConeDR04VsEtBkg_[1] -> Fill ( mcJetPt, ecalIso);   
	p_ecalRecHitSumEtConeDR04VsEtBkg_[1] -> Fill ( mcJetPt, ecalIso);   

	h2_hcalTowerSumEtConeDR04VsEtBkg_[1] -> Fill ( mcJetPt, hcalIso);      
	p_hcalTowerSumEtConeDR04VsEtBkg_[1] -> Fill ( mcJetPt, hcalIso);

	h2_isoTrkSolidConeDR04VsEtBkg_[1] -> Fill (mcJetPt, trkIso); 
	p_isoTrkSolidConeDR04VsEtBkg_[1] -> Fill (mcJetPt, trkIso); 

	h2_nTrkSolidConeDR04VsEtBkg_[1] -> Fill (mcJetPt, nIsoTrk); 
	p_nTrkSolidConeDR04VsEtBkg_[1] -> Fill (mcJetPt, nIsoTrk); 


      } else if ( phoIsInEndcap )  {
	
	h_r9Bkg_[2]->Fill( r9 );
	h_r1Bkg_[2]->Fill( r1 );
	h_r2Bkg_[2]->Fill( r2 );

	h_sigmaIetaIetaBkg_[2]->Fill( sigmaIetaIeta );
	h_hOverEBkg_[2]->Fill( hOverE );
	h_ecalRecHitSumEtConeDR04Bkg_[2]->Fill( ecalIso );
	h_hcalTowerSumEtConeDR04Bkg_[2]->Fill( hcalIso );
	h_isoTrkSolidConeDR04Bkg_[2]->Fill( trkIso );
	h_nTrkSolidConeDR04Bkg_[2]->Fill( nIsoTrk );

	h2_sigmaIetaIetaVsEtBkg_[2] -> Fill (mcJetPt, sigmaIetaIeta);
	p_sigmaIetaIetaVsEtBkg_[2] -> Fill (mcJetPt, sigmaIetaIeta);

	h2_ecalRecHitSumEtConeDR04VsEtBkg_[2] -> Fill ( mcJetPt, ecalIso);   
	p_ecalRecHitSumEtConeDR04VsEtBkg_[2] -> Fill ( mcJetPt, ecalIso);   

	h2_hcalTowerSumEtConeDR04VsEtBkg_[2] -> Fill ( mcJetPt, hcalIso);      
	p_hcalTowerSumEtConeDR04VsEtBkg_[2] -> Fill ( mcJetPt, hcalIso);

	h2_isoTrkSolidConeDR04VsEtBkg_[2] -> Fill (mcJetPt, trkIso); 
	p_isoTrkSolidConeDR04VsEtBkg_[2] -> Fill (mcJetPt, trkIso); 

	h2_nTrkSolidConeDR04VsEtBkg_[2] -> Fill (mcJetPt, nIsoTrk); 
	p_nTrkSolidConeDR04VsEtBkg_[2] -> Fill (mcJetPt, nIsoTrk); 


      }

      if ( !fastSim_) {
	////////////////// plot quantities related to conversions
	reco::ConversionRefVector conversions = matchingPho.conversions();
	for (unsigned int iConv=0; iConv<conversions.size(); iConv++) {
	  reco::ConversionRef aConv=conversions[iConv];
	  //std::vector<reco::TrackRef> tracks = aConv->tracks();
	  const std::vector<edm::RefToBase<reco::Track> > tracks = aConv->tracks();
	  double like = aConv->MVAout();
	  if ( like < likelihoodCut_ ) continue;      
	  if ( tracks.size() < 2 ) continue; 	
	  h_convEtaBkg_->Fill( aConv->caloCluster()[0]->eta() );
	  h_convPhiBkg_->Fill( aConv->caloCluster()[0]->phi() );
	  h_mvaOutBkg_[0]-> Fill(like);
	  float eoverp= aConv->EoverP();
	  h_EoverPTracksBkg_[0] ->Fill( eoverp ) ;
	  h_PoverETracksBkg_[0] ->Fill( 1./eoverp ) ;
	  h_DCotTracksBkg_[0] ->Fill ( aConv->pairCotThetaSeparation() );
	  float  dPhiTracksAtVtx =  aConv->dPhiTracksAtVtx();
	  h_DPhiTracksAtVtxBkg_[0]->Fill( dPhiTracksAtVtx);
	
	  if ( phoIsInBarrel ) {
	    h_mvaOutBkg_[1]-> Fill(like);
	    h_EoverPTracksBkg_[1] ->Fill( eoverp ) ;
	    h_PoverETracksBkg_[1] ->Fill( 1./eoverp ) ;
	    h_DCotTracksBkg_[1] ->Fill ( aConv->pairCotThetaSeparation() );
	    h_DPhiTracksAtVtxBkg_[1]->Fill( dPhiTracksAtVtx);
	  }     else if ( phoIsInEndcap )  {
	    h_mvaOutBkg_[2]-> Fill(like);
	    h_EoverPTracksBkg_[2] ->Fill( eoverp ) ;
	    h_PoverETracksBkg_[2] ->Fill( 1./eoverp ) ;
	    h_DCotTracksBkg_[2] ->Fill ( aConv->pairCotThetaSeparation() );
	    h_DPhiTracksAtVtxBkg_[2]->Fill( dPhiTracksAtVtx);
	  }
	
	  if ( aConv->conversionVertex().isValid() ) {
	  
	    double convR= sqrt(aConv->conversionVertex().position().perp2());
	    double scalar = aConv->conversionVertex().position().x()*aConv->pairMomentum().x() + 
	      aConv->conversionVertex().position().y()*aConv->pairMomentum().y();
	    if ( scalar < 0 ) convR= -sqrt(aConv->conversionVertex().position().perp2());
	  
	    h_convVtxRvsZBkg_[0] ->Fill ( fabs (aConv->conversionVertex().position().z() ),  sqrt(aConv->conversionVertex().position().perp2())  ) ;
	    if ( fabs(aConv->caloCluster()[0]->eta() ) <= 1.) {
	    
	      h_convVtxYvsXBkg_ ->Fill ( aConv->conversionVertex().position().y() , aConv->conversionVertex().position().x()  ) ;
	      h_convVtxRvsZBkg_[1] ->Fill ( fabs (aConv->conversionVertex().position().z() ),  convR  ) ;
	    }
	  } // end vertex valid
	
	
	} // end loop over conversions
      } // if !fastSim
  } // end loop over sim jets
  
  h_nPho_->Fill(float(nPho));

}





void PhotonValidator::endJob() {



  bool outputMEsInRootFile = parameters_.getParameter<bool>("OutputMEsInRootFile");
  std::string outputFileName = parameters_.getParameter<std::string>("OutputFileName");
  if(outputMEsInRootFile){
    dbe_->save(outputFileName);
  }
  
  edm::LogInfo("PhotonValidator") << "Analyzed " << nEvt_  << "\n";
  // std::cout  << "::endJob Analyzed " << nEvt_ << " events " << " with total " << nPho_ << " Photons " << "\n";
  //  std::cout  << "PhotonValidator::endJob Analyzed " << nEvt_ << " events " << "\n";
    
  return ;
}
 
float PhotonValidator::phiNormalization(float & phi)
{
  //---Definitions
  const float PI    = 3.1415927;
  const float TWOPI = 2.0*PI;


  if(phi >  PI) {phi = phi - TWOPI;}
  if(phi < -PI) {phi = phi + TWOPI;}

  //  cout << " Float_t PHInormalization out " << PHI << endl;
  return phi;

}


float PhotonValidator::etaTransformation(  float EtaParticle , float Zvertex)  {

  //---Definitions
  const float PI    = 3.1415927;

  //---Definitions for ECAL
  const float R_ECAL           = 136.5;
  const float Z_Endcap         = 328.0;
  const float etaBarrelEndcap  = 1.479; 
   
  //---ETA correction

  float Theta = 0.0  ; 
  float ZEcal = R_ECAL*sinh(EtaParticle)+Zvertex;

  if(ZEcal != 0.0) Theta = atan(R_ECAL/ZEcal);
  if(Theta<0.0) Theta = Theta+PI ;
  float ETA = - log(tan(0.5*Theta));
         
  if( fabs(ETA) > etaBarrelEndcap )
    {
      float Zend = Z_Endcap ;
      if(EtaParticle<0.0 )  Zend = -Zend ;
      float Zlen = Zend - Zvertex ;
      float RR = Zlen/sinh(EtaParticle); 
      Theta = atan(RR/Zend);
      if(Theta<0.0) Theta = Theta+PI ;
      ETA = - log(tan(0.5*Theta));		      
    } 
  //---Return the result
  return ETA;
  //---end
}



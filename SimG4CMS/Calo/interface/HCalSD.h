#ifndef SimG4CMS_HCalSD_h
#define SimG4CMS_HCalSD_h
///////////////////////////////////////////////////////////////////////////////
// File: HCalSD.h
// Description: Stores hits of Hadron calorimeter in appropriate container
// Use in your sensitive detector builder:
//    HCalSD* hcalSD = new HCalSD(SDname, new CaloNumberingScheme());
///////////////////////////////////////////////////////////////////////////////

#include "SimG4CMS/Calo/interface/CaloSD.h"
#include "SimG4CMS/Calo/interface/HFShower.h"
#include "SimG4CMS/Calo/interface/HFShowerLibrary.h"
#include "SimG4CMS/Calo/interface/HFShowerParam.h"
#include "SimG4CMS/Calo/interface/HFShowerPMT.h"
#include "SimG4CMS/Calo/interface/HFShowerFibreBundle.h"
#include "SimG4CMS/Calo/interface/HcalNumberingScheme.h"
#include "CondFormats/HcalObjects/interface/HBHEDarkening.h"
#include "SimG4CMS/Calo/interface/HFDarkening.h"
#include "DetectorDescription/Core/interface/DDsvalues.h"
#include "SimG4Core/Notification/interface/BeginOfJob.h"
#include "Geometry/HcalCommonData/interface/HcalNumberingFromDDD.h"
#include "Geometry/HcalCommonData/interface/HcalDDDSimConstants.h"

#include "G4String.hh"
#include <map>
#include <string>
#include <TH1F.h>

class DDCompactView;
class DDFilteredView;
class G4LogicalVolume;
class G4Material;
class G4Step;
class HcalTestNS;

class HCalSD : public CaloSD, public Observer<const BeginOfJob *> {

public:    

  HCalSD(const std::string& , const DDCompactView &, const SensitiveDetectorCatalog &,
         edm::ParameterSet const &, const SimTrackManager*);
  ~HCalSD() override;
  bool                  ProcessHits(G4Step * , G4TouchableHistory * ) override;
  double                getEnergyDeposit(G4Step* ) override;
  uint32_t              setDetUnitId(const G4Step* step) override;
  void                  setNumberingScheme(HcalNumberingScheme* );

protected:

  void                  update(const BeginOfJob *) override;
  void                  initRun() override;
  bool                  filterHit(CaloG4Hit*, double) override;

private:    

  uint32_t                      setDetUnitId(int, const G4ThreeVector&, int, int);
  uint32_t                      setDetUnitId(HcalNumberingFromDDD::HcalID& tmp);
  std::vector<double>           getDDDArray(const std::string&, 
                                            const DDsvalues_type&);
  std::vector<G4String>         getNames(DDFilteredView&);
  bool                          isItHF(const G4Step *);
  bool                          isItHF(const G4String&);
  bool                          isItFibre(const G4LogicalVolume*);
  bool                          isItFibre(const G4String&);
  bool                          isItPMT(const G4LogicalVolume*);
  bool                          isItStraightBundle(const G4LogicalVolume*);
  bool                          isItConicalBundle(const G4LogicalVolume*);
  bool                          isItScintillator(const G4Material*);
  bool                          isItinFidVolume (const G4ThreeVector&);
  void                          getFromLibrary(G4Step * step, double weight);
  void                          hitForFibre(const G4Step * step, double weight);
  void                          getFromParam(G4Step * step, double weight);
  void                          getHitPMT(const G4Step * step);
  void                          getHitFibreBundle(const G4Step * step, bool type);
  int                           setTrackID(const G4Step * step);
  void                          readWeightFromFile(const std::string&);
  double                        layerWeight(int, const G4ThreeVector&, int, int);
  void                          plotProfile(const G4Step* step, const G4ThreeVector& pos, 
                                            double edep, double time, int id);
  void                          plotHF(const G4ThreeVector& pos, bool emType);
  void                          modifyDepth(HcalNumberingFromDDD::HcalID& id);

  HcalDDDSimConstants*          hcalConstants;
  HcalNumberingFromDDD*         numberingFromDDD;
  HcalNumberingScheme*          numberingScheme;
  HFShowerLibrary *             showerLibrary;
  HFShower *                    hfshower;
  HFShowerParam *               showerParam;
  HFShowerPMT *                 showerPMT;
  HFShowerFibreBundle *         showerBundle;
  bool                          agingFlagHB, agingFlagHE;
  const HBHEDarkening*          m_HBDarkening;
  const HBHEDarkening*          m_HEDarkening;
  std::unique_ptr<HFDarkening>  m_HFDarkening;
  HcalTestNS *                  hcalTestNS_;
  bool                          useBirk, useLayerWt, useFibreBundle, usePMTHit;
  bool                          testNumber, neutralDensity, testNS_;
  double                        birk1, birk2, birk3, betaThr;
  bool                          useHF, useShowerLibrary, useParam, applyFidCut;
  bool                          isEM;
  double                        eminHitHB, eminHitHE, eminHitHO, eminHitHF;
  double                        deliveredLumi;
  G4int                         depth_;
  std::vector<double>           gpar;
  std::vector<int>              hfLevels;
  std::vector<G4String>         hfNames, fibreNames, matNames;
  std::vector<const G4Material*>      materials;
  std::vector<const G4LogicalVolume*> hfLV, fibreLV, pmtLV, fibre1LV, fibre2LV;
  std::map<uint32_t,double>     layerWeights;
  TH1F                          *hit_[9], *time_[9], *dist_[9], *hzvem, *hzvhad;

};

#endif // HCalSD_h

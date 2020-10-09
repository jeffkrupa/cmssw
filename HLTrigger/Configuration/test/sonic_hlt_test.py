import FWCore.ParameterSet.Config as cms
import os,sys
#sys.argv = sys.argv[0:1]

#sys.path = [os.path.expandvars("$CMSSW_BASE/src/HLTrigger/Configuration/test/"), os.path.expandvars("$CMSSW_RELEASE_BASE/src/HLTrigger/Configuration/test/")]

sys.path.insert(0,os.path.expandvars("$CMSSW_RELEASE_BASE/src/HLTrigger/Configuration/test"))
from OnLine_HLT_GRun import process

process.hltHbherecopre = process.hltHbhereco.clone()
process.hltHbherecopre.__setattr__('makeRecHits',False)
process.hltHbherecopre.__setattr__('saveInfos',True)

process.hltHbhereco = cms.EDProducer("HcalReconstructor",
    Client = cms.PSet(
        batchSize = cms.untracked.uint32(16000),
        address = cms.untracked.string("ailab01.fnal.gov"),
        port = cms.untracked.uint32(8001),
        timeout = cms.untracked.uint32(300),
        modelName = cms.string("facile_all_v2"),
        mode = cms.string("Async"),
        modelVersion = cms.int32(-1),#string("Async"),
        verbose = cms.untracked.bool(True),
        allowedTries = cms.untracked.uint32(5),
        outputs = cms.untracked.vstring("output/BiasAdd"),
    ),
    ChannelInfoName = cms.InputTag("hltHbherecopre")
)

process.HLTDoLocalHcalSequence = cms.Sequence( process.hltHcalDigis + process.hltHbherecopre + process.hltHbhereco + process.hltHfprereco + process.hltHfreco + process.hltHoreco )
process.HLTStoppedHSCPLocalHcalReco = cms.Sequence( process.hltHcalDigis + process.hltHbherecopre + process.hltHbhereco)

_customInfo = {}
_customInfo['menuType'  ]= "GRun"
_customInfo['globalTags']= {}
_customInfo['globalTags'][True ] = "auto:run3_hlt_GRun"
_customInfo['globalTags'][False] = "112X_mcRun3_2021_realistic_v7" #"auto:phase1_2021_realistic"
_customInfo['inputFiles']={}
_customInfo['inputFiles'][True]  = "file:RelVal_Raw_GRun_DATA.root"
_customInfo['inputFiles'][False] = "root://cmsxrootd.fnal.gov//store/relval/CMSSW_11_2_0_pre6_ROOT622/RelValTTbar_14TeV/GEN-SIM-DIGI-RAW/112X_mcRun3_2021_realistic_v7-v1/20000/FED4709C-569E-0A42-8FF7-20E565ABE999.root" #"root://cmsxrootd.fnal.gov//store/relval/CMSSW_10_6_0/RelValProdTTbar_13UP17/GEN-SIM-RAW/106X_mc2017_realistic_v3-v1/10000/723AD11F-750D-A74C-A364-9E80132075DF.root"
_customInfo['maxEvents' ]=  100
_customInfo['globalTag' ]= "112X_mcRun3_2021_realistic_v7" #"auto:phase1_2021_realistic"
_customInfo['inputFile' ]=  ['root://cmsxrootd.fnal.gov//store/relval/CMSSW_11_2_0_pre6_ROOT622/RelValTTbar_14TeV/GEN-SIM-DIGI-RAW/112X_mcRun3_2021_realistic_v7-v1/20000/FED4709C-569E-0A42-8FF7-20E565ABE999.root']#'root://cmsxrootd.fnal.gov//store/relval/CMSSW_10_6_0/RelValProdTTbar_13UP17/GEN-SIM-RAW/106X_mc2017_realistic_v3-v1/10000/723AD11F-750D-A74C-A364-9E80132075DF.root']
_customInfo['realData'  ]=  False #True

    
process.options.numberOfThreads = cms.untracked.uint32(1)
process.options.numberOfStreams = cms.untracked.uint32(0)

from HLTrigger.Configuration.customizeHLTforALL import customizeHLTforAll
process = customizeHLTforAll(process,"GRun",_customInfo)

from HLTrigger.Configuration.customizeHLTforCMSSW import customizeHLTforCMSSW
process = customizeHLTforCMSSW(process,"GRun")

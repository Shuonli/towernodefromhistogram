#ifndef FUN4ALL_RUNDST_C
#define FUN4ALL_RUNDST_C

#include <QA.C>
#include <Calo_Calib.C>

#include <caloreco/CaloTowerBuilder.h>
#include <caloreco/CaloTowerCalib.h>
#include <caloreco/CaloTowerStatus.h>
#include <caloreco/CaloWaveformProcessing.h>
#include <caloreco/DeadHotMapLoader.h>
#include <caloreco/RawClusterBuilderTemplate.h>
#include <caloreco/RawClusterDeadHotMask.h>
#include <caloreco/RawClusterPositionCorrection.h>
#include <caloreco/TowerInfoDeadHotMask.h>
#include <calotowerbuilderfromhisto/CaloTowerBuilderFromHisto.h>

#include <mbd/MbdReco.h>

#include <globalvertex/GlobalVertexReco.h>

#include <ffamodules/CDBInterface.h>
#include <ffamodules/FlagHandler.h>
#include <ffamodules/HeadReco.h>
#include <ffamodules/SyncReco.h>

#include <fun4allraw/Fun4AllPrdfInputManager.h>

#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllDummyInputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllRunNodeInputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllUtils.h>
#include <fun4all/SubsysReco.h>

#include <phool/recoConsts.h>

#include <calotrigger/MinimumBiasClassifier.h>
#include <centrality/CentralityReco.h>

#include <calovalid/CaloValid.h>

#include <clusteriso/ClusterIso.h>

//#include </sphenix/user/shuhangli/2024calofastana/source/CaloAna24.h>

//#include </sphenix/user/shuhangli/2024calofastana/HCalana/CaloAna24.h>
//#include </sphenix/user/shuhangli/2024calofastana/pi0mass/CaloAna24.h>
//#include </sphenix/user/shuhangli/2024calofastana/ZDCtiming/CaloAna24.h>
//#include </sphenix/user/shuhangli/2024calofastana/etamass/CaloAna24.h>
#include </sphenix/user/shuhangli/FastSimValid/pi0mass/CaloAna24.h>
//#include </sphenix/user/shuhangli/2024calofastana/photoniso/CaloAna24.h>

#include <TSystem.h>
//string
#include <string>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libcalo_reco.so)
R__LOAD_LIBRARY(libcalotrigger.so)
R__LOAD_LIBRARY(libcentrality.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libmbd.so)
R__LOAD_LIBRARY(libglobalvertex.so)
R__LOAD_LIBRARY(libcalovalid.so)
R__LOAD_LIBRARY(libCaloAna24.so)
R__LOAD_LIBRARY(libclusteriso.so)
R__LOAD_LIBRARY(libCaloTowerBuilderFromHisto.so)

std::string mapname(int i)
{
  std::string name = "h_eta_phi_5_" + std::to_string(i);
  return name;
}

void Fun4All_runDST(
    const std::string &fname = "/gpfs/mnt/gpfs02/sphenix/user/yeonjugo/jetML/samples/2023-09-15_timr_emcalonly_background_model_samples/hijing_cent0_images.root",
        int nEvents = 0,
        const std::string& dbtag= "2024p005"
    ) {

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(1);

  //iso ET stuff:
  recoConsts *rc = recoConsts::instance();
  // conditions DB flags and timestamp
  rc->set_StringFlag("CDB_GLOBALTAG", dbtag);
  rc->set_uint64Flag("TIMESTAMP", 45000);
  CDBInterface::instance()->Verbosity(1);
 
  Fun4AllInputManager *intrue2 = new Fun4AllRunNodeInputManager("DST_GEO");
  std::string geoLocation = CDBInterface::instance()->getUrl("calo_geo");
  intrue2->AddFile(geoLocation);
  se->registerInputManager(intrue2);

  CaloTowerBuilderFromHisto *caloTowerBuilderFromHisto = new CaloTowerBuilderFromHisto("CaloTowerBuilderFromHisto");
  caloTowerBuilderFromHisto->set_inputFileName("/gpfs/mnt/gpfs02/sphenix/user/yeonjugo/jetML/samples/2023-09-15_timr_emcalonly_background_model_samples/hijing_cent0_images.root");
  caloTowerBuilderFromHisto->set_detector_type(CaloTowerDefs::CEMC);
  caloTowerBuilderFromHisto->set_outputNodeName("TOWER_CALIB_HISTO_CEMC");
  caloTowerBuilderFromHisto->set_nomapping(false);
  caloTowerBuilderFromHisto->set_mapping_function(mapname);
  se->registerSubsystem(caloTowerBuilderFromHisto);

 

  std::cout << "Building clusters" << std::endl;
  RawClusterBuilderTemplate *ClusterBuilder = new RawClusterBuilderTemplate("EmcRawClusterBuilderTemplate");
  ClusterBuilder->Detector("CEMC");
  ClusterBuilder->setInputTowerNodeName("TOWER_CALIB_HISTO_CEMC");
  ClusterBuilder->set_threshold_energy(0.030);  // for when using basic calibration
  std::string emc_prof = getenv("CALIBRATIONROOT");
  emc_prof += "/EmcProfile/CEMCprof_Thresh30MeV.root";
  ClusterBuilder->LoadProfile(emc_prof);
  ClusterBuilder->set_UseTowerInfo(1);  // to use towerinfo objects rather than old RawTower
  se->registerSubsystem(ClusterBuilder);


  //Process_Calo_Calib();
  
  CaloAna24 *caloana24 = new CaloAna24();
  se->registerSubsystem(caloana24);
  
  Fun4AllInputManager *in = new Fun4AllDummyInputManager("JADE");
  in->Verbosity(1);

  //se->registerInputManager(In2);

  se->run(1000);
  se->End();

  se->PrintTimer();
  delete se;
  std::cout << "All done!" << std::endl;
  gSystem->Exit(0);
}

#endif

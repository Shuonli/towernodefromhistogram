#include "CaloTowerBuilderFromHisto.h"

#include <calobase/TowerInfo.h>
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfoContainerv1.h>
#include <calobase/TowerInfoContainerv2.h>
#include <calobase/TowerInfoContainerv3.h>
#include <calobase/TowerInfoContainerv4.h>

#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>   // for PHIODataNode
#include <phool/PHNode.h>         // for PHNode
#include <phool/PHNodeIterator.h> // for PHNodeIterator
#include <phool/PHObject.h>       // for PHObject
#include <phool/getClass.h>

#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/SubsysReco.h> // for SubsysReco
#include <phool/recoConsts.h>

#include <TFile.h>
#include <TH2.h>
#include <TKey.h>
#include <TSystem.h>

#include <iostream>
#include <memory>
#include <utility> // for pair, make_pair
#include <vector>  // for vector

CaloTowerBuilderFromHisto::CaloTowerBuilderFromHisto(const std::string &name)
    : SubsysReco(name)
{
}

CaloTowerBuilderFromHisto::~CaloTowerBuilderFromHisto()
{
  delete m_file;
}

int CaloTowerBuilderFromHisto::Init(PHCompositeNode *topNode)
{
  std::cout<< "CaloTowerBuilderFromHisto::Init - start" << std::endl;
  m_file = new TFile(m_inputFileName.c_str(), "READ");

  if (!m_file)
  {
    std::cout << "Could not open file " << m_inputFileName << " stop processing " << std::endl;
    exit(1);
  }
  if(nomapping)
  {
    // try to put all histograms in a vector
    TIter next(m_file->GetListOfKeys());
    TKey *key;
    while ((key = (TKey *)next()))
    {
      if (key->IsFolder())
      {
        continue;
      }
      TH2 *histo = dynamic_cast<TH2 *>(key->ReadObj());
      // get name of the histogram
      std::string histoname = histo->GetName();
      std::cout << "CaloTowerBuilderFromHisto::Init - histogram name: " << histoname << std::endl;
      if (histo)
      {
        // m_histos.push_back(histo);
        m_histonames.push_back(histoname);
        delete histo;
      }
      if ((int)m_histonames.size() == maxhistos)
      {
        break;
      }
    }
  }

  std::cout << "CaloTowerBuilderFromHisto::Init - number of histograms: " << m_histonames.size() << std::endl;

  if (m_dettype == CaloTowerDefs::CEMC)
  {
    encode_tower = TowerInfoDefs::encode_emcal;
  }
  else if (m_dettype == CaloTowerDefs::HCALIN)
  {
    encode_tower = TowerInfoDefs::encode_hcal;
  }
  else if (m_dettype == CaloTowerDefs::HCALOUT)
  {
    encode_tower = TowerInfoDefs::encode_hcal;
  }
  else
  {
    std::cout << PHWHERE << " Invalid detector type " << m_dettype << std::endl;
    gSystem->Exit(1);
    exit(1);
  }
  // if we want to use the mapping function, it should be set
  if (!func_name_map && !nomapping)
  {
    std::cout << "CaloTowerBuilderFromHisto::Init - invalid mapping function" << std::endl;
    gSystem->Exit(1);
    exit(1);
  }

  CreateNodeTree(topNode);

  return Fun4AllReturnCodes::EVENT_OK;
}

int CaloTowerBuilderFromHisto::process_histo(TH2 *histo)
{
  if (!histo)
  {
    std::cout << "CaloTowerBuilderFromHisto::process_histos - invalid histogram" << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  if (!m_CaloInfoContainer)
  {
    std::cout << "CaloTowerBuilderFromHisto::process_histos - invalid CaloInfoContainer" << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  int nbinx = histo->GetNbinsX();
  int nbiny = histo->GetNbinsY();

  for (int ix = 1; ix <= nbinx; ix++)
  {
    for (int iy = 1; iy <= nbiny; iy++)
    {
      float energy = histo->GetBinContent(ix, iy);

      int ieta = ix - 1;
      int iphi = iy - 1;
      unsigned int key = encode_tower(ieta, iphi);
      TowerInfo *towerinfo = m_CaloInfoContainer->get_tower_at_key(key);
      if (!towerinfo)
      {
        std::cout << "CaloTowerBuilderFromHisto::process_histos - invalid index ieta: " << ieta << " iphi: " << iphi << std::endl;
      }
      towerinfo->set_energy(energy);
    }
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

int CaloTowerBuilderFromHisto::process_event(PHCompositeNode * /*topNode*/)
{
  if (Verbosity() > 0)
  {
    std::cout << "CaloTowerBuilderFromHisto::process_event - event: " << ievent << std::endl;
  }
  TH2 *histo = nullptr;
  if (nomapping)
  {
    int histoindex = (ievent) % int(m_histonames.size());
    std::string histoname = m_histonames.at(histoindex);
    histo = dynamic_cast<TH2 *>(m_file->Get(histoname.c_str()));
  }
  else
  {
    std::string histoname = func_name_map(ievent);
    // std::cout << "CaloTowerBuilderFromHisto::process_event - histogram name: " << histoname << std::endl;
    histo = dynamic_cast<TH2 *>(m_file->Get(histoname.c_str()));
  }
  // std::cout<< histo<<std::endl;
  ievent++;
  process_histo(histo);
  delete histo;
  return Fun4AllReturnCodes::EVENT_OK;
}

void CaloTowerBuilderFromHisto::CreateNodeTree(PHCompositeNode *topNode)
{
  PHNodeIterator topNodeItr(topNode);
  // DST node
  PHCompositeNode *dstNode = dynamic_cast<PHCompositeNode *>(topNodeItr.findFirst("PHCompositeNode", "DST"));
  if (!dstNode)
  {
    std::cout << "PHComposite node created: DST" << std::endl;
    dstNode = new PHCompositeNode("DST");
    topNode->addNode(dstNode);
  }

  PHNodeIterator nodeItr(dstNode);
  PHCompositeNode *DetNode;
  // enum CaloTowerDefs::DetectorSystem and TowerInfoContainer::DETECTOR are different!!!!
  TowerInfoContainer::DETECTOR DetectorEnum = TowerInfoContainer::DETECTOR::DETECTOR_INVALID;
  std::string DetectorNodeName;

  if (m_dettype == CaloTowerDefs::CEMC)
  {
    DetectorEnum = TowerInfoContainer::DETECTOR::EMCAL;
    DetectorNodeName = "CEMC";
  }
  else if (m_dettype == CaloTowerDefs::HCALIN)
  {
    DetectorEnum = TowerInfoContainer::DETECTOR::HCAL;
    DetectorNodeName = "HCALIN";
  }
  else if (m_dettype == CaloTowerDefs::HCALOUT)
  {
    DetectorEnum = TowerInfoContainer::DETECTOR::HCAL;
    DetectorNodeName = "HCALOUT";
  }
  else
  {
    std::cout << PHWHERE << " Invalid detector type " << m_dettype << std::endl;
    gSystem->Exit(1);
    exit(1);
  }
  DetNode = dynamic_cast<PHCompositeNode *>(nodeItr.findFirst("PHCompositeNode", DetectorNodeName));
  if (!DetNode)
  {
    DetNode = new PHCompositeNode(DetectorNodeName);
    dstNode->addNode(DetNode);
  }
  m_CaloInfoContainer = findNode::getClass<TowerInfoContainer>(dstNode, m_outputNodeName);
  if (m_CaloInfoContainer)
  {
    std::cout << "CaloTowerBuilderFromHisto::CreateNodeTree - TowerInfoContainer found with name " << m_outputNodeName << std::endl;
    std::cout << "it is going to be overwritten" << std::endl;
  }
  else
  {
    m_CaloInfoContainer = new TowerInfoContainerv4(DetectorEnum);
    PHIODataNode<PHObject> *towerNode = new PHIODataNode<PHObject>(m_CaloInfoContainer, m_outputNodeName, "PHObject");
    DetNode->addNode(towerNode);
  }
}
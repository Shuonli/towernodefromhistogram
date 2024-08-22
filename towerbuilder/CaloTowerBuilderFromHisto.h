// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef CALOTOWERBUILDERFROMHISTO_H
#define CALOTOWERBUILDERFROMHISTO_H

#include <caloreco/CaloTowerDefs.h>
#include <fun4all/SubsysReco.h>
#include <calobase/TowerInfoDefs.h>

#include <string>
#include <vector>
#include <functional>

class PHCompositeNode;
class TowerInfoContainer;
class TH2;
class TFile;

class CaloTowerBuilderFromHisto :public SubsysReco
{
    public:
    explicit CaloTowerBuilderFromHisto(const std::string &name = "CaloTowerBuilderFromHisto");

    ~CaloTowerBuilderFromHisto() override;

    int Init(PHCompositeNode *topNode) override;

    int process_event(PHCompositeNode *topNode) override;

    void CreateNodeTree(PHCompositeNode *topNode);
    //this method set the tower energy from the histogram
    int process_histo( TH2* histo);

    void set_detector_type(CaloTowerDefs::DetectorSystem dettype)
  {
    m_dettype = dettype;
    return;
  }

  void set_outputNodeName(const std::string &name)
  {
    m_outputNodeName = name;
    return;
  }

    void set_inputFileName(const std::string &name)
    {
        m_inputFileName = name;
        return;
    }

    void set_mapping_function(std::function<std::string(int)> func)
    {
        func_name_map = func;
    }
    //if we just want to get everything without knowing the histogram names
    void set_nomapping(bool b)
    {
        nomapping = b;
    }

    void set_maxhistos(int max)
    {
        maxhistos = max;
    }


    private:
    TFile* m_file{nullptr};

    int startcount{0};
    int ievent{0};

    TowerInfoContainer *m_CaloInfoContainer{nullptr};

    CaloTowerDefs::DetectorSystem m_dettype{CaloTowerDefs::CEMC};

    std::string m_detector{"CEMC"};
    std::string m_outputNodeName{"TOWERS_CALIB_HISTO_CEMC"};
    std::string m_inputFileName{"hijing_cent0_images.root"};

    std::vector<TH2*> m_histos;
    std::vector<std::string> m_histonames;

    //event number to string mapping
    std::function<std::string(int)> func_name_map{nullptr};

    unsigned int (*encode_tower)(const unsigned int etabin, const unsigned int phibin){TowerInfoDefs::encode_emcal};

    bool nomapping{false};

    int maxhistos{1000000};
};

#endif // CALOTOWERBUILDERFROMHISTO_H
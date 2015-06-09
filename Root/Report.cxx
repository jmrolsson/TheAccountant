#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>
#include <TheAccountant/Report.h>

// Infrastructure includes
#include "xAODRootAccess/Init.h"

// EDM includes
#include "xAODEventInfo/EventInfo.h"
#include "xAODJet/JetContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODMissingET/MissingETContainer.h"
//#include "xAODBTaggingEfficiency/BTaggingEfficiencyTool.h"
#include "xAODBTagging/BTagging.h"

// xAH includes
#include "xAODAnaHelpers/HelperFunctions.h"
#include "xAODAnaHelpers/tools/ReturnCheck.h"

namespace HF = HelperFunctions;

// this is needed to distribute the algorithm to the workers
ClassImp(Report)

Report :: Report () {}

EL::StatusCode Report :: setupJob (EL::Job& job)
{
  job.useXAOD();
  xAOD::Init("Report").ignore();

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode Report :: histInitialize () {
  // initialize all histograms here

  m_jetPlots["all/jetsLargeR"]          = new TheAccountant::IParticleKinematicHists( "all/jetsLargeR/" );
  m_jetPlots["all/jets"]         = new TheAccountant::IParticleKinematicHists( "all/jets/" );
  m_jetMETPlots["all/jetsLargeR"]       = new TheAccountant::JetMETHists( "all/jetsLargeR/" );
  m_jetMETPlots["all/jets"]      = new TheAccountant::JetMETHists( "all/jets/" );
  m_METPlots["all/MET"]               = new TheAccountant::METHists( "all/MET/" );

  if(m_passPreSel){
    m_jetPlots["presel/jetsLargeR"]     = new TheAccountant::IParticleKinematicHists( "presel/jetsLargeR/" );
    m_jetPlots["presel/jets"]    = new TheAccountant::IParticleKinematicHists( "presel/jets/" );

    m_jetMETPlots["presel/jetsLargeR"]  = new TheAccountant::JetMETHists( "presel/jetsLargeR/" );
    m_jetMETPlots["presel/jets"] = new TheAccountant::JetMETHists( "presel/jets/" );

    m_METPlots["presel/MET"]          = new TheAccountant::METHists( "presel/MET/" );
  }

  // tagged jets
  //all/jets/bTag
  //presel/jets/bTag
  if(!m_decor_jetTags_b.empty()){
    m_jetPlots["all/jets/bTag"]          = new TheAccountant::IParticleKinematicHists("all/jets/bTag/");
    m_jetMETPlots["all/jets/bTag"]       = new TheAccountant::JetMETHists("all/jets/bTag/");

    if(m_passPreSel){
      m_jetPlots["presel/jets/bTag"]     = new TheAccountant::IParticleKinematicHists("presel/jets/bTag/");
      m_jetMETPlots["presel/jets/bTag"]  = new TheAccountant::JetMETHists("presel/jets/bTag/");
    }
  }

  //all/jetsLargeR/topTag
  //presel/jetsLargeR/topTag
  if(!m_decor_jetTags_top.empty()){
    m_jetPlots["all/jetsLargeR/topTag"]         = new TheAccountant::IParticleKinematicHists("all/jetsLargeR/topTag/");
    m_jetMETPlots["all/jetsLargeR/topTag"]      = new TheAccountant::JetMETHists("all/jetsLargeR/topTag/");

    if(m_passPreSel){
      m_jetPlots["presel/jetsLargeR/topTag"]    = new TheAccountant::IParticleKinematicHists("presel/jetsLargeR/topTag/");
      m_jetMETPlots["presel/jetsLargeR/topTag"] = new TheAccountant::JetMETHists("presel/jetsLargeR/topTag/");
    }
  }

  //all/jetsLargeR/wTag
  //presel/jetsLargeR/wTag
  if(!m_decor_jetTags_w.empty()){
    m_jetPlots["all/jetsLargeR/wTag"]           = new TheAccountant::IParticleKinematicHists("all/jetsLargeR/wTag/");
    m_jetMETPlots["all/jetsLargeR/wTag"]        = new TheAccountant::JetMETHists("all/jetsLargeR/wTag/");

    if(m_passPreSel){
      m_jetPlots["presel/jetsLargeR/wTag"]      = new TheAccountant::IParticleKinematicHists("presel/jetsLargeR/wTag/");
      m_jetMETPlots["presel/jetsLargeR/wTag"]   = new TheAccountant::JetMETHists("presel/jetsLargeR/wTag/");
    }
  }

  // enable jet counting for jet plots above, set type to jet
  for(auto jetPlot: m_jetPlots){
    jetPlot.second->m_countParticles = true;
    jetPlot.second->m_particleType   = "jet";
  }

  // set the numLeadingJets for the JetMET histograms
  for(auto jetMETPlot: m_jetMETPlots) jetMETPlot.second->m_numLeadingJets = m_numLeadingJets;

  // NLeadingJets
  for(int i=1; i <= m_numLeadingJets; ++i){
    //all/jetX
    //all jetX
    m_jetPlots["all/jetLargeR"+std::to_string(i)] = new TheAccountant::IParticleKinematicHists( "all/jetLargeR"+std::to_string(i)+"/" );
    m_jetPlots["all/jet"+std::to_string(i)] = new TheAccountant::IParticleKinematicHists( "all/jet"+std::to_string(i)+"/" );

    //presel/jetLargeRX
    //presel/jetX
    if(m_passPreSel){
      m_jetPlots["presel/jetLargeR"+std::to_string(i)] = new TheAccountant::IParticleKinematicHists( "presel/jetLargeR"+std::to_string(i)+"/" );
      m_jetPlots["presel/jet"+std::to_string(i)] = new TheAccountant::IParticleKinematicHists( "presel/jet"+std::to_string(i)+"/" );
    }

    // tagged jets
    //all/jetX_bTag
    //presel/jetX_bTag
    if(!m_decor_jetTags_b.empty()){
      m_jetPlots["all/jet"+std::to_string(i)+"/bTag"] = new TheAccountant::IParticleKinematicHists("all/jet"+std::to_string(i)+"/bTag/");
      if(m_passPreSel) m_jetPlots["presel/jet"+std::to_string(i)+"/bTag"] = new TheAccountant::IParticleKinematicHists("presel/jet"+std::to_string(i)+"/bTag/");
    }

    //all/jetLargeRX_topTag
    //presel/jetLargeRX_topTag
    if(!m_decor_jetTags_top.empty()){
      m_jetPlots["all/jetLargeR"+std::to_string(i)+"/topTag"] = new TheAccountant::IParticleKinematicHists("all/jetLargeR"+std::to_string(i)+"/topTag/");
      if(m_passPreSel) m_jetPlots["presel/jetLargeR"+std::to_string(i)+"/topTag"] = new TheAccountant::IParticleKinematicHists("presel/jetLargeR"+std::to_string(i)+"/topTag/");
    }

    //all/jetLargeRX_wTag
    //presel/jetLargeRX_wTag
    if(!m_decor_jetTags_w.empty()){
      m_jetPlots["all/jetLargeR"+std::to_string(i)+"/wTag"] = new TheAccountant::IParticleKinematicHists("all/jetLargeR"+std::to_string(i)+"/wTag/");
      if(m_passPreSel) m_jetPlots["presel/jetLargeR"+std::to_string(i)+"/wTag"] = new TheAccountant::IParticleKinematicHists("presel/jetLargeR"+std::to_string(i)+"/wTag/");
    }
  }

  for(auto jetPlot: m_jetPlots){
    RETURN_CHECK("Report::initialize()", jetPlot.second->initialize(), "");
    jetPlot.second->record( wk() );
  }

  for(auto jetMETPlot: m_jetMETPlots){
    RETURN_CHECK("Report::initialize()", jetMETPlot.second->initialize(), "");
    jetMETPlot.second->record( wk() );
  }

  for(auto METPlot: m_METPlots){
    RETURN_CHECK("Report::initialize()", METPlot.second->initialize(), "");
    METPlot.second->record( wk() );
  }

  return EL::StatusCode::SUCCESS;
}
EL::StatusCode Report :: fileExecute () { return EL::StatusCode::SUCCESS; }
EL::StatusCode Report :: changeInput (bool /*firstFile*/) { return EL::StatusCode::SUCCESS; }

EL::StatusCode Report :: initialize ()
{
  m_event = wk()->xaodEvent();
  m_store = wk()->xaodStore();
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode Report :: execute ()
{
  const xAOD::EventInfo*                eventInfo     (nullptr);
  const xAOD::JetContainer*             in_jetsLargeR (nullptr);
  const xAOD::JetContainer*             in_jets       (nullptr);
  const xAOD::MissingETContainer*       in_missinget  (nullptr);
  const xAOD::ElectronContainer*        in_electrons  (nullptr);
  const xAOD::MuonContainer*            in_muons      (nullptr);
  const xAOD::TauJetContainer*          in_taus       (nullptr);
  const xAOD::PhotonContainer*          in_photons    (nullptr);
  //const xAOD::TruthParticleContainer*   in_truth    (nullptr);

  // start grabbing all the containers that we can
  RETURN_CHECK("Report::execute()", HF::retrieve(eventInfo,    m_eventInfo,        m_event, m_store, m_debug), "Could not get the EventInfo container.");
  RETURN_CHECK("Report::execute()", HF::retrieve(in_jetsLargeR,      m_inputLargeRJets,        m_event, m_store, m_debug), "Could not get the inputLargeRJets container.");
  RETURN_CHECK("Report::execute()", HF::retrieve(in_jets,     m_inputJets,       m_event, m_store, m_debug), "Could not get the inputJets container.");
  if(!m_inputMET.empty())
    RETURN_CHECK("Report::execute()", HF::retrieve(in_missinget, m_inputMET,         m_event, m_store, m_debug), "Could not get the inputMET container.");
  if(!m_inputElectrons.empty())
    RETURN_CHECK("Report::execute()", HF::retrieve(in_electrons, m_inputElectrons,   m_event, m_store, m_debug), "Could not get the inputElectrons container.");
  if(!m_inputMuons.empty())
    RETURN_CHECK("Report::execute()", HF::retrieve(in_muons,     m_inputMuons,       m_event, m_store, m_debug), "Could not get the inputMuons container.");
  if(!m_inputTauJets.empty())
    RETURN_CHECK("Report::execute()", HF::retrieve(in_taus,      m_inputTauJets,     m_event, m_store, m_debug), "Could not get the inputTauJets container.");
  if(!m_inputPhotons.empty())
    RETURN_CHECK("Report::execute()", HF::retrieve(in_photons,   m_inputPhotons,     m_event, m_store, m_debug), "Could not get the inputPhotons container.");

  // retrieve CalibMET_RefFinal for METContainer
  xAOD::MissingETContainer::const_iterator met_final = in_missinget->find("Final");
  if (met_final == in_missinget->end()) {
    Error("execute()", "No RefFinal inside MET container" );
    return EL::StatusCode::FAILURE;
  }
  // dereference the iterator since it's just a single object
  const xAOD::MissingET* in_met = *met_final;

  float eventWeight(eventInfo->mcEventWeight());

  // standard all jetsLargeR and all jets
  RETURN_CHECK("Report::execute()", m_jetPlots["all/jetsLargeR"]->execute(in_jets, eventWeight), "");
  RETURN_CHECK("Report::execute()", m_jetPlots["all/jets"]->execute(in_jets, eventWeight), "");
  RETURN_CHECK("Report::execute()", m_jetMETPlots["all/jetsLargeR"]->execute(in_jets, in_met, eventWeight), "");
  RETURN_CHECK("Report::execute()", m_jetMETPlots["all/jets"]->execute(in_jets, in_met, eventWeight), "");
  RETURN_CHECK("Report::execute()", m_METPlots["all/MET"]->execute(in_met, eventWeight), "");

  //build up the tagged containers
  ConstDataVector<xAOD::JetContainer> jets_bTagged(SG::VIEW_ELEMENTS);
  ConstDataVector<xAOD::JetContainer> jetsLargeR_topTagged(SG::VIEW_ELEMENTS);
  ConstDataVector<xAOD::JetContainer> jetsLargeR_wTagged(SG::VIEW_ELEMENTS);

  if(!m_decor_jetTags_b.empty() || !m_decor_jetTags_top.empty() || !m_decor_jetTags_w.empty()){
    static SG::AuxElement::Accessor< int > decor_jetTags_b(m_decor_jetTags_b);
    static SG::AuxElement::Accessor< int > decor_jetTags_top(m_decor_jetTags_top);
    static SG::AuxElement::Accessor< int > decor_jetTags_w(m_decor_jetTags_w);

    for(const auto jet: *in_jets){
      if(!m_decor_jetTags_b.empty()){
        if(decor_jetTags_b.isAvailable(*jet)){
          if(decor_jetTags_b(*jet) == 1) jets_bTagged.push_back(jet);
        } else {
          Error("Report::execute()", "m_decor_jetTags_b is set but the decoration is missing on this jet.");
          return EL::StatusCode::FAILURE;
        }
      }
    }

    for(const auto jet: *in_jetsLargeR){
      if(!m_decor_jetTags_top.empty()){
        if(decor_jetTags_top.isAvailable(*jet)){
          if(decor_jetTags_top(*jet) == 1) jetsLargeR_topTagged.push_back(jet);
        } else {
          Error("Report::execute()", "m_decor_jetTags_top is set but the decoration is missing on this jet.");
          return EL::StatusCode::FAILURE;
        }
      }

      if(!m_decor_jetTags_w.empty()){
        if(decor_jetTags_w.isAvailable(*jet)){
          if(decor_jetTags_w(*jet) == 1) jetsLargeR_wTagged.push_back(jet);
        } else {
          Error("Report::execute()", "m_decor_jetTags_w is set but the decoration is missing on this jet.");
          return EL::StatusCode::FAILURE;
        }
      }
    }

    //all/jets/bTag
    if(!m_decor_jetTags_b.empty()){
      RETURN_CHECK("Report::execute()", m_jetPlots["all/jets/bTag"]->execute(jets_bTagged.asDataVector(), eventWeight), "");
      RETURN_CHECK("Report::execute()", m_jetMETPlots["all/jets/bTag"]->execute(jets_bTagged.asDataVector(), in_met, eventWeight), "");
    }

    //all/jetsLargeR/topTag
    if(!m_decor_jetTags_top.empty()){
      RETURN_CHECK("Report::execute()", m_jetPlots["all/jetsLargeR/topTag"]->execute(jetsLargeR_topTagged.asDataVector(), eventWeight), "");
      RETURN_CHECK("Report::execute()", m_jetMETPlots["all/jetsLargeR/topTag"]->execute(jetsLargeR_topTagged.asDataVector(), in_met, eventWeight), "");
    }

    //all/jetsLargeR/wTag
    if(!m_decor_jetTags_w.empty()){
      RETURN_CHECK("Report::execute()", m_jetPlots["all/jetsLargeR/wTag"]->execute(jetsLargeR_wTagged.asDataVector(), eventWeight), "");
      RETURN_CHECK("Report::execute()", m_jetMETPlots["all/jetsLargeR/wTag"]->execute(jetsLargeR_wTagged.asDataVector(), in_met, eventWeight), "");
    }
  }

  //all/jetLargeRX
  for(int i=1; i <= std::min<int>( m_numLeadingJets, in_jetsLargeR->size() ); ++i ){
    RETURN_CHECK("Report::execute()", m_jetPlots["all/jetLargeR"+std::to_string(i)]->execute(in_jetsLargeR->at(i-1), eventWeight), "");
  }

  //all/jetX
  for(int i=1; i <= std::min<int>( m_numLeadingJets, in_jets->size() ); ++i ){
    RETURN_CHECK("Report::execute()", m_jetPlots["all/jet"+std::to_string(i)]->execute(in_jets->at(i-1), eventWeight), "");
  }

  //all/jetX_bTag
  if(!m_decor_jetTags_b.empty()){
    for(int i=1; i <= std::min<int>( m_numLeadingJets, jets_bTagged.size() ); ++i){
      RETURN_CHECK("Report::execute()", m_jetPlots["all/jet"+std::to_string(i)+"/bTag"]->execute(jets_bTagged.at(i-1), eventWeight), "");
    }
  }

  //all/jetLargeRX_topTag
  if(!m_decor_jetTags_top.empty()){
    for(int i=1; i <= std::min<int>( m_numLeadingJets, jetsLargeR_topTagged.size() ); ++i){
      RETURN_CHECK("Report::execute()", m_jetPlots["all/jetLargeR"+std::to_string(i)+"/topTag"]->execute(jetsLargeR_topTagged.at(i-1), eventWeight), "");
    }
  }

  //all/jetLargeRX_wTag
  if(!m_decor_jetTags_w.empty()){
    for(int i=1; i <= std::min<int>( m_numLeadingJets, jetsLargeR_wTagged.size() ); ++i){
      RETURN_CHECK("Report::execute()", m_jetPlots["all/jetLargeR"+std::to_string(i)+"/wTag"]->execute(jetsLargeR_wTagged.at(i-1), eventWeight), "");
    }
  }

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode Report :: postExecute () { return EL::StatusCode::SUCCESS; }

EL::StatusCode Report :: finalize () {
  for( auto jetPlot : m_jetPlots ) {
    if(jetPlot.second) delete jetPlot.second;
  }
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode Report :: histFinalize () { return EL::StatusCode::SUCCESS; }

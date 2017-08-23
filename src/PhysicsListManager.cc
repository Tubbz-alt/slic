#include "PhysicsListManager.hh"

// geant4
#include "G4RunManager.hh"
#include "G4VUserPhysicsList.hh"
#include "G4UserSpecialCuts.hh"
#include "G4StepLimiter.hh"
#include "G4PhysListFactory.hh"

// slic
#include "LCOpticalPhysics.hh"
#include "HepPDTManager.hh"
#include "LCExtendedParticles.hh"

namespace slic {

// QGSP_BERT is the default physics list.
const std::string PhysicsListManager::m_defaultListName = std::string("QGSP_BERT");

PhysicsListManager::PhysicsListManager() :
        Module("PhysicsListManager"), m_currentList(0), m_isInitialized(false), m_isFromDefault(false), m_enableLimits(false), m_setupLimits(false), m_enableOptical(false) {
    // Set the current list to the default.
    m_currentListName = m_defaultListName;

    // Create the messenger class for the manager.
    m_messenger = new PhysicsMessenger();

    // Create the list factory.
    m_factory = new G4PhysListFactory();
}

PhysicsListManager::~PhysicsListManager() {
}

void PhysicsListManager::setCurrentListName(const std::string& n) {
    m_currentListName = n;
    m_isFromDefault = false;
}

G4VUserPhysicsList* PhysicsListManager::createPhysicsList() {

#ifdef SLIC_LOG
    if ( m_isFromDefault ) {
        log() << LOG::okay << "Using default physics list <" << m_defaultListName << ">." << LOG::done;
    }
#endif

    // Create the physics list from the name argument.
    m_currentList = m_factory->GetReferencePhysList(m_currentListName);

    // Enable optical processes.
    if (enableOptical()) {
        dynamic_cast<G4VModularPhysicsList*>(m_currentList)->RegisterPhysics(new LCOpticalPhysics("optical"));
    }

    if (HepPDTManager::instance()->getParticleDataTable()) {
        dynamic_cast<G4VModularPhysicsList*>(m_currentList)->RegisterPhysics(new LCExtendedParticles());
    }

    return m_currentList;
}

void PhysicsListManager::setupUserLimitsProcesses() {

    G4ParticleTable* ptbl = G4ParticleTable::GetParticleTable();
    G4ParticleTable::G4PTblDicIterator* piter = ptbl->GetIterator();
    G4UserSpecialCuts* cuts = new G4UserSpecialCuts("UserSpecialCuts");
    G4StepLimiter* slim = new G4StepLimiter("StepLimiter");

    piter->reset();
    while ((*piter)()) {
        G4ParticleDefinition* pdef = piter->value();
        G4ProcessManager* pmgr = pdef->GetProcessManager();

        // Add user limit processes for steps and special cuts.
        if (pmgr) {
            pmgr->AddProcess(slim, -1, -1, 3);

            pmgr->AddProcess(cuts, -1, -1, 4);
        } else {
            G4Exception("", "", FatalException, "G4ProcessManager is null.");
        }
    }
}

void PhysicsListManager::printAvailablePhysicsLists() {
    const std::vector<G4String>& lists = m_factory->AvailablePhysLists();
    for (std::vector<G4String>::const_iterator it = lists.begin(); it != lists.end(); it++) {
        log().always(*it);
    }
}
}

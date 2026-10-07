#include "ChemEngine/Base/SimulationObject.h"

namespace ChemEngine {

	SimulationObject::SimulationObject()
		: m_name(L"Unnamed")
		, m_description(L"")
		, m_type(L"Generic")
	{
	}

	bool SimulationObject::solve()
	{
		m_status = SimulationObjectStatus::Calculating;

		m_status = SimulationObjectStatus::Calculated;
		return true;
	}

}
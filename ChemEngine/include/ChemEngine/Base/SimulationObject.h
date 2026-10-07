#pragma once

#include "ChemEngine/Interfaces/ISimulationObject.h"
#include <string>

namespace ChemEngine {

	class Flowsheet;

	class SimulationObject : public ISimulationObject
	{
	public:
		SimulationObject();
		virtual ~SimulationObject() = default;

		std::wstring getObjectName() const override { return m_name; }
		void setObjectName(const std::wstring& name) override { m_name = name; }

		std::wstring getObjectDescription() const override { return m_description; }
		void setObjectDescription(const std::wstring& desc) override { m_description = desc; }

		std::wstring getObjectType() const override { return m_type; }

		SimulationObjectStatus getStatus() const override { return m_status; }
		void setStatus(SimulationObjectStatus status) override { m_status = status; }

		Flowsheet* getFlowsheet() const override { return m_flowsheet; }
		void setFlowsheet(Flowsheet* fs) override { m_flowsheet = fs; }

		virtual bool solve() override;

	protected:
		std::wstring m_name;
		std::wstring m_description;
		std::wstring m_type;
		SimulationObjectStatus m_status = SimulationObjectStatus::NotCalculated;
		Flowsheet* m_flowsheet = nullptr;
	};

}
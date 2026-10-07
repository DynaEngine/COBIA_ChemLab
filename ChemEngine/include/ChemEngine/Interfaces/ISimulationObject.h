#pragma once

#include <string>
#include <vector>
#include <memory>

namespace ChemEngine {

	class Flowsheet;

	enum class SimulationObjectStatus
	{
		Idle,
		Calculating,
		Calculated,
		Error,
		NotCalculated
	};

	class ISimulationObject
	{
	public:
		virtual ~ISimulationObject() = default;

		virtual std::wstring getObjectName() const = 0;
		virtual void setObjectName(const std::wstring& name) = 0;

		virtual std::wstring getObjectDescription() const = 0;
		virtual void setObjectDescription(const std::wstring& desc) = 0;

		virtual std::wstring getObjectType() const = 0;

		virtual SimulationObjectStatus getStatus() const = 0;
		virtual void setStatus(SimulationObjectStatus status) = 0;

		virtual bool solve() = 0;

	virtual Flowsheet* getFlowsheet() const = 0;
	virtual void setFlowsheet(Flowsheet* fs) = 0;

	virtual std::vector<std::wstring> getUpstreamObjectNames() const { return {}; }
	virtual std::vector<std::wstring> getDownstreamObjectNames() const { return {}; }
};

using SimulationObjectPtr = std::shared_ptr<ISimulationObject>;

}
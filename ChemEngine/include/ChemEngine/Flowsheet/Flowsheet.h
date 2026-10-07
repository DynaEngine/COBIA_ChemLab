#pragma once

#include "ChemEngine/Interfaces/ISimulationObject.h"
#include "ChemEngine/Base/CompoundConstantProperties.h"
#include <COBIA.h>
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace ChemEngine {

	class FlowsheetSolver;
	class Flowsheet;
	class UnitOperationBase;
	class PropertyPackage;

	enum class FlowsheetObjectType
	{
		MaterialStream,
		UnitOperation,
		EnergyStream,
		SignalStream,
		LogicalBlock,
		PythonScript,
		Custom
	};

	struct FlowsheetObjectInfo
	{
		std::wstring name;
		std::wstring id;
		FlowsheetObjectType type = FlowsheetObjectType::MaterialStream;
		SimulationObjectPtr object;
		bool isCalculated = false;
	};

	struct Connection
	{
		std::wstring name;
		std::wstring fromObject;
		std::wstring toObject;
		std::wstring fromPort;
		std::wstring toPort;
	};

	class Flowsheet
	{
	public:
		Flowsheet();
		virtual ~Flowsheet();

		const std::wstring& getName() const { return m_name; }
		void setName(const std::wstring& name) { m_name = name; }

		const CompoundList& getSelectedCompounds() const { return m_selectedCompounds; }
		void addCompound(const Compound& comp);
		void removeCompound(const std::wstring& name);
		void clearCompounds() { m_selectedCompounds.clear(); m_isDirty = true; }
		Compound* findCompound(const std::wstring& name);
		size_t getCompoundCount() const { return m_selectedCompounds.size(); }

		void addPropertyPackage(std::shared_ptr<PropertyPackage> pp) { m_propertyPackage = pp; }
		std::shared_ptr<PropertyPackage> getPropertyPackage() const { return m_propertyPackage; }

		SimulationObjectPtr createMaterialStream(const std::wstring& name);
		SimulationObjectPtr createUnitOperation(const std::wstring& name,
			::COBIA::CapeInterface& unitInterface);

		void addObject(SimulationObjectPtr obj, FlowsheetObjectType type);
		void removeObject(const std::wstring& name);
		SimulationObjectPtr findObject(const std::wstring& name) const;
		std::vector<FlowsheetObjectInfo>& getObjects() { return m_objects; }
		const std::vector<FlowsheetObjectInfo>& getObjects() const { return m_objects; }

		void connectObjects(const std::wstring& from, const std::wstring& fromPort,
			const std::wstring& to, const std::wstring& toPort);
		void addConnection(const Connection& conn);
		void clearConnections();

		void setDirty(bool dirty) { m_isDirty = dirty; }
		bool isDirty() const { return m_isDirty; }

		void setSolver(std::shared_ptr<FlowsheetSolver> solver);
		std::shared_ptr<FlowsheetSolver> getSolver() const { return m_solver; }

		bool solve();

		using ProgressCallback = std::function<void(const std::wstring&, double)>;
		void setProgressCallback(ProgressCallback cb) { m_progressCb = cb; }

	private:
		std::wstring m_name;
		CompoundList m_selectedCompounds;
		std::vector<FlowsheetObjectInfo> m_objects;
		std::shared_ptr<PropertyPackage> m_propertyPackage;
		bool m_isDirty = false;
		std::shared_ptr<FlowsheetSolver> m_solver;
		ProgressCallback m_progressCb;
	};

}
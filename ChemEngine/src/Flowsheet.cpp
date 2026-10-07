#include "ChemEngine/Flowsheet/Flowsheet.h"
#include "ChemEngine/Solver/FlowsheetSolver.h"
#include "ChemEngine/COBIA/CapeMaterialStream.h"
#include "ChemEngine/COBIA/CapeUnitWrapper.h"
#include "ChemEngine/Units/UnitOperationBase.h"
#include <algorithm>

namespace ChemEngine {

	Flowsheet::Flowsheet()
		: m_name(L"New Flowsheet")
	{
	}

	Flowsheet::~Flowsheet() = default;

	void Flowsheet::addCompound(const Compound& comp)
	{
		m_selectedCompounds.push_back(comp);
		m_isDirty = true;
	}

	void Flowsheet::removeCompound(const std::wstring& name)
	{
		m_selectedCompounds.erase(
			std::remove_if(m_selectedCompounds.begin(), m_selectedCompounds.end(),
				[&](const Compound& c) {
					return c.getConstantProperties().name == name;
				}),
			m_selectedCompounds.end());
		m_isDirty = true;
	}

	Compound* Flowsheet::findCompound(const std::wstring& name)
	{
		for (auto& c : m_selectedCompounds)
			if (c.getConstantProperties().name == name)
				return &c;
		return nullptr;
	}

	SimulationObjectPtr Flowsheet::createMaterialStream(const std::wstring& name)
	{
		auto stream = std::make_shared<COBIA::CapeMaterialStream>();
		stream->setObjectName(name);
		stream->setFlowsheet(this);
		addObject(stream, FlowsheetObjectType::MaterialStream);
		return stream;
	}

	SimulationObjectPtr Flowsheet::createUnitOperation(const std::wstring& name,
		::COBIA::CapeInterface& unitInterface)
	{
		auto unit = std::make_shared<COBIA::CapeUnitWrapper>();
		unit->setObjectName(name);
		unit->setFlowsheet(this);
		if (!unit->loadUnit(unitInterface)) return nullptr;
		addObject(unit, FlowsheetObjectType::UnitOperation);
		return unit;
	}

	void Flowsheet::addObject(SimulationObjectPtr obj, FlowsheetObjectType type)
	{
		if (!obj) return;
		obj->setFlowsheet(this);
		FlowsheetObjectInfo info;
		info.name = obj->getObjectName();
		info.id = info.name;
		info.type = type;
		info.object = obj;
		m_objects.push_back(info);
		m_isDirty = true;
	}

	void Flowsheet::removeObject(const std::wstring& name)
	{
		m_objects.erase(
			std::remove_if(m_objects.begin(), m_objects.end(),
				[&](const FlowsheetObjectInfo& i) { return i.name == name; }),
			m_objects.end());
		m_isDirty = true;
	}

	SimulationObjectPtr Flowsheet::findObject(const std::wstring& name) const
	{
		for (const auto& info : m_objects)
			if (info.name == name) return info.object;
		return nullptr;
	}

	void Flowsheet::connectObjects(const std::wstring& from, const std::wstring& fromPort,
		const std::wstring& to, const std::wstring& toPort)
	{
		auto fromObj = findObject(from);
		auto toObj = findObject(to);
		if (!fromObj || !toObj) return;

		auto fromStream = std::dynamic_pointer_cast<COBIA::CapeMaterialStream>(fromObj);
		auto fromBuiltIn = std::dynamic_pointer_cast<UnitOperationBase>(fromObj);
		auto fromWrapper = std::dynamic_pointer_cast<COBIA::CapeUnitWrapper>(fromObj);

		auto toStream = std::dynamic_pointer_cast<COBIA::CapeMaterialStream>(toObj);
		auto toBuiltIn = std::dynamic_pointer_cast<UnitOperationBase>(toObj);
		auto toWrapper = std::dynamic_pointer_cast<COBIA::CapeUnitWrapper>(toObj);

		if (fromStream)
		{
			if (!fromPort.empty())
				fromStream->connectPort(fromPort, to, toPort);
			else
				fromStream->connectPort(L"Outlet", to, toPort);
		}
		else if (fromBuiltIn)
		{
			fromBuiltIn->connectPort(fromPort.empty() ? L"Outlet" : fromPort, to, toPort);
		}
		else if (fromWrapper)
		{
			if (toStream)
				fromWrapper->connectPort(fromPort, toStream.get());
		}

		if (toStream)
		{
			if (!toPort.empty())
				toStream->connectPort(toPort, from, fromPort);
			else
				toStream->connectPort(L"Inlet", from, fromPort);
		}
		else if (toBuiltIn)
		{
			toBuiltIn->connectPort(toPort.empty() ? L"Inlet" : toPort, from, fromPort);
		}
		else if (toWrapper)
		{
			if (fromStream)
				toWrapper->connectPort(toPort, fromStream.get());
		}
	}

	void Flowsheet::addConnection(const Connection& conn)
	{
		connectObjects(conn.fromObject, conn.fromPort, conn.toObject, conn.toPort);
	}

	void Flowsheet::clearConnections()
	{
		for (auto& info : m_objects)
		{
			if (!info.object) continue;

			auto builtIn = std::dynamic_pointer_cast<UnitOperationBase>(info.object);
			if (builtIn)
			{
				for (auto& port : builtIn->getPorts())
				{
					port.connectedObjectName.clear();
					port.connectedPortName.clear();
				}
				continue;
			}

			auto stream = std::dynamic_pointer_cast<COBIA::CapeMaterialStream>(info.object);
			if (stream)
			{
				for (auto& port : stream->getPorts())
				{
					port.connectedObjectName.clear();
					port.connectedPortName.clear();
				}
				continue;
			}

			auto wrapper = std::dynamic_pointer_cast<COBIA::CapeUnitWrapper>(info.object);
			if (wrapper)
			{
				for (auto& port : wrapper->getPorts())
					port.connectedStream = nullptr;
			}
		}
	}

	void Flowsheet::setSolver(std::shared_ptr<FlowsheetSolver> solver)
	{
		m_solver = solver;
		if (m_solver) m_solver->setFlowsheet(this);
	}

	bool Flowsheet::solve()
	{
		if (!m_solver)
		{
			m_solver = std::make_shared<FlowsheetSolver>();
			m_solver->setFlowsheet(this);
		}
		return m_solver->solve();
	}

}
#include "ChemEngine/COBIA/CapeUnitWrapper.h"
#include "ChemEngine/COBIA/CapeMaterialStream.h"

namespace ChemEngine {
namespace COBIA {

	CapeUnitWrapper::CapeUnitWrapper()
	{
		m_type = L"UnitOperation";
	}

	CapeUnitWrapper::~CapeUnitWrapper()
	{
	}

	bool CapeUnitWrapper::loadUnit(::COBIA::CapeInterface& unitInterface)
	{
		try
		{
			if (!unitInterface)
				return false;

			m_capeUnit = CAPEOPEN_1_2::CapeUnit(unitInterface);

			CapeStringImpl n;
			CAPEOPEN_1_2::CapeIdentification id(unitInterface);
			id.getComponentName(n);
			m_name = n.c_str();
			m_isLoaded = true;

			enumeratePorts();

			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	void CapeUnitWrapper::enumeratePorts()
	{
		m_ports.clear();
		if (!m_capeUnit) return;

		try
		{
			auto portColl = m_capeUnit.ports();
			CapeInteger count = (CapeInteger)portColl.size();
			for (CapeInteger i = 0; i < count; ++i)
			{
				CAPEOPEN_1_2::CapeUnitPort port = portColl[(size_t)i];

				UnitPortInfo info;
				CapeStringImpl n, d;
				CAPEOPEN_1_2::CapeIdentification pid(port);
				pid.getComponentName(n);
				pid.getComponentDescription(d);
				info.name = n.c_str();
				info.description = d.c_str();
				info.type = port.getPortType();
				info.direction = port.getDirection();

				m_ports.push_back(info);
			}
		}
		catch (...)
		{
		}
	}

	bool CapeUnitWrapper::connectPort(const std::wstring& portName,
		CapeMaterialStream* stream)
	{
		if (!m_capeUnit || !stream) return false;

		try
		{
			auto portColl = m_capeUnit.ports();
			CapeInteger count = (CapeInteger)portColl.size();
			for (CapeInteger i = 0; i < count; ++i)
			{
				CAPEOPEN_1_2::CapeUnitPort port = portColl[(size_t)i];
				CapeStringImpl n;
				CAPEOPEN_1_2::CapeIdentification pid(port);
				pid.getComponentName(n);

				std::wstring pn = n.c_str();
				if (pn == portName)
				{
					auto mat = stream->getMaterial();
					port.Connect(
						(ICapeInterface*)
						(CAPEOPEN_1_2::ICapeThermoMaterial*)mat);

					if ((size_t)i < m_ports.size())
						m_ports[(size_t)i].connectedStream = stream;
					return true;
				}
			}
			return false;
		}
		catch (...)
		{
			return false;
		}
	}

	bool CapeUnitWrapper::disconnectPort(const std::wstring& portName)
	{
		if (!m_capeUnit) return false;

		try
		{
			auto portColl = m_capeUnit.ports();
			CapeInteger count = (CapeInteger)portColl.size();
			for (CapeInteger i = 0; i < count; ++i)
			{
				CAPEOPEN_1_2::CapeUnitPort port = portColl[(size_t)i];
				CapeStringImpl n;
				CAPEOPEN_1_2::CapeIdentification pid(port);
				pid.getComponentName(n);

				std::wstring pn = n.c_str();
				if (pn == portName)
				{
					port.Disconnect();
					if ((size_t)i < m_ports.size())
						m_ports[(size_t)i].connectedStream = nullptr;
					return true;
				}
			}
			return false;
		}
		catch (...)
		{
			return false;
		}
	}

	bool CapeUnitWrapper::validate()
	{
		if (!m_capeUnit) return false;

		try
		{
			CapeStringImpl message;
			CapeBoolean valid = m_capeUnit.Validate(message);
			m_validationMessage = message.c_str();
			return valid != 0;
		}
		catch (...)
		{
			m_validationMessage = L"Validation error";
			return false;
		}
	}

	bool CapeUnitWrapper::solve()
	{
		if (!m_capeUnit || !m_isLoaded)
		{
			m_status = SimulationObjectStatus::Error;
			return false;
		}

		m_status = SimulationObjectStatus::Calculating;

		try
		{
			m_capeUnit.Calculate();
			m_status = SimulationObjectStatus::Calculated;
			return true;
		}
		catch (...)
		{
			m_status = SimulationObjectStatus::Error;
			return false;
		}
	}

	CapeMaterialStream* CapeUnitWrapper::getFeedStream(size_t index)
	{
		size_t feedCount = 0;
		for (auto& port : m_ports)
		{
			if (port.direction == CAPEOPEN_1_2::CAPE_INLET &&
				port.type == CAPEOPEN_1_2::CAPE_MATERIAL)
			{
				if (feedCount == index)
					return port.connectedStream;
				++feedCount;
			}
		}
		return nullptr;
	}

	CapeMaterialStream* CapeUnitWrapper::getProductStream(size_t index)
	{
		size_t prodCount = 0;
		for (auto& port : m_ports)
		{
			if (port.direction == CAPEOPEN_1_2::CAPE_OUTLET &&
				port.type == CAPEOPEN_1_2::CAPE_MATERIAL)
			{
				if (prodCount == index)
					return port.connectedStream;
				++prodCount;
			}
		}
		return nullptr;
	}

	std::vector<std::wstring> CapeUnitWrapper::getUpstreamObjectNames() const
	{
		std::vector<std::wstring> result;
		for (const auto& p : m_ports)
			if (p.direction == CAPEOPEN_1_2::CAPE_INLET && p.connectedStream)
				result.push_back(p.connectedStream->getObjectName());
		return result;
	}

	std::vector<std::wstring> CapeUnitWrapper::getDownstreamObjectNames() const
	{
		std::vector<std::wstring> result;
		for (const auto& p : m_ports)
			if (p.direction == CAPEOPEN_1_2::CAPE_OUTLET && p.connectedStream)
				result.push_back(p.connectedStream->getObjectName());
		return result;
	}

}}
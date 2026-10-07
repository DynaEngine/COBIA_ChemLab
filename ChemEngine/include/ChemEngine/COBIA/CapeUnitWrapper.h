#pragma once

#include "ChemEngine/Base/SimulationObject.h"
#include <COBIA.h>
#include <CapeInterfaces_1_2.h>
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace ChemEngine {
namespace COBIA {

	class CapeMaterialStream;

	struct UnitPortInfo
	{
		std::wstring name;
		std::wstring description;
		CAPEOPEN_1_2::CapePortType type = CAPEOPEN_1_2::CAPE_MATERIAL;
		CAPEOPEN_1_2::CapePortDirection direction = CAPEOPEN_1_2::CAPE_INLET;
		CapeMaterialStream* connectedStream = nullptr;
	};

	class CapeUnitWrapper : public SimulationObject
	{
	public:
		CapeUnitWrapper();
		virtual ~CapeUnitWrapper();

		bool loadUnit(::COBIA::CapeInterface& unitInterface);

		CAPEOPEN_1_2::CapeUnit getCapeUnit() const { return m_capeUnit; }

		std::vector<UnitPortInfo>& getPorts() { return m_ports; }
		const std::vector<UnitPortInfo>& getPorts() const { return m_ports; }

		bool connectPort(const std::wstring& portName, CapeMaterialStream* stream);
		bool disconnectPort(const std::wstring& portName);

		bool validate();
		std::wstring getValidationMessage() const { return m_validationMessage; }

		std::vector<std::wstring> getUpstreamObjectNames() const override;
		std::vector<std::wstring> getDownstreamObjectNames() const override;

		bool solve() override;

		CapeMaterialStream* getFeedStream(size_t index);
		CapeMaterialStream* getProductStream(size_t index);

	private:
		void enumeratePorts();

		CAPEOPEN_1_2::CapeUnit m_capeUnit;
		std::vector<UnitPortInfo> m_ports;
		std::wstring m_progId;
		bool m_isLoaded = false;
		std::wstring m_validationMessage;
	};

	using CapeUnitWrapperPtr = std::shared_ptr<CapeUnitWrapper>;

}}
#pragma once

#include <COBIA.h>
#include <CapeInterfaces_1_2.h>
#include <string>
#include <vector>
#include <memory>

namespace ChemEngine {
namespace COBIA {

	enum class UnitOperationType
	{
		Unknown,
		COBIA,
		CAPE_OPEN_1_0,
		CAPE_OPEN_1_1
	};

	struct UnitOperationInfo
	{
		std::wstring name;
		std::wstring progId;
		std::wstring description;
		std::wstring vendor;
		std::wstring version;
		std::wstring clsid;
		UnitOperationType type = UnitOperationType::Unknown;
		bool isAvailable = false;
	};

	class UnitOperationManager
	{
	public:
		UnitOperationManager();
		~UnitOperationManager();

		int enumerateUnits();

		::COBIA::CapeInterface createUnit(const std::wstring& progId);

		size_t getUnitCount() const { return m_units.size(); }
		const std::vector<UnitOperationInfo>& getUnits() const { return m_units; }
		UnitOperationInfo* getUnit(size_t index);
		UnitOperationInfo* getUnit(const std::wstring& progId);

	private:
		int enumerateViaCOM();
		int enumerateViaCOBIA();
		int enumerateViaRegistry();

		std::vector<UnitOperationInfo> m_units;
	};

	using UnitOperationManagerPtr = std::shared_ptr<UnitOperationManager>;

}}
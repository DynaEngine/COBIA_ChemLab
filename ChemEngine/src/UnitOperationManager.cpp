#include "ChemEngine/COBIA/UnitOperationManager.h"

#ifdef _WIN32
#include <windows.h>
#include <comcat.h>
#endif

#include <algorithm>
#include <sstream>

namespace {

#ifdef _WIN32

	const GUID kUnitOpCatId_1_0 = {
		0x678c09a1, 0x7d66, 0x11d2,
		{ 0xa6, 0x7d, 0x00, 0x10, 0x5a, 0x42, 0x88, 0x7f }
	};

	const GUID kUnitOpCatId_1_1 = {
		0x678c09a2, 0x7d66, 0x11d2,
		{ 0xa6, 0x7d, 0x00, 0x10, 0x5a, 0x42, 0x88, 0x7f }
	};

	const GUID kAllUnitOpCatIds[] = { kUnitOpCatId_1_1, kUnitOpCatId_1_0 };
	const int kNumUnitOpCats = 2;

	const GUID kPropPackCatId_COBIA = {
		0xcf51e383, 0x0110, 0x4ed8,
		{ 0xac, 0xb7, 0xb5, 0x0c, 0xfd, 0xe6, 0x90, 0x8e }
	};

	const GUID kPropPackCatId_1_0 = {
		0x678c09a3, 0x7d66, 0x11d2,
		{ 0xa6, 0x7d, 0x00, 0x10, 0x5a, 0x42, 0x88, 0x7f }
	};

	const GUID kPropPackCatId_1_1 = {
		0x678c09a4, 0x7d66, 0x11d2,
		{ 0xa6, 0x7d, 0x00, 0x10, 0x5a, 0x42, 0x88, 0x7f }
	};

	const GUID kExclusionCatIds[] = {
		kPropPackCatId_COBIA,
		kPropPackCatId_1_0,
		kPropPackCatId_1_1
	};
	const int kNumExclusionCats = 3;

	const GUID IID_ICapeUnit = {
		0x3f2c5caf, 0xe157, 0x4785,
		{ 0x83, 0x7f, 0x7a, 0x93, 0xdb, 0xcd, 0x03, 0xfb }
	};

	bool validateUnitOperation(const std::wstring& clsidStr)
	{
		CLSID clsId;
		if (FAILED(CLSIDFromString(clsidStr.c_str(), &clsId)))
			return false;

		IUnknown* pUnknown = nullptr;
		HRESULT hr = CoCreateInstance(clsId, nullptr,
			CLSCTX_INPROC_SERVER,
			IID_IUnknown, (void**)&pUnknown);
		if (FAILED(hr) || !pUnknown)
			return false;

		IUnknown* pUnit = nullptr;
		hr = pUnknown->QueryInterface(IID_ICapeUnit, (void**)&pUnit);
		pUnknown->Release();

		if (SUCCEEDED(hr) && pUnit)
		{
			pUnit->Release();
			return true;
		}
		return false;
	}

	bool guidToString(const GUID& guid, std::wstring& out)
	{
		wchar_t buf[40];
		int len = swprintf(buf, 40,
			L"{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
			guid.Data1, guid.Data2, guid.Data3,
			guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
			guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
		if (len > 0)
		{
			out.assign(buf, len);
			return true;
		}
		return false;
	}

	std::wstring progIdFromCLSID(const std::wstring& clsidStr)
	{
		std::wstring path = L"CLSID\\" + clsidStr;
		HKEY hClsid = nullptr;
		if (RegOpenKeyExW(HKEY_CLASSES_ROOT, path.c_str(), 0, KEY_READ, &hClsid) == ERROR_SUCCESS)
		{
			wchar_t buf[512] = { 0 };
			DWORD type = REG_SZ, size = sizeof(buf);
			if (RegQueryValueExW(hClsid, nullptr, nullptr, &type, (LPBYTE)buf, &size) == ERROR_SUCCESS)
			{
				RegCloseKey(hClsid);
				return std::wstring(buf);
			}

			std::wstring progIdPath = L"CLSID\\" + clsidStr + L"\\ProgID";
			HKEY hProgId = nullptr;
			if (RegOpenKeyExW(HKEY_CLASSES_ROOT, progIdPath.c_str(), 0, KEY_READ, &hProgId) == ERROR_SUCCESS)
			{
				wchar_t pid[512] = { 0 };
				DWORD t = REG_SZ, sz = sizeof(pid);
				if (RegQueryValueExW(hProgId, nullptr, nullptr, &t, (LPBYTE)pid, &sz) == ERROR_SUCCESS)
				{
					RegCloseKey(hProgId);
					RegCloseKey(hClsid);
					return std::wstring(pid);
				}
				RegCloseKey(hProgId);
			}
			RegCloseKey(hClsid);
		}
		return L"";
	}

	std::wstring getUnitNameFromProgId(const std::wstring& progId)
	{
		size_t dot = progId.find_last_of(L'.');
		if (dot != std::wstring::npos)
		{
			std::wstring name = progId.substr(0, dot);
			for (auto& c : name)
				if (c == L'.') c = L' ';
			return name;
		}
		return progId;
	}

	bool hasCategory(const std::wstring& clsidStr, const GUID& catId)
	{
		std::wstring catStr;
		guidToString(catId, catStr);
		std::wstring path = L"CLSID\\" + clsidStr + L"\\Implemented Categories\\" + catStr;
		HKEY hCat = nullptr;
		if (RegOpenKeyExW(HKEY_CLASSES_ROOT, path.c_str(), 0, KEY_READ, &hCat) == ERROR_SUCCESS)
		{
			RegCloseKey(hCat);
			return true;
		}
		return false;
	}

	bool hasAnyExclusionCategory(const std::wstring& clsidStr)
	{
		for (int i = 0; i < kNumExclusionCats; ++i)
		{
			if (hasCategory(clsidStr, kExclusionCatIds[i]))
				return true;
		}
		return false;
	}

#endif

}

namespace ChemEngine {
namespace COBIA {

	UnitOperationManager::UnitOperationManager()
	{
	}

	UnitOperationManager::~UnitOperationManager()
	{
	}

	UnitOperationInfo* UnitOperationManager::getUnit(size_t index)
	{
		if (index < m_units.size())
			return &m_units[index];
		return nullptr;
	}

	UnitOperationInfo* UnitOperationManager::getUnit(const std::wstring& progId)
	{
		for (auto& u : m_units)
		{
			if (u.progId == progId)
				return &u;
		}
		return nullptr;
	}

	int UnitOperationManager::enumerateUnits()
	{
		m_units.clear();
		int count = 0;

		count += enumerateViaCOM();
		count += enumerateViaCOBIA();
		count += enumerateViaRegistry();

		return count;
	}

	int UnitOperationManager::enumerateViaCOM()
	{
#ifdef _WIN32
		int found = 0;

		ICatInformation* pCatInfo = nullptr;
		HRESULT hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr,
			nullptr, CLSCTX_INPROC_SERVER, IID_ICatInformation,
			(void**)&pCatInfo);

		if (SUCCEEDED(hr) && pCatInfo)
		{
			IEnumCLSID* pEnum = nullptr;
			hr = pCatInfo->EnumClassesOfCategories(
				kNumUnitOpCats, const_cast<GUID*>(kAllUnitOpCatIds),
				0, nullptr, &pEnum);

			if (SUCCEEDED(hr) && pEnum)
			{
				CLSID clsId;
				ULONG fetched = 0;
				while (pEnum->Next(1, &clsId, &fetched) == S_OK)
				{
					std::wstring clsidStr;
					guidToString(clsId, clsidStr);

					std::wstring progId = progIdFromCLSID(clsidStr);
					if (progId.empty())
						progId = clsidStr;

					bool exists = false;
					for (const auto& u : m_units)
					{
						if (u.progId == progId)
						{
							exists = true;
							break;
						}
					}

					if (!exists)
					{
						UnitOperationInfo info;
						info.progId = progId;
						info.clsid = clsidStr;
						info.name = getUnitNameFromProgId(progId);
						info.description = L"CAPE-OPEN Unit Operation";
						info.vendor = L"";
						info.version = L"";
						info.type = UnitOperationType::CAPE_OPEN_1_1;
						info.isAvailable = true;

						if (!hasAnyExclusionCategory(clsidStr) && validateUnitOperation(clsidStr))
						{
							m_units.push_back(info);
							found++;
						}
					}
				}
				pEnum->Release();
			}
			pCatInfo->Release();
		}

		return found;
#else
		return 0;
#endif
	}

	int UnitOperationManager::enumerateViaCOBIA()
{
	return 0;
}

	int UnitOperationManager::enumerateViaRegistry()
	{
#ifdef _WIN32
		int found = 0;

		HKEY hClsid = nullptr;
		if (RegOpenKeyExW(HKEY_CLASSES_ROOT, L"CLSID", 0, KEY_READ, &hClsid) != ERROR_SUCCESS)
			return 0;

		DWORD idx = 0;
		wchar_t nameBuf[256];
		while (true)
		{
			DWORD nameLen = 256;
			HRESULT r = RegEnumKeyExW(hClsid, idx, nameBuf, &nameLen,
				nullptr, nullptr, nullptr, nullptr);
			if (r != ERROR_SUCCESS) break;

			std::wstring clsStr(nameBuf);

			bool unitCatFound = false;
			UnitOperationType uType = UnitOperationType::Unknown;

			for (int c = 0; c < kNumUnitOpCats; ++c)
			{
				if (hasCategory(clsStr, kAllUnitOpCatIds[c]))
				{
					unitCatFound = true;
					uType = (c == 0) ? UnitOperationType::CAPE_OPEN_1_1
						: UnitOperationType::CAPE_OPEN_1_0;
					break;
				}
			}

			if (unitCatFound)
			{
				std::wstring progId = progIdFromCLSID(clsStr);
				if (progId.empty()) progId = clsStr;

				bool exists = false;
				for (const auto& u : m_units)
				{
					if (u.progId == progId) { exists = true; break; }
				}

				if (!exists)
				{
					UnitOperationInfo info;
					info.progId = progId;
					info.clsid = clsStr;
					info.name = getUnitNameFromProgId(progId);
					info.description = L"CAPE-OPEN Unit Operation";
					info.type = uType;
					info.isAvailable = true;

					if (!hasAnyExclusionCategory(clsStr) && validateUnitOperation(clsStr))
					{
						m_units.push_back(info);
						found++;
					}
				}
			}
			idx++;
		}

		RegCloseKey(hClsid);
		return found;
#else
		return 0;
#endif
	}

	::COBIA::CapeInterface UnitOperationManager::createUnit(const std::wstring& progId)
	{
		::COBIA::CapeInterface result;

#ifdef _WIN32
		CLSID clsId;
		HRESULT hr = CLSIDFromProgID(progId.c_str(), &clsId);
		if (FAILED(hr))
		{
			hr = CLSIDFromString(progId.c_str(), &clsId);
			if (FAILED(hr))
				return result;
		}

		::COBIA::ICapeInterface* rawInterface = nullptr;
		hr = CoCreateInstance(clsId, nullptr,
			CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER,
			IID_IUnknown, (void**)&rawInterface);
		if (FAILED(hr) || !rawInterface)
			return result;

		result = ::COBIA::CapeInterface(rawInterface);
		rawInterface->vTbl->release(rawInterface->me);
#endif

		return result;
	}

}}
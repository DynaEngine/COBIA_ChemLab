#include "ChemEngine/COBIA/PropertyPackageManager.h"
#include "ChemEngine/COBIA/CapeMaterialStream.h"
#include "ChemEngine/COBIA/CapeMaterialObject.h"

#ifdef _WIN32
#include <windows.h>
#include <comcat.h>
#include <shlwapi.h>
#endif

#include <algorithm>
#include <iostream>
#include <sstream>
#include <iomanip>

namespace {

#ifdef _WIN32

	const GUID kPropertyPackageCatId = {
		0xcf51e383, 0x0110, 0x4ed8,
		{ 0xac, 0xb7, 0xb5, 0x0c, 0xfd, 0xe6, 0x90, 0x8e }
	};

	const GUID IID_ICapeThermoMaterialContext = {
		0x84f1eb12, 0x7af4, 0x0744,
		{ 0xb5, 0x2a, 0x7f, 0xcc, 0x0a, 0x70, 0x45, 0x1c }
	};

	const GUID IID_ICapeThermoEquilibriumRoutine = {
		0x963f5433, 0x0897, 0xf34e,
		{ 0x92, 0x27, 0x3f, 0x8d, 0x37, 0x5f, 0x6e, 0xb5 }
	};

	const GUID IID_ICapeThermoPropertyRoutine = {
		0x58a4f682, 0xd72c, 0x5d4d,
		{ 0x8d, 0x6f, 0xb8, 0x02, 0x78, 0xda, 0x3e, 0xdb }
	};

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

	std::wstring progIdFromCLSID(const std::wstring& clsidStr, HKEY hBaseKey = HKEY_CLASSES_ROOT)
	{
		std::wstring path = L"CLSID\\" + clsidStr;
		HKEY hClsid = nullptr;
		if (RegOpenKeyExW(hBaseKey, path.c_str(), 0, KEY_READ, &hClsid) == ERROR_SUCCESS)
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
			if (RegOpenKeyExW(hBaseKey, progIdPath.c_str(), 0, KEY_READ, &hProgId) == ERROR_SUCCESS)
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

	std::wstring getPackageNameFromProgId(const std::wstring& progId)
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

#endif

}

namespace ChemEngine {
namespace COBIA {

	PropertyPackageManager::PropertyPackageManager()
	{
	}

	void PropertyPackageManager::releaseCOMInstances()
	{
#ifdef _WIN32
		for (auto& pkg : m_packages)
		{
			if (pkg.comInstance)
			{
				pkg.comInstance->Release();
				pkg.comInstance = nullptr;
			}
		}
#endif
	}

	PropertyPackageManager::~PropertyPackageManager()
	{
		releaseCOMInstances();
		if (m_materialObj)
		{
			::COBIA::CapeOpenObject<CapeMaterialObject>::release(
				static_cast<void*>(m_materialObj));
			m_materialObj = nullptr;
		}
	}

	PropertyPackageInfo* PropertyPackageManager::getPackage(size_t index)
	{
		if (index < m_packages.size())
			return &m_packages[index];
		return nullptr;
	}

	int PropertyPackageManager::enumeratePackages()
	{
		m_packages.clear();
		return enumerateViaCOBIA();
	}

	int PropertyPackageManager::enumerateViaCOM()
	{
#ifdef _WIN32
		int found = 0;

		ICatInformation* pCatInfo = nullptr;
		HRESULT hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr,
			nullptr, CLSCTX_INPROC_SERVER, IID_ICatInformation,
			(void**)&pCatInfo);

		if (SUCCEEDED(hr) && pCatInfo)
		{
			CATID catIds[] = { kPropertyPackageCatId };
			IEnumCLSID* pEnum = nullptr;
			hr = pCatInfo->EnumClassesOfCategories(1, catIds, 0, nullptr, &pEnum);

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
					{
						progId = clsidStr;
					}

					bool exists = false;
					for (const auto& p : m_packages)
					{
						if (p.progId == progId)
						{
							exists = true;
							break;
						}
					}

					if (!exists)
					{
						PropertyPackageInfo info;
						info.progId = progId;
						info.name = getPackageNameFromProgId(progId);
						info.description = L"CAPE-OPEN Property Package";
						info.vendor = L"";
						info.version = L"";
						info.type = PropertyPackageType::CAPE_OPEN_1_2;
						info.isActive = false;
						info.isAvailable = true;

						m_packages.push_back(info);
						found++;
					}
				}
				pEnum->Release();
			}
			pCatInfo->Release();
		}

		if (found == 0)
		{
			HKEY hClsid = nullptr;
			if (RegOpenKeyExW(HKEY_CLASSES_ROOT, L"CLSID", 0, KEY_READ, &hClsid) == ERROR_SUCCESS)
			{
				std::wstring catStr;
				guidToString(kPropertyPackageCatId, catStr);

				DWORD idx = 0;
				wchar_t nameBuf[256];
				while (true)
				{
					DWORD nameLen = 256;
					HRESULT r = RegEnumKeyExW(hClsid, idx, nameBuf, &nameLen,
						nullptr, nullptr, nullptr, nullptr);
					if (r != ERROR_SUCCESS) break;

					std::wstring clsStr(nameBuf);
					std::wstring implCatPath = L"CLSID\\" + clsStr + L"\\Implemented Categories\\" + catStr;
					HKEY hCat = nullptr;
					if (RegOpenKeyExW(HKEY_CLASSES_ROOT, implCatPath.c_str(),
						0, KEY_READ, &hCat) == ERROR_SUCCESS)
					{
						std::wstring progId = progIdFromCLSID(clsStr);
						if (progId.empty()) progId = clsStr;

						bool exists = false;
						for (const auto& p : m_packages)
						{
							if (p.progId == progId) { exists = true; break; }
						}

						if (!exists)
						{
							PropertyPackageInfo info;
							info.progId = progId;
							info.name = getPackageNameFromProgId(progId);
							info.description = L"CAPE-OPEN Property Package";
							info.type = PropertyPackageType::CAPE_OPEN_1_2;
							info.isAvailable = true;

							m_packages.push_back(info);
							found++;
						}
						RegCloseKey(hCat);
					}
					idx++;
				}
				RegCloseKey(hClsid);
			}
		}

		return found;
#else
		return 0;
#endif
	}

	int PropertyPackageManager::enumerateViaCOBIA()
	{
#ifdef _WIN32
		int found = 0;

		::COBIA::CapePMCEnumerator enumerator;

		const CapeUUID catId = {
			{0xcf,0x51,0xe3,0x83,0x01,0x10,0x4e,0xd8,0xac,0xb7,0xb5,0x0c,0xfd,0xe6,0x90,0x8e}
		};

		::COBIA::CobiaCollection<::COBIA::CapePMCRegistrationDetails> pmcs;
		try
		{
			pmcs = enumerator.getPMCsByCategory(&catId, 1);
		}
		catch (...)
		{
			return 0;
		}

		CapeInteger nPMC = (CapeInteger)pmcs.size();
		for (CapeInteger i = 0; i < nPMC; ++i)
		{
			try
			{
				::COBIA::CapePMCRegistrationDetails details = pmcs[(size_t)i];
				if (!details)
					continue;

				PropertyPackageInfo info;

				auto name = details.getName();
				if (!name.empty())
					info.name.assign(name.begin(), name.end());

				auto desc = details.getDescription();
				if (!desc.empty())
					info.description.assign(desc.begin(), desc.end());

				auto progId = details.getProgId();
				if (!progId.empty())
					info.progId.assign(progId.begin(), progId.end());

				auto vendor = details.getVendorURL();
				if (!vendor.empty())
					info.vendor.assign(vendor.begin(), vendor.end());

				auto version = details.getComponentVersion();
				if (!version.empty())
					info.version.assign(version.begin(), version.end());

				info.type = PropertyPackageType::COBIA;
				info.isAvailable = false;

				bool already = false;
				for (const auto& p : m_packages)
				{
					if (p.progId == info.progId)
					{
						already = true;
						break;
					}
				}

				if (already || info.progId.empty())
					continue;

				info.isAvailable = true;

				m_packages.push_back(info);
				found++;
			}
			catch (...)
			{
				continue;
			}
		}

		return found;
#else
		return 0;
#endif
	}

	bool PropertyPackageManager::tryInstantiatePackage(const std::wstring& progId, PropertyPackageInfo& outInfo)
	{
#ifdef _WIN32
		CLSID clsId;
		HRESULT hr = CLSIDFromProgID(progId.c_str(), &clsId);
		if (FAILED(hr))
		{
			hr = CLSIDFromString(progId.c_str(), &clsId);
			if (FAILED(hr))
				return false;
		}

		IUnknown* unk = nullptr;
		hr = CoCreateInstance(clsId, nullptr, CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER,
			IID_IUnknown, (void**)&unk);
		if (FAILED(hr) || !unk)
			return false;

		CAPEOPEN_1_2::ICapeThermoPropertyPackageManager* ppMgr = nullptr;
		hr = unk->QueryInterface(IID_IUnknown, (void**)&ppMgr);
		if (SUCCEEDED(hr) && ppMgr)
		{
			outInfo.isAvailable = true;
		}

		unk->Release();
		return outInfo.isAvailable;
#else
		return false;
#endif
	}

	bool PropertyPackageManager::loadPackage(const std::wstring& progId)
	{
		for (auto& pkg : m_packages)
		{
			if (pkg.progId == progId && pkg.isActive && !pkg.compounds.empty())
				return true;
		}

		for (auto& pkg : m_packages)
			pkg.isActive = false;

#ifdef _WIN32
		for (auto& pkg : m_packages)
		{
			if (pkg.comInstance)
			{
				pkg.comInstance->Release();
				pkg.comInstance = nullptr;
			}
		}
#endif

		for (auto& pkg : m_packages)
		{
			if (pkg.progId == progId)
			{
				pkg.compounds.clear();
				pkg.pmcInstance = ::COBIA::CapeInterface();
				pkg.ppInstance = ::COBIA::CapeInterface();
				pkg.manager = CAPEOPEN_1_2::CapeThermoPropertyPackageManager();
#ifdef _WIN32
				if (pkg.comInstance)
				{
					pkg.comInstance->Release();
					pkg.comInstance = nullptr;
				}
#endif

				bool loaded = false;

				try
				{
					::COBIA::CapePMCEnumerator enumerator;
					::COBIA::CapePMCRegistrationDetails details = enumerator.getPMC(progId.c_str());

					if (!details)
					{
						std::wcerr << L"[PP] loadPackage: getPMC direct failed for " << progId
							<< L", trying category search..." << std::endl;

						::COBIA::CapeUUID capeCatId;
						capeCatId = kPropertyPackageCatId;
						auto pmcList = enumerator.getPMCsByCategory(&capeCatId, 1);
						for (size_t i = 0; i < pmcList.size(); ++i)
						{
							auto pid = pmcList[i].getProgId();
							if (!pid.empty())
							{
								std::wstring pidStr(pid.begin(), pid.end());
								if (pidStr == progId)
								{
									details = pmcList[i];
									break;
								}
							}
						}
					}

					if (!details)
					{
						std::wcerr << L"[PP] loadPackage: getPMC category search also failed for " << progId << std::endl;
					}
					else
					{
						pkg.pmcInstance = details.createInstance(CapePMCCreationFlag_Default);
						if (!pkg.pmcInstance)
						{
							std::wcerr << L"[PP] loadPackage: createInstance failed for " << progId << std::endl;
						}
						else
						{
							std::wcerr << L"[PP] loadPackage: PMC instance created, trying direct COBIA path..." << std::endl;

							readCompoundsFromPP(pkg.pmcInstance, pkg.compounds);

							if (!pkg.compounds.empty())
							{
								std::wcerr << L"[PP] loadPackage: direct COBIA read OK, "
									<< pkg.compounds.size() << L" compounds" << std::endl;
								pkg.ppInstance = pkg.pmcInstance;
								loaded = true;
							}
							else
							{
								std::wcerr << L"[PP] loadPackage: direct read empty, trying manager path..." << std::endl;

								pkg.manager = pkg.pmcInstance;
								if (!pkg.manager)
								{
									std::wcerr << L"[PP] loadPackage: manager assignment failed" << std::endl;
								}
								else
								{
									std::vector<std::wstring> ppNameList;
									{
										::COBIA::CapeArrayStringImpl arr;
										pkg.manager.getPropertyPackageList(
											static_cast<::COBIA::ICapeArrayString*>(&arr));
										for (size_t j = 0; j < arr.size(); ++j)
											ppNameList.push_back(arr[j]);
									}

									if (!ppNameList.empty())
									{
										::COBIA::CapeStringImpl wantedName(ppNameList[0].c_str());
										pkg.ppInstance = pkg.manager.GetPropertyPackage(
											static_cast<::COBIA::ICapeString*>(&wantedName));

										if (pkg.ppInstance)
										{
											std::wcerr << L"[PP] loadPackage: PP created via manager, reading compounds..." << std::endl;
											readCompoundsFromPP(pkg.ppInstance, pkg.compounds);
											if (!pkg.compounds.empty())
											{
												std::wcerr << L"[PP] loadPackage: PP read OK, "
													<< pkg.compounds.size() << L" compounds" << std::endl;
												loaded = true;
											}
											else
											{
												std::wcerr << L"[PP] loadPackage: PP read also empty" << std::endl;
											}
										}
										else
										{
											std::wcerr << L"[PP] loadPackage: GetPropertyPackage returned null" << std::endl;
										}
									}
									else
									{
										std::wcerr << L"[PP] loadPackage: getPropertyPackageList returned empty" << std::endl;
									}
								}
							}
						}
					}
				}
				catch (const std::exception& e)
				{
					std::wcerr << L"[PP] loadPackage exception: " << e.what() << std::endl;
				}
				catch (...)
				{
					std::wcerr << L"[PP] loadPackage: unknown exception" << std::endl;
				}

				if (loaded)
				{
					pkg.isActive = true;
					pkg.isAvailable = true;
					return true;
				}
				else
				{
					std::wcerr << L"[PP] loadPackage: failed to load compounds for " << progId << std::endl;
					return false;
				}
			}
		}

		PropertyPackageInfo info;
		info.progId = progId;
		info.name = getPackageNameFromProgId(progId);
		info.description = L"Registered Property Package: " + progId;
		info.vendor = L"External";
		info.type = PropertyPackageType::Unknown;
		info.isActive = false;
		info.isAvailable = false;

#ifdef _WIN32
		{
			CLSID clsId;
			HRESULT hr = CLSIDFromProgID(progId.c_str(), &clsId);
			if (FAILED(hr))
			{
				hr = CLSIDFromString(progId.c_str(), &clsId);
			}
			if (SUCCEEDED(hr))
			{
				IUnknown* unk = nullptr;
				hr = CoCreateInstance(clsId, nullptr,
					CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER,
					IID_IUnknown, (void**)&unk);
				if (SUCCEEDED(hr) && unk)
				{
					info.isAvailable = true;
					info.comInstance = unk;
				}
			}
		}
#else
		tryInstantiatePackage(progId, info);
#endif

		if (info.isAvailable)
		{
			info.isActive = true;
		}

		m_packages.push_back(info);
		return info.isActive;
	}

	void PropertyPackageManager::readCompoundsFromPP(::COBIA::CapeInterface& pp, CompoundList& out)
	{
		out.clear();

		::COBIA::ICapeInterface* rawIf = pp;
		if (!rawIf)
		{
			std::wcerr << L"[PP] readCompounds: null CapeInterface" << std::endl;
			return;
		}

		try
		{
			CAPEOPEN_1_2::CapeThermoCompounds compounds(pp);
			if (!compounds)
			{
				std::wcerr << L"[PP] readCompounds: CapeThermoCompounds QI returned null" << std::endl;
				return;
			}
			std::wcerr << L"[PP] readCompounds: CapeThermoCompounds QI OK" << std::endl;

			::COBIA::CapeArrayStringImpl compIds;
			::COBIA::CapeArrayStringImpl formulae;
			::COBIA::CapeArrayStringImpl names;
			::COBIA::CapeArrayRealImpl boilTemps;
			::COBIA::CapeArrayRealImpl molwts;
			::COBIA::CapeArrayStringImpl casnos;

			std::wcerr << L"[PP] readCompounds: calling GetCompoundList..." << std::endl;
			compounds.GetCompoundList(
				static_cast<::COBIA::ICapeArrayString*>(&compIds),
				static_cast<::COBIA::ICapeArrayString*>(&formulae),
				static_cast<::COBIA::ICapeArrayString*>(&names),
				static_cast<::COBIA::ICapeArrayReal*>(&boilTemps),
				static_cast<::COBIA::ICapeArrayReal*>(&molwts),
				static_cast<::COBIA::ICapeArrayString*>(&casnos));

			std::wcerr << L"[PP] readCompounds: got " << compIds.size() << L" compounds" << std::endl;

			for (size_t k = 0; k < compIds.size(); ++k)
			{
				CompoundConstantProperties props;
				props.formula = (k < formulae.size()) ? std::wstring(formulae[k]) : L"";
				if (k < names.size() && !names[k].empty())
					props.name = std::wstring(names[k]);
				else
					props.name = (k < compIds.size()) ? std::wstring(compIds[k]) : L"";
				props.normalBoilingPoint = (k < boilTemps.size()) ? (double)boilTemps[k] : 0.0;
				props.molecularWeight = (k < molwts.size()) ? (double)molwts[k] : 0.0;
				props.casNumber = (k < casnos.size()) ? std::wstring(casnos[k]) : L"";
				out.push_back(Compound(props));
			}
		}
		catch (::COBIA::cape_open_error& e)
		{
			std::wcerr << L"[PP] readCompounds: cape_open_error code=" << e.getErrorCode()
				<< L" what=" << e.what() << std::endl;
		}
		catch (const std::exception& e)
		{
			std::wcerr << L"[PP] readCompounds: std::exception: " << e.what() << std::endl;
		}
		catch (...)
		{
			std::wcerr << L"[PP] readCompounds: unknown exception" << std::endl;
		}
	}

	PropertyPackageInfo* PropertyPackageManager::getActivePackage()
	{
		for (auto& pkg : m_packages)
		{
			if (pkg.isActive)
				return &pkg;
		}
		return nullptr;
	}

	bool PropertyPackageManager::setActivePackage(size_t index)
	{
		if (index >= m_packages.size())
			return false;

		for (auto& pkg : m_packages)
			pkg.isActive = false;

		m_packages[index].isActive = true;
		return true;
	}

	bool PropertyPackageManager::setActivePackage(const std::wstring& name)
	{
		for (size_t i = 0; i < m_packages.size(); ++i)
		{
			if (m_packages[i].name == name)
				return setActivePackage(i);
		}
		return false;
	}

	void PropertyPackageManager::setMaterial(CapeMaterialStream* stream)
{
	m_currentStream = stream;
}

const CompoundList& PropertyPackageManager::getCompoundsFromPackage() const
{
	for (const auto& pkg : m_packages)
	{
		if (pkg.isActive)
			return pkg.compounds;
	}
	return m_packageCompounds;
}

void PropertyPackageManager::setCompounds(const CompoundList& compounds)
{
	for (auto& pkg : m_packages)
	{
		if (pkg.isActive)
		{
			pkg.compounds = compounds;
			return;
		}
	}
	m_packageCompounds = compounds;
}

void PropertyPackageManager::createMaterialObject()
{
	if (m_materialObj)
	{
		::COBIA::CapeOpenObject<CapeMaterialObject>::release(
			static_cast<void*>(m_materialObj));
		m_materialObj = nullptr;
	}
	m_materialObj = new CapeMaterialObject();
	::COBIA::CapeOpenObject<CapeMaterialObject>::addReference(
		static_cast<void*>(m_materialObj));
	m_materialObj->Initialize();
}

void PropertyPackageManager::setMaterialCompounds(const CompoundList& compounds)
{
	if (!m_materialObj)
		return;

	m_materialObj->clearCompounds();

	for (const auto& comp : compounds)
	{
		const auto& cp = comp.getConstantProperties();
		m_materialObj->setCompound(
			cp.name,
			cp.formula,
			cp.name,
			cp.normalBoilingPoint,
			cp.molecularWeight,
			cp.casNumber
		);
	}

	if (!compounds.empty())
	{
		std::vector<double> fracs(compounds.size(), 1.0 / compounds.size());
		m_materialObj->setOverallComposition(fracs);
	}
}

bool PropertyPackageManager::calculateEquilibrium(
	const std::wstring& spec1,
	const std::wstring& spec2)
{
	if (!m_materialObj || m_materialObj->getNumCompounds() == 0)
		return false;

	auto* active = getActivePackage();
	if (!active)
		return false;

	if (m_currentStream)
	{
		m_materialObj->setOverallTemperature(m_currentStream->getTemperature());
		m_materialObj->setOverallPressure(m_currentStream->getPressure());
	}

	CAPEOPEN_1_2::ICapeThermoMaterial* rawMat =
		static_cast<CAPEOPEN_1_2::ICapeThermoMaterial*>(m_materialObj);
	CAPEOPEN_1_2::CapeThermoMaterial matWrapper(rawMat);

	::COBIA::CapeArrayStringImpl spec1Arr;
	::COBIA::CapeArrayStringImpl spec2Arr;
	spec1Arr.push_back(::COBIA::CapeStringImpl(spec1.c_str()));
	spec1Arr.push_back(::COBIA::CapeStringImpl(COBIATEXT("")));
	spec1Arr.push_back(::COBIA::CapeStringImpl(COBIATEXT("Overall")));
	spec2Arr.push_back(::COBIA::CapeStringImpl(spec2.c_str()));
	spec2Arr.push_back(::COBIA::CapeStringImpl(COBIATEXT("")));
	spec2Arr.push_back(::COBIA::CapeStringImpl(COBIATEXT("Overall")));
	::COBIA::CapeStringImpl solnType(COBIATEXT("unspecified"));

	if (active->ppInstance)
	{
		try
		{
			CAPEOPEN_1_2::CapeThermoMaterialContext ctx(active->ppInstance);
			if (ctx)
			{
				ctx.SetMaterial(matWrapper);
				CAPEOPEN_1_2::CapeThermoEquilibriumRoutine eqRoutine(active->ppInstance);
				if (eqRoutine)
				{
					eqRoutine.CalcEquilibrium(
						static_cast<ICapeArrayString*>(&spec1Arr),
						static_cast<ICapeArrayString*>(&spec2Arr),
						static_cast<ICapeString*>(&solnType));
				}
				ctx.UnsetMaterial();
				return true;
			}
		}
		catch (const std::exception& e)
		{
			std::wcerr << L"[PP] calculateEquilibrium COBIA exception: " << e.what() << std::endl;
		}
		catch (...)
		{
			std::wcerr << L"[PP] calculateEquilibrium COBIA unknown exception" << std::endl;
		}
		return false;
	}

	if (active->progId.empty())
		return false;

#ifdef _WIN32
	if (active->comInstance)
	{
		CAPEOPEN_1_2::ICapeThermoMaterialContext* rawCtx = nullptr;
		active->comInstance->QueryInterface(IID_ICapeThermoMaterialContext, (void**)&rawCtx);
		if (rawCtx)
		{
			CAPEOPEN_1_2::CapeThermoMaterialContext ctx(rawCtx);
			ctx.SetMaterial(matWrapper);

			CAPEOPEN_1_2::ICapeThermoEquilibriumRoutine* rawEq = nullptr;
			active->comInstance->QueryInterface(IID_ICapeThermoEquilibriumRoutine, (void**)&rawEq);
			if (rawEq)
			{
				CAPEOPEN_1_2::CapeThermoEquilibriumRoutine eqRoutine(rawEq);
				eqRoutine.CalcEquilibrium(
					static_cast<ICapeArrayString*>(&spec1Arr),
					static_cast<ICapeArrayString*>(&spec2Arr),
					static_cast<ICapeString*>(&solnType));
			}

			ctx.UnsetMaterial();
		}
		return true;
	}
#endif

	CLSID clsId;
	HRESULT hr = CLSIDFromProgID(active->progId.c_str(), &clsId);
	if (FAILED(hr))
	{
		hr = CLSIDFromString(active->progId.c_str(), &clsId);
		if (FAILED(hr))
			return false;
	}

	IUnknown* unk = nullptr;
	hr = CoCreateInstance(clsId, nullptr,
		CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER,
		IID_IUnknown, (void**)&unk);
	if (FAILED(hr) || !unk)
		return false;

	CAPEOPEN_1_2::ICapeThermoMaterialContext* rawCtx = nullptr;
	unk->QueryInterface(IID_ICapeThermoMaterialContext, (void**)&rawCtx);
	if (rawCtx)
	{
		CAPEOPEN_1_2::CapeThermoMaterialContext ctx(rawCtx);
		ctx.SetMaterial(matWrapper);

		CAPEOPEN_1_2::ICapeThermoEquilibriumRoutine* rawEq = nullptr;
		unk->QueryInterface(IID_ICapeThermoEquilibriumRoutine, (void**)&rawEq);
		if (rawEq)
		{
			CAPEOPEN_1_2::CapeThermoEquilibriumRoutine eqRoutine(rawEq);
			eqRoutine.CalcEquilibrium(
				static_cast<ICapeArrayString*>(&spec1Arr),
				static_cast<ICapeArrayString*>(&spec2Arr),
				static_cast<ICapeString*>(&solnType));
		}

		ctx.UnsetMaterial();
	}

	unk->Release();
	return true;
}

bool PropertyPackageManager::calculateProperty(const std::wstring& prop,
	const std::wstring& phaseLabel)
{
	calculateProperties({prop}, phaseLabel);
	return true;
}

bool PropertyPackageManager::calculateProperties(
	const std::vector<std::wstring>& props,
	const std::wstring& phaseLabel)
{
	if (!m_materialObj || m_materialObj->getNumCompounds() == 0)
		return false;

	if (props.empty())
		return false;

	auto* active = getActivePackage();
	if (!active)
		return false;

	if (m_currentStream)
	{
		m_materialObj->setOverallTemperature(m_currentStream->getTemperature());
		m_materialObj->setOverallPressure(m_currentStream->getPressure());
	}

	CAPEOPEN_1_2::ICapeThermoMaterial* rawMat =
		static_cast<CAPEOPEN_1_2::ICapeThermoMaterial*>(m_materialObj);
	CAPEOPEN_1_2::CapeThermoMaterial matWrapper(rawMat);

	::COBIA::CapeArrayStringImpl propList;
	for (const auto& p : props)
		propList.push_back(::COBIA::CapeStringImpl(p.c_str()));
	::COBIA::CapeStringImpl phaseStr(phaseLabel.empty()
		? COBIATEXT("Overall") : phaseLabel.c_str());

	if (active->ppInstance)
	{
		try
		{
			CAPEOPEN_1_2::CapeThermoMaterialContext ctx(active->ppInstance);
			if (ctx)
			{
				ctx.SetMaterial(matWrapper);
				CAPEOPEN_1_2::CapeThermoPropertyRoutine propRoutine(active->ppInstance);
				if (propRoutine)
				{
					propRoutine.CalcSinglePhaseProp(
						static_cast<::COBIA::ICapeArrayString*>(&propList),
						static_cast<::COBIA::ICapeString*>(&phaseStr));
				}
				ctx.UnsetMaterial();
				return true;
			}
		}
		catch (const std::exception& e)
		{
			std::wcerr << L"[PP] calculateProperties COBIA exception: " << e.what() << std::endl;
		}
		catch (...)
		{
			std::wcerr << L"[PP] calculateProperties COBIA unknown exception" << std::endl;
		}
		return false;
	}

	if (active->progId.empty())
		return false;

#ifdef _WIN32
	if (active->comInstance)
	{
		CAPEOPEN_1_2::ICapeThermoMaterialContext* rawCtx2 = nullptr;
		active->comInstance->QueryInterface(IID_ICapeThermoMaterialContext, (void**)&rawCtx2);
		if (rawCtx2)
		{
			CAPEOPEN_1_2::CapeThermoMaterialContext ctx(rawCtx2);
			ctx.SetMaterial(matWrapper);

			CAPEOPEN_1_2::ICapeThermoPropertyRoutine* rawProp = nullptr;
			active->comInstance->QueryInterface(IID_ICapeThermoPropertyRoutine, (void**)&rawProp);
			if (rawProp)
			{
				CAPEOPEN_1_2::CapeThermoPropertyRoutine propRoutine(rawProp);
				propRoutine.CalcSinglePhaseProp(
					static_cast<::COBIA::ICapeArrayString*>(&propList),
					static_cast<::COBIA::ICapeString*>(&phaseStr));
			}

			ctx.UnsetMaterial();
		}
		return true;
	}
#endif

	CLSID clsId;
	HRESULT hr = CLSIDFromProgID(active->progId.c_str(), &clsId);
	if (FAILED(hr))
	{
		hr = CLSIDFromString(active->progId.c_str(), &clsId);
		if (FAILED(hr))
			return false;
	}

	IUnknown* unk = nullptr;
	hr = CoCreateInstance(clsId, nullptr,
		CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER,
		IID_IUnknown, (void**)&unk);
	if (FAILED(hr) || !unk)
		return false;

	CAPEOPEN_1_2::ICapeThermoMaterialContext* rawCtx2 = nullptr;
	unk->QueryInterface(IID_ICapeThermoMaterialContext, (void**)&rawCtx2);
	if (rawCtx2)
	{
		CAPEOPEN_1_2::CapeThermoMaterialContext ctx(rawCtx2);
		ctx.SetMaterial(matWrapper);

		CAPEOPEN_1_2::ICapeThermoPropertyRoutine* rawProp = nullptr;
		unk->QueryInterface(IID_ICapeThermoPropertyRoutine, (void**)&rawProp);
		if (rawProp)
		{
			CAPEOPEN_1_2::CapeThermoPropertyRoutine propRoutine(rawProp);
			propRoutine.CalcSinglePhaseProp(
				static_cast<::COBIA::ICapeArrayString*>(&propList),
				static_cast<::COBIA::ICapeString*>(&phaseStr));
		}

		ctx.UnsetMaterial();
	}

	unk->Release();
	return true;
}

}}
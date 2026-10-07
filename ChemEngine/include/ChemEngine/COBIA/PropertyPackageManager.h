#pragma once

#include "ChemEngine/Base/CompoundConstantProperties.h"
#include <COBIA.h>
#include <CapeInterfaces_1_2.h>
#include <string>
#include <vector>
#include <map>
#include <memory>

#ifdef _WIN32
#include <windows.h>
#endif

namespace ChemEngine {
namespace COBIA {

class CapeMaterialStream;
class CapeMaterialObject;

enum class PropertyPackageType
{
    Unknown,
    COBIA,
    CAPE_OPEN_1_1,
    CAPE_OPEN_1_2
};

struct PropertyPackageInfo
{
    std::wstring name;
    std::wstring progId;
    std::wstring description;
    std::wstring vendor;
    std::wstring version;
    PropertyPackageType type = PropertyPackageType::Unknown;
    CAPEOPEN_1_2::CapeThermoPropertyPackageManager manager;
    ::COBIA::CapeInterface pmcInstance;
    ::COBIA::CapeInterface ppInstance;
    bool isActive = false;
    bool isAvailable = false;
    CompoundList compounds;
#ifdef _WIN32
    IUnknown* comInstance = nullptr;
#endif
};

class PropertyPackageManager
{
public:
    PropertyPackageManager();
    ~PropertyPackageManager();

    int enumeratePackages();

    bool loadPackage(const std::wstring& progId);

    size_t getPackageCount() const { return m_packages.size(); }
    PropertyPackageInfo* getActivePackage();

    bool setActivePackage(size_t index);
    bool setActivePackage(const std::wstring& name);

    void setMaterial(CapeMaterialStream* stream);
    CapeMaterialStream* getMaterial() const { return m_currentStream; }

    bool calculateEquilibrium(
        const std::wstring& spec1 = L"temperature",
        const std::wstring& spec2 = L"pressure");

    bool calculateProperty(const std::wstring& prop,
        const std::wstring& phaseLabel);

    bool calculateProperties(const std::vector<std::wstring>& props,
        const std::wstring& phaseLabel);

    const CompoundList& getCompoundsFromPackage() const;
    void setCompounds(const CompoundList& compounds);

    const std::vector<PropertyPackageInfo>& getPackages() const { return m_packages; }
    PropertyPackageInfo* getPackage(size_t index);

    CapeMaterialObject* getMaterialObject() const { return m_materialObj; }
    void createMaterialObject();
    void setMaterialCompounds(const CompoundList& compounds);

    void setActiveCompounds(const CompoundList& compounds) { m_activeCompounds = compounds; }
    const CompoundList& getActiveCompounds() const { return m_activeCompounds; }

private:
    int enumerateViaCOM();
    int enumerateViaCOBIA();
    bool tryInstantiatePackage(const std::wstring& progId, PropertyPackageInfo& outInfo);
    static void readCompoundsFromPP(::COBIA::CapeInterface& pp, CompoundList& out);
    void releaseCOMInstances();

    std::vector<PropertyPackageInfo> m_packages;
    CapeMaterialStream* m_currentStream = nullptr;
    CapeMaterialObject* m_materialObj = nullptr;
    CompoundList m_packageCompounds;
    CompoundList m_activeCompounds;
};

using PropertyPackageManagerPtr = std::shared_ptr<PropertyPackageManager>;

}}
#include "ChemEngine/COBIA/CapeMaterialObject.h"
#include <stdexcept>

namespace ChemEngine {
namespace COBIA {

CapeMaterialObject::CapeMaterialObject()
{
}

CapeMaterialObject::~CapeMaterialObject()
{
}

void CapeMaterialObject::Initialize()
{
}

void CapeMaterialObject::Terminate()
{
}

void CapeMaterialObject::setCompound(const std::wstring& id,
    const std::wstring& formula, const std::wstring& name,
    double boilT, double molwt, const std::wstring& cas)
{
    m_compounds.push_back({id, formula, name, boilT, molwt, cas});
}

void CapeMaterialObject::clearCompounds()
{
    m_compounds.clear();
    m_moleFractions.clear();
    m_overallProps.clear();
    m_singlePhaseProps.clear();
    m_phaseLabels.clear();
    m_phaseStatuses.clear();
}

void CapeMaterialObject::setOverallTemperature(double T_K)
{
    m_T = T_K;
    m_overallProps[L"temperature"] = T_K;
}

void CapeMaterialObject::setOverallPressure(double P_Pa)
{
    m_P = P_Pa;
    m_overallProps[L"pressure"] = P_Pa;
}

void CapeMaterialObject::setOverallComposition(const std::vector<double>& moleFractions)
{
    m_moleFractions = moleFractions;
}

void CapeMaterialObject::setOverallFlow(double molarFlow)
{
    m_totalFlow = molarFlow;
    m_overallProps[L"totalflow"] = molarFlow;
}

void CapeMaterialObject::copyFrom(const CapeMaterialObject& source)
{
    m_compounds = source.m_compounds;
    m_moleFractions = source.m_moleFractions;
    m_T = source.m_T;
    m_P = source.m_P;
    m_totalFlow = source.m_totalFlow;
    m_phaseLabels = source.m_phaseLabels;
    m_phaseStatuses = source.m_phaseStatuses;
    m_overallProps = source.m_overallProps;
    m_singlePhaseProps = source.m_singlePhaseProps;
}

double CapeMaterialObject::getCachedOverallProp(const std::wstring& prop) const
{
    auto it = m_overallProps.find(prop);
    if (it != m_overallProps.end()) return it->second;
    if (prop == L"temperature") return m_T;
    if (prop == L"pressure") return m_P;
    if (prop == L"totalflow") return m_totalFlow;
    return 0.0;
}

double CapeMaterialObject::getCachedSinglePhaseProp(
    const std::wstring& prop, const std::wstring& phaseLabel) const
{
    auto it = m_singlePhaseProps.find(phaseLabel);
    if (it != m_singlePhaseProps.end())
    {
        auto jt = it->second.find(prop);
        if (jt != it->second.end()) return jt->second;
    }
    if (prop == L"temperature") return m_T;
    if (prop == L"pressure") return m_P;
    return 0.0;
}

::COBIA::CapeInteger CapeMaterialObject::getNumCompounds()
{
    return static_cast<::COBIA::CapeInteger>(m_compounds.size());
}

void CapeMaterialObject::GetCompoundList(
    ::COBIA::CapeArrayString compIds,
    ::COBIA::CapeArrayString formulae,
    ::COBIA::CapeArrayString names,
    ::COBIA::CapeArrayReal boilTemps,
    ::COBIA::CapeArrayReal molwts,
    ::COBIA::CapeArrayString casnos)
{
    size_t n = m_compounds.size();
    compIds.resize(n);
    formulae.resize(n);
    names.resize(n);
    boilTemps.resize(n);
    molwts.resize(n);
    casnos.resize(n);

    for (size_t i = 0; i < n; ++i)
	{
		compIds[i] = ::COBIA::CapeStringImpl(m_compounds[i].id.c_str());
		formulae[i] = ::COBIA::CapeStringImpl(m_compounds[i].formula.c_str());
		names[i] = ::COBIA::CapeStringImpl(m_compounds[i].name.c_str());
		boilTemps[i] = m_compounds[i].boilTemp;
		molwts[i] = m_compounds[i].molwt;
		casnos[i] = ::COBIA::CapeStringImpl(m_compounds[i].cas.c_str());
	}
}

void CapeMaterialObject::getConstPropList(::COBIA::CapeArrayString props)
{
    props.resize(0);
}

void CapeMaterialObject::GetCompoundConstant(
    ::COBIA::CapeArrayString props,
    ::COBIA::CapeArrayString compIds,
    ::COBIA::CapeBoolean& containsMissingValues,
    ::COBIA::CapeArrayValue propVals)
{
    containsMissingValues = true;
}

void CapeMaterialObject::getPDependentPropList(::COBIA::CapeArrayString props)
{
    props.resize(0);
}

void CapeMaterialObject::GetPDependentProperty(
    ::COBIA::CapeArrayString props,
    ::COBIA::CapeReal pressure,
    ::COBIA::CapeArrayString compIds,
    ::COBIA::CapeBoolean& containsMissingValues,
    ::COBIA::CapeArrayReal propVals)
{
    containsMissingValues = true;
}

void CapeMaterialObject::getTDependentPropList(::COBIA::CapeArrayString props)
{
    props.resize(0);
}

void CapeMaterialObject::GetTDependentProperty(
    ::COBIA::CapeArrayString props,
    ::COBIA::CapeReal temperature,
    ::COBIA::CapeArrayString compIds,
    ::COBIA::CapeBoolean& containsMissingValues,
    ::COBIA::CapeArrayReal propVals)
{
    containsMissingValues = true;
}

void CapeMaterialObject::ClearAllProps()
{
    m_overallProps.clear();
    m_singlePhaseProps.clear();
}

void CapeMaterialObject::CopyFromMaterial(CAPEOPEN_1_2::CapeThermoMaterial source)
{
    throw ::COBIA::cape_open_error(COBIAERR_NotImplemented);
}

CAPEOPEN_1_2::CapeThermoMaterial CapeMaterialObject::CreateMaterial()
{
    throw ::COBIA::cape_open_error(COBIAERR_NotImplemented);
}

void CapeMaterialObject::GetOverallProp(
    ::COBIA::CapeString property,
    ::COBIA::CapeString basis,
    ::COBIA::CapeArrayReal results)
{
    std::wstring propStr = static_cast<std::wstring>(property);
    std::wstring basisStr = basis.empty() ? L"" : static_cast<std::wstring>(basis);

    if (propStr == L"fraction" || propStr == L"composition")
    {
        size_t n = m_moleFractions.empty() ? m_compounds.size() : m_moleFractions.size();
        results.resize(n);
        for (size_t i = 0; i < n; ++i)
        {
            results[i] = (i < m_moleFractions.size()) ? m_moleFractions[i] : 0.0;
        }
        return;
    }

    double v = getCachedOverallProp(propStr);
    results.resize(1);
    results[0] = v;
}

void CapeMaterialObject::GetOverallTPFraction(
    ::COBIA::CapeReal& temperature,
    ::COBIA::CapeReal& pressure,
    ::COBIA::CapeArrayReal composition)
{
    temperature = m_T;
    pressure = m_P;

    size_t n = m_moleFractions.empty() ? m_compounds.size() : m_moleFractions.size();
    composition.resize(n);
    for (size_t i = 0; i < n; ++i)
    {
        composition[i] = (i < m_moleFractions.size()) ? m_moleFractions[i] : 0.0;
    }
}

void CapeMaterialObject::GetPresentPhases(
    ::COBIA::CapeArrayString phaseLabels,
    ::COBIA::CapeArrayEnumeration<CAPEOPEN_1_2::CapePhaseStatus> phaseStatus)
{
    phaseLabels.resize(m_phaseLabels.size());
    phaseStatus.resize(m_phaseLabels.size());
    for (size_t i = 0; i < m_phaseLabels.size(); ++i)
    {
        phaseLabels[i] = ::COBIA::CapeStringImpl(m_phaseLabels[i].c_str());
        phaseStatus[i] = m_phaseStatuses[i];
    }
}

void CapeMaterialObject::GetSinglePhaseProp(
    ::COBIA::CapeString property,
    ::COBIA::CapeString phaseLabel,
    ::COBIA::CapeString basis,
    ::COBIA::CapeArrayReal results)
{
    std::wstring propStr = static_cast<std::wstring>(property);
    std::wstring phaseStr = phaseLabel.empty() ? L"" : static_cast<std::wstring>(phaseLabel);
    std::wstring basisStr = basis.empty() ? L"" : static_cast<std::wstring>(basis);

    if (propStr == L"fraction" || propStr == L"composition")
    {
        size_t n = m_moleFractions.empty() ? m_compounds.size() : m_moleFractions.size();
        results.resize(n);
        for (size_t i = 0; i < n; ++i)
        {
            results[i] = (i < m_moleFractions.size()) ? m_moleFractions[i] : 0.0;
        }
        return;
    }

    double v = getCachedSinglePhaseProp(propStr, phaseStr);
    if (v == 0.0 && propStr == L"temperature")
        v = m_T;
    else if (v == 0.0 && propStr == L"pressure")
        v = m_P;
    results.resize(1);
    results[0] = v;
}

void CapeMaterialObject::GetTPFraction(
    ::COBIA::CapeString phaseLabel,
    ::COBIA::CapeReal& temperature,
    ::COBIA::CapeReal& pressure,
    ::COBIA::CapeArrayReal composition)
{
    std::wstring phaseStr = phaseLabel.empty() ? L"" : static_cast<std::wstring>(phaseLabel);
    temperature = getCachedSinglePhaseProp(L"temperature", phaseStr);
    pressure = getCachedSinglePhaseProp(L"pressure", phaseStr);
    if (temperature == 0.0) temperature = m_T;
    if (pressure == 0.0) pressure = m_P;
    size_t n = m_moleFractions.empty() ? m_compounds.size() : m_moleFractions.size();
    composition.resize(n);
    for (size_t i = 0; i < n; ++i)
    {
        composition[i] = (i < m_moleFractions.size()) ? m_moleFractions[i] : 0.0;
    }
}

void CapeMaterialObject::GetTwoPhaseProp(
    ::COBIA::CapeString property,
    ::COBIA::CapeArrayString phaseLabels,
    ::COBIA::CapeString basis,
    ::COBIA::CapeArrayReal results)
{
    results.resize(1);
    results[0] = 0.0;
}

void CapeMaterialObject::SetOverallProp(
    ::COBIA::CapeString property,
    ::COBIA::CapeString basis,
    ::COBIA::CapeArrayReal values)
{
    std::wstring propStr = static_cast<std::wstring>(property);

    if (propStr == L"temperature" && values.size() > 0)
    {
        m_T = values[0];
    }
    else if (propStr == L"pressure" && values.size() > 0)
    {
        m_P = values[0];
    }
    else if (propStr == L"totalflow" && values.size() > 0)
    {
        m_totalFlow = values[0];
    }

    if (values.size() > 0)
    {
        m_overallProps[propStr] = values[0];
    }
}

void CapeMaterialObject::SetPresentPhases(
    ::COBIA::CapeArrayString phaseLabels,
    ::COBIA::CapeArrayEnumeration<CAPEOPEN_1_2::CapePhaseStatus> phaseStatus)
{
    m_phaseLabels.clear();
    m_phaseStatuses.clear();
    for (size_t i = 0; i < phaseLabels.size(); ++i)
    {
        m_phaseLabels.push_back(static_cast<std::wstring>(phaseLabels[i]));
        m_phaseStatuses.push_back(static_cast<CAPEOPEN_1_2::CapePhaseStatus>(phaseStatus[i]));
    }
}

void CapeMaterialObject::SetSinglePhaseProp(
    ::COBIA::CapeString property,
    ::COBIA::CapeString phaseLabel,
    ::COBIA::CapeString basis,
    ::COBIA::CapeArrayReal values)
{
    std::wstring propStr = static_cast<std::wstring>(property);
    std::wstring phaseStr = phaseLabel.empty() ? L"Overall" : static_cast<std::wstring>(phaseLabel);

    if (values.size() > 0)
    {
        m_singlePhaseProps[phaseStr][propStr] = values[0];
    }
}

void CapeMaterialObject::SetTwoPhaseProp(
    ::COBIA::CapeString property,
    ::COBIA::CapeArrayString phaseLabels,
    ::COBIA::CapeString basis,
    ::COBIA::CapeArrayReal values)
{
    if (values.size() > 0)
    {
        m_overallProps[static_cast<std::wstring>(property)] = values[0];
    }
}

void CapeMaterialObject::SetTPFraction(
    ::COBIA::CapeString phaseLabel,
    ::COBIA::CapeReal temperature,
    ::COBIA::CapeReal pressure,
    ::COBIA::CapeArrayReal composition)
{
    std::wstring phaseStr = phaseLabel.empty() ? L"Overall" : static_cast<std::wstring>(phaseLabel);
    m_singlePhaseProps[phaseStr][L"temperature"] = temperature;
    m_singlePhaseProps[phaseStr][L"pressure"] = pressure;
}

}}
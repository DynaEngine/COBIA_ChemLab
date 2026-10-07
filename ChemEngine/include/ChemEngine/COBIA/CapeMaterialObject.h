#pragma once

#include <COBIA.h>
#include <CapeInterfaceAdapters_1_2.h>
#include <string>
#include <vector>
#include <map>

namespace ChemEngine {
namespace COBIA {

class CapeMaterialObject :
    public ::COBIA::CapeOpenObject<CapeMaterialObject>,
    public CAPEOPEN_1_2::CapeThermoMaterialAdapter<CapeMaterialObject>,
    public CAPEOPEN_1_2::CapeThermoCompoundsAdapter<CapeMaterialObject>
{
public:
    CapeMaterialObject();
    virtual ~CapeMaterialObject();

    static const ::COBIA::CapeUUID getObjectUUID()
    {
        return ::COBIA::CapeUUID{{0xA1,0xB2,0xC3,0xD4,0xE5,0xF6,0x17,0x28,
            0x39,0x4A,0x5B,0x6C,0x7D,0x8E,0x9F,0x00}};
    }

    static void Register(::COBIA::CapePMCRegistrar registrar)
    {
        registrar.putName(COBIATEXT("ChemLab Material"));
        registrar.putDescription(COBIATEXT("ChemLab PME Material Object"));
        registrar.putCapeVersion(COBIATEXT("1.2"));
        registrar.putComponentVersion(COBIATEXT("1.0.0.0"));
        registrar.putProgId(COBIATEXT("ChemLab.CapeMaterialObject"));
    }

    void Initialize();
    void Terminate();
	::COBIA::CapeStringImpl getDescriptionForErrorSource()
	{
		return ::COBIA::CapeStringImpl(COBIATEXT("CapeMaterialObject"));
	}

    void setCompound(const std::wstring& id, const std::wstring& formula,
        const std::wstring& name, double boilT, double molwt, const std::wstring& cas);
    void clearCompounds();

    void setOverallTemperature(double T_K);
    void setOverallPressure(double P_Pa);
    void setOverallComposition(const std::vector<double>& moleFractions);
    void setOverallFlow(double molarFlow);

    void copyFrom(const CapeMaterialObject& source);

    double getCachedOverallProp(const std::wstring& prop) const;
    double getCachedSinglePhaseProp(const std::wstring& prop,
        const std::wstring& phaseLabel) const;
    const std::vector<std::wstring>& getPhaseLabels() const { return m_phaseLabels; }
	const std::vector<double>& getCachedComposition() const { return m_moleFractions; }
	double getMolecularWeight(size_t index = 0) const
    {
        if (index < m_compounds.size())
            return m_compounds[index].molwt;
        return 0.0;
    }

    // ICapeThermoCompounds
    ::COBIA::CapeInteger getNumCompounds();
    void GetCompoundList(
        ::COBIA::CapeArrayString compIds,
        ::COBIA::CapeArrayString formulae,
        ::COBIA::CapeArrayString names,
        ::COBIA::CapeArrayReal boilTemps,
        ::COBIA::CapeArrayReal molwts,
        ::COBIA::CapeArrayString casnos);

    void getConstPropList(::COBIA::CapeArrayString props);
    void GetCompoundConstant(
        ::COBIA::CapeArrayString props,
        ::COBIA::CapeArrayString compIds,
        ::COBIA::CapeBoolean& containsMissingValues,
        ::COBIA::CapeArrayValue propVals);

    void getPDependentPropList(::COBIA::CapeArrayString props);
    void GetPDependentProperty(
        ::COBIA::CapeArrayString props,
        ::COBIA::CapeReal pressure,
        ::COBIA::CapeArrayString compIds,
        ::COBIA::CapeBoolean& containsMissingValues,
        ::COBIA::CapeArrayReal propVals);

    void getTDependentPropList(::COBIA::CapeArrayString props);
    void GetTDependentProperty(
        ::COBIA::CapeArrayString props,
        ::COBIA::CapeReal temperature,
        ::COBIA::CapeArrayString compIds,
        ::COBIA::CapeBoolean& containsMissingValues,
        ::COBIA::CapeArrayReal propVals);

    // ICapeThermoMaterial
    void ClearAllProps();
    void CopyFromMaterial(CAPEOPEN_1_2::CapeThermoMaterial source);
    CAPEOPEN_1_2::CapeThermoMaterial CreateMaterial();

    void GetOverallProp(
        ::COBIA::CapeString property,
        ::COBIA::CapeString basis,
        ::COBIA::CapeArrayReal results);

    void GetOverallTPFraction(
        ::COBIA::CapeReal& temperature,
        ::COBIA::CapeReal& pressure,
        ::COBIA::CapeArrayReal composition);

    void GetPresentPhases(
        ::COBIA::CapeArrayString phaseLabels,
        ::COBIA::CapeArrayEnumeration<CAPEOPEN_1_2::CapePhaseStatus> phaseStatus);

    void GetSinglePhaseProp(
        ::COBIA::CapeString property,
        ::COBIA::CapeString phaseLabel,
        ::COBIA::CapeString basis,
        ::COBIA::CapeArrayReal results);

    void GetTPFraction(
        ::COBIA::CapeString phaseLabel,
        ::COBIA::CapeReal& temperature,
        ::COBIA::CapeReal& pressure,
        ::COBIA::CapeArrayReal composition);

    void GetTwoPhaseProp(
        ::COBIA::CapeString property,
        ::COBIA::CapeArrayString phaseLabels,
        ::COBIA::CapeString basis,
        ::COBIA::CapeArrayReal results);

    void SetOverallProp(
        ::COBIA::CapeString property,
        ::COBIA::CapeString basis,
        ::COBIA::CapeArrayReal values);

    void SetPresentPhases(
        ::COBIA::CapeArrayString phaseLabels,
        ::COBIA::CapeArrayEnumeration<CAPEOPEN_1_2::CapePhaseStatus> phaseStatus);

    void SetSinglePhaseProp(
        ::COBIA::CapeString property,
        ::COBIA::CapeString phaseLabel,
        ::COBIA::CapeString basis,
        ::COBIA::CapeArrayReal values);

    void SetTwoPhaseProp(
        ::COBIA::CapeString property,
        ::COBIA::CapeArrayString phaseLabels,
        ::COBIA::CapeString basis,
        ::COBIA::CapeArrayReal values);

    void SetTPFraction(
        ::COBIA::CapeString phaseLabel,
        ::COBIA::CapeReal temperature,
        ::COBIA::CapeReal pressure,
        ::COBIA::CapeArrayReal composition);

private:
    struct CompoundEntry
    {
        std::wstring id;
        std::wstring formula;
        std::wstring name;
        double boilTemp = 0.0;
        double molwt = 0.0;
        std::wstring cas;
    };

    std::vector<CompoundEntry> m_compounds;
    std::vector<double> m_moleFractions;

    double m_T = 298.15;
    double m_P = 101325.0;
    double m_totalFlow = 0.0;

    std::vector<std::wstring> m_phaseLabels;
    std::vector<CAPEOPEN_1_2::CapePhaseStatus> m_phaseStatuses;

    std::map<std::wstring, double> m_overallProps;
    std::map<std::wstring, std::map<std::wstring, double>> m_singlePhaseProps;
};

}}
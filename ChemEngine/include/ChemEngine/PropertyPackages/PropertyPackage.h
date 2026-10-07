#pragma once

#include "ChemEngine/Base/CompoundConstantProperties.h"
#include <string>
#include <vector>
#include <memory>

namespace ChemEngine {

enum class PropertyPackageType
{
    Unknown,
    Water,
    Air,
    IdealGas,
    Mixture,
    COBIA_External,
    CAPE_OPEN_1_1,
    CAPE_OPEN_1_2
};

class PropertyPackage
{
public:
    PropertyPackage() = default;
    virtual ~PropertyPackage() = default;

    virtual std::wstring getName() const = 0;
    virtual PropertyPackageType getType() const = 0;
    virtual bool isValid() const = 0;

    virtual double h_pt(double P_MPa, double T_C) const = 0;
    virtual double v_pt(double P_MPa, double T_C) const = 0;
    virtual double cp_pt(double P_MPa, double T_C) const = 0;
    virtual double cv_pt(double P_MPa, double T_C) const = 0;
    virtual double s_pt(double P_MPa, double T_C) const = 0;

    virtual double viscosity_pt(double P_MPa, double T_C) const { return 0.0; }
    virtual double thermalConductivity_pt(double P_MPa, double T_C) const { return 0.0; }
    virtual double surfaceTension_T(double T_C) const { return 0.0; }
    virtual double speedOfSound_pt(double P_MPa, double T_C) const { return 0.0; }

    virtual double vaporFraction_pt(double P_MPa, double T_C) const { return -1.0; }
    virtual bool isTwoPhase_pt(double P_MPa, double T_C) const { return false; }

    virtual double saturationPressure_T(double T_C) const { return 0.0; }
    virtual double saturationTemperature_P(double P_MPa) const { return 100.0; }
    virtual double hVaporization_T(double T_C) const { return 0.0; }

    virtual double getMolecularWeight() const { return 28.96; }

    virtual const CompoundList& getCompounds() const { return m_emptyCompounds; }

protected:
    static CompoundList m_emptyCompounds;
};

using PropertyPackagePtr = std::shared_ptr<PropertyPackage>;

}
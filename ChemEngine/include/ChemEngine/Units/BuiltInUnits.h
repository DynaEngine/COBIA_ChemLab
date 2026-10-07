#pragma once

#include "ChemEngine/Units/UnitOperationBase.h"
#include <vector>
#include <string>
#include <memory>

namespace ChemEngine {

class BuiltInMixer : public UnitOperationBase
{
public:
    BuiltInMixer();
    bool solve() override;
    bool validate() override;
};

class BuiltInHeater : public UnitOperationBase
{
public:
    BuiltInHeater();
    bool solve() override;
    void setOutletTemperature(double TK) { m_outletT = TK; }
    void setPressureDrop(double dP) { m_dP = dP; }

private:
    double m_outletT = 353.15;
    double m_dP = 0.0;
};

class BuiltInFlash : public UnitOperationBase
{
public:
    BuiltInFlash();
    bool solve() override;
    void setFlashTemperature(double TK) { m_T = TK; }
    void setFlashPressure(double Pa) { m_P = Pa; }

private:
    double m_T = 351.45;
    double m_P = 101325.0;
};

class BuiltInValve : public UnitOperationBase
{
public:
    BuiltInValve();
    bool solve() override;
    void setPressureDrop(double dP) { m_dP = dP; }

private:
    double m_dP = 50000.0;
};

class BuiltInSplitter : public UnitOperationBase
{
public:
    BuiltInSplitter();
    bool solve() override;
    void setSplitRatio(double ratio) { m_ratio = ratio; }

private:
    double m_ratio = 0.5;
};

class BuiltInCooler : public UnitOperationBase
{
public:
    enum class CoolerMode { OutletTemperature, HeatRemoved };

    BuiltInCooler();
    bool solve() override;
    void setOutletTemperature(double TK) { m_outletT = TK; }
    void setHeatRemoved(double Q) { m_Q = Q; }
    void setPressureDrop(double dP) { m_dP = dP; }
    void setMode(CoolerMode mode) { m_mode = mode; }

private:
    double m_outletT = 298.15;
    double m_Q = 10000.0;
    double m_dP = 0.0;
    CoolerMode m_mode = CoolerMode::OutletTemperature;
};

class BuiltInCompressor : public UnitOperationBase
{
public:
    BuiltInCompressor();
    bool solve() override;
    void setOutletPressure(double Pa) { m_Pout = Pa; }
    void setIsentropicEfficiency(double eta) { m_eta = eta; }

private:
    double m_Pout = 300000.0;
    double m_eta = 0.75;
};

class BuiltInPump : public UnitOperationBase
{
public:
    BuiltInPump();
    bool solve() override;
    void setOutletPressure(double Pa) { m_Pout = Pa; }
    void setEfficiency(double eta) { m_eta = eta; }

private:
    double m_Pout = 300000.0;
    double m_eta = 0.75;
};

class BuiltInEnergyStream : public UnitOperationBase
{
public:
    BuiltInEnergyStream();
    bool solve() override;
    void setEnergyFlow(double Q) { m_energyFlow = Q; }
    double getEnergyFlow() const { return m_energyFlow; }

private:
    double m_energyFlow = 0.0;
};

class BuiltInSignalStream : public UnitOperationBase
{
public:
    BuiltInSignalStream();
    bool solve() override;
    void setSignalValue(double v) { m_signalValue = v; }
    double getSignalValue() const { return m_signalValue; }

private:
    double m_signalValue = 0.0;
};

}
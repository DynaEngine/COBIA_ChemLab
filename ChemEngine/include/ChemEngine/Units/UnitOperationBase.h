#pragma once

#include "ChemEngine/Base/SimulationObject.h"
#include <string>
#include <vector>
#include <map>

namespace ChemEngine {

enum class PortDirection
{
    Inlet,
    Outlet,
    InletOrOutlet
};

enum class PortType
{
    Material,
    Energy,
    Information
};

struct Port
{
    std::wstring name;
    PortDirection direction = PortDirection::Inlet;
    PortType type = PortType::Material;
    std::wstring connectedObjectName;
    std::wstring connectedPortName;
};

class UnitOperationBase : public SimulationObject
{
public:
    UnitOperationBase();
    virtual ~UnitOperationBase() = default;

    const std::vector<Port>& getPorts() const { return m_ports; }
    std::vector<Port>& getPorts() { return m_ports; }

    Port* findPort(const std::wstring& name);
    const Port* findPort(const std::wstring& name) const;

    std::vector<Port*> getInletPorts();
    std::vector<Port*> getOutletPorts();
    std::vector<const Port*> getInletPorts() const;
    std::vector<const Port*> getOutletPorts() const;

    bool connectPort(const std::wstring& portName, const std::wstring& objName,
        const std::wstring& otherPortName);
    bool disconnectPort(const std::wstring& portName);

    std::vector<std::wstring> getUpstreamObjectNames() const override;
    std::vector<std::wstring> getDownstreamObjectNames() const override;

    const std::wstring& getValidationMessage() const { return m_valMsg; }

    virtual bool solve() override;
    virtual bool validate();

protected:
    std::vector<Port> m_ports;
    std::wstring m_valMsg;
};

}
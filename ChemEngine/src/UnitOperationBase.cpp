#include "ChemEngine/Units/UnitOperationBase.h"
#include <algorithm>

namespace ChemEngine {

UnitOperationBase::UnitOperationBase()
{
    m_type = L"UnitOperation";
}

Port* UnitOperationBase::findPort(const std::wstring& name)
{
    for (auto& p : m_ports)
        if (p.name == name) return &p;
    return nullptr;
}

const Port* UnitOperationBase::findPort(const std::wstring& name) const
{
    for (const auto& p : m_ports)
        if (p.name == name) return &p;
    return nullptr;
}

std::vector<Port*> UnitOperationBase::getInletPorts()
{
    std::vector<Port*> result;
    for (auto& p : m_ports)
        if (p.direction == PortDirection::Inlet)
            result.push_back(&p);
    return result;
}

std::vector<Port*> UnitOperationBase::getOutletPorts()
{
    std::vector<Port*> result;
    for (auto& p : m_ports)
        if (p.direction == PortDirection::Outlet)
            result.push_back(&p);
    return result;
}

std::vector<const Port*> UnitOperationBase::getInletPorts() const
{
    std::vector<const Port*> result;
    for (const auto& p : m_ports)
        if (p.direction == PortDirection::Inlet)
            result.push_back(&p);
    return result;
}

std::vector<const Port*> UnitOperationBase::getOutletPorts() const
{
    std::vector<const Port*> result;
    for (const auto& p : m_ports)
        if (p.direction == PortDirection::Outlet)
            result.push_back(&p);
    return result;
}

bool UnitOperationBase::connectPort(const std::wstring& portName,
    const std::wstring& objName, const std::wstring& otherPortName)
{
    auto* port = findPort(portName);
    if (!port) return false;
    port->connectedObjectName = objName;
    port->connectedPortName = otherPortName;
    return true;
}

bool UnitOperationBase::disconnectPort(const std::wstring& portName)
{
    auto* port = findPort(portName);
    if (!port) return false;
    port->connectedObjectName.clear();
    port->connectedPortName.clear();
    return true;
}

bool UnitOperationBase::validate()
{
    m_valMsg.clear();
    for (const auto& p : m_ports)
    {
        if (p.connectedObjectName.empty())
        {
            m_valMsg = L"Port '" + p.name + L"' is not connected";
            return false;
        }
    }
    return true;
}

bool UnitOperationBase::solve()
{
    setStatus(SimulationObjectStatus::Calculated);
    return true;
}

std::vector<std::wstring> UnitOperationBase::getUpstreamObjectNames() const
{
    std::vector<std::wstring> result;
    for (const auto& p : m_ports)
        if (p.direction == PortDirection::Inlet && !p.connectedObjectName.empty())
            result.push_back(p.connectedObjectName);
    return result;
}

std::vector<std::wstring> UnitOperationBase::getDownstreamObjectNames() const
{
    std::vector<std::wstring> result;
    for (const auto& p : m_ports)
        if (p.direction == PortDirection::Outlet && !p.connectedObjectName.empty())
            result.push_back(p.connectedObjectName);
    return result;
}

}
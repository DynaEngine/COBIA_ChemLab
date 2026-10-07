#include "ChemEngine/Units/BuiltInUnits.h"
#include "ChemEngine/COBIA/CapeMaterialStream.h"
#include "ChemEngine/COBIA/CapeMaterialObject.h"
#include "ChemEngine/Flowsheet/Flowsheet.h"
#include "ChemEngine/Solver/FlowsheetSolver.h"
#include <algorithm>
#include <cmath>

namespace ChemEngine {

using CapeMS = COBIA::CapeMaterialStream;

static CapeMS* findConnStream(Flowsheet* fs, const Port& port)
{
    if (!fs || port.connectedObjectName.empty()) return nullptr;
    return dynamic_cast<CapeMS*>(fs->findObject(port.connectedObjectName).get());
}

BuiltInMixer::BuiltInMixer()
{
    m_name = L"Mixer";
    m_type = L"BuiltInMixer";
    m_ports = {
        {L"Inlet1",  PortDirection::Inlet},
        {L"Inlet2",  PortDirection::Inlet},
        {L"Outlet",  PortDirection::Outlet},
    };
}

bool BuiltInMixer::validate()
{
	m_valMsg.clear();

	bool anyInlet = false;
	for (const auto& p : m_ports)
	{
		if (p.direction == PortDirection::Inlet && !p.connectedObjectName.empty())
			anyInlet = true;
	}
	if (!anyInlet)
	{
		m_valMsg = L"Mixer requires at least one inlet connection";
		return false;
	}

	bool anyOutlet = false;
	for (const auto& p : m_ports)
	{
		if (p.direction == PortDirection::Outlet && !p.connectedObjectName.empty())
			anyOutlet = true;
	}
	if (!anyOutlet)
	{
		m_valMsg = L"Mixer requires an outlet connection";
		return false;
	}

	return true;
}

bool BuiltInMixer::solve()
{
	setStatus(SimulationObjectStatus::Calculating);

	auto* fs = m_flowsheet;
	if (!fs) { setStatus(SimulationObjectStatus::Error); return false; }

	auto solver = fs->getSolver();
	if (!solver) { setStatus(SimulationObjectStatus::Error); return false; }

	const auto& order = solver->getCalculationOrder();
	auto it = std::find(order.begin(), order.end(), m_name);
	if (it == order.end()) { setStatus(SimulationObjectStatus::Error); return false; }

	std::vector<CapeMS*> inputs;
	for (const auto& p : m_ports)
	{
		if (p.direction == PortDirection::Inlet)
		{
			auto* s = findConnStream(fs, p);
			if (s) inputs.push_back(s);
		}
	}

	if (inputs.empty())
	{
		setStatus(SimulationObjectStatus::Calculated);
		return true;
	}

	CapeMS* out = nullptr;
	for (const auto& p : m_ports)
	{
		if (p.direction == PortDirection::Outlet)
		{
			out = findConnStream(fs, p);
			break;
		}
	}

	if (!out)
	{
		setStatus(SimulationObjectStatus::Calculated);
		return true;
	}

	const auto& fsCompounds = fs->getSelectedCompounds();
	if (!out->hasCompounds() && !fsCompounds.empty())
		out->addCompounds(fsCompounds);

	size_t nComp = fsCompounds.size();

	if (inputs.size() == 1)
	{
		auto* in = inputs[0];
		out->setTemperature(in->getTemperature());
		out->setPressure(in->getPressure());
		out->setTotalMolarFlow(in->getTotalMolarFlow());
		if (auto* matObj = out->getCapeMaterialObject())
			matObj->setOverallFlow(in->getTotalMolarFlow());
		out->setMoleFractions(in->getMoleFractions());
		out->setStatus(SimulationObjectStatus::Calculated);
		setStatus(SimulationObjectStatus::Calculated);
		return true;
	}

	double totalFlow = 0.0;
	for (auto* s : inputs)
	{
		double f = s->getTotalMolarFlow();
		if (f > 0.0) totalFlow += f;
	}

	if (totalFlow <= 0.0)
	{
		setStatus(SimulationObjectStatus::Calculated);
		return true;
	}

	double mixedP = 1e100;
	bool foundP = false;
	for (auto* s : inputs)
	{
		double f = s->getTotalMolarFlow();
		if (f > 0.0)
		{
			double p = s->getPressure();
			if (p < mixedP) mixedP = p;
			foundP = true;
		}
	}
	if (!foundP) mixedP = 101325.0;

	std::vector<double> mixedZ(nComp, 0.0);
	for (auto* s : inputs)
	{
		double flow = s->getTotalMolarFlow();
		if (flow <= 0.0) continue;
		auto fracs = s->getMoleFractions();
		for (size_t j = 0; j < nComp && j < fracs.size(); ++j)
			mixedZ[j] += flow * fracs[j] / totalFlow;
	}

	bool useEnthalpyBalance = out->hasMaterial();
	for (auto* s : inputs)
	{
		if (!s->hasMaterial()) { useEnthalpyBalance = false; break; }
	}

	if (useEnthalpyBalance)
	{
		double mixedH = 0.0;
		for (auto* s : inputs)
		{
			double f = s->getTotalMolarFlow();
			if (f > 0.0)
				mixedH += s->getEnthalpy(L"mole") * f / totalFlow;
		}

		out->setPressure(mixedP);
		out->setMoleFractions(mixedZ);
		out->setTotalMolarFlow(totalFlow);
		if (auto* matObj = out->getCapeMaterialObject())
			matObj->setOverallFlow(totalFlow);
		out->setEnthalpy(mixedH);
		out->flashPH();
	}
	else
	{
		double mixedT = 0.0;
		for (auto* s : inputs)
		{
			double f = s->getTotalMolarFlow();
			if (f > 0.0) mixedT += s->getTemperature() * f / totalFlow;
		}

		out->setTemperature(mixedT);
		out->setPressure(mixedP);
		out->setTotalMolarFlow(totalFlow);
		if (auto* matObj = out->getCapeMaterialObject())
			matObj->setOverallFlow(totalFlow);
		out->setMoleFractions(mixedZ);
	}
	out->setStatus(SimulationObjectStatus::Calculated);

	setStatus(SimulationObjectStatus::Calculated);
	return true;
}

BuiltInHeater::BuiltInHeater()
    : m_outletT(353.15), m_dP(0.0)
{
    m_name = L"Heater";
    m_type = L"BuiltInHeater";
    m_ports = {
        {L"Inlet",  PortDirection::Inlet},
        {L"Outlet", PortDirection::Outlet},
    };
}

bool BuiltInHeater::solve()
{
	setStatus(SimulationObjectStatus::Calculating);

	auto* fs = m_flowsheet;
	if (!fs) { setStatus(SimulationObjectStatus::Error); return false; }

	CapeMS* in = nullptr;
	CapeMS* out = nullptr;
	for (const auto& p : m_ports)
	{
		if (p.direction == PortDirection::Inlet)
			in = findConnStream(fs, p);
		else
			out = findConnStream(fs, p);
	}

	if (in)
	{
		if (out)
		{
			if (!out->hasCompounds())
			{
				const auto& fsCompounds = fs->getSelectedCompounds();
				if (!fsCompounds.empty())
					out->addCompounds(fsCompounds);
			}

			out->setTemperature(m_outletT);
			out->setPressure(in->getPressure() + m_dP);
			out->setTotalMolarFlow(in->getTotalMolarFlow());
			if (auto* matObj = out->getCapeMaterialObject())
				matObj->setOverallFlow(in->getTotalMolarFlow());
			out->setMoleFractions(in->getMoleFractions());
			out->setStatus(SimulationObjectStatus::Calculated);
		}
		setStatus(SimulationObjectStatus::Calculated);
		return true;
	}

	setStatus(SimulationObjectStatus::Error);
	return false;
}

BuiltInFlash::BuiltInFlash()
    : m_T(351.45), m_P(101325.0)
{
    m_name = L"Flash";
    m_type = L"BuiltInFlash";
    m_ports = {
        {L"Inlet",        PortDirection::Inlet},
        {L"VaporOutlet",  PortDirection::Outlet},
        {L"LiquidOutlet", PortDirection::Outlet},
    };
}

bool BuiltInFlash::solve()
{
    setStatus(SimulationObjectStatus::Calculating);

    auto* fs = m_flowsheet;
    if (!fs) { setStatus(SimulationObjectStatus::Error); return false; }

    CapeMS* feed = findConnStream(fs, m_ports[0]);
    CapeMS* vapor = findConnStream(fs, m_ports[1]);
    CapeMS* liquid = findConnStream(fs, m_ports[2]);

    if (!feed) { setStatus(SimulationObjectStatus::Error); return false; }

    double total = feed->getTotalMolarFlow();
    double vf = 0.3;

    if (vapor)
    {
        vapor->setTemperature(m_T);
        vapor->setPressure(m_P);
        vapor->setTotalMolarFlow(total * vf);
        if (auto* matObj = vapor->getCapeMaterialObject())
            matObj->setOverallFlow(total * vf);
        vapor->setStatus(SimulationObjectStatus::Calculated);
    }
    if (liquid)
    {
        liquid->setTemperature(m_T);
        liquid->setPressure(m_P);
        liquid->setTotalMolarFlow(total * (1.0 - vf));
        if (auto* matObj = liquid->getCapeMaterialObject())
            matObj->setOverallFlow(total * (1.0 - vf));
        liquid->setStatus(SimulationObjectStatus::Calculated);
    }

    setStatus(SimulationObjectStatus::Calculated);
    return true;
}

BuiltInValve::BuiltInValve()
    : m_dP(50000.0)
{
    m_name = L"Valve";
    m_type = L"BuiltInValve";
    m_ports = {
        {L"Inlet",  PortDirection::Inlet},
        {L"Outlet", PortDirection::Outlet},
    };
}

bool BuiltInValve::solve()
{
    setStatus(SimulationObjectStatus::Calculating);

    auto* fs = m_flowsheet;
    if (!fs) { setStatus(SimulationObjectStatus::Error); return false; }

    CapeMS* in = nullptr;
    CapeMS* out = nullptr;
    for (const auto& p : m_ports)
    {
        if (p.direction == PortDirection::Inlet)
            in = findConnStream(fs, p);
        else
            out = findConnStream(fs, p);
    }

    if (!in) { setStatus(SimulationObjectStatus::Error); return false; }

    double Pin = in->getPressure();
    double Hin = in->getEnthalpy();
    double Fin = in->getTotalMolarFlow();
    auto z = in->getMoleFractions();

    double Pout = Pin - m_dP;
    if (Pout < 100.0) Pout = 100.0;

    if (out)
    {
        if (!out->hasCompounds())
        {
            const auto& fsCompounds = fs->getSelectedCompounds();
            if (!fsCompounds.empty())
                out->addCompounds(fsCompounds);
        }

        out->setPressure(Pout);
        out->setEnthalpy(Hin);
        out->setTotalMolarFlow(Fin);
        if (auto* matObj = out->getCapeMaterialObject())
            matObj->setOverallFlow(Fin);
        out->setMoleFractions(z);
        out->flashPH();
        out->setStatus(SimulationObjectStatus::Calculated);
    }

    setStatus(SimulationObjectStatus::Calculated);
    return true;
}

BuiltInSplitter::BuiltInSplitter()
    : m_ratio(0.5)
{
    m_name = L"Splitter";
    m_type = L"BuiltInSplitter";
    m_ports = {
        {L"Inlet",   PortDirection::Inlet},
        {L"Outlet1", PortDirection::Outlet},
        {L"Outlet2", PortDirection::Outlet},
    };
}

bool BuiltInSplitter::solve()
{
    setStatus(SimulationObjectStatus::Calculating);

    auto* fs = m_flowsheet;
    if (!fs) { setStatus(SimulationObjectStatus::Error); return false; }

    CapeMS* in = nullptr;
    CapeMS* out1 = nullptr;
    CapeMS* out2 = nullptr;
    for (const auto& p : m_ports)
    {
        if (p.direction == PortDirection::Inlet)
            in = findConnStream(fs, p);
        else if (p.name == L"Outlet1")
            out1 = findConnStream(fs, p);
        else if (p.name == L"Outlet2")
            out2 = findConnStream(fs, p);
    }

    if (!in) { setStatus(SimulationObjectStatus::Error); return false; }

    double Tin = in->getTemperature();
    double Pin = in->getPressure();
    double Fin = in->getTotalMolarFlow();
    auto z = in->getMoleFractions();

    double r = m_ratio;
    if (r < 0.0) r = 0.0;
    if (r > 1.0) r = 1.0;

    if (out1)
    {
        if (!out1->hasCompounds())
        {
            const auto& fsCompounds = fs->getSelectedCompounds();
            if (!fsCompounds.empty())
                out1->addCompounds(fsCompounds);
        }
        out1->setTemperature(Tin);
        out1->setPressure(Pin);
        out1->setTotalMolarFlow(Fin * r);
        if (auto* matObj = out1->getCapeMaterialObject())
            matObj->setOverallFlow(Fin * r);
        out1->setMoleFractions(z);
        out1->setStatus(SimulationObjectStatus::Calculated);
    }

    if (out2)
    {
        if (!out2->hasCompounds())
        {
            const auto& fsCompounds = fs->getSelectedCompounds();
            if (!fsCompounds.empty())
                out2->addCompounds(fsCompounds);
        }
        out2->setTemperature(Tin);
        out2->setPressure(Pin);
        out2->setTotalMolarFlow(Fin * (1.0 - r));
        if (auto* matObj = out2->getCapeMaterialObject())
            matObj->setOverallFlow(Fin * (1.0 - r));
        out2->setMoleFractions(z);
        out2->setStatus(SimulationObjectStatus::Calculated);
    }

    setStatus(SimulationObjectStatus::Calculated);
    return true;
}

BuiltInCooler::BuiltInCooler()
    : m_outletT(298.15), m_Q(10000.0), m_dP(0.0), m_mode(CoolerMode::OutletTemperature)
{
    m_name = L"Cooler";
    m_type = L"BuiltInCooler";
    m_ports = {
        {L"Inlet",  PortDirection::Inlet},
        {L"Outlet", PortDirection::Outlet},
    };
}

bool BuiltInCooler::solve()
{
    setStatus(SimulationObjectStatus::Calculating);

    auto* fs = m_flowsheet;
    if (!fs) { setStatus(SimulationObjectStatus::Error); return false; }

    CapeMS* in = nullptr;
    CapeMS* out = nullptr;
    for (const auto& p : m_ports)
    {
        if (p.direction == PortDirection::Inlet)
            in = findConnStream(fs, p);
        else
            out = findConnStream(fs, p);
    }

    if (!in) { setStatus(SimulationObjectStatus::Error); return false; }

    double Tin = in->getTemperature();
    double Pin = in->getPressure();
    double Fin = in->getTotalMolarFlow();
    auto z = in->getMoleFractions();

    double Pout = Pin - m_dP;
    if (Pout < 100.0) Pout = 100.0;

    if (out)
    {
        if (!out->hasCompounds())
        {
            const auto& fsCompounds = fs->getSelectedCompounds();
            if (!fsCompounds.empty())
                out->addCompounds(fsCompounds);
        }

        out->setPressure(Pout);
        out->setTotalMolarFlow(Fin);
        if (auto* matObj = out->getCapeMaterialObject())
            matObj->setOverallFlow(Fin);
        out->setMoleFractions(z);

        if (m_mode == CoolerMode::OutletTemperature)
        {
            out->setTemperature(m_outletT);
            out->flashTP();
        }
        else
        {
            double Hin = in->getEnthalpy();
            double Hout = Hin - m_Q / (Fin > 0.0 ? Fin : 1.0);

            out->setEnthalpy(Hout);
            out->flashPH();
        }
        out->setStatus(SimulationObjectStatus::Calculated);
    }

    setStatus(SimulationObjectStatus::Calculated);
    return true;
}

BuiltInCompressor::BuiltInCompressor()
    : m_Pout(300000.0), m_eta(0.75)
{
    m_name = L"Compressor";
    m_type = L"BuiltInCompressor";
    m_ports = {
        {L"Inlet",  PortDirection::Inlet},
        {L"Outlet", PortDirection::Outlet},
    };
}

bool BuiltInCompressor::solve()
{
    setStatus(SimulationObjectStatus::Calculating);

    auto* fs = m_flowsheet;
    if (!fs) { setStatus(SimulationObjectStatus::Error); return false; }

    CapeMS* in = nullptr;
    CapeMS* out = nullptr;
    for (const auto& p : m_ports)
    {
        if (p.direction == PortDirection::Inlet)
            in = findConnStream(fs, p);
        else
            out = findConnStream(fs, p);
    }

    if (!in) { setStatus(SimulationObjectStatus::Error); return false; }

    double Tin = in->getTemperature();
    double Pin = in->getPressure();
    double Fin = in->getTotalMolarFlow();
    auto z = in->getMoleFractions();

    double gamma = 1.4;
    double pressureRatio = m_Pout / (Pin > 0.0 ? Pin : 101325.0);
    double ToutIsentropic = Tin * std::pow(pressureRatio, (gamma - 1.0) / gamma);
    double Tout = Tin + (ToutIsentropic - Tin) / (m_eta > 0.01 ? m_eta : 0.75);

    if (out)
    {
        if (!out->hasCompounds())
        {
            const auto& fsCompounds = fs->getSelectedCompounds();
            if (!fsCompounds.empty())
                out->addCompounds(fsCompounds);
        }

        out->setTemperature(Tout);
        out->setPressure(m_Pout);
        out->setTotalMolarFlow(Fin);
        if (auto* matObj = out->getCapeMaterialObject())
            matObj->setOverallFlow(Fin);
        out->setMoleFractions(z);
        out->flashTP();
        out->setStatus(SimulationObjectStatus::Calculated);
    }

    setStatus(SimulationObjectStatus::Calculated);
    return true;
}

BuiltInPump::BuiltInPump()
    : m_Pout(300000.0), m_eta(0.75)
{
    m_name = L"Pump";
    m_type = L"BuiltInPump";
    m_ports = {
        {L"Inlet",  PortDirection::Inlet},
        {L"Outlet", PortDirection::Outlet},
    };
}

bool BuiltInPump::solve()
{
    setStatus(SimulationObjectStatus::Calculating);

    auto* fs = m_flowsheet;
    if (!fs) { setStatus(SimulationObjectStatus::Error); return false; }

    CapeMS* in = nullptr;
    CapeMS* out = nullptr;
    for (const auto& p : m_ports)
    {
        if (p.direction == PortDirection::Inlet)
            in = findConnStream(fs, p);
        else
            out = findConnStream(fs, p);
    }

    if (!in) { setStatus(SimulationObjectStatus::Error); return false; }

    double Tin = in->getTemperature();
    double Pin = in->getPressure();
    double Fin = in->getTotalMolarFlow();
    auto z = in->getMoleFractions();

    double deltaTK = 2.0 / (m_eta > 0.01 ? m_eta : 0.75);
    double Tout = Tin + deltaTK;

    if (out)
    {
        if (!out->hasCompounds())
        {
            const auto& fsCompounds = fs->getSelectedCompounds();
            if (!fsCompounds.empty())
                out->addCompounds(fsCompounds);
        }

        out->setTemperature(Tout);
        out->setPressure(m_Pout);
        out->setTotalMolarFlow(Fin);
        if (auto* matObj = out->getCapeMaterialObject())
            matObj->setOverallFlow(Fin);
        out->setMoleFractions(z);
        out->flashTP();
        out->setStatus(SimulationObjectStatus::Calculated);
    }

    setStatus(SimulationObjectStatus::Calculated);
    return true;
}

BuiltInEnergyStream::BuiltInEnergyStream()
    : m_energyFlow(0.0)
{
    m_name = L"EnergyStream";
    m_type = L"BuiltInEnergyStream";
    m_ports = {
        {L"Inlet",  PortDirection::Inlet,  PortType::Energy},
        {L"Outlet", PortDirection::Outlet, PortType::Energy},
    };
}

bool BuiltInEnergyStream::solve()
{
    setStatus(SimulationObjectStatus::Calculating);

    auto* inlet = findPort(L"Inlet");
    if (inlet && !inlet->connectedObjectName.empty() && m_flowsheet)
    {
        auto srcObj = m_flowsheet->findObject(inlet->connectedObjectName);
        auto srcEnergy = std::dynamic_pointer_cast<BuiltInEnergyStream>(srcObj);
        if (srcEnergy && srcEnergy->getStatus() == SimulationObjectStatus::Calculated)
            m_energyFlow = srcEnergy->getEnergyFlow();
    }

    setStatus(SimulationObjectStatus::Calculated);
    return true;
}

BuiltInSignalStream::BuiltInSignalStream()
    : m_signalValue(0.0)
{
    m_name = L"SignalStream";
    m_type = L"BuiltInSignalStream";
    m_ports = {
        {L"Inlet",  PortDirection::Inlet,  PortType::Information},
        {L"Outlet", PortDirection::Outlet, PortType::Information},
    };
}

bool BuiltInSignalStream::solve()
{
    setStatus(SimulationObjectStatus::Calculating);

    auto* inlet = findPort(L"Inlet");
    if (inlet && !inlet->connectedObjectName.empty() && m_flowsheet)
    {
        auto srcObj = m_flowsheet->findObject(inlet->connectedObjectName);
        auto srcSignal = std::dynamic_pointer_cast<BuiltInSignalStream>(srcObj);
        if (srcSignal && srcSignal->getStatus() == SimulationObjectStatus::Calculated)
            m_signalValue = srcSignal->getSignalValue();
    }

    setStatus(SimulationObjectStatus::Calculated);
    return true;
}

}
#include "simulationmanager.h"

#include "ChemEngine/ChemEngine.h"
#include "ChemEngine/COBIA/PropertyPackageManager.h"
#include "ChemEngine/COBIA/UnitOperationManager.h"
#include "ChemEngine/COBIA/CapeMaterialStream.h"
#include "ChemEngine/COBIA/CapeUnitWrapper.h"
#include "ChemEngine/Solver/FlowsheetSolver.h"

#include <functional>
#include <QDebug>

SimulationManager::SimulationManager(QObject* parent)
	: QObject(parent)
{
}

SimulationManager::~SimulationManager() = default;

bool SimulationManager::initialize()
{
	m_flowsheet = std::make_unique<ChemEngine::Flowsheet>();
	m_flowsheet->setName(L"Flowsheet 1");

	m_ppManager = std::make_unique<ChemEngine::COBIA::PropertyPackageManager>();
	m_unitOpMgr = std::make_unique<ChemEngine::COBIA::UnitOperationManager>();

	emit logMessage(QStringLiteral("[Engine] ChemEngine initialized."));
	return true;
}

int SimulationManager::enumeratePropertyPackages()
{
	if (!m_ppManager)
	{
		emit logMessage(QStringLiteral("[PP] Error: PropertyPackageManager not initialized."));
		return 0;
	}

	emit logMessage(QStringLiteral("[PP] Enumerating registered Property Packages..."));

	int count = m_ppManager->enumeratePackages();

	emit logMessage(QStringLiteral("[PP] Found %1 Property Package(s).").arg(count));

	const auto& pkgs = m_ppManager->getPackages();
	for (const auto& pkg : pkgs)
	{
		QString avail = pkg.isAvailable ? QStringLiteral("available") : QStringLiteral("unavailable");
		QString typeStr;
		switch (pkg.type)
		{
		case ChemEngine::COBIA::PropertyPackageType::COBIA: typeStr = QStringLiteral("COBIA"); break;
		case ChemEngine::COBIA::PropertyPackageType::CAPE_OPEN_1_2: typeStr = QStringLiteral("CAPE-OPEN 1.2"); break;
		case ChemEngine::COBIA::PropertyPackageType::CAPE_OPEN_1_1: typeStr = QStringLiteral("CAPE-OPEN 1.1"); break;
		default: typeStr = QStringLiteral("Unknown"); break;
		}
		emit logMessage(QStringLiteral("[PP]   %1 (%2) [%3] %4")
			.arg(QString::fromStdWString(pkg.name))
			.arg(QString::fromStdWString(pkg.progId))
			.arg(typeStr)
			.arg(avail));
	}

	return count;
}

bool SimulationManager::loadPropertyPackage(const std::wstring& progId)
{
	if (!m_ppManager)
	{
		emit logMessage(QStringLiteral("[PP] Error: PropertyPackageManager not initialized."));
		return false;
	}

	auto* active = m_ppManager->getActivePackage();
	if (active && active->progId == progId && !active->compounds.empty())
	{
		emit logMessage(QStringLiteral("[PP] Already loaded: %1 (%2 compounds)")
			.arg(QString::fromStdWString(progId))
			.arg(static_cast<int>(active->compounds.size())));
		return true;
	}

	m_flowsheet->clearCompounds();

	bool ok = m_ppManager->loadPackage(progId);
	if (ok)
	{
		emit logMessage(QStringLiteral("[PP] Loaded: %1").arg(QString::fromStdWString(progId)));

		const auto& pkgCompounds = m_ppManager->getCompoundsFromPackage();
		emit logMessage(QStringLiteral("[PP] %1 compounds from COBIA package.").arg(static_cast<int>(pkgCompounds.size())));

		for (const auto& c : pkgCompounds)
		{
			m_flowsheet->addCompound(c);
			emit logMessage(QStringLiteral("[PP]   Compound: %1 (%2) MW=%3")
				.arg(QString::fromStdWString(c.getConstantProperties().name))
				.arg(QString::fromStdWString(c.getConstantProperties().formula))
				.arg(c.getConstantProperties().molecularWeight, 0, 'f', 3));
		}
	}
	else
	{
		emit logMessage(QStringLiteral("[PP] Failed to load: %1").arg(QString::fromStdWString(progId)));
	}
	return ok;
}

bool SimulationManager::setActivePropertyPackage(int index)
{
	if (!m_ppManager) return false;

	bool ok = m_ppManager->setActivePackage(static_cast<size_t>(index));
	if (ok)
	{
		auto* info = m_ppManager->getActivePackage();
		if (info)
		{
			emit logMessage(QStringLiteral("[PP] Active: %1").arg(QString::fromStdWString(info->name)));
		}
	}
	return ok;
}

int SimulationManager::propertyPackageCount() const
{
	if (!m_ppManager) return 0;
	return static_cast<int>(m_ppManager->getPackageCount());
}

QStringList SimulationManager::propertyPackageNames() const
{
	QStringList names;
	if (!m_ppManager) return names;

	const auto& pkgs = m_ppManager->getPackages();
	for (const auto& pkg : pkgs)
	{
		names.append(QString::fromStdWString(pkg.name));
	}
	return names;
}

QStringList SimulationManager::availableCompounds() const
{
	QStringList list;
	if (!m_ppManager) return list;

	const auto& compounds = m_ppManager->getCompoundsFromPackage();
	for (const auto& c : compounds)
	{
		list.append(QString::fromStdWString(c.getConstantProperties().name));
	}
	return list;
}

void SimulationManager::setSelectedCompounds(const QStringList& names)
{
	if (!m_flowsheet) return;

	m_flowsheet->clearCompounds();

	for (const auto& name : names)
	{
		ChemEngine::CompoundConstantProperties props;
		props.name = name.toStdWString();
		ChemEngine::Compound comp(props);
		m_flowsheet->addCompound(comp);
	}

	emit logMessage(QStringLiteral("[Flowsheet] Selected %1 compound(s).").arg(names.size()));
}

bool SimulationManager::solve()
{
	if (!m_flowsheet)
	{
		emit logMessage(QStringLiteral("[Solver] Error: Flowsheet not ready."));
		emit solveFinished(false);
		return false;
	}

	auto* activePP = m_flowsheet->getPropertyPackage().get();
	if (!activePP)
	{
		auto* cobiaInfo = m_ppManager ? m_ppManager->getActivePackage() : nullptr;
		if (!cobiaInfo)
		{
			emit logMessage(QStringLiteral("[Solver] Error: No active Property Package."));
			emit solveFinished(false);
			return false;
		}
	}

	emit logMessage(QStringLiteral("[Solver] Starting solve..."));

	auto solver = m_flowsheet->getSolver();
	if (!solver)
	{
		solver = std::make_shared<ChemEngine::FlowsheetSolver>();
		solver->setFlowsheet(m_flowsheet.get());
		m_flowsheet->setSolver(solver);
	}

	solver->setSolveCallback([this](const std::wstring& name, int iter, bool done) {
		if (done)
		{
			emit solveProgress(QStringLiteral("Done"), iter, true);
		}
		else
		{
			emit solveProgress(QString::fromStdWString(name), iter, false);
		}
	});

	bool result = m_flowsheet->solve();

	if (result)
	{
		emit logMessage(QStringLiteral("[Solver] Solve completed successfully."));
	}
	else
	{
		emit logMessage(QStringLiteral("[Solver] Solve failed."));
	}

	emit solveFinished(result);
	return result;
}

void SimulationManager::setTolerance(double tol)
{
	if (m_flowsheet && m_flowsheet->getSolver())
	{
		m_flowsheet->getSolver()->setTolerance(tol);
	}
}

double SimulationManager::tolerance() const
{
	if (m_flowsheet && m_flowsheet->getSolver())
	{
		return m_flowsheet->getSolver()->getTolerance();
	}
	return 1e-6;
}

void SimulationManager::setMaxIterations(int n)
{
	if (m_flowsheet && m_flowsheet->getSolver())
	{
		m_flowsheet->getSolver()->setMaxIterations(n);
	}
}

int SimulationManager::maxIterations() const
{
	if (m_flowsheet && m_flowsheet->getSolver())
	{
		return m_flowsheet->getSolver()->getMaxIterations();
	}
	return 100;
}

void SimulationManager::setLogCallback(LogCallback cb)
{
	m_logCallback = cb;
}

int SimulationManager::enumerateUnitOperations()
{
	if (!m_unitOpMgr)
	{
		emit logMessage(QStringLiteral("[UO] Error: UnitOperationManager not initialized."));
		return 0;
	}

	emit logMessage(QStringLiteral("[UO] Enumerating registered Unit Operations..."));

	int count = m_unitOpMgr->enumerateUnits();

	emit logMessage(QStringLiteral("[UO] Found %1 Unit Operation(s).").arg(count));

	const auto& units = m_unitOpMgr->getUnits();
	for (const auto& unit : units)
	{
		QString typeStr;
		switch (unit.type)
		{
		case ChemEngine::COBIA::UnitOperationType::COBIA: typeStr = QStringLiteral("COBIA"); break;
		case ChemEngine::COBIA::UnitOperationType::CAPE_OPEN_1_1: typeStr = QStringLiteral("CAPE-OPEN 1.1"); break;
		case ChemEngine::COBIA::UnitOperationType::CAPE_OPEN_1_0: typeStr = QStringLiteral("CAPE-OPEN 1.0"); break;
		default: typeStr = QStringLiteral("Unknown"); break;
		}
		emit logMessage(QStringLiteral("[UO]   %1 (%2) [%3]")
			.arg(QString::fromStdWString(unit.name))
			.arg(QString::fromStdWString(unit.progId))
			.arg(typeStr));
	}

	return count;
}

QStringList SimulationManager::availableUnitOperations() const
{
	QStringList list;
	if (!m_unitOpMgr) return list;

	const auto& units = m_unitOpMgr->getUnits();
	for (const auto& u : units)
	{
		list.append(QString::fromStdWString(u.name));
	}
	return list;
}

bool SimulationManager::loadUnitOperation(const std::wstring& progId, const std::wstring& name)
{
	if (!m_flowsheet || !m_unitOpMgr)
	{
		emit logMessage(QStringLiteral("[UO] Error: Flowsheet or UO manager not ready."));
		return false;
	}

	auto unitInterface = m_unitOpMgr->createUnit(progId);
	if (!unitInterface)
	{
		emit logMessage(QStringLiteral("[UO] Failed to create unit: %1").arg(QString::fromStdWString(progId)));
		return false;
	}

	auto obj = m_flowsheet->createUnitOperation(name, unitInterface);
	if (!obj)
	{
		emit logMessage(QStringLiteral("[UO] Failed to add unit to flowsheet: %1").arg(QString::fromStdWString(name)));
		return false;
	}

	emit logMessage(QStringLiteral("[UO] Unit loaded: %1 (%2)").arg(QString::fromStdWString(name), QString::fromStdWString(progId)));
	return true;
}

void SimulationManager::quickDemo()
{
	if (!m_flowsheet || !m_ppManager)
	{
		emit logMessage(QStringLiteral("[Demo] Error: Engine not initialized."));
		return;
	}

	m_flowsheet->setName(L"Demo 1 - Mixer + Heater + Flash");
	m_flowsheet->clearCompounds();

	emit logMessage(QStringLiteral("[Demo] ===== Building Mixer+Heater+Flash Flowsheet ====="));

	ChemEngine::CompoundConstantProperties h2oProps;
	h2oProps.name = L"Water";
	h2oProps.formula = L"H2O";
	h2oProps.molecularWeight = 18.015;
	h2oProps.criticalTemperature = 647.1;
	h2oProps.criticalPressure = 22.064e6;
	h2oProps.acentricFactor = 0.344;
	m_flowsheet->addCompound(ChemEngine::Compound(h2oProps));

	ChemEngine::CompoundConstantProperties etohProps;
	etohProps.name = L"Ethanol";
	etohProps.formula = L"C2H5OH";
	etohProps.molecularWeight = 46.069;
	etohProps.criticalTemperature = 514.0;
	etohProps.criticalPressure = 6.137e6;
	etohProps.acentricFactor = 0.644;
	m_flowsheet->addCompound(ChemEngine::Compound(etohProps));

	emit logMessage(QStringLiteral("[Demo] Compounds: Water, Ethanol"));

	auto feed1 = m_flowsheet->createMaterialStream(L"Feed1");
	auto* f1Mat = dynamic_cast<ChemEngine::COBIA::CapeMaterialStream*>(feed1.get());
	if (f1Mat)
	{
		::COBIA::CapeStringImpl n(L"Feed1");
		f1Mat->createMaterial(n);
		f1Mat->setTemperature(298.15);
		f1Mat->setPressure(101325.0);
		f1Mat->setTotalMolarFlow(50.0);
		f1Mat->setMoleFractions({ 0.5, 0.5 });
		emit logMessage(QStringLiteral("[Demo] Feed1: T=298K, P=1atm, F=50 mol/s, H2O/EtOH=0.5/0.5"));
	}

	auto feed2 = m_flowsheet->createMaterialStream(L"Feed2");
	auto* f2Mat = dynamic_cast<ChemEngine::COBIA::CapeMaterialStream*>(feed2.get());
	if (f2Mat)
	{
		::COBIA::CapeStringImpl n(L"Feed2");
		f2Mat->createMaterial(n);
		f2Mat->setTemperature(298.15);
		f2Mat->setPressure(101325.0);
		f2Mat->setTotalMolarFlow(50.0);
		f2Mat->setMoleFractions({ 0.5, 0.5 });
		emit logMessage(QStringLiteral("[Demo] Feed2: T=298K, P=1atm, F=50 mol/s, H2O/EtOH=0.5/0.5"));
	}

	auto mixer = std::make_shared<ChemEngine::BuiltInMixer>();
	m_flowsheet->addObject(mixer, ChemEngine::FlowsheetObjectType::UnitOperation);

	auto mixed = m_flowsheet->createMaterialStream(L"Mixed");
	auto* mxMat = dynamic_cast<ChemEngine::COBIA::CapeMaterialStream*>(mixed.get());
	if (mxMat)
	{
		::COBIA::CapeStringImpl n(L"Mixed");
		mxMat->createMaterial(n);
	}

	auto heater = std::make_shared<ChemEngine::BuiltInHeater>();
	heater->setOutletTemperature(353.15);
	m_flowsheet->addObject(heater, ChemEngine::FlowsheetObjectType::UnitOperation);

	auto hot = m_flowsheet->createMaterialStream(L"HotFeed");
	auto* hotMat = dynamic_cast<ChemEngine::COBIA::CapeMaterialStream*>(hot.get());
	if (hotMat)
	{
		::COBIA::CapeStringImpl n(L"HotFeed");
		hotMat->createMaterial(n);
	}

	auto flash = std::make_shared<ChemEngine::BuiltInFlash>();
	flash->setFlashTemperature(351.45);
	flash->setFlashPressure(101325.0);
	m_flowsheet->addObject(flash, ChemEngine::FlowsheetObjectType::UnitOperation);

	auto vapor = m_flowsheet->createMaterialStream(L"Vapor");
	auto* vapMat = dynamic_cast<ChemEngine::COBIA::CapeMaterialStream*>(vapor.get());
	if (vapMat)
	{
		::COBIA::CapeStringImpl n(L"Vapor");
		vapMat->createMaterial(n);
	}

	auto liquid = m_flowsheet->createMaterialStream(L"Liquid");
	auto* liqMat = dynamic_cast<ChemEngine::COBIA::CapeMaterialStream*>(liquid.get());
	if (liqMat)
	{
		::COBIA::CapeStringImpl n(L"Liquid");
		liqMat->createMaterial(n);
	}

	m_flowsheet->connectObjects(L"Feed1", L"outlet", L"Mixer", L"Inlet1");
	m_flowsheet->connectObjects(L"Feed2", L"outlet", L"Mixer", L"Inlet2");
	m_flowsheet->connectObjects(L"Mixer", L"Outlet", L"Mixed", L"inlet");
	m_flowsheet->connectObjects(L"Mixed", L"outlet", L"Heater", L"Inlet");
	m_flowsheet->connectObjects(L"Heater", L"Outlet", L"HotFeed", L"inlet");
	m_flowsheet->connectObjects(L"HotFeed", L"outlet", L"Flash", L"Inlet");
	m_flowsheet->connectObjects(L"Flash", L"VaporOutlet", L"Vapor", L"inlet");
	m_flowsheet->connectObjects(L"Flash", L"LiquidOutlet", L"Liquid", L"inlet");

	emit logMessage(QStringLiteral("[Demo] Topology: Feed1+Feed2 -> Mixer -> Mixed -> Heater -> HotFeed -> Flash -> Vapor + Liquid"));
	emit logMessage(QStringLiteral("[Demo] ===== Demo ready. Press Solve to simulate. ====="));

	m_flowsheet->setDirty(true);
}

bool SimulationManager::loadBuiltInUnitOperation(const QString& typeName, const std::wstring& name)
{
	if (!m_flowsheet) return false;

	std::shared_ptr<ChemEngine::SimulationObject> unit;

	if (typeName == QStringLiteral("mixer"))
	{
		unit = std::make_shared<ChemEngine::BuiltInMixer>();
	}
	else if (typeName == QStringLiteral("heater"))
	{
		unit = std::make_shared<ChemEngine::BuiltInHeater>();
	}
	else if (typeName == QStringLiteral("flash"))
	{
		unit = std::make_shared<ChemEngine::BuiltInFlash>();
	}
	else if (typeName == QStringLiteral("valve"))
	{
		unit = std::make_shared<ChemEngine::BuiltInValve>();
	}
	else if (typeName == QStringLiteral("splitter"))
	{
		unit = std::make_shared<ChemEngine::BuiltInSplitter>();
	}
	else if (typeName == QStringLiteral("cooler"))
	{
		unit = std::make_shared<ChemEngine::BuiltInCooler>();
	}
	else if (typeName == QStringLiteral("compressor"))
	{
		unit = std::make_shared<ChemEngine::BuiltInCompressor>();
	}
	else if (typeName == QStringLiteral("pump"))
	{
		unit = std::make_shared<ChemEngine::BuiltInPump>();
	}
	else if (typeName == QStringLiteral("energystream"))
	{
		auto energy = std::make_shared<ChemEngine::BuiltInEnergyStream>();
		energy->setObjectName(name);
		m_flowsheet->addObject(energy, ChemEngine::FlowsheetObjectType::EnergyStream);
		emit logMessage(QStringLiteral("[Energy] Created: %1 (Q=0 kW)").arg(QString::fromStdWString(name)));
		return true;
	}
	else if (typeName == QStringLiteral("signalstream"))
	{
		auto signal = std::make_shared<ChemEngine::BuiltInSignalStream>();
		signal->setObjectName(name);
		m_flowsheet->addObject(signal, ChemEngine::FlowsheetObjectType::SignalStream);
		emit logMessage(QStringLiteral("[Signal] Created: %1").arg(QString::fromStdWString(name)));
		return true;
	}
	else if (typeName == QStringLiteral("materialstream"))
	{
		auto obj = m_flowsheet->createMaterialStream(name);
		auto* stream = dynamic_cast<ChemEngine::COBIA::CapeMaterialStream*>(obj.get());
		if (stream)
		{
			stream->setTemperature(298.15);
			stream->setPressure(101325.0);
			stream->setTotalMolarFlow(100.0);

			const auto& compounds = m_flowsheet->getSelectedCompounds();
			if (!compounds.empty())
			{
				double frac = 1.0 / compounds.size();
				std::vector<double> x(compounds.size(), frac);
				stream->setMoleFractions(x);
			}

			emit logMessage(QStringLiteral("[Stream] Created: %1 (T=298K, P=1atm, F=100mol/s)").arg(QString::fromStdWString(name)));
			return true;
		}
		emit logMessage(QStringLiteral("[Stream] Failed to create: %1").arg(QString::fromStdWString(name)));
		return false;
	}
	else
	{
		emit logMessage(QStringLiteral("[UO] Unknown built-in unit type: %1").arg(typeName));
		return false;
	}

	unit->setObjectName(name);
	m_flowsheet->addObject(unit, ChemEngine::FlowsheetObjectType::UnitOperation);
	emit logMessage(QStringLiteral("[UO] Built-in unit loaded: %1 (%2)")
		.arg(QString::fromStdWString(name), typeName));
	return true;
}

bool SimulationManager::addMaterialStream(const std::wstring& name)
{
	if (!m_flowsheet) return false;

	auto stream = m_flowsheet->createMaterialStream(name);
	if (!stream) return false;

	emit logMessage(QStringLiteral("[Stream] Created: %1").arg(QString::fromStdWString(name)));
	return true;
}

bool SimulationManager::connectFlowsheetObjects(const std::wstring& from, const std::wstring& fromPort,
	const std::wstring& to, const std::wstring& toPort)
{
	if (!m_flowsheet) return false;

	m_flowsheet->connectObjects(from, fromPort, to, toPort);
	emit logMessage(QStringLiteral("[Connect] %1:%2 -> %3:%4")
			.arg(QString::fromStdWString(from))
			.arg(QString::fromStdWString(fromPort))
			.arg(QString::fromStdWString(to))
			.arg(QString::fromStdWString(toPort)));
	return true;
}

void SimulationManager::clearFlowsheet()
{
	if (m_flowsheet)
	{
		auto& objs = m_flowsheet->getObjects();
		while (!objs.empty())
			m_flowsheet->removeObject(objs.front().name);
		m_flowsheet->clearCompounds();
		m_flowsheet->clearConnections();
	}
	emit logMessage(QStringLiteral("[Flowsheet] Cleared."));
}
#pragma once

#include <QObject>
#include <QString>
#include <memory>
#include <functional>

namespace ChemEngine {
	class Flowsheet;
	class FlowsheetSolver;
	namespace COBIA {
		class PropertyPackageManager;
		class UnitOperationManager;
		class CapeMaterialStream;
		class CapeUnitWrapper;
	}
}

class SimulationManager : public QObject
{
	Q_OBJECT

public:
	explicit SimulationManager(QObject* parent = nullptr);
	~SimulationManager() override;

	bool initialize();
	int enumeratePropertyPackages();
	int enumerateUnitOperations();

	ChemEngine::Flowsheet* flowsheet() const { return m_flowsheet.get(); }
	ChemEngine::COBIA::PropertyPackageManager* propertyPackageManager() const { return m_ppManager.get(); }
	ChemEngine::COBIA::UnitOperationManager* unitOperationManager() const { return m_unitOpMgr.get(); }

	bool loadPropertyPackage(const std::wstring& progId);
	bool setActivePropertyPackage(int index);
	int propertyPackageCount() const;
	QStringList propertyPackageNames() const;

	QStringList availableCompounds() const;
	void setSelectedCompounds(const QStringList& names);

	bool loadUnitOperation(const std::wstring& progId, const std::wstring& name);
	bool loadBuiltInUnitOperation(const QString& typeName, const std::wstring& name);
	QStringList availableUnitOperations() const;

	bool addMaterialStream(const std::wstring& name);
	bool connectFlowsheetObjects(const std::wstring& from, const std::wstring& fromPort,
		const std::wstring& to, const std::wstring& toPort);

	void clearFlowsheet();
	void buildFromCanvas();

	bool solve();
	void quickDemo();
	void setTolerance(double tol);
	double tolerance() const;
	void setMaxIterations(int n);
	int maxIterations() const;

	using LogCallback = std::function<void(const QString&)>;
	void setLogCallback(LogCallback cb);

signals:
	void logMessage(const QString& msg);
	void solveProgress(const QString& step, int iteration, bool done);
	void solveFinished(bool success);

private:
	std::unique_ptr<ChemEngine::Flowsheet> m_flowsheet;
	std::unique_ptr<ChemEngine::COBIA::PropertyPackageManager> m_ppManager;
	std::unique_ptr<ChemEngine::COBIA::UnitOperationManager> m_unitOpMgr;
	LogCallback m_logCallback;
};
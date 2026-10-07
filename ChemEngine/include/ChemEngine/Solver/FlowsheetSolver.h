#pragma once

#include "ChemEngine/Interfaces/ISimulationObject.h"
#include <vector>
#include <string>
#include <map>
#include <functional>

namespace ChemEngine {

	class Flowsheet;

	struct SolverConnection
	{
		std::wstring fromObject;
		std::wstring toObject;
		std::wstring fromPort;
		std::wstring toPort;
	};

	struct TearStream
	{
		std::wstring streamName;
		std::wstring fromObject;
	};

	class FlowsheetSolver
	{
	public:
		FlowsheetSolver();
		virtual ~FlowsheetSolver();

		void setFlowsheet(Flowsheet* fs) { m_flowsheet = fs; }
		Flowsheet* getFlowsheet() const { return m_flowsheet; }

		void setMaxIterations(int n) { m_maxIterations = n; }
		int getMaxIterations() const { return m_maxIterations; }

		void setMaxRecycleLoops(int n) { m_maxRecycleLoops = n; }
		int getMaxRecycleLoops() const { return m_maxRecycleLoops; }

		void setTolerance(double tol) { m_tolerance = tol; }
		double getTolerance() const { return m_tolerance; }

		bool solve();

		const std::vector<std::wstring>& getCalculationOrder() const { return m_calcOrder; }
		const std::vector<TearStream>& getTearStreams() const { return m_tearStreams; }

		using SolveCallback = std::function<void(const std::wstring&, int, bool)>;
		void setSolveCallback(SolveCallback cb) { m_solveCb = cb; }

	private:
		void buildGraph();
		bool topologicalSort();
		void detectCyclesAndTear();
		bool runSolveLoop();

		Flowsheet* m_flowsheet = nullptr;

		std::map<std::wstring, std::vector<std::wstring>> m_downstream;
		std::map<std::wstring, int> m_inDegree;

		int m_maxIterations = 100;
		int m_maxRecycleLoops = 50;
		double m_tolerance = 1e-6;

		std::vector<std::wstring> m_calcOrder;
		std::vector<TearStream> m_tearStreams;
		SolveCallback m_solveCb;
	};

}
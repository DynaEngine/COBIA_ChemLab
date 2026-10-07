#include "ChemEngine/Solver/FlowsheetSolver.h"
#include "ChemEngine/Flowsheet/Flowsheet.h"
#include <deque>
#include <set>
#include <algorithm>

namespace ChemEngine {

	FlowsheetSolver::FlowsheetSolver() = default;
	FlowsheetSolver::~FlowsheetSolver() = default;

	void FlowsheetSolver::buildGraph()
	{
		m_downstream.clear();
		m_inDegree.clear();

		if (!m_flowsheet) return;

		const auto& objects = m_flowsheet->getObjects();

		for (const auto& info : objects)
		{
			m_inDegree[info.name] = 0;
			m_downstream[info.name];
		}

		for (const auto& info : objects)
		{
			if (!info.object) continue;

			auto targets = info.object->getDownstreamObjectNames();
			for (const auto& target : targets)
			{
				m_downstream[info.name].push_back(target);
				if (m_inDegree.count(target))
					m_inDegree[target]++;
			}
		}
	}

	bool FlowsheetSolver::topologicalSort()
	{
		m_calcOrder.clear();

		buildGraph();

		const auto& objects = m_flowsheet->getObjects();
		if (objects.empty()) return true;

		std::map<std::wstring, int> degree = m_inDegree;
		std::deque<std::wstring> queue;

		for (const auto& [name, d] : degree)
			if (d == 0) queue.push_back(name);

		while (!queue.empty())
		{
			std::wstring current = std::move(queue.front());
			queue.pop_front();
			m_calcOrder.push_back(current);

			for (const auto& neighbor : m_downstream[current])
			{
				if (--degree[neighbor] == 0)
					queue.push_back(neighbor);
			}
		}

		if (m_calcOrder.size() < objects.size())
		{
			m_calcOrder.clear();
			return false;
		}
		return true;
	}

	void FlowsheetSolver::detectCyclesAndTear()
	{
		m_tearStreams.clear();

		const auto& objects = m_flowsheet->getObjects();

		auto degree = m_inDegree;
		std::set<std::wstring> remaining;
		for (const auto& info : objects)
			remaining.insert(info.name);

		std::deque<std::wstring> queue;
		for (const auto& info : objects)
			if (degree[info.name] == 0)
				queue.push_back(info.name);

		while (!queue.empty())
		{
			std::wstring current = std::move(queue.front());
			queue.pop_front();
			remaining.erase(current);

			for (const auto& neighbor : m_downstream[current])
			{
				if (--degree[neighbor] == 0)
					queue.push_back(neighbor);
			}
		}

		if (remaining.empty()) return;

		std::set<std::wstring> torn;
		for (const auto& name : remaining)
		{
			for (const auto& target : m_downstream[name])
			{
				if (remaining.count(target) && !torn.count(target))
				{
					m_tearStreams.push_back({target, name});
					torn.insert(target);
					degree[target]--;
					if (degree[target] <= 0)
						queue.push_back(target);
				}
			}
		}

		m_calcOrder.clear();
		for (const auto& [name, d] : degree)
			if (d == 0 && !torn.count(name))
				queue.push_back(name);
			else if (d == 0)
				queue.push_back(name);

		while (!queue.empty())
		{
			std::wstring current = std::move(queue.front());
			queue.pop_front();
			m_calcOrder.push_back(current);

			for (const auto& neighbor : m_downstream[current])
			{
				bool isTorn = false;
				for (const auto& tear : m_tearStreams)
				{
					if (neighbor == tear.streamName && current == tear.fromObject)
					{
						isTorn = true;
						break;
					}
				}
				if (isTorn) continue;

				if (--degree[neighbor] == 0)
					queue.push_back(neighbor);
			}
		}

		for (const auto& tear : m_tearStreams)
		{
			if (std::find(m_calcOrder.begin(), m_calcOrder.end(), tear.streamName) == m_calcOrder.end())
				m_calcOrder.push_back(tear.streamName);
		}
	}

	bool FlowsheetSolver::runSolveLoop()
	{
		if (!m_flowsheet) return false;
		if (m_calcOrder.empty()) return true;

		bool hasCycles = !m_tearStreams.empty();
		int maxLoops = hasCycles ? m_maxRecycleLoops : 1;

		for (int loop = 0; loop < maxLoops; ++loop)
		{
			bool allCalculated = true;

			for (const auto& name : m_calcOrder)
			{
				auto obj = m_flowsheet->findObject(name);
				if (!obj)
				{
					allCalculated = false;
					continue;
				}

				if (m_solveCb) m_solveCb(name, loop, false);

				obj->setStatus(SimulationObjectStatus::Calculating);
				if (!obj->solve())
				{
					obj->setStatus(SimulationObjectStatus::Error);
					allCalculated = false;
				}
				else
				{
					obj->setStatus(SimulationObjectStatus::Calculated);
				}
			}

			if (!hasCycles)
			{
				if (m_solveCb) m_solveCb(L"", loop, true);
				return allCalculated;
			}

			if (allCalculated)
			{
				if (m_solveCb) m_solveCb(L"", loop, true);
				return true;
			}
		}

		if (m_solveCb) m_solveCb(L"", maxLoops - 1, true);
		return false;
	}

	bool FlowsheetSolver::solve()
	{
		if (!m_flowsheet) return false;
		if (m_flowsheet->getObjects().empty()) return true;

		buildGraph();

		bool acyclic = topologicalSort();

		if (acyclic)
		{
			return runSolveLoop();
		}

		detectCyclesAndTear();
		return runSolveLoop();
	}

}
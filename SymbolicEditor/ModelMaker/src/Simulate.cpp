#include "MainWindow.h"
#include <sc/SCLib.h>
#include <sc/ISolutionBuffer.h>
#include <type_traits>
#include <chrono>
#include "textEditor/View.h"
#include "BaseView.h"
#include "ModelSettings.h"
#include "DataDrawer.h"
#include <td/Timer.h>
#include <dp/IDatabase.h>
#include <complex>
#include "ModelSettings.h"

using LogType = LogView::LogType;

template <template <typename> class SolverTemplate, typename T>
class SolutionBuffer : public sc::ISolutionBuffer
{
	static constexpr bool isComplex = std::is_same<T, td::cmplx>::value;

	const std::vector<ModelSettings::PlotDesc> &functions;
	const std::shared_ptr<LogView> logger;
	std::shared_ptr<Results> _results;
    SolverTemplate<T> *solver;

    dp::IDataSet* _pDS;
	std::chrono::steady_clock::time_point startTime;
	

	const td::String& modelName;
	std::vector<std::pair<double*, td::String>> _valuePtrAndName;
	int _indexOfTimeParameter;
    bool _timeDomain = false;
	double _precision;

	inline td::String addCmplxTag(const td::String& name, const bool& isComplex) {
		if (isComplex)
			return td::String(name) += ".imag";
		else
			return td::String(name) += ".real";
	}

	inline td::String getCmplxString(td::cmplx number) {
		cnt::StringBuilderSmall str;
		str << number;
		td::String result;
		str.getString(result);
		return result;
	}


public:
    SolutionBuffer(const td::String &modelName, const std::shared_ptr<LogView> logger, std::shared_ptr<Results> resultTable, \
	const std::vector<ModelSettings::PlotDesc> &functions, SolverTemplate<T>* solver, bool initSucess)
    : functions(functions)
    , logger(logger)
    , solver(solver)
    , modelName(modelName)
    , startTime(std::chrono::steady_clock::now())
    , _results(resultTable)
    {
        
        if(!initSucess)
            return;
        
        auto iTime = solver->getITime();
        if (iTime)
            _timeDomain = true;
        else
            _timeDomain = false;
        
        T* _pSymbolValues = solver->getSymbolValuesPtr();
        
		cnt::SafeFullVector<td::INT4> outSymbols;
        solver->getOutSymbols(outSymbols);
	

        
        if (_timeDomain)
		_indexOfTimeParameter = solver->getParamIndex("t");
		if(_indexOfTimeParameter >= 0 && _timeDomain){
			if constexpr(isComplex)
				_valuePtrAndName.emplace_back(&(reinterpret_cast<T::value_type(&)[2]>(*(solver->getParamsPtr() + _indexOfTimeParameter))[0]), "t");
			else
				_valuePtrAndName.emplace_back(solver->getParamsPtr() + _indexOfTimeParameter, "t");
		}

		if (!_timeDomain){
			//_valuePtrAndName.emplace_back(&solver->getSolvedPrecision(), "eps"); ovo bi trebalo da bude
			_valuePtrAndName.emplace_back(&_precision, "eps");
		}

		for (const auto& index : outSymbols){
			if constexpr(isComplex){
				_valuePtrAndName.emplace_back(reinterpret_cast<T::value_type(&)[2]>(*(_pSymbolValues + index)), addCmplxTag(solver->getSymbolName(index), false));
				_valuePtrAndName.emplace_back(reinterpret_cast<T::value_type(&)[2]>(*(_pSymbolValues + index)) + 1, addCmplxTag(solver->getSymbolName(index), true));
			}
			else
				_valuePtrAndName.emplace_back(_pSymbolValues + index, solver->getSymbolName(index));
		}

		size_t columnCnt = _valuePtrAndName.size();
        _pDS = _results->getDataSet();
        if (_pDS) //Provjera da li je potrebno mijenjati kolone tabele
        {
            size_t nCols = _pDS->getNumberOfCols();
            if (nCols != columnCnt)
            {
                _pDS = nullptr;
            }
            else
            {
				for(int i = 0; i<columnCnt; ++i){
					if(_valuePtrAndName[i].second != _pDS->getColName(i)){
						_pDS = nullptr;
						break;
					}
				}
			}
		}
                         
        if (!_pDS)
        {
            //create DataSet
            _pDS = dp::createConnectionlessDataSet(dp::IDataSet::Size::Medium);
            size_t nCols = columnCnt;
            
			dp::DSColumns cols(_pDS->allocBindColumns(nCols));

			for(int i = 0; i<columnCnt; ++i)
				cols << _valuePtrAndName[i].second.c_str() << td::real8;

            _pDS->execute();
            
            
            if (_timeDomain)//????
            {
                td::LUINT8 lVal(1);
                td::Variant userInfo(lVal);
                _pDS->setUserInfo(userInfo);
            }
            else
            {
                td::LUINT8 lVal(0);
                td::Variant userInfo(lVal);
                _pDS->setUserInfo(userInfo);
            }
        }
        else
        {
            _pDS->removeAll();
        }
	}

	void put() override
	{
		
		auto& row = _pDS->getEmptyRow();
		
		if(!_timeDomain)
			_precision = solver->getSolvedPrecision();

		for(int i = 0; i<_valuePtrAndName.size(); ++i)
			row[i] = *(_valuePtrAndName[i].first);
		
		_pDS->push_back();
		
	}

	void finalize(Status status) override
	{

		cnt::StringBuilderSmall str;
		td::String log;
		if (status == sc::ISolutionBuffer::Status::Error) {
			str << "Simulation stopped with error: " << solver->getLastErrorStr();
			str.getString(log);
			logger->appendLog(log, LogType::Error);
			//_results->show(_pDS); EB: ovo ce u 99% slucajeva prouzrokovati krahiranje tako da stvarno ne moze
			return; 
		}
		else {
			std::chrono::duration<double> d = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() - startTime);
			str << "Simulation finished sucessfully in " << d.count() << " seconds.";
			_results->show(_pDS);
			str.getString(log);
			logger->appendLog(log, LogType::Info);
		}




		cnt::PushBackVector<td::String> names;
		cnt::PushBackVector<cnt::PushBackVector<double>> data;
		for(const auto &valAndName : _valuePtrAndName)
			names.push_back(valAndName.second);	

		_pDS->getFPValues(names, data);

		std::map<td::String, double*> NameAndDataPtr;
		for(int i = 0; i<_valuePtrAndName.size(); ++i){
			if(isComplex && _valuePtrAndName[i].second.cCompare("eps") == 0)
			{
				//sve varijable koje korisnik navede dodaje im se .real ili .imag tag. Sa obzirom da u tabeli eps se zove eps a ne eps.real onda se mora dodati ovdje tag
				NameAndDataPtr[addCmplxTag(_valuePtrAndName[i].second, false)] = &(data[i][0]);
			}
			else
				NameAndDataPtr[_valuePtrAndName[i].second] = &(data[i][0]);
		}

		size_t size = (data.size() > 0) ? data[0].size() : 0;

		for(int i = 0; i<functions.size(); ++i){
			if(/*provjeriti da li se # ikad koristi u funkcijama*/ true){
				double *ptr = new double[size];
				if constexpr (isComplex)
					NameAndDataPtr["#.real"] = ptr;
				else
					NameAndDataPtr["#"] = ptr;
				for(int j = 0; j<size; ++j){
					ptr[j] = j;
				}

				break;
			}
		}

		

		
		decltype(NameAndDataPtr)::iterator itX, itY, end = NameAndDataPtr.end();
		td::String xAxis, yAxis;
		int paramIntY;
		int dataSize;
		int cnt = 0;

		for(const ModelSettings::PlotDesc &funD : functions){//zahtjevi za crtane grafova/tabela
			++cnt;
			DataDraw::PlotDesc plot;
			plot.type = funD.type;

			if constexpr (!isComplex) {
				if (funD.xComplex)
					continue;
				plot.xName = funD.xAxis;
			}
			else {
				plot.xName = addCmplxTag(funD.xAxis, funD.xComplex);
			}

			itX = NameAndDataPtr.find(plot.xName);
			plot.xName = funD.xAxis;

			if(itX != end){
				plot.x = itX->second;
				for(const ModelSettings::PlotDesc::FunctionDesc &yFuns: funD.yAxis){
					if(yFuns.name.getFirstNonWhiteSpacePosition() == -1)
						continue;
					paramIntY = solver->getParamIndex(yFuns.name.c_str());

					if constexpr(!isComplex){
						if(yFuns.complex)
							continue;
						itY = NameAndDataPtr.find(yFuns.name);
					}
					else{
						yAxis = addCmplxTag(yFuns.name, yFuns.complex);
						itY = NameAndDataPtr.find(yAxis);
					}

					
					double *data = nullptr;
					unsigned int dataSize = 0;


					if(itY != end){
						dataSize = size;
						data = itY->second;
						if(dataSize < 2)
							continue;
					}
					else if(paramIntY >= 0){
						dataSize = 1;
						if constexpr(!isComplex)
							data = solver->getParamsPtr() + paramIntY;
						else{
							data = (yFuns.complex) ? reinterpret_cast<T::value_type(&)[2]>(*(solver->getParamsPtr() + paramIntY)) + 1: reinterpret_cast<T::value_type(&)[2]>(*(solver->getParamsPtr() + paramIntY));

						}
					}
					else{
						td::String log("Discarding plot ");
						log += yFuns.name;
						logger->appendLog(log, LogType::Warning);
						continue;
					}
					
					//if(paramIntY >= 0 || itY != end)
						plot.yAxis.emplace_back(std::move(yFuns), dataSize, data);
					
				}
				td::String nameAndType;
				nameAndType.format("%s-%d", modelName.c_str(), cnt);
				GlobalEvents::getMainWindowPtr()->getDataDrawer()->addData(nameAndType, plot);
				continue;
			}

		
			td::String log("Discarding plot ");
			log += funD.xAxis;
			logger->appendLog(log, LogType::Warning);


		}


		if(NameAndDataPtr.contains("#"));
			delete[] NameAndDataPtr["#"];
	
	}
};



enum class EquationTypes { NR, DAE, ODE, WLS };

int MainWindow::simulate(ViewForTab *tab)
{	
	const auto& logView = tab->getLog();
	bool error;
	const ModelNode &model = tab->getModelNode(error);
	if(error){
		logView->appendLog(tr("simulationFailed"), LogType::Error);
		return -1;
	}
	
	bool isComplex;
	if(auto it = model._attribs.find("domain"); it == model._attribs.end())
		isComplex = false;
	else{
		isComplex = (it->second.cCompareNoCase("cmplx") == 0 || it->second.cCompareNoCase("complex") == 0) ? true : false;
		if(!isComplex && it->second.cCompareNoCase("real") != 0)
			logView->appendLog("model attribute 'domain' is not 'real' or 'complex', assuming real", LogType::Warning);
	}


    const char* testFileName = nullptr;
    
	EquationTypes equationType;
	if(auto it = model._attribs.find("type"); it == model._attribs.end()){
		logView->appendLog("model does not have attribute 'type', assuming 'NR'(nonlinear equations)", LogType::Warning);
		equationType = EquationTypes::NR;
	}else{
		if(it->second.cCompareNoCase("NR") == 0)
			equationType = EquationTypes::NR;
		else if(it->second.cCompareNoCase("DAE") == 0)
			equationType = EquationTypes::DAE;
		else if(it->second.cCompareNoCase("ODE") == 0)
			equationType = EquationTypes::ODE;
		else if(it->second.cCompareNoCase("WLS") == 0)
			equationType = EquationTypes::WLS;
		else{
			td::String msg("attribute 'type' has unsupported value ");
			msg += it->second;
			msg += ", simulation stopping";
			logView->appendLog(msg, LogType::Error);
			logView->appendLog("Supported types are: NR, DAE, ODE, WLS", LogType::Info);
			return -2;
		}
	}


	auto simSettings = tab->getSimulationSettings();

	const auto &funcs = tab->getPlots();

	td::String modelStr;
	model.printNodeToString(modelStr);	

	
#ifdef MU_DEBUG
#ifdef MU_MACOS
	td::String mPath = "/Volumes/RAMDisk/modelForSimulator.xml";
	model.printNode(mPath); //u slucaju problema da se moze pogledati finalni generisani model koji je poslan solveru
#else
	td::String mPath = "./modelForSimulator.xml";
	model.printNode(mPath);
#endif
#endif

	//mPath = "/home/bots/Desktop/model-Solver/real/ACGenWith1Load_WithLimit_Comp_DAE_RK4.xml";
	//mPath = "/home/bots/Desktop/model-Solver/real/PF_WLimits.xml";

	auto initSimulation = [&](auto pSolver)
    {
		td::String err;
		bool initSucess = false;
		if constexpr (std::is_same<decltype(pSolver), sc::IDblSolver*>::value)
        {
			switch (equationType) {
			case EquationTypes::NR:
                pSolver = sc::createDblNRSolver(simSettings.maxIterations);
                pSolver->setReportResultsPerNRIteration(sc::ReportRate::InitialAndIterationsAndFinal);
				break;
			case EquationTypes::ODE:
				pSolver = sc::createDblODESolver();
				break;
			case EquationTypes::DAE:
				pSolver = sc::createDblDAESolver(simSettings.maxIterations);
				break;
			case EquationTypes::WLS:
                pSolver = sc::createDblWLSSolver(simSettings.maxIterations);
                pSolver->setReportResultsPerNRIteration(sc::ReportRate::InitialAndIterationsAndFinal);
				break;
			}
            initSucess = pSolver->init(modelStr, sc::IDblSolver::SourceType::Memory);
			//initSucess = s->init(mPath, sc::IDblSolver::SourceType::File);
		}
		else if constexpr (std::is_same<decltype(pSolver), sc::ICmplxSolver*>::value)
        {
			switch (equationType){
            case EquationTypes::NR:
                pSolver = sc::createCmplxNRSolver(simSettings.maxIterations);
                pSolver->setReportResultsPerNRIteration(sc::ReportRate::InitialAndIterationsAndFinal);
				break;
			case EquationTypes::ODE:
                    logView->appendLog("Complex ODE solver will be implemented in the future", LogType::Error);
                    return -1;
					//s = sc::createCmplxODESolver();
				break;
			case EquationTypes::DAE:
				//s = sc::createCmplxDAESolver(maxIter);
                logView->appendLog("Complex DAE solver will be implemented in the future", LogType::Error);
                return -1;
			case EquationTypes::WLS:
                pSolver = sc::createCmplxWLSSolver(simSettings.maxIterations);
                pSolver->setReportResultsPerNRIteration(sc::ReportRate::InitialAndIterationsAndFinal);
				break;
			}
			initSucess = pSolver->init(modelStr, sc::ICmplxSolver::SourceType::Memory);
		}
	else{
		static_assert(pSolver==nullptr, "wrong pointer type for initSimulation");
		return 0;
	}


		bool useAutoFuncs = false;
		std::vector<ModelSettings::PlotDesc> autoFuncs;
		//int size = (equationType == EquationTypes::NR || equationType == EquationTypes::WLS) ? 1 : 1 + std::abs(startTime - endTime) / stepTime;

		if (funcs.empty() && initSucess && simSettings.useAutoFuncs) {
			useAutoFuncs = true;
			cnt::SafeFullVector<td::INT4> symbs;

			pSolver->getOutSymbols(symbs);
			int timeIndex = pSolver->getParamIndex("t");
			
			if (timeIndex < 0){
				autoFuncs.emplace_back("Accuracy", "#");
				for (const auto& symIndex : symbs){
					autoFuncs.back().yAxis.emplace_back(pSolver->getSymbolName(symIndex));
					if(autoFuncs.size() >= 16)
						break;
				}
			}
			else{
				autoFuncs.emplace_back("", "t");
				for (const auto& symIndex : symbs){
					autoFuncs.back().yAxis.emplace_back(pSolver->getSymbolName(symIndex));
					if(autoFuncs.size() >= 16)
						break;
				}
			}
			if (autoFuncs.empty())
				logView->appendLog("No out variables found. You must add 'out=true' attribute to a single variable or the variable declaration tag for them to be visible in results", LogType::Warning);
		}
        auto pResults = tab->getResults();
        td::Timer timer;
        timer.start();
		SolutionBuffer buffer(tab->getName(), logView, pResults, useAutoFuncs ? autoFuncs : funcs, pSolver, initSucess);
        auto initDuration = timer.getDurationInSeconds();
        td::String strTmp;
        strTmp.format("Preparation time:  %.3lf seconds", initDuration);
        logView->appendLog(strTmp, LogType::Info);

		if(initSucess)
        {
			if(equationType == EquationTypes::DAE || equationType == EquationTypes::ODE)
            {
                pSolver->setOutFilter(sc::OutFilter::InitialValues);
                pSolver->solve(simSettings.startTime, simSettings.stepTime, simSettings.endTime, &buffer, 0);
            }
			if(equationType == EquationTypes::NR || equationType == EquationTypes::WLS)
            {
                pSolver->setConsumer(&buffer);
				auto sol = pSolver->solve();
                
                switch (sol)
                {
                    case sc::Solution::OK:
                    {
                        int noOfIters = pSolver->getIterationsNo();
                        double eps = pSolver->getSolvedPrecision();
                        strTmp.format("Solver in %d iterations with precision eps %g", noOfIters, eps);
                        logView->appendLog(strTmp, LogType::Info);
                    }
                        break;
                    case sc::Solution::FactorizationError:
                        logView->appendLog("Factorization error!", LogType::Error);
                        break;
                    case sc::Solution::SolutionError:
                        logView->appendLog("Solution error!", LogType::Error);
                        break;
                    case sc::Solution::Oscillation:
                        logView->appendLog("Osclillation detected!", LogType::Error);
                        break;
                    case sc::Solution::MaximumIteration:
                        logView->appendLog("MaximumIteration error!", LogType::Error);
                        break;
                    case sc::Solution::SubmodelFailed:
                        logView->appendLog("Solving Submodel Failed!", LogType::Error);
                        break;
                }
                
                if (sol != sc::Solution::OK)
                {
                    
                    err = pSolver->getLastErrorStr();
                    if (err.length() > 0)
                    {
                        logView->appendLog(err, LogType::Error);//EB: ovo se treba izbaciti ja mislim, jer u solver.finalize() se ispisuju greske tako da jedno od dvoje dode viska. A finalize se MORA pozvati a gresja se mora ispisivati i za DAE modele, ovdje je error checking samo za NR i WLS
                    }
                }
				
				if(equationType == EquationTypes::WLS){
					buffer.put();
					buffer.finalize(err.isNull() ? sc::ISolutionBuffer::Status::SuccessfullyCompleted : sc::ISolutionBuffer::Status::Error);
				}
			}
			return 0;
		}

		buffer.finalize(sc::ISolutionBuffer::Status::Error);

		return -1;

	};


	if (isComplex) {
		sc::ICmplxSolver* s = nullptr;
		initSimulation(s);
	}
	else {
		sc::IDblSolver* s = nullptr; 
		initSimulation(s);
	}
    
    return 0;

}


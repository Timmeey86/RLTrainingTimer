#pragma once

#include <string>
#include <bakkesmod/wrappers/cvarmanagerwrapper.h>
#include <configuration/control/TrainingProgramListConfigurationControl.h>
#include <training/control/TrainingProgramFlowControl.h>

namespace injection
{
	/** This class allows passing and executing training programs from another plugin. */
	class TrainingProgramInjector
	{
	public:
		explicit TrainingProgramInjector(
			std::shared_ptr<CVarManagerWrapper> cvarManager,
			std::shared_ptr<configuration::TrainingProgramListConfigurationControl> _programListControl,
			std::shared_ptr<training::TrainingProgramFlowControl> _programFlowControl
		);

		/** Registers the notifiers to be called by other plugins. */
		void registerNotifiers();

	private:
		/** Receives a training program from another plugin.
		 *
		 * \returns		the training program UUID.
		 */
		std::string receiveTrainingProgram(const std::vector<std::string>& params);

		/** Executes the training program with the given ID. */
		void executeTrainingProgram(const std::string& uuid) const;

		std::shared_ptr<CVarManagerWrapper> _cvarManager;
		std::shared_ptr<configuration::TrainingProgramListConfigurationControl> _programListControl;
		std::shared_ptr<training::TrainingProgramFlowControl> _programFlowControl;
		static const std::string InjectProgramNotifier;
		static const std::string RunProgramNotifier;
		static const std::string InjectAndRunProgramNotifier;
	};
}
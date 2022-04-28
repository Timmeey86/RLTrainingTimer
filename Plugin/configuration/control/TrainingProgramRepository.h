#pragma once

#include "../data/TrainingProgramData.h"

#include <DLLImportExport.h>

namespace configuration
{
	/** The job of this class is to write training program data to the file system and read it back from there. */
	class RLTT_IMPORT_EXPORT TrainingProgramRepository : public ITrainingProgramRepository
	{
	public:
		TrainingProgramRepository(const std::shared_ptr<GameWrapper>& gameWrapper);

		/** Stores the given training program list persistently. */
		void storeData(const TrainingProgramListData& data) override;
		/** Restores the training program list from the persistent location. */
		TrainingProgramListData restoreData() const override;

	private:
		std::filesystem::path _storagePath;
	};
}
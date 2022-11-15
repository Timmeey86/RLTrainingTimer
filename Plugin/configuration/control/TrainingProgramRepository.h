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

		/** Stores the given training program list persistently at the default location. */
		void storeData(const TrainingProgramListData& data) override;
		/** Stores the given training program list persistently at the specified location. */
		void storeData(const TrainingProgramListData& data, const std::filesystem::path &path) override;
		/** Restores the training program list from the default persistent location. */
		TrainingProgramListData restoreData() const override;
		/** Restores the training program list from the specified persistent location. */
		TrainingProgramListData restoreData(const std::filesystem::path &path) const override;

	private:
		std::filesystem::path _storagePath;
	};
}
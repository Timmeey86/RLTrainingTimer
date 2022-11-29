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

		/** Stores the given training program list at the default location in the file system. */
		void storeData(const TrainingProgramListData& data) override;
		/** Stores the given training program list at the specified location in the file system. */
		virtual void storeData(const TrainingProgramListData& data, const std::string& location) override;
		/** Restores the training program list from the default location in the file system. */
		TrainingProgramListData restoreData() const override;
		/** Restores the training program list from the specified location in the file system. */
		virtual TrainingProgramListData restoreData(const std::string& location) const override;
		/** Exports a single training program to the specified location on the file system. */
		virtual void exportSingleTrainingProgram(const TrainingProgramData& data, const std::string& location) override;
		/** Imports a single training program from the specified location on the file system. */
		virtual TrainingProgramData importSingleTrainingProgram(const std::string& location) const override;

	private:
		std::filesystem::path _storagePath;

		TrainingProgramListData restoreDataImpl(const std::filesystem::path& path) const;
		void storeDataImpl(const TrainingProgramListData& data, const std::filesystem::path& path);
	};
}
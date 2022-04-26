#include <pch.h>
#include "TrainingProgramConfigurationControl.h"

#include <core/kernel/InvalidValueException.h>
#include <core/kernel/IndexOutOfBoundsException.h>

#include <qol/vectorext.h>

namespace configuration
{
    TrainingProgramConfigurationControl::TrainingProgramConfigurationControl(std::shared_ptr<std::map<uint64_t, TrainingProgramData>> trainingProgramData)
        : _trainingProgramData{ std::move(trainingProgramData) }
    {
    }

    void TrainingProgramConfigurationControl::addEntry(uint64_t trainingProgramId, const TrainingProgramEntry& entry)
    {
        auto data = internalData(trainingProgramId);
        data->Entries.push_back(entry);
        data->Duration += entry.Duration;
    }

    void TrainingProgramConfigurationControl::removeEntry(uint64_t trainingProgramId, int position)
    {
        auto data = internalData(trainingProgramId);
        validatePosition(data, position, "position");

        data->Duration -= data->Entries.at(position).Duration;
        remove(data->Entries, position);
    }

    void TrainingProgramConfigurationControl::renameEntry(uint64_t trainingProgramId, int position, const std::string& newName)
    {
        auto data = internalData(trainingProgramId);
        validatePosition(data, position, "position");

        data->Entries.at(position).Name = newName;
    }

    void TrainingProgramConfigurationControl::changeEntryDuration(uint64_t trainingProgramId, int position, const std::chrono::milliseconds& newDuration)
    {
        auto data = internalData(trainingProgramId);
        validatePosition(data, position, "position");

        auto affectedEntry = data->Entries.at(position);
        data->Duration -= affectedEntry.Duration;
        data->Duration += newDuration;
        affectedEntry.Duration = newDuration;
    }

    void TrainingProgramConfigurationControl::swapEntries(uint64_t trainingProgramId, int firstPosition, int secondPosition)
    {
        auto data = internalData(trainingProgramId);
        validatePosition(data, firstPosition, "first position");
        validatePosition(data, secondPosition, "second position");

        std::swap(data->Entries[firstPosition], data->Entries[secondPosition]);
        // Duration will stay identical, no need to update it
    }

    void TrainingProgramConfigurationControl::renameProgram(uint64_t trainingProgramId, const std::string& newName)
    {
        auto data = internalData(trainingProgramId);
        data->Name = newName;
    }

    TrainingProgramData TrainingProgramConfigurationControl::getData(uint64_t trainingProgramId) const
    {
        return *internalData(trainingProgramId);
    }

    TrainingProgramData* TrainingProgramConfigurationControl::internalData(uint64_t trainingProgramId) const
    {
        if (_trainingProgramData->count(trainingProgramId) == 0)
        {
            throw Core::Kernel::InvalidValueException(
                "Configuration",
                "TrainingProgramConfigurationControl",
                "id",
                "The training program with the given ID does not exist.",
                std::to_string(trainingProgramId)
            );
        }
        return &(*_trainingProgramData)[trainingProgramId];
    }
    
    void TrainingProgramConfigurationControl::validatePosition(const TrainingProgramData* const data, int position, const std::string& variableName) const
    {
        if (position < 0 || position >= data->Entries.size())
        {
            throw Core::Kernel::IndexOutOfBoundsException(
                "Configuration",
                "TrainingProgramConfigurationControl",
                variableName,
                0,
                (int)data->Entries.size() - 1,
                position
            );
        }
    }
}
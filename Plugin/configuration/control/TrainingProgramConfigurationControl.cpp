#include <pch.h>
#include "TrainingProgramConfigurationControl.h"

template <typename T>
void remove(std::vector<T>& vec, size_t pos)
{
    auto it = vec.begin();
    std::advance(it, pos);
    vec.erase(it);
}

namespace configuration
{
    TrainingProgramConfigurationControl::TrainingProgramConfigurationControl(
        std::shared_ptr<std::map<std::string, TrainingProgramData>> trainingProgramData,
        std::function<void()> changeNotificationCallback)
        : _trainingProgramData{ std::move(trainingProgramData) }
        , _changeNotificationCallback{ std::move(changeNotificationCallback) }
    {
    }

    void TrainingProgramConfigurationControl::addEntry(const std::string& trainingProgramId, const TrainingProgramEntry& entry)
    {
        auto data = internalData(trainingProgramId);
        data->Entries.push_back(entry);
        if (entry.TimeMode == TrainingProgramCompletionMode::Timed)
        {
            data->Duration += entry.Duration;
        }

        _changeNotificationCallback();
    }

    void TrainingProgramConfigurationControl::removeEntry(const std::string& trainingProgramId, int position)
    {
        auto data = internalData(trainingProgramId);
        validatePosition(data, position, "position");

        auto& affectedEntry = data->Entries.at(position);
        if (affectedEntry.TimeMode == TrainingProgramCompletionMode::Timed)
        {
            data->Duration -= affectedEntry.Duration;
        }
        remove(data->Entries, position);

        _changeNotificationCallback();
    }

    void TrainingProgramConfigurationControl::renameEntry(const std::string& trainingProgramId, int position, const std::string& newName)
    {
        auto data = internalData(trainingProgramId);
        validatePosition(data, position, "position");

        data->Entries.at(position).Name = newName;

        _changeNotificationCallback();
    }

    void TrainingProgramConfigurationControl::changeEntryDuration(const std::string& trainingProgramId, int position, const std::chrono::milliseconds& newDuration)
    {
        auto data = internalData(trainingProgramId);
        validatePosition(data, position, "position");

        auto& affectedEntry = data->Entries.at(position);
        if (affectedEntry.TimeMode == TrainingProgramCompletionMode::Timed)
        {
            data->Duration -= affectedEntry.Duration;
            data->Duration += newDuration;
        }
        // Store the duration in either case, in case the time mode is changed back to Timed
        affectedEntry.Duration = newDuration;

        _changeNotificationCallback();
    }

    void TrainingProgramConfigurationControl::swapEntries(const std::string& trainingProgramId, int firstPosition, int secondPosition)
    {
        auto data = internalData(trainingProgramId);
        validatePosition(data, firstPosition, "first position");
        validatePosition(data, secondPosition, "second position");

        std::swap(data->Entries[firstPosition], data->Entries[secondPosition]);
        // Duration will stay identical, no need to update it

        _changeNotificationCallback();
    }

    void TrainingProgramConfigurationControl::changeEntryType(const std::string& trainingProgramId, int position, TrainingProgramEntryType type)
    {
        auto data = internalData(trainingProgramId);
        validatePosition(data, position, "position");

        if (type < TrainingProgramEntryType::Unspecified || type > TrainingProgramEntryType::WorkshopMap)
        {
            throw std::runtime_error(fmt::format("Value {} is not valid for enum TrainingProgramEntryType", (int)type));
        }

        auto& affectedEntry = data->Entries.at(position);
        affectedEntry.Type = type;

        _changeNotificationCallback();
    }

    void TrainingProgramConfigurationControl::changeEntryCompletionMode(const std::string& trainingProgramId, int position, TrainingProgramCompletionMode CompletionMode)
    {
        auto data = internalData(trainingProgramId);
        validatePosition(data, position, "position");

        auto& affectedEntry = data->Entries.at(position);
        if (affectedEntry.TimeMode == TrainingProgramCompletionMode::Timed && CompletionMode != TrainingProgramCompletionMode::Timed)
        {
            // We don't know how long this entry will take, therefore we need to substract the duration from the total one
            data->Duration -= affectedEntry.Duration;
        }
        else if (affectedEntry.TimeMode != TrainingProgramCompletionMode::Timed && CompletionMode == TrainingProgramCompletionMode::Timed)
        {
            // Add the duration, so switching back and forth between timed and something else will not break the result
            data->Duration += affectedEntry.Duration;
        }
        affectedEntry.TimeMode = CompletionMode;

        _changeNotificationCallback();
    }

    void TrainingProgramConfigurationControl::changeTrainingPackCode(const std::string& trainingProgramId, int position, const std::string& trainingPackCode)
    {
        auto data = internalData(trainingProgramId);
        validatePosition(data, position, "position");

        data->Entries.at(position).TrainingPackCode = trainingPackCode;

        _changeNotificationCallback();
    }

    void TrainingProgramConfigurationControl::changeWorkshopMapPath(const std::string& trainingProgramId, int position, const std::string& workshopMapPath)
    {
        auto data = internalData(trainingProgramId);
        validatePosition(data, position, "position");

        data->Entries.at(position).WorkshopMapPath = workshopMapPath;

        _changeNotificationCallback();
    }

    void TrainingProgramConfigurationControl::renameProgram(const std::string& trainingProgramId, const std::string& newName)
    {
        auto data = internalData(trainingProgramId);
        data->Name = newName;

        _changeNotificationCallback();
    }

    TrainingProgramData TrainingProgramConfigurationControl::getData(const std::string& trainingProgramId) const
    {
        return *internalData(trainingProgramId);
    }

    TrainingProgramData* TrainingProgramConfigurationControl::internalData(const std::string& trainingProgramId) const
    {
        if (_trainingProgramData->count(trainingProgramId) == 0)
        {
            throw std::runtime_error(fmt::format("There is no training program with ID {}", trainingProgramId));
        }
        return &(*_trainingProgramData)[trainingProgramId];
    }
    
    void TrainingProgramConfigurationControl::validatePosition(const TrainingProgramData* const data, int position, const std::string& variableName) const
    {
        if (position < 0 || position >= data->Entries.size())
        {
            throw std::runtime_error(fmt::format("{} {} is invalid: Should be between 0 and {}", variableName, position, (int)data->Entries.size() - 1));
        }
    }
}
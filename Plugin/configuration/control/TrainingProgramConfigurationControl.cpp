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
        std::shared_ptr<std::map<uint64_t, TrainingProgramData>> trainingProgramData,
        std::function<void()> changeNotificationCallback)
        : _trainingProgramData{ std::move(trainingProgramData) }
        , _changeNotificationCallback{ std::move(changeNotificationCallback) }
    {
    }

    void TrainingProgramConfigurationControl::addEntry(uint64_t trainingProgramId, const TrainingProgramEntry& entry)
    {
        auto data = internalData(trainingProgramId);
        data->Entries.push_back(entry);
        data->Duration += entry.Duration;

        _changeNotificationCallback();
    }

    void TrainingProgramConfigurationControl::removeEntry(uint64_t trainingProgramId, int position)
    {
        auto data = internalData(trainingProgramId);
        validatePosition(data, position, "position");

        data->Duration -= data->Entries.at(position).Duration;
        remove(data->Entries, position);

        _changeNotificationCallback();
    }

    void TrainingProgramConfigurationControl::renameEntry(uint64_t trainingProgramId, int position, const std::string& newName)
    {
        auto data = internalData(trainingProgramId);
        validatePosition(data, position, "position");

        data->Entries.at(position).Name = newName;

        _changeNotificationCallback();
    }

    void TrainingProgramConfigurationControl::changeEntryDuration(uint64_t trainingProgramId, int position, const std::chrono::milliseconds& newDuration)
    {
        auto data = internalData(trainingProgramId);
        validatePosition(data, position, "position");

        auto& affectedEntry = data->Entries.at(position);
        data->Duration -= affectedEntry.Duration;
        data->Duration += newDuration;
        affectedEntry.Duration = newDuration;

        _changeNotificationCallback();
    }

    void TrainingProgramConfigurationControl::swapEntries(uint64_t trainingProgramId, int firstPosition, int secondPosition)
    {
        auto data = internalData(trainingProgramId);
        validatePosition(data, firstPosition, "first position");
        validatePosition(data, secondPosition, "second position");

        std::swap(data->Entries[firstPosition], data->Entries[secondPosition]);
        // Duration will stay identical, no need to update it

        _changeNotificationCallback();
    }

    void TrainingProgramConfigurationControl::changeEntryType(uint64_t trainingProgramId, int position, TrainingProgramEntryType type)
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

    void TrainingProgramConfigurationControl::changeTrainingPackCode(uint64_t trainingProgramId, int position, const std::string& trainingPackCode)
    {
        auto data = internalData(trainingProgramId);
        validatePosition(data, position, "position");

        data->Entries.at(position).TrainingPackCode = trainingPackCode;

        _changeNotificationCallback();
    }

    void TrainingProgramConfigurationControl::changeWorkshopMapPath(uint64_t trainingProgramId, int position, const std::string& workshopMapPath)
    {
        auto data = internalData(trainingProgramId);
        validatePosition(data, position, "position");

        data->Entries.at(position).WorkshopMapPath = workshopMapPath;

        _changeNotificationCallback();
    }

    void TrainingProgramConfigurationControl::renameProgram(uint64_t trainingProgramId, const std::string& newName)
    {
        auto data = internalData(trainingProgramId);
        data->Name = newName;

        _changeNotificationCallback();
    }

    TrainingProgramData TrainingProgramConfigurationControl::getData(uint64_t trainingProgramId) const
    {
        return *internalData(trainingProgramId);
    }

    TrainingProgramData* TrainingProgramConfigurationControl::internalData(uint64_t trainingProgramId) const
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
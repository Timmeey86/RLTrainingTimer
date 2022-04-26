#include <pch.h>
#include "TrainingProgramListConfigurationControl.h"

#include <core/kernel/InvalidValueException.h>

#include <qol/vectorext.h>

namespace configuration
{
    TrainingProgramListConfigurationControl::TrainingProgramListConfigurationControl(std::shared_ptr<std::map<uint64_t, TrainingProgramData>> trainingProgramData)
        : _trainingProgramData{ std::move(trainingProgramData) }
    {

    }

    uint64_t TrainingProgramListConfigurationControl::addTrainingProgram()
    {
        // Since _trainingProgramData is a map ordered by keys, we can retreive a new ID by adding 1 to the last element (which is guaranteed to have the highest key)
        auto newId = _trainingProgramData->rbegin()->first + 1;

        auto data = TrainingProgramData{ };
        data.Id = newId;
        data.Name = "New Training Program";
        data.Duration = std::chrono::milliseconds(0);

        _trainingProgramData->emplace(newId, data);
        _trainingProgramOrder.push_back(newId);

        return newId;        
    }


    void TrainingProgramListConfigurationControl::removeTrainingProgram(uint64_t trainingProgramId)
    {
        ensureIdIsKnown(trainingProgramId, "training program ID");

        removeOne(_trainingProgramOrder, trainingProgramId);
        _trainingProgramData->erase(trainingProgramId);
    }

    void TrainingProgramListConfigurationControl::swapTrainingPrograms(uint64_t firstProgramId, uint64_t secondProgramId)
    {
        ensureIdIsKnown(firstProgramId, "first training program ID");
        ensureIdIsKnown(secondProgramId, "second training program ID");

        auto firstIter = std::find(_trainingProgramOrder.begin(), _trainingProgramOrder.end(), firstProgramId);
        auto secondIter = std::find(_trainingProgramOrder.begin(), _trainingProgramOrder.end(), secondProgramId);
        if (firstIter != _trainingProgramOrder.end() && secondIter != _trainingProgramOrder.end())
        {
            std::iter_swap(firstIter, secondIter);
        }
        else
        {
            // This should not be possible unless ensureIdIsKnown has an error
            throw std::runtime_error("Training Program List is inconsistent");
        }
    }

    /** Provides a copy of the training program list data (e.g. for display). */
    TrainingProgramListData TrainingProgramListConfigurationControl::getTrainingProgramList() const
    {
        TrainingProgramListData data;
        data.TrainingProgramOrder = std::vector<uint64_t>(_trainingProgramOrder);
        for (auto& [id, programData]: *_trainingProgramData)
        {
            data.TrainingProgramData.emplace(id, programData);
        }
        return data;
    }

    /** Provides a copy of data of a single training program (e.g. for display). */
    TrainingProgramData TrainingProgramListConfigurationControl::getTrainingProgramData(uint64_t trainingProgramId) const
    {
        ensureIdIsKnown(trainingProgramId, "training program ID");
        return (*_trainingProgramData)[trainingProgramId];
    }

    /** Overrides any internal data. Use this only for restoring a saved state. */
    void TrainingProgramListConfigurationControl::restoreData(const TrainingProgramListData& data)
    {
        _trainingProgramOrder = data.TrainingProgramOrder;
        for (auto [id, programData] : data.TrainingProgramData)
        {
            _trainingProgramData->emplace(id, programData);
        }
    }

    void TrainingProgramListConfigurationControl::ensureIdDoesntExist(uint64_t trainingProgramId) const
    {
        if (_trainingProgramData->count(trainingProgramId) > 0)
        {
            throw Core::Kernel::InvalidValueException(
                "Configuration",
                "TrainingProgramListConfigurationControl",
                "training program ID",
                "There is already a program with such an ID.",
                std::to_string(trainingProgramId)
            );
        }
    }
    void TrainingProgramListConfigurationControl::ensureIdIsKnown(uint64_t trainingProgramId, const std::string& parameterName) const
    {
        if (_trainingProgramData->count(trainingProgramId) == 0)
        {
            throw Core::Kernel::InvalidValueException(
                "Configuration",
                "TrainingProgramListConfigurationControl",
                parameterName,
                "There is no training program with such an ID.",
                std::to_string(trainingProgramId)
            );
        }
    }
}
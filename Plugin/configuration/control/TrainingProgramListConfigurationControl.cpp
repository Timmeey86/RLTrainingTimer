#include <pch.h>
#include "TrainingProgramListConfigurationControl.h"

template <typename T>
bool removeOne(std::vector<T>& vec, const T& value)
{
    auto it = std::find(vec.begin(), vec.end(), value);
    if (it != vec.end()) {
        vec.erase(it);
        return true;
    }
    return false;
}

namespace configuration
{
    TrainingProgramListConfigurationControl::TrainingProgramListConfigurationControl(std::shared_ptr<std::map<uint64_t, TrainingProgramData>> trainingProgramData)
        : _trainingProgramData{ std::move(trainingProgramData) }
    {

    }

    void TrainingProgramListConfigurationControl::registerTrainingProgramListReceiver(std::shared_ptr<ITrainingProgramListReceiver> receiver)
    {
        _receivers.push_back(receiver);
    }

    uint64_t TrainingProgramListConfigurationControl::addTrainingProgram()
    {
        // Since _trainingProgramData is a map ordered by keys, we can retreive a new ID by adding 1 to the last element (which is guaranteed to have the highest key)
        uint64_t newId;
        if (_trainingProgramData->empty())
        {
            newId = 0;
        }
        else
        {
            newId = _trainingProgramData->rbegin()->first + 1;
        }

        auto data = TrainingProgramData{ };
        data.Id = newId;
        data.Name = "New Training Program";
        data.Duration = std::chrono::milliseconds(0);

        _trainingProgramData->emplace(newId, data);
        _trainingProgramOrder.push_back(newId);

        notifyReceivers();

        return newId;        
    }


    void TrainingProgramListConfigurationControl::removeTrainingProgram(uint64_t trainingProgramId)
    {
        ensureIdIsKnown(trainingProgramId, "training program ID");

        removeOne(_trainingProgramOrder, trainingProgramId);
        _trainingProgramData->erase(trainingProgramId);

        notifyReceivers();
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
            notifyReceivers();
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

        notifyReceivers();
    }

    void TrainingProgramListConfigurationControl::notifyReceivers()
    {
        auto listData = getTrainingProgramList();
        for (const auto& receiver : _receivers)
        {
            receiver->receiveListData(listData);
        }
    }

    void TrainingProgramListConfigurationControl::ensureIdDoesntExist(uint64_t trainingProgramId) const
    {
        if (_trainingProgramData->count(trainingProgramId) > 0)
        {
            throw std::runtime_error(fmt::format("There is already a training program with ID {}", trainingProgramId));
        }
    }
    void TrainingProgramListConfigurationControl::ensureIdIsKnown(uint64_t trainingProgramId, const std::string& parameterName) const
    {
        if (_trainingProgramData->count(trainingProgramId) == 0)
        {
            throw std::runtime_error(fmt::format("There is no training program with ID {} (parameter {})", trainingProgramId, parameterName));
        }
    }
}
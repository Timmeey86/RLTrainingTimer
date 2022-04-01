#include <pch.h>
#include "TrainingProgramList.h"
#include "../events/TrainingProgramEvents.h"

#include <core/kernel/InvalidValueException.h>

#include <qol/vectorext.h>

namespace Core::Configuration::Domain
{
    std::shared_ptr<Kernel::DomainEvent> TrainingProgramList::addTrainingProgram(uint64_t trainingProgramId)
    {
        ensureIdDoesntExist(trainingProgramId);

        _trainingProgramOrder.push_back(trainingProgramId);
        _trainingPrograms.emplace(trainingProgramId, std::make_shared<TrainingProgram>(trainingProgramId));

        auto eventData = std::make_shared<Events::TrainingProgramAddedEvent>();
        eventData->TrainingProgramId = trainingProgramId;
        return eventData;
    }

    std::shared_ptr<Kernel::DomainEvent> TrainingProgramList::removeTrainingProgram(uint64_t trainingProgramId)
    {
        ensureIdIsKnown(trainingProgramId, "training program ID");

        removeOne(_trainingProgramOrder, trainingProgramId);
        _trainingPrograms.erase(trainingProgramId); // this will delete the program, unless anyone else references it.

        auto eventData = std::make_shared<Events::TrainingProgramRemovedEvent>();
        eventData->TrainingProgramId = trainingProgramId;
        return eventData;
    }
    std::shared_ptr<Kernel::DomainEvent> TrainingProgramList::renameTrainingProgram(uint64_t trainingProgramId, const std::string& newName)
    {
        ensureIdIsKnown(trainingProgramId, "training program ID");

        _trainingPrograms[trainingProgramId]->renameProgram(newName);

        auto eventData = std::make_shared<Events::TrainingProgramRenamedEvent>();
        eventData->TrainingProgramId = trainingProgramId;
        eventData->TrainingProgramName = newName;
        return eventData;
    }
    std::shared_ptr<Kernel::DomainEvent> TrainingProgramList::swapTrainingPrograms(uint64_t firstProgramId, uint64_t secondProgramId)
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

        auto eventData = std::make_shared<Events::TrainingProgramSwappedEvent>();
        eventData->FirstTrainingProgramId = firstProgramId;
        eventData->SecondTrainingProgramId = secondProgramId;
        return eventData;
    }
    void TrainingProgramList::applyEvents(const std::vector<std::shared_ptr<Kernel::DomainEvent>>& events)
    {
        for (auto genericEvent : events)
        {
            if (auto additionEvent = dynamic_cast<Events::TrainingProgramAddedEvent*>(genericEvent.get()))
            {
                addTrainingProgram(additionEvent->TrainingProgramId);
            }
            else if (auto removalEvent = dynamic_cast<Events::TrainingProgramRemovedEvent*>(genericEvent.get()))
            {
                removeTrainingProgram(removalEvent->TrainingProgramId);
            }
            else if (auto swapEvent = dynamic_cast<Events::TrainingProgramSwappedEvent*>(genericEvent.get()))
            {
                swapTrainingPrograms(swapEvent->FirstTrainingProgramId, swapEvent->SecondTrainingProgramId);
            }
            else if (auto renameEvent = dynamic_cast<Events::TrainingProgramRenamedEvent*>(genericEvent.get()))
            {
                renameTrainingProgram(renameEvent->TrainingProgramId, renameEvent->TrainingProgramName);
            }
            // else: ignore
        }
    }
    std::shared_ptr<TrainingProgram> TrainingProgramList::getTrainingProgram(uint64_t trainingProgramId) const
    {
        if (_trainingPrograms.count(trainingProgramId) == 0)
        {
            throw Kernel::InvalidValueException(
                "Configuration",
                "Aggregate",
                "TrainingProgramList",
                "trainingProgramId",
                "The given training program is not known.",
                std::to_string(trainingProgramId)
            );
        }
        return _trainingPrograms.at(trainingProgramId);
    }
    std::vector<TrainingProgramListEntry> TrainingProgramList::getListEntries() const
    {
        std::vector<TrainingProgramListEntry> entries;
        for (auto trainingProgramId : _trainingProgramOrder)
        {
            const auto& trainingProgram = _trainingPrograms.at(trainingProgramId);
            entries.push_back({
                trainingProgram->id(),
                trainingProgram->name(),
                trainingProgram->programDuration()
                });
        }
        return entries;
    }
    void TrainingProgramList::ensureIdDoesntExist(uint64_t trainingProgramId)
    {
        if (_trainingPrograms.count(trainingProgramId) > 0)
        {
            throw Kernel::InvalidValueException(
                "Configuration",
                "Aggregate",
                "TrainingProgramList",
                "training program ID",
                "There is already a program with such an ID.",
                std::to_string(trainingProgramId)
            );
        }
    }
    void TrainingProgramList::ensureIdIsKnown(uint64_t trainingProgramId, const std::string& parameterName)
    {
        if (_trainingPrograms.count(trainingProgramId) == 0)
        {
            throw Kernel::InvalidValueException(
                "Configuration",
                "Aggregate",
                "TrainingProgramList",
                parameterName,
                "There is no training program with such an ID.",
                std::to_string(trainingProgramId)
            );
        }
    }
}
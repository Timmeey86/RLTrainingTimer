#include <pch.h>
#include "TrainingProgramList.h"
#include "../events/TrainingProgramEvents.h"

#include <core/kernel/InvalidValueException.h>

#include <qol/vectorext.h>

namespace Core::Configuration::Domain
{
    std::vector<std::shared_ptr<Kernel::DomainEvent>> TrainingProgramList::addTrainingProgram(uint64_t trainingProgramId)
    {
        ensureIdDoesntExist(trainingProgramId);

        _trainingProgramOrder.push_back(trainingProgramId);
        _trainingPrograms.emplace(trainingProgramId, std::make_shared<TrainingProgram>(trainingProgramId));

        auto addEvent = std::make_shared<Events::TrainingProgramAddedEvent>();
        addEvent->AffectedTrainingProgramIds.push_back(trainingProgramId);

        return addListChangedEvent({ addEvent });
    }


    std::vector<std::shared_ptr<Kernel::DomainEvent>> TrainingProgramList::removeTrainingProgram(uint64_t trainingProgramId)
    {
        ensureIdIsKnown(trainingProgramId, "training program ID");

        removeOne(_trainingProgramOrder, trainingProgramId);
        _trainingPrograms.erase(trainingProgramId); // this will delete the program, unless anyone else references it.

        auto removeEvent = std::make_shared<Events::TrainingProgramRemovedEvent>();
        removeEvent->AffectedTrainingProgramIds.push_back(trainingProgramId);

        return addListChangedEvent({ removeEvent });
    }
    std::vector<std::shared_ptr<Kernel::DomainEvent>> TrainingProgramList::swapTrainingPrograms(uint64_t firstProgramId, uint64_t secondProgramId)
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

        auto swapEvent = std::make_shared<Events::TrainingProgramSwappedEvent>();
        swapEvent->AffectedTrainingProgramIds.push_back(firstProgramId);
        swapEvent->AffectedTrainingProgramIds.push_back(secondProgramId);

        return addListChangedEvent({ swapEvent });
    }
    void TrainingProgramList::applyEvents(const std::vector<std::shared_ptr<Kernel::DomainEvent>>& events)
    {
        for (auto genericEvent : events)
        {
            if (auto additionEvent = dynamic_cast<Events::TrainingProgramAddedEvent*>(genericEvent.get()))
            {
                addTrainingProgram(additionEvent->AffectedTrainingProgramIds.front());
            }
            else if (auto removalEvent = dynamic_cast<Events::TrainingProgramRemovedEvent*>(genericEvent.get()))
            {
                removeTrainingProgram(removalEvent->AffectedTrainingProgramIds.front());
            }
            else if (auto swapEvent = dynamic_cast<Events::TrainingProgramSwappedEvent*>(genericEvent.get()))
            {
                swapTrainingPrograms(swapEvent->AffectedTrainingProgramIds.front(), swapEvent->AffectedTrainingProgramIds.back());
            }
            // else: ignore, this might be an event targeting user interfaces instead
        }
    }
    std::shared_ptr<TrainingProgram> TrainingProgramList::getTrainingProgram(uint64_t trainingProgramId) const
    {
        ensureIdIsKnown(trainingProgramId, "trainingProgramId");
        return _trainingPrograms.at(trainingProgramId);
    }
    std::shared_ptr<Kernel::DomainEvent> TrainingProgramList::createListChangedEvent()
    {
        auto changedEvent  = std::make_shared<Events::TrainingProgramListChangedEvent>();
        for (auto index = (uint16_t)0; index < (uint16_t)_trainingProgramOrder.size(); index++)
        {
            const auto& trainingProgramIdAtIndex = _trainingProgramOrder.at(index);
            const auto& trainingProgramAtIndex = _trainingPrograms.at(trainingProgramIdAtIndex);
            changedEvent->TrainingProgramListInfo.push_back(Events::TrainingProgramInfo{
                trainingProgramIdAtIndex,
                index,
                trainingProgramAtIndex->name(),
                std::chrono::milliseconds(trainingProgramAtIndex->programDuration()),
                trainingProgramAtIndex->entryCount()
                });
        }
        return changedEvent;
    }
    uint64_t TrainingProgramList::getMaximumId() const
    {
        auto maxId = 0ULL;
        for (auto id : _trainingProgramOrder)
        {
            if (id > maxId)
            {
                maxId = id;
            }
        }
        return maxId;
    }

    void TrainingProgramList::ensureIdDoesntExist(uint64_t trainingProgramId) const
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
    void TrainingProgramList::ensureIdIsKnown(uint64_t trainingProgramId, const std::string& parameterName) const
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
    std::vector<std::shared_ptr<Kernel::DomainEvent>> TrainingProgramList::addListChangedEvent(std::vector<std::shared_ptr<Kernel::DomainEvent>> otherEvents)
    {
        otherEvents.push_back(createListChangedEvent());
        return otherEvents;
    }
}
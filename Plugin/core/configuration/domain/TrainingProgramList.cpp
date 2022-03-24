#include <pch.h>
#include "TrainingProgramList.h"

#include <core/kernel/InvalidValueException.h>

#include <qol/vectorext.h>

namespace Core::Configuration::Domain
{
    Events::TrainingProgramAddedEvent TrainingProgramList::addTrainingProgram(uint64_t trainingProgramId)
    {
        ensureIdDoesntExist(trainingProgramId);

        _trainingProgramOrder.push_back(trainingProgramId);
        _trainingPrograms.emplace(trainingProgramId, std::make_shared<TrainingProgram>(trainingProgramId));

        Events::TrainingProgramAddedEvent additionEvent;
        additionEvent.TrainingProgramId = trainingProgramId;
        return additionEvent;
    }

    Events::TrainingProgramRemovedEvent TrainingProgramList::removeTrainingProgram(uint64_t trainingProgramId)
    {
        ensureIdIsKnown(trainingProgramId, "training program ID");

        removeOne(_trainingProgramOrder, trainingProgramId);
        _trainingPrograms.erase(trainingProgramId); // this will delete the program, unless anyone else references it.

        Events::TrainingProgramRemovedEvent removalEvent;
        removalEvent.TrainingProgramId = trainingProgramId;
        return removalEvent;
    }
    Events::TrainingProgramRenamedEvent TrainingProgramList::renameTrainingProgram(uint64_t trainingProgramId, const std::string& newName)
    {
        ensureIdIsKnown(trainingProgramId, "training program ID");

        _trainingPrograms[trainingProgramId]->renameProgram(newName);

        Events::TrainingProgramRenamedEvent renameEvent;
        renameEvent.TrainingProgramId = trainingProgramId;
        renameEvent.TrainingProgramName = newName;
        return renameEvent;
    }
    Events::TrainingProgramSwappedEvent TrainingProgramList::swapTrainingPrograms(uint64_t firstProgramId, uint64_t secondProgramId)
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

        Events::TrainingProgramSwappedEvent swapEvent;
        swapEvent.FirstTrainingProgramId = firstProgramId;
        swapEvent.SecondTrainingProgramId = secondProgramId;
        return swapEvent;
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
                trainingProgram->name()
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
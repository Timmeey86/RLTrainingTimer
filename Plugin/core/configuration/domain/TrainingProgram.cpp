#include <pch.h>
#include "TrainingProgram.h"

#include <core/kernel/IndexOutOfBoundsException.h>
#include "../events/TrainingProgramEntryEvents.h"
#include "../events/TrainingProgramEvents.h"

#include <qol/vectorext.h>

namespace Core::Configuration::Domain
{
    TrainingProgram::TrainingProgram(uint64_t id)
        : _id{ id }
        , _name{}
    {
    }

    std::shared_ptr<Kernel::DomainEvent> TrainingProgram::addEntry(const TrainingProgramEntry& entry)
    {
        _entries.push_back(entry);
        _duration += entry.duration();

        auto eventData = std::make_shared<Events::TrainingProgramEntryAddedEvent>();
        eventData->TrainingProgramId = _id;
        eventData->TrainingProgramEntryName = entry.name();
        eventData->TrainingProgramEntryDuration = entry.duration();
        return eventData;
    }

    std::shared_ptr<Kernel::DomainEvent> TrainingProgram::removeEntry(int position)
    {
        validatePosition(position, "position");

        auto oldEntry = _entries.at(position);
        _duration -= oldEntry.duration();

        remove(_entries, position);

        auto eventData = std::make_shared<Events::TrainingProgramEntryRemovedEvent>();
        eventData->TrainingProgramId = _id;
        eventData->TrainingProgramEntryPosition = position;
        return eventData;
    }

    std::shared_ptr<Kernel::DomainEvent> TrainingProgram::replaceEntry(int position, const TrainingProgramEntry& newEntry)
    {
        validatePosition(position, "position");

        auto oldEntry = _entries.at(position);
        _duration -= oldEntry.duration();

        replace(_entries, position, newEntry);
        _duration += newEntry.duration();

        auto eventData = std::make_shared<Events::TrainingProgramEntryUpdatedEvent>();
        eventData->TrainingProgramId = _id;
        eventData->TrainingProgramEntryName = newEntry.name();
        eventData->TrainingProgramEntryDuration = newEntry.duration();
        return eventData;
    }

    std::shared_ptr<Kernel::DomainEvent> TrainingProgram::swapEntries(int firstPosition, int secondPosition)
    {
        validatePosition(firstPosition, "first position");
        validatePosition(secondPosition, "second position");

        std::swap(_entries[firstPosition], _entries[secondPosition]);
        // Duration will stay identical, no need to update it

        auto eventData = std::make_shared<Events::TrainingProgramEntrySwappedEvent>();
        eventData->TrainingProgramId = _id;
        eventData->FirstTrainingProgramEntryPosition = firstPosition;
        eventData->SecondTrainingProgramEntryPosition = secondPosition;
        return eventData;
    }

    std::shared_ptr<Kernel::DomainEvent> TrainingProgram::renameProgram(const std::string& newName)
    {
        _name = newName;

        auto eventData = std::make_shared<Events::TrainingProgramRenamedEvent>();
        eventData->TrainingProgramId = _id;
        eventData->TrainingProgramName = _name;
        return eventData;
    }

    void TrainingProgram::applyEvents(const std::vector<std::shared_ptr<Kernel::DomainEvent>>& events)
    {
        for (auto genericEvent : events)
        {
            if (auto additionEvent = dynamic_cast<Events::TrainingProgramEntryAddedEvent*>(genericEvent.get()))
            {
                if (additionEvent->TrainingProgramId != _id) { continue; }
                addEntry({ additionEvent->TrainingProgramEntryName, additionEvent->TrainingProgramEntryDuration });
            }
            else if (auto removalEvent = dynamic_cast<Events::TrainingProgramEntryRemovedEvent*>(genericEvent.get()))
            {
                if (removalEvent->TrainingProgramId != _id) { continue; }
                removeEntry(removalEvent->TrainingProgramEntryPosition);
            }
            else if (auto updateEvent = dynamic_cast<Events::TrainingProgramEntryUpdatedEvent*>(genericEvent.get()))
            {
                if (updateEvent->TrainingProgramId != _id) { continue; }
                replaceEntry(updateEvent->TrainingProgramEntryPosition, { updateEvent->TrainingProgramEntryName, updateEvent->TrainingProgramEntryDuration });
            }
            else if (auto swapEvent = dynamic_cast<Events::TrainingProgramEntrySwappedEvent*>(genericEvent.get()))
            {
                if (swapEvent->TrainingProgramId != _id) { continue; }
                swapEntries(swapEvent->FirstTrainingProgramEntryPosition, swapEvent->SecondTrainingProgramEntryPosition);
            }
            // else: the event was most likely intended for a different object
        }
    }

    std::vector<TrainingProgramEntry> TrainingProgram::entries() const 
    {
        return std::vector<TrainingProgramEntry>(_entries);
    }

    uint32_t TrainingProgram::programDuration() const
    {
        return _duration;
    }

    uint64_t TrainingProgram::id() const
    {
        return _id;
    }

    std::string TrainingProgram::name() const 
    {
        return _name;
    }
    
    void TrainingProgram::validatePosition(int position, const std::string& variableName) const
    {
        if (position < 0 || position >= _entries.size())
        {
            throw Kernel::IndexOutOfBoundsException(
                "Configuration",
                "Aggregate",
                "TrainingProgram",
                variableName,
                0,
                (int)_entries.size() - 1,
                position
            );
        }
    }
}
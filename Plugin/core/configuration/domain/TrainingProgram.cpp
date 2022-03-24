#include <pch.h>
#include "TrainingProgram.h"

#include <core/kernel/IndexOutOfBoundsException.h>

#include <qol/vectorext.h>

namespace Core::Configuration::Domain
{
    TrainingProgram::TrainingProgram(uint64_t id)
        : _id{ id }
        , _name{ "New Training Program" }
    {
    }

    Events::TrainingProgramEntryAddedEvent TrainingProgram::addEntry(const TrainingProgramEntry& entry)
    {
        _entries.push_back(entry);
        _duration += entry.duration();

        Events::TrainingProgramEntryAddedEvent eventData;
        eventData.TrainingProgramId = _id;
        eventData.TrainingProgramEntryName = entry.name();
        eventData.TrainingProgramEntryDuration = entry.duration();
        return eventData;
    }

    Events::TrainingProgramEntryRemovedEvent TrainingProgram::removeEntry(int position)
    {
        validatePosition(position, "position");

        auto oldEntry = _entries.at(position);
        _duration -= oldEntry.duration();

        remove(_entries, position);

        Events::TrainingProgramEntryRemovedEvent eventData;
        eventData.TrainingProgramId = _id;
        eventData.TrainingProgramEntryPosition = position;
        return eventData;
    }

    Events::TrainingProgramEntryUpdatedEvent TrainingProgram::replaceEntry(int position, const TrainingProgramEntry& newEntry)
    {
        validatePosition(position, "position");

        auto oldEntry = _entries.at(position);
        _duration -= oldEntry.duration();

        replace(_entries, position, newEntry);
        _duration += newEntry.duration();

        Events::TrainingProgramEntryUpdatedEvent eventData;
        eventData.TrainingProgramId = _id;
        eventData.TrainingProgramEntryName = newEntry.name();
        eventData.TrainingProgramEntryDuration = newEntry.duration();
        return eventData;
    }

    Events::TrainingProgramEntrySwappedEvent TrainingProgram::swapEntries(int firstPosition, int secondPosition)
    {
        validatePosition(firstPosition, "first position");
        validatePosition(secondPosition, "second position");

        std::swap(_entries[firstPosition], _entries[secondPosition]);
        // Duration will stay identical, no need to update it

        Events::TrainingProgramEntrySwappedEvent eventData;
        eventData.TrainingProgramId = _id;
        eventData.FirstTrainingProgramEntryPosition = firstPosition;
        eventData.SecondTrainingProgramEntryPosition = secondPosition;
        return eventData;
    }

    Events::TrainingProgramRenamedEvent TrainingProgram::renameProgram(const std::string& newName)
    {
        _name = newName;

        Events::TrainingProgramRenamedEvent renamedEvent;
        renamedEvent.TrainingProgramId = _id;
        renamedEvent.TrainingProgramName = _name;
        return renamedEvent;
    }

    void TrainingProgram::loadFromEvents(const std::vector<Kernel::DomainEvent>& events)
    {
        for (auto genericEvent : events)
        {
            if (auto additionEvent = dynamic_cast<Events::TrainingProgramEntryAddedEvent*>(&genericEvent))
            {
                if (additionEvent->TrainingProgramId != _id) { continue; }
                addEntry({ additionEvent->TrainingProgramEntryName, additionEvent->TrainingProgramEntryDuration });
            }
            else if (auto removalEvent = dynamic_cast<Events::TrainingProgramEntryRemovedEvent*>(&genericEvent))
            {
                if (removalEvent->TrainingProgramId != _id) { continue; }
                removeEntry(removalEvent->TrainingProgramEntryPosition);
            }
            else if (auto updateEvent = dynamic_cast<Events::TrainingProgramEntryUpdatedEvent*>(&genericEvent))
            {
                if (updateEvent->TrainingProgramId != _id) { continue; }
                replaceEntry(updateEvent->TrainingProgramEntryPosition, { updateEvent->TrainingProgramEntryName, updateEvent->TrainingProgramEntryDuration });
            }
            else if (auto swapEvent = dynamic_cast<Events::TrainingProgramEntrySwappedEvent*>(&genericEvent))
            {
                if (swapEvent->TrainingProgramId != _id) { continue; }
                swapEntries(swapEvent->FirstTrainingProgramEntryPosition, swapEvent->SecondTrainingProgramEntryPosition);
            }
            else if (auto renameEvent = dynamic_cast<Events::TrainingProgramRenamedEvent*>(&genericEvent))
            {
                if (renameEvent->TrainingProgramId != _id) { continue; }
                renameProgram(renameEvent->TrainingProgramName);
            }
            // else: the event was most likely intended for a different object
        }
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
                (int)_entries.size(),
                position
            );
        }
    }
}
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
    {
    }

    std::vector<std::shared_ptr<Kernel::DomainEvent>> TrainingProgram::addEntry(const TrainingProgramEntry& entry)
    {
        _entries.push_back(entry);
        _duration += entry.Duration;

        auto eventData = std::make_shared<Events::TrainingProgramEntryAddedEvent>();
        eventData->TrainingProgramId = _id;
        eventData->TrainingProgramEntryName = entry.Name;
        eventData->TrainingProgramEntryDuration = entry.Duration;

        return addProgramChangedEvent({ eventData });
    }

    std::vector<std::shared_ptr<Kernel::DomainEvent>> TrainingProgram::removeEntry(int position)
    {
        validatePosition(position, "position");

        auto oldEntry = _entries.at(position);
        _duration -= oldEntry.Duration;

        remove(_entries, position);

        auto eventData = std::make_shared<Events::TrainingProgramEntryRemovedEvent>();
        eventData->TrainingProgramId = _id;
        eventData->TrainingProgramEntryPosition = position;

        return addProgramChangedEvent({ eventData });
    }

    std::vector<std::shared_ptr<Kernel::DomainEvent>> TrainingProgram::renameEntry(int position, const std::string& newName)
    {
        validatePosition(position, "position");

        auto& affectedEntry = _entries.at(position);
        affectedEntry.Name = newName;

        auto eventData = std::make_shared<Events::TrainingProgramEntryRenamedEvent>();
        eventData->TrainingProgramId = _id;
        eventData->TrainingProgramEntryName = newName;
        eventData->TrainingProgramEntryPosition = position;

        return addProgramChangedEvent({ eventData });
    }
    std::vector<std::shared_ptr<Kernel::DomainEvent>> TrainingProgram::changeEntryDuration(int position, const std::chrono::milliseconds& newDuration)
    {
        validatePosition(position, "position");

        auto& affectedEntry = _entries.at(position);
        _duration -= affectedEntry.Duration;
        _duration += newDuration;
        affectedEntry.Duration = newDuration;

        auto eventData = std::make_shared<Events::TrainingProgramEntryDurationChangedEvent>();
        eventData->TrainingProgramId = _id;
        eventData->TrainingProgramEntryDuration = affectedEntry.Duration;
        eventData->TrainingProgramEntryPosition = position;

        return addProgramChangedEvent({ eventData });
    }

    std::vector<std::shared_ptr<Kernel::DomainEvent>> TrainingProgram::swapEntries(int firstPosition, int secondPosition)
    {
        validatePosition(firstPosition, "first position");
        validatePosition(secondPosition, "second position");

        std::swap(_entries[firstPosition], _entries[secondPosition]);
        // Duration will stay identical, no need to update it

        auto eventData = std::make_shared<Events::TrainingProgramEntrySwappedEvent>();
        eventData->TrainingProgramId = _id;
        eventData->FirstTrainingProgramEntryPosition = firstPosition;
        eventData->SecondTrainingProgramEntryPosition = secondPosition;

        return addProgramChangedEvent({ eventData });
    }

    std::vector<std::shared_ptr<Kernel::DomainEvent>> TrainingProgram::renameProgram(const std::string& newName)
    {
        _name = newName;

        auto renameEvent = std::make_shared<Events::TrainingProgramRenamedEvent>();
        renameEvent->AffectedTrainingProgramIds.push_back(_id);
        renameEvent->TrainingProgramName = newName;

        return addProgramChangedEvent({ renameEvent });
    }

    std::vector<std::shared_ptr<Kernel::DomainEvent>> TrainingProgram::addProgramChangedEvent(std::vector<std::shared_ptr<Kernel::DomainEvent>> otherEvents)
    {
        auto changedEvent = std::make_shared<Events::TrainingProgramChangedEvent>();
        changedEvent->TrainingProgramId = _id;
        changedEvent->TrainingProgramName = _name;
        changedEvent->TrainingProgramDuration = _duration; 
        for (const auto& entry : _entries)
        {
            changedEvent->TrainingProgramEntries.push_back({ entry.Name, entry.Duration });
        }

        otherEvents.push_back(changedEvent);
        return otherEvents;
    }

    std::vector<TrainingProgramEntry> TrainingProgram::entries() const 
    {
        return std::vector<TrainingProgramEntry>(_entries);
    }

    std::chrono::milliseconds TrainingProgram::programDuration() const
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
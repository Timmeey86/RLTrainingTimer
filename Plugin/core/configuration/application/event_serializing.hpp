#pragma once

#include "../events/TrainingProgramEntryEvents.h"
#include "../events/TrainingProgramEvents.h"

#include <external/nlohmann/json.hpp>
#include "json_serializer.hpp"

#define NLOHMANN_DEFINE_EMPTY_TYPE_NON_INTRUSIVE(Type) \
	inline void to_json(nlohmann::json& nlohmann_json_j, const Type& nlohmann_json_t) { } \
	inline void from_json(const nlohmann::json& nlohmann_json_j, Type& nlohmann_json_t) { }

#define NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_BASE(Type, Base, ...)  \
    inline void to_json(nlohmann::json& nlohmann_json_j, const Type& nlohmann_json_t) { ::to_json(nlohmann_json_j, static_cast<const Base&>(nlohmann_json_t)); NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_TO, __VA_ARGS__)) } \
    inline void from_json(const nlohmann::json& nlohmann_json_j, Type& nlohmann_json_t) { ::from_json(nlohmann_json_j, static_cast<Base&>(nlohmann_json_t)); NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_FROM, __VA_ARGS__)) }

// Allow serialization and deserialization of events
NLOHMANN_DEFINE_EMPTY_TYPE_NON_INTRUSIVE(Core::Kernel::DomainEvent);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_BASE(Core::Configuration::Events::TrainingProgramRenamedEvent, Core::Kernel::DomainEvent, AffectedTrainingProgramIds, TrainingProgramName);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_BASE(Core::Configuration::Events::TrainingProgramAddedEvent, Core::Kernel::DomainEvent, AffectedTrainingProgramIds);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_BASE(Core::Configuration::Events::TrainingProgramRemovedEvent, Core::Kernel::DomainEvent, AffectedTrainingProgramIds);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_BASE(Core::Configuration::Events::TrainingProgramSwappedEvent, Core::Kernel::DomainEvent, AffectedTrainingProgramIds);

// Allow serialization of std::chrono::milliseconds
namespace nlohmann {
	template <>
	struct adl_serializer<std::chrono::milliseconds> {
		static void to_json(json& j, const std::chrono::milliseconds& ms) { j = ms.count(); }
		static void from_json(const json& j, std::chrono::milliseconds& ms) { ms = std::chrono::milliseconds(j); }
	};
}
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Core::Configuration::Events::TrainingProgramInfo, Id, ListIndex, Name, Duration, NumberOfSteps);
namespace nlohmann {
	// Not sure why this is needed, but fails to compile without it
	template<>
	struct adl_serializer<std::vector<Core::Configuration::Events::TrainingProgramInfo>> {
		static void to_json(json& j, const std::vector<Core::Configuration::Events::TrainingProgramInfo>& v)
		{
			j = v;
		}
		static void from_json(const json& j, std::vector<Core::Configuration::Events::TrainingProgramInfo>& v)
		{
			v = j;
		}
	};
}
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_BASE(Core::Configuration::Events::TrainingProgramListChangedEvent, Core::Kernel::DomainEvent, TrainingProgramListInfo);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_BASE(Core::Configuration::Events::TrainingProgramEntryAddedEvent, Core::Kernel::DomainEvent, TrainingProgramId, TrainingProgramEntryName, TrainingProgramEntryDuration);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_BASE(Core::Configuration::Events::TrainingProgramEntryRemovedEvent, Core::Kernel::DomainEvent, TrainingProgramId, TrainingProgramEntryPosition);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_BASE(Core::Configuration::Events::TrainingProgramEntryRenamedEvent, Core::Kernel::DomainEvent, TrainingProgramId, TrainingProgramEntryPosition, TrainingProgramEntryName);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_BASE(Core::Configuration::Events::TrainingProgramEntryDurationChangedEvent, Core::Kernel::DomainEvent, TrainingProgramId, TrainingProgramEntryPosition, TrainingProgramEntryDuration);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_BASE(Core::Configuration::Events::TrainingProgramEntrySwappedEvent, Core::Kernel::DomainEvent, TrainingProgramId, FirstTrainingProgramEntryPosition, SecondTrainingProgramEntryPosition);

namespace nlohmann
{
	template <>
	struct adl_serializer<Core::Kernel::DomainEvent>
		: PolymorphicJsonSerializer<Core::Kernel::DomainEvent> { };

	template <typename T>
	struct adl_serializer<std::shared_ptr<T>> : PolymorphicJsonSerializer<Core::Kernel::DomainEvent>
	{
		static void to_json(json& j, const std::shared_ptr<T>& opt)
		{
			if (opt)
			{
				j = *opt;
			}
			else
			{
				j = nullptr;
			}
		}

		static void from_json(const json& j, std::shared_ptr<T>& opt)
		{
			if (j.is_null())
			{
				opt = nullptr;
			}
			else
			{
				const auto serializer = _serializers.at(j.at("_type").get<std::string>());
				opt = serializer.construct();
				serializer.from_json(j, *opt);
			}
		}
	};
}
#pragma once

#include <nlohmann/json.hpp>

#include <iostream>
#include <memory>
#include <vector>
#include <unordered_map>
#include <typeinfo>
#include <typeindex>

using json = nlohmann::json;

namespace PolymorphicJsonSerializer_impl {
	template <class Base>
	struct Serializer {
		void (*to_json)(json& j, Base const& o);
		void (*from_json)(json const& j, Base& o);
		std::shared_ptr<Base>(*construct)();
	};

	template <class Base, class Derived>
	Serializer<Base> serializerFor() {
		return {
			[](json& j, Base const& o) {
				return ::to_json(j, static_cast<Derived const&>(o));
			},
			[](json const& j, Base& o) {
				return ::from_json(j, static_cast<Derived&>(o));
			},
			[] { return std::static_pointer_cast<Base>(std::make_shared<Derived>()); }
		};
	}
}

template <class Base>
struct PolymorphicJsonSerializer {

	// Maps typeid(x).name() to the from/to serialization functions
	static inline std::unordered_map<
		std::string,
		PolymorphicJsonSerializer_impl::Serializer<Base>
	> _serializers;

	template <class... Derived>
	static void register_types() {
		(_serializers.emplace(
			typeid(Derived).name(),
			PolymorphicJsonSerializer_impl::serializerFor<Base, Derived>()
		), ...);
	}

	static void to_json(json& j, Base const& o) {
		char const* typeName = typeid(o).name();
		try
		{
			_serializers.at(typeName).to_json(j, o);
			j["_type"] = typeName;
		}
		catch (std::out_of_range e)
		{
			LOG("Could not serialize {}", typeName);
		}
	}

	static void from_json(json const& j, Base& o) {
		_serializers.at(j.at("_type").get<std::string>().c_str()).from_json(j, o);
	}
};

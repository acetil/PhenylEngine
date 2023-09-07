#pragma once

#include <memory>

#include "serializer.h"
#include "util/optional.h"

namespace common {
    class JSONDeserializeVisitor : public DeserializeVisitor {
    protected:
        virtual bool openJson (const std::string& jsonStr) = 0;
        virtual void closeJson () = 0;
    public:
        template <Serializable T>
        util::Optional<T> deserialize (const std::string& jsonStr) {
            if (!openJson(jsonStr)) {
                return util::NullOpt;
            }

            T obj = detail::SerializerFactory<T>();
            bool res = visit(obj);
            closeJson();

            return res ? util::Optional<T>{std::move(obj)} : util::Optional<T>{};
        }

        static std::unique_ptr<JSONDeserializeVisitor> Make ();
    };
}
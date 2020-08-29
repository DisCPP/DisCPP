//
// Created by SeanOMik on 7/10/2020.
// Github: https://github.com/SeanOMik
// Email: seanomik@gmail.com
//

#include "json_object.h"

#ifdef RAPIDJSON_BACKEND
#include <rapidjson/writer.h>
#endif

discpp::JsonObject::JsonObject(const std::string &json_str) {

}

#ifdef RAPIDJSON_BACKEND
discpp::JsonObject::JsonObject(const rapidjson::Document &json) {
    inner.CopyFrom(json, inner.GetAllocator());
}

discpp::JsonObject::JsonObject(const rapidjson::Value &json) {
    inner.CopyFrom(json, inner.GetAllocator());
}

discpp::JsonObject::JsonObject(const discpp::JsonObject &json) {
    inner.CopyFrom(json.inner, inner.GetAllocator());
}

discpp::JsonObject& discpp::JsonObject::operator=(const discpp::JsonObject &other) {
    inner.CopyFrom(other.inner, inner.GetAllocator());
    return *this;
}
#elif SIMDJSON_BACKEND

#endif

void discpp::JsonObject::IterateThrough(const std::function<bool(JsonObject &)> &func) const {
#ifdef RAPIDJSON_BACKEND
    for (auto const& object : inner.GetArray()) {
        if (!object.IsNull()) {
            rapidjson::Document object_json;
            object_json.CopyFrom(object, object_json.GetAllocator());

            JsonObject obj(object_json);
            if (!func(obj)) break;
        }
    }
#elif SIMDJSON_BACKEND

#endif
}

void discpp::JsonObject::IterateThrough(const char *value_name, const std::function<bool(JsonObject &)> &func) const {
#ifdef RAPIDJSON_BACKEND
    for (auto const& object : inner[value_name].GetArray()) {
        if (!object.IsNull()) {
            rapidjson::Document object_json;
            object_json.CopyFrom(object, object_json.GetAllocator());

            JsonObject obj(object_json);
            if (!func(obj)) break;
        }
    }
#elif SIMDJSON_BACKEND

#endif
}

std::string discpp::JsonObject::DumpJson() const {
#ifdef RAPIDJSON_BACKEND
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    inner.Accept(writer);
    std::string tmp = buffer.GetString();

    return tmp;
#elif SIMDJSON_BACKEND

#endif
}

bool discpp::JsonObject::ContainsNotNull(const char *value_name) const {
#ifdef RAPIDJSON_BACKEND
    rapidjson::Value::ConstMemberIterator itr = inner.FindMember(value_name);
    if (itr != inner.MemberEnd()) {
        return !inner[value_name].IsNull();
    }

    return false;
#elif SIMDJSON_BACKEND

#endif
}

discpp::JsonObject discpp::JsonObject::operator[](const char *val) const {
#ifdef RAPIDJSON_BACKEND
    rapidjson::Document doc(rapidjson::kObjectType);
    doc.CopyFrom(inner[val], doc.GetAllocator());
    return JsonObject(doc);
#elif SIMDJSON_BACKEND

#endif
}

std::string discpp::JsonObject::GetString() const {
#ifdef RAPIDJSON_BACKEND
    return inner.GetString();
#elif SIMDJSON_BACKEND

#endif
}

int discpp::JsonObject::GetInt() const {
#ifdef RAPIDJSON_BACKEND
    return inner.GetInt();
#elif SIMDJSON_BACKEND

#endif
}

bool discpp::JsonObject::GetBool() const {
#ifdef RAPIDJSON_BACKEND
    return inner.GetBool();
#elif SIMDJSON_BACKEND

#endif
}

bool discpp::JsonObject::IsEmpty() const {
#ifdef RAPIDJSON_BACKEND
    return inner.Empty();
#elif SIMDJSON_BACKEND

#endif
}

#ifdef RAPIDJSON_BACKEND
std::unique_ptr<rapidjson::Document> discpp::JsonObject::GetRawJson() const {
    auto json = std::make_unique<rapidjson::Document>(inner.GetType());
    json->CopyFrom(inner, json->GetAllocator());

    return json;
}
#elif SIMDJSON_BACKEND

#endif

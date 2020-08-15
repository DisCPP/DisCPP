//
// Created by SeanOMik on 7/10/2020.
// Github: https://github.com/SeanOMik
// Email: seanomik@gmail.com
//

#ifndef DISCPP_JSON_OBJECT_H
#define DISCPP_JSON_OBJECT_H

#include "snowflake.h"

#include <functional>

#ifdef RAPIDJSON_BACKEND
#ifndef RAPIDJSON_HAS_STDSTRING
#define RAPIDJSON_HAS_STDSTRING 1
#endif

#include <rapidjson/document.h>
#endif

namespace discpp {
    class JsonObject {
    public:
        explicit JsonObject(const std::string& json_str);

#ifdef RAPIDJSON_BACKEND
    private:
        rapidjson::Document inner;
    public:
        explicit JsonObject(const rapidjson::Document& json);
        explicit JsonObject(const rapidjson::Value& json);

        JsonObject(const JsonObject& json);
        JsonObject& operator=(const JsonObject& other);
#elif SIMDJSON_BACKEND

#endif

        void IterateThrough(const std::function<void(JsonObject&)>& func);
        void IterateThrough(const char* value_name, const std::function<void(JsonObject&)>& func);

        inline discpp::Snowflake GetIDSafely(rapidjson::Document& json, const char* value_name) {
#ifdef RAPIDJSON_BACKEND
            rapidjson::Value::ConstMemberIterator itr = json.FindMember(value_name);
            if (itr != json.MemberEnd()) {
                if (!json[value_name].IsNull()) {
                    rapidjson::Document t_doc;
                    t_doc.CopyFrom(json[value_name], t_doc.GetAllocator());

                    return Snowflake(std::string(t_doc.GetString()));
                }
            }

            return 0;
#elif SIMDJSON_BACKEND

#endif
        }

        template<typename T>
        inline T GetDataSafely(const rapidjson::Document & json, const char* value_name) {
#ifdef RAPIDJSON_BACKEND
            rapidjson::Value::ConstMemberIterator itr = json.FindMember(value_name);
            if (itr != json.MemberEnd()) {
                if (!json[value_name].IsNull()) {
                    rapidjson::Document t_doc;
                    t_doc.CopyFrom(json[value_name], t_doc.GetAllocator());

                    return t_doc.Get<T>();
                }
            }

            return T();
#elif SIMDJSON_BACKEND

#endif
        }

        template<class T>
        inline T ConstructDiscppObjectFromID(const rapidjson::Document& doc, const char* value_name, T default_val) {
#ifdef RAPIDJSON_BACKEND
            rapidjson::Value::ConstMemberIterator itr = doc.FindMember(value_name);
            if (itr != doc.MemberEnd()) {
                if (!doc[value_name].IsNull()) {
                    rapidjson::Document t_doc;
                    t_doc.CopyFrom(doc[value_name], t_doc.GetAllocator());

                    return T(SnowflakeFromString(t_doc.GetString()));
                }
            }

            return default_val;
#elif SIMDJSON_BACKEND

#endif
        }

        template<class T>
        inline T ConstructDiscppObjectFromJson(const char* value_name, T default_val) {
#ifdef RAPIDJSON_BACKEND
            rapidjson::Value::ConstMemberIterator itr = doc.FindMember(value_name);
            if (itr != doc.MemberEnd()) {
                if (!doc[value_name].IsNull()) {
                    rapidjson::Document t_doc;
                    t_doc.CopyFrom(doc[value_name], t_doc.GetAllocator());

                    return T(t_doc);
                }
            }

            return default_val;
#elif SIMDJSON_BACKEND

#endif
        }

        std::string DumpJson();
        bool ContainsNotNull(const char* value_name);
        void GetInnerJson(const char* value_name, JsonObject& obj);
    };
}

#endif //DISCPP_JSON_OBJECT_H
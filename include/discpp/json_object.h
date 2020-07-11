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
#elif SIMDJSON_BACKEND

#endif

namespace discpp {
    class JsonObject {
    public:
        JsonObject() = default;
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
        /**
         * @brief Iterate through a JsonObject.
         *
         * @param[in] func What to do each iteration. Return `false` to stop.
         *
         * @return void
         */
        void IterateThrough(const std::function<bool(JsonObject&)>& func) const;

        /**
         * @brief Iterate through a json value in this JsonObject.
         *
         * @param[in] func What to do each iteration. Return `false` to stop.
         *
         * @return void
         */
        void IterateThrough(const char* value_name, const std::function<bool(JsonObject&)>& func) const;

        /**
         * @brief Get an objects ID safely.
         *
         * Checks if `this` contains a non-null value with the given name
         *
         * @param[in] value_name The value in the json that contains the ID.
         *
         * @return void
         */
        inline discpp::Snowflake GetIDSafely(const char* value_name) const {
#ifdef RAPIDJSON_BACKEND
            if (ContainsNotNull(value_name)) {
                rapidjson::Document t_doc;
                t_doc.CopyFrom(inner[value_name], t_doc.GetAllocator());

                return Snowflake(std::string(t_doc.GetString()));
            }

            return 0;
#elif SIMDJSON_BACKEND

#endif
        }

        /**
         * @brief Get a value safely as a certain type.
         *
         * Checks if `this` contains a non-null value with the given name
         *
         * @param[in] value_name The value in the json that contains the value.
         *
         * @return void
         */
        template<typename T>
        inline T Get(const char* value_name) const {
#ifdef RAPIDJSON_BACKEND
            if (ContainsNotNull(value_name)) {
                return inner[value_name].Get<T>();
            }

            return T();
#elif SIMDJSON_BACKEND

#endif
        }

        /**
         * @brief Get this JsonObject as a certain type.
         *
         * Checks if `this` is not null.
         *
         * @return void
         */
        template<typename T>
        inline T Get() const {
#ifdef RAPIDJSON_BACKEND
            if (!inner.IsNull()) {
                return inner.Get<T>();
            }

            return T();
#elif SIMDJSON_BACKEND

#endif
        }

        /**
         * @brief Construct a discpp object from an id in the json if it contains the ID.
         *
         * Checks if `this` contains a non-null value with the given name
         *
         * @param[in] value_name The value in the json that contains the value.
         *
         * @return void
         */
        template<class T>
        inline T ConstructDiscppObjectFromID(const char* value_name, T default_val) const {
#ifdef RAPIDJSON_BACKEND
            if (ContainsNotNull(value_name)) {
                return T(Snowflake(inner[value_name].GetString()));
            }

            return default_val;
#elif SIMDJSON_BACKEND

#endif
        }

        /**
         * @brief Construct a discpp object from json if it contains the value.
         *
         * Checks if `this` contains a non-null value with the given name
         *
         * @param[in] value_name The value in the json that contains the value.
         *
         * @return void
         */
        template<class T>
        inline T ConstructDiscppObjectFromJson(const char* value_name, T default_val) const {
#ifdef RAPIDJSON_BACKEND
            if (ContainsNotNull(value_name)) {
                return T((*this)[value_name]);
            }

            return default_val;
#elif SIMDJSON_BACKEND

#endif
        }

        /**
         * @brief Get a JsonObject inside of this object.
         *
         * @param[in] val The value in the json that contains the JsonObject you want.
         *
         * @return discpp::JsonObject
         */
        discpp::JsonObject operator[](const char* val) const;

        /**
         * @brief Get `this` as a string.
         *
         * @return std::string
         */
        std::string GetString() const;

        /**
         * @brief Get `this` as an integer.
         *
         * @return int
         */
        int GetInt() const;

        /**
         * @brief Get `this` as a bool.
         *
         * @return bool
         */
        bool GetBool() const;

        /**
         * @brief Check if `this` is empty.
         *
         * @return bool
         */
        bool IsEmpty() const;

        /**
         * @brief Dump this object as a Json string.
         *
         * @return std::string
         */
        std::string DumpJson() const;

        /**
         * @brief Check if this Json contains a certain field.
         *
         * @param[in] value_name The value to check if `this` contains.
         *
         * @return bool
         */
        bool ContainsNotNull(const char* value_name) const;

#ifdef RAPIDJSON_BACKEND
        /**
         * @brief Get this object as the raw json representation.
         *
         * @return std::unique_ptr<rapidjson::Document>
         */
        std::unique_ptr<rapidjson::Document> GetRawJson() const;
#elif SIMDJSON_BACKEND

#endif
    };
}

#endif //DISCPP_JSON_OBJECT_H
#include "MongoDBHandler.h"

#include <iterator>
#include <exception>

#include "Logger.h"
#include "../Common/ServerMacros.h"

#include "mongoc.h"


USING_DATA_BASE;


MongoDBHandler::~MongoDBHandler()
{
	Logger::logInfo("$mongo db hander destory!!!");
}

MongoDBHandler::MongoDBHandler(std::string& dbUrl, std::string& dbName) : m_dbUrl(dbUrl), m_dbName(dbName)
{
	mongoc_init();
    bson_error_t error;
    mongoc_uri_t* uri = mongoc_uri_new_with_error(dbUrl.c_str(), &error);
    if (!uri) {
        fprintf(stderr,
            "failed to parse URI: %s\n"
            "error message:       %s\n",
            dbUrl.c_str(),
            error.message);
        Logger::logError("connect mongo db failed, url:%s", dbUrl.c_str());
        THROW_EXCEPTION("connect mongo db failed")
    }

    mongoc_client_t* client = mongoc_client_new_from_uri(uri);
    if (!client) {
        Logger::logError("new mongo db client failed, url:%s", dbUrl.c_str());
        THROW_EXCEPTION("new mongo db client failed");
    }

    mongoc_client_set_error_api(client, 2);

    bson_t query;
    bson_init(&query);
    const char* collection_name = "test";
    mongoc_collection_t* collection = mongoc_client_get_collection(client, "test", collection_name);
    mongoc_cursor_t* cursor = mongoc_collection_find_with_opts(
        collection,
        &query,
        NULL,  /* additional options */
        NULL); /* read prefs, NULL for default */

    const bson_t* doc;
    while (mongoc_cursor_next(cursor, &doc)) {
        char* str = bson_as_canonical_extended_json(doc, NULL);
        fprintf(stdout, "%s\n", str);
        bson_free(str);
    }

    if (mongoc_cursor_error(cursor, &error)) {
        fprintf(stderr, "Cursor Failure: %s\n", error.message);
        return;
    }

    bson_destroy(&query);
    mongoc_cursor_destroy(cursor);
    mongoc_collection_destroy(collection);
    mongoc_uri_destroy(uri);
    mongoc_client_destroy(client);
    mongoc_cleanup();
}

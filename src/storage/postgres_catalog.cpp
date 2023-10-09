#include "storage/postgres_catalog.hpp"
#include "storage/postgres_schema_entry.hpp"
#include "storage/postgres_transaction.hpp"
#include "postgres_connection.hpp"
#include "duckdb/storage/database_size.hpp"

namespace duckdb {

PostgresCatalog::PostgresCatalog(AttachedDatabase &db_p, const string &path, AccessMode access_mode)
    : Catalog(db_p), path(path), access_mode(access_mode) {
}

PostgresCatalog::~PostgresCatalog() {
}

void PostgresCatalog::Initialize(bool load_builtin) {
}

optional_ptr<CatalogEntry> PostgresCatalog::CreateSchema(CatalogTransaction transaction, CreateSchemaInfo &info) {
	throw BinderException("Postgres databases do not support creating new schemas");
}

void PostgresCatalog::ScanSchemas(ClientContext &context, std::function<void(SchemaCatalogEntry &)> callback) {
	auto &transaction = PostgresTransaction::Get(context, *this);
	for(auto &entry : transaction.GetSchemas().GetEntries()) {
		callback(entry.second->Cast<PostgresSchemaEntry>());
	}
}

optional_ptr<SchemaCatalogEntry> PostgresCatalog::GetSchema(CatalogTransaction transaction, const string &schema_name,
                                                          OnEntryNotFound if_not_found,
                                                          QueryErrorContext error_context) {
	if (schema_name == DEFAULT_SCHEMA) {
		return GetSchema(transaction, "public", if_not_found, error_context);
	}
	auto &postgres_transaction = PostgresTransaction::Get(transaction.GetContext(), *this);
	auto entry = postgres_transaction.GetSchemas().GetEntry(schema_name);
	if (!entry && if_not_found != OnEntryNotFound::RETURN_NULL) {
		throw BinderException("Schema with name \"%s\" not found", schema_name);
	}
	return reinterpret_cast<SchemaCatalogEntry *>(entry.get());
}

bool PostgresCatalog::InMemory() {
	return false;
}

string PostgresCatalog::GetDBPath() {
	return string();
}

void PostgresCatalog::DropSchema(ClientContext &context, DropInfo &info) {
	throw BinderException("Postgres databases do not support dropping schemas");
}

DatabaseSize PostgresCatalog::GetDatabaseSize(ClientContext &context) {
	throw InternalException("GetDatabaseSize");
}

unique_ptr<PhysicalOperator> PostgresCatalog::PlanUpdate(ClientContext &context, LogicalUpdate &op,
										unique_ptr<PhysicalOperator> plan) {
	throw InternalException("FIXME: PostgresCatalog::PlanUpdate");
}

unique_ptr<LogicalOperator> PostgresCatalog::BindCreateIndex(Binder &binder, CreateStatement &stmt, TableCatalogEntry &table,
											unique_ptr<LogicalOperator> plan) {
	throw InternalException("FIXME: PostgresCatalog::BindCreateIndex");
}


} // namespace duckdb


library(adbcdrivermanager)

# lhh-pgsql-public.ebi.ac.uk is 193.62.192.243
uri <- "postgresql://193.62.192.243:5432/pfmegrnargs?user=reader&password=NWDMCE5xdipIjRrp"

db <- adbc_database_init(adbcpostgresql::adbcpostgresql(), uri=uri)
con <- adbc_connection_init(db)

res <- read_adbc(con, "SELECT * FROM xref LIMIT 500")
print(class(res))
res

res$release()  # as we have not consumed the record
adbc_connection_release(con)
adbc_database_release(db)


## get the nanoarrow external pointer to the struct aka RecordBatch
narb <- acane::create_struct_example()

con <- DBI::dbConnect(duckdb::duckdb(), dbdir = ":memory:")
duckdb::duckdb_register(con, "pp", narb)
result <- DBI::dbGetQuery(con, "SELECT fred, ginger FROM pp where fred > 0")
result
DBI::dbDisconnect(con, shutdown=TRUE)



suppressMessages({
    library(dplyr)
    library(arrow)
})
ds <- arrow::InMemoryDataset$create(mtcars)
ds |>
  filter(mpg < 30) |>
  arrow::to_duckdb() |>
  group_by(cyl) |>
  summarize(mean_mpg = mean(mpg, na.rm = TRUE)) |>
  arrow::to_arrow() |>
  collect()

ds <- arrow::InMemoryDataset$create(mtcars)
ds |>
  filter(mpg < 30) |>
  arrow::to_duckdb() |>
  group_by(cyl) |>
  summarize(mean_mpg = mean(mpg, na.rm = TRUE)) |>
  arrow::to_arrow() -> ap
ap

pdf <- polars::as_polars_df(ap)
print(pdf)

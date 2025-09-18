
library(acane)

s <- create_struct_example()
expect_true(inherits(s, "nanoarrow_array"))
df <- as.data.frame(s)
expect_true(inherits(df, "data.frame"))
expect_equal(colnames(df), c("fred", "ginger"))
expect_equal(dim(df), c(4,2))
expect_true(inherits(df[,1], "integer"))
expect_true(inherits(df[,2], "numeric"))
expect_equal(df[,1], c(100L, 101L, NA, 102L))
expect_equal(df[,2], c(1.0, NA, 1.1, 1.2))

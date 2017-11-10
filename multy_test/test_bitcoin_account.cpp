/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/account.h"

#include "multy_test/serialized_keys_test_base.h"

#include "gtest/gtest.h"

namespace
{
SerializedKeyTestCase TEST_CASES[] = {
        {
            "L5GRrPvFZswYD74UdHWsg1yVbZqvMDe9jj6frutVx8Y6Y2mgWtEk",
            "",
            "12pWhnTAfMro4rJVk32YjvFq1NqtwmBNwU"
        },
        {
            "L33peSRvmY3MyRMdVqMKx72h4v8wugNhMwLCyzfD8xS1eZTRvkW1",
            "mUyLSFTU6XUuNYJjHEoMJiUEsZdva77TYxV8tY4PD2VN",
            "1fjRrB4XXJWeiw1686zCKYGSNjFqLchYQ"
        },
        {
            "L2ZG56cGSPDyvbAJoxUKmhUVRmKpG5bmRoY775ZYNPvvCnx58b9n",
            "oUfskkKeD4ttBMtdMkvZNHdCHjzquRqnFVuXTjkMKfYx",
            "14kHzG9194ojtiXFdbcdTkUCUsFbEfu5MW"
        },
        {
            "KwHeGE1qDjmBe4ecJoZww6WMj7BqnxsjXx4HWPneSRQDY7NBph4z",
            "fA7fqi3yibH7cUwfMH7BBaghhDYmZSLt5BmRUqe4koGH",
            "15XSfkydZEYkWyQGSUp9gGrC1dTi6wM2Ei"
        },
        {
            "L3fQB13UBadjNAGAZpL2us4xw6YWRV1ykgK8akvtQks3wQcgVAjG",
            "2Ahw45DdDSLcsSRbwiajV1QDzDaMfw2XH6Kt91ZRebDFv",
            "15prjQSoMX5cCKhxSAgcdz47bZRN2ge2th"
        },
        {
            "L3cN7tZ3gmnRQtFcqD6R44s6kzUfebUJ8GrmGCtWj65M6C9LX1J4",
            "2BQab9VLwL8HnDwJYvZqP4wntAx2MEVLHfFieTVs384Wu",
            "18NpPnJfTaDyAPaz36m3TUHcT77dGHRnoK"
        },
        {
            "KxgcSQsS3LZBHm1T6K8ijsNyobWEbN3v5QZKDPY4XSYoLCqdeHpr",
            "bjgoXKUmQnTYKxGU7VDC5RZ2WjGebudF7UoFJoY2Hqc8",
            "19cmiDmj4FwJVvcfDZXhC8kYeJLZytisZ8"
        },
        {
            "5KC4ejrDjv152FGwP386VD1i2NYc5KkfSMyv1nGy1VGDxGHqVY3",
            "MxXTacR6Fijwv6uAukGRUbb16J3UBjuxshFcCL8RzY6Cy6hM6b1Xv8kgmK3rnBTyP2"
            "m8yd7NxErcpy6vRKUZ4tRJ",
            "1F5y5E5FMc5YzdJtB9hLaUe43GDxEKXENJ"
        },
        {
            "5HtqcFguVHA22E3bcjJR2p4HHMEGnEXxVL5hnxmPQvRedSQSuT4",
            "PPT6wvt6t9EwpQpfba6LmL3ZoYqVfk5JoEEFB935rBs5t5uX4BjuhqUNB6wNwRzYv6"
            "hUFcWH4rbuwjMUMmRg2YBB",
            "1M9TbQnXzESQZjR1J3pMooQUpHEqUnrKZh"
        },
        {
            "L1gz2qcYqxn3rzMW5hdZZDAR8hB4mke6NnpYvjxGHSfYGJ9bFLJV",
            "ymybJgusbqjGD2oGw224ESjRTK3mJJKR6au6kPhagrkG",
            "1Mh7iAQstKFJDePMJ5MG1C1ztsnmuLV5Y6"
        },
        {
            "L5ABUi5Dup2KUXduFpRTa2jZyaK8Jv8gxKS71XXvCe2KWxqSWP6s",
            "jxJGdL3YiAQz3kzXDU1YniQUcSKoBtLd1qZefTRmhUYc",
            "1Mu8765kCAuP5NaoKZUMgBieTT7KqcUBbZ"
        },
        {
            "5HxWvvfubhXpYYpS3tJkw6fq9jE9j18THftkZjHHfmFiWtmAbrj",
            "MhYEtBZVFPa7stZRurPVT8JFGztqZLMj5f9K53BvLhqgZ9CfHsNDNFeiC5hkWb2GZs"
            "QALysHYrVPaURsNfaPER9r",
            "1QFqqMUD55ZV3PJEJZtaKCsQmjLT6JkjvJ"
        },
        {
            "KzpFvx31hSeesrRzEUYdZkcebNV5HfehYCdjtHB5kNEFBudXEBao",
            "",
            "12T6zBkXZT5Tyg1W7ssXL27MLoE3c8NmwX"
        }
};

INSTANTIATE_TEST_CASE_P(
        Bitcoin, SerializedKeyTestP,
        ::testing::Combine(
                ::testing::Values(CURRENCY_BITCOIN),
                ::testing::ValuesIn(TEST_CASES)));

} // namespace

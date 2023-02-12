//
//  ChunkAlloc.c
//  Allocators
//
//  Created by Agent5S on 27/1/23.
//

#include <ChunkAlloc.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdio.h>
#include <cmocka.h>

typedef struct 
{
    uint64_t a;
    uint64_t b;
} TestStruct;

static void test_chunk_alloc_init(void** state) {
    const int arrayLen = 4;

    ChunkAlloc alloc;
    //Test initialization with smaller-than-allowed size
    expect_assert_failure(ChunkAlloc_Init(&alloc, sizeof(size_t) - 1, 16));
    //Test normal initialization
    assert_return_code(ChunkAlloc_Init(&alloc, sizeof(TestStruct), arrayLen), 0);
    assert_non_null(alloc.pool);
    assert_int_equal(alloc.itemSize, sizeof(TestStruct));
    assert_int_equal(alloc.maxItems, arrayLen);
    assert_ptr_equal(alloc.current, alloc.pool);
    for (int i = 0; i < arrayLen - 2; i++) {
        TestStruct* chunk = &((TestStruct*)alloc.pool)[i];
        assert_ptr_equal(*(TestStruct**)chunk, &(((TestStruct*)alloc.pool)[i + 1]));
    }
    assert_null(((TestStruct**)alloc.pool)[arrayLen - 1]);

    //TODO: Test malloc failure

    ChunkAlloc_Deinit(&alloc);
}

static void test_chunk_alloc_deinit(void** state) {
    const int arrayLen = 4;

    ChunkAlloc alloc;
    ChunkAlloc_Init(&alloc, sizeof(TestStruct), arrayLen);
    ChunkAlloc_Deinit(&alloc);
    assert_null(alloc.pool);
    assert_null(alloc.current);
    assert_int_equal(alloc.itemSize, 0);
    assert_int_equal(alloc.maxItems, 0);
}

static void test_chunk_alloc_get(void** state) {
    const int arrayLen = 4;

    ChunkAlloc alloc;
    ChunkAlloc_Init(&alloc, sizeof(TestStruct), arrayLen);

    TestStruct* chunk = ChunkAlloc_Get(&alloc);
    assert_non_null(chunk);
    assert_ptr_equal(alloc.current, (TestStruct*)alloc.pool + 1);
    ChunkAlloc_Free(&alloc, chunk);
    chunk = NULL;

    ChunkAlloc_Deinit(&alloc);
}

static void test_chunk_alloc_get_multiple(void** state) {
    const int arrayLen = 4;

    ChunkAlloc alloc;
    ChunkAlloc_Init(&alloc, sizeof(TestStruct), arrayLen);

    TestStruct* chunks[arrayLen];
    for (int i = 0; i < arrayLen; i++) {
        chunks[i] = ChunkAlloc_Get(&alloc);
        assert_non_null(chunks[i]);
    }
    assert_null(alloc.current);

    //Test overallocating
    expect_assert_failure(ChunkAlloc_Get(&alloc));

    for (int i = 0; i < arrayLen; i++) {
        ChunkAlloc_Free(&alloc, chunks[i]);
        chunks[i] = NULL;
    }
    
    ChunkAlloc_Deinit(&alloc);
}

static void test_chunk_alloc_free(void** state) {
    const int arrayLen = 4;

    ChunkAlloc alloc;
    ChunkAlloc_Init(&alloc, sizeof(TestStruct), 4);

    TestStruct* chunk = ChunkAlloc_Get(&alloc);
    assert_ptr_equal(alloc.current, (TestStruct*)alloc.pool + 1);

    ChunkAlloc_Free(&alloc, chunk);
    assert_ptr_equal(alloc.current, chunk);
    assert_ptr_equal(*(void**)alloc.current, chunk + 1);
    //Correct usages should assign NULL to the chunk pointer

    //Test freeing memory out of the pool
    expect_assert_failure(ChunkAlloc_Free(&alloc, chunk - 1));
    expect_assert_failure(ChunkAlloc_Free(&alloc, chunk + arrayLen));

    //Test double freeing
    expect_assert_failure(ChunkAlloc_Free(&alloc, chunk));

    ChunkAlloc_Deinit(&alloc);
}

static void test_chunk_alloc_free_multiple(void** state) {
    const int arraySize = 4;

    ChunkAlloc alloc;
    ChunkAlloc_Init(&alloc, sizeof(TestStruct), arraySize);

    TestStruct* chunks[arraySize];
    for (int i = 0; i < arraySize; i++) {
        chunks[i] = ChunkAlloc_Get(&alloc);
        assert_non_null(chunks[i]);
    }
    assert_null(alloc.current);
    for (int i = 0; i < arraySize; i++) {
        ChunkAlloc_Free(&alloc, chunks[i]);
        assert_ptr_equal(alloc.current, chunks[i]);
        chunks[i] = NULL;
    }

    ChunkAlloc_Deinit(&alloc);
}

static void test_chunk_alloc_get_free_multiple(void** state) {
    uint8_t deallocArray[4] = { 2, 3, 1, 0 };
    const int arraySize = sizeof(deallocArray);
    const int deallocIdx1 = 0;
    const int deallocIdx2 = 2;

    ChunkAlloc alloc;
    ChunkAlloc_Init(&alloc, sizeof(TestStruct), arraySize);

    TestStruct* chunks[arraySize];
    for (int i = 0; i < arraySize; i++) {
        chunks[i] = ChunkAlloc_Get(&alloc);
        assert_non_null(chunks[i]);
    }
    assert_null(alloc.current);

    ChunkAlloc_Free(&alloc, chunks[deallocIdx1]);
    assert_ptr_equal(alloc.current, chunks[deallocIdx1]);
    assert_null(*(TestStruct**)alloc.current);
    //Correct usages should assign NULL to the chunk pointer

    ChunkAlloc_Free(&alloc, chunks[deallocIdx2]);
    assert_ptr_equal(alloc.current, chunks[deallocIdx2]);
    assert_ptr_equal(*(TestStruct**)alloc.current, chunks[deallocIdx1]);
    //Correct usages should assign NULL to the chunk pointer

    chunks[deallocIdx1] = ChunkAlloc_Get(&alloc);
    assert_ptr_equal(alloc.current, alloc.pool);

    chunks[deallocIdx2] = ChunkAlloc_Get(&alloc);
    assert_null(alloc.current);

    for (int i = 0; i < arraySize; i++) {
        uint8_t idx = deallocArray[i];
        ChunkAlloc_Free(&alloc, chunks[idx]);
        assert_ptr_equal(alloc.current, chunks[idx]);
        chunks[idx] = NULL;
    }

    ChunkAlloc_Deinit(&alloc);
}

int main(int argc, const char * argv[]) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_chunk_alloc_init),
        cmocka_unit_test(test_chunk_alloc_deinit),
        cmocka_unit_test(test_chunk_alloc_get),
        cmocka_unit_test(test_chunk_alloc_get_multiple),
        cmocka_unit_test(test_chunk_alloc_free),
        cmocka_unit_test(test_chunk_alloc_free_multiple),
        cmocka_unit_test(test_chunk_alloc_get_free_multiple),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}

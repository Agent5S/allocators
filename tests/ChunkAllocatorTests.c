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
#include <cmocka.h>

typedef struct 
{
    uint64_t a;
    uint64_t b;
} TestStruct;

static void test_chunk_alloc_init(void** state) {
    ChunkAlloc alloc;
    //TODO: Test asserts
    //expect_assert_failure(ChunkAlloc_Init(&alloc, sizeof(size_t) - 1, 16));
    
    const int arrayLen = 4;
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
    assert_null(((TestStruct**)alloc.pool)[3]);

    //TODO: Test malloc failure

    ChunkAlloc_Deinit(&alloc);
}

static void test_chunk_alloc_deinit(void** state) {
    ChunkAlloc alloc;

    ChunkAlloc_Init(&alloc, sizeof(TestStruct), 16);
    ChunkAlloc_Deinit(&alloc);
    assert_null(alloc.pool);
    assert_null(alloc.current);
    assert_int_equal(alloc.itemSize, 0);
    assert_int_equal(alloc.maxItems, 0);
}

static void test_chunk_alloc_get(void** state) {
    ChunkAlloc alloc;
    ChunkAlloc_Init(&alloc, sizeof(TestStruct), 8);

    TestStruct* chunk = ChunkAlloc_Get(&alloc);
    assert_non_null(chunk);
    assert_ptr_equal(alloc.current, (TestStruct*)alloc.pool + 1);
    ChunkAlloc_Free(&alloc, chunk);
    //Correct usages should assign NULL to the chunk pointer

    ChunkAlloc_Deinit(&alloc);
}

static void test_chunk_alloc_get_multiple(void** state) {
    ChunkAlloc alloc;
    ChunkAlloc_Init(&alloc, sizeof(TestStruct), 4);

    TestStruct* chunks[4];
    for (int i = 0; i < 4; i++) {
        chunks[i] = ChunkAlloc_Get(&alloc);
        assert_non_null(chunks[i]);
    }
    assert_null(alloc.current);
    for (int i = 0; i < 4; i++) {
        ChunkAlloc_Free(&alloc, chunks[i]);
        //Correct usages should assign NULL to the chunk pointer
    }

    //TODO: Test overflow

    ChunkAlloc_Deinit(&alloc);
}

static void test_chunk_alloc_free(void** state) {
    ChunkAlloc alloc;
    ChunkAlloc_Init(&alloc, sizeof(TestStruct), 8);

    TestStruct* chunk = ChunkAlloc_Get(&alloc);
    assert_ptr_equal(alloc.current, (TestStruct*)alloc.pool + 1);

    ChunkAlloc_Free(&alloc, chunk);
    assert_ptr_equal(alloc.current, chunk);
    assert_ptr_equal(*(void**)alloc.current, chunk + 1);
    //Correct usages should assign NULL to the chunk pointer

    //Test double freeing

    ChunkAlloc_Deinit(&alloc);
}

static void test_chunk_alloc_free_multiple(void** state) {
    ChunkAlloc alloc;
    ChunkAlloc_Init(&alloc, sizeof(TestStruct), 4);

    TestStruct* chunks[4];
    for (int i = 0; i < 4; i++) {
        chunks[i] = ChunkAlloc_Get(&alloc);
        assert_non_null(chunks[i]);
    }
    assert_null(alloc.current);
    for (int i = 0; i < 4; i++) {
        ChunkAlloc_Free(&alloc, chunks[i]);
        assert_ptr_equal(alloc.current, chunks[i]);
        //Correct usages should assign NULL to the chunk pointer
    }

    ChunkAlloc_Deinit(&alloc);
}

static void test_chunk_alloc_get_free_multiple(void** state) {
    ChunkAlloc alloc;
    ChunkAlloc_Init(&alloc, sizeof(TestStruct), 4);

    TestStruct* chunks[4];
    for (int i = 0; i < 4; i++) {
        chunks[i] = ChunkAlloc_Get(&alloc);
        assert_non_null(chunks[i]);
    }
    assert_null(alloc.current);

    ChunkAlloc_Free(&alloc, chunks[0]);
    assert_ptr_equal(alloc.current, chunks[0]);
    assert_null(*(TestStruct**)alloc.current);
    //Correct usages should assign NULL to the chunk pointer

    ChunkAlloc_Free(&alloc, chunks[2]);
    assert_ptr_equal(alloc.current, chunks[2]);
    assert_ptr_equal(*(TestStruct**)alloc.current, chunks[0]);
    //Correct usages should assign NULL to the chunk pointer

    chunks[0] = ChunkAlloc_Get(&alloc);
    assert_ptr_equal(alloc.current, alloc.pool);

    chunks[2] = ChunkAlloc_Get(&alloc);
    assert_null(alloc.current);

    uint8_t deallocOrder[4] = { 2, 3, 1, 0 };
    for (int i = 0; i < 4; i++) {
        uint8_t idx = deallocOrder[i];
        ChunkAlloc_Free(&alloc, chunks[idx]);
        assert_ptr_equal(alloc.current, chunks[idx]);
        //Correct usages should assign NULL to the chunk pointer
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

#include <assert.h>
#include <errno.h>
#include <stdio.h>
/* #include <process.h> */
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../epr_api.h"
#include "../epr_core.h"
#include "../epr_string.h"
#include "../epr_ptrarray.h"
#include "../epr_swap.h"
#include "../epr_field.h"
#include "../epr_record.h"
#include "../epr_param.h"
#include "../epr_dsd.h"
#include "../epr_msph.h"
#include "../epr_band.h"
#include "../epr_bitmask.h"

#include "../../bccunit/src/bccunit.h"

#if defined(WIN32) && defined(_DEBUG)
#include <crtdbg.h>
#endif /* if defined(WIN32) && defined(_DEBUG) */

static void loghandler(EPR_ELogLevel log_level, const char* log_message) {
}
char* init_test_data_path(int argc, char** argv);

char ll = e_log_debug;

BC_BEGIN_TEST(test_get_element_value)

    EPR_SProductId* product_id;
    EPR_SDatasetId* dataset_id = NULL;
    EPR_SRecord* record = NULL;
    EPR_SField* field = NULL;

    epr_init_api(ll, loghandler, NULL);

    product_id = epr_open_product("testdata/MER_RR__2PNRAL20100429_160201_000003102089_00040_42679_0001.N1");
    if (product_id == NULL)
       BC_ERROR("cannot open product");

    dataset_id = epr_get_dataset_id(product_id, "Tie_points_ADS");
    BC_ASSERT_NOT_NULL(dataset_id);

    record = epr_create_record(dataset_id);
    BC_ASSERT_NOT_NULL(record);

    record = epr_read_record(dataset_id, 2, record);
    BC_ASSERT_NOT_NULL(record);
    BC_ASSERT_NOT_NULL(record->fields);

    field = record->fields[4];

    BC_ASSERT_SAME(93, ((uint*) field->elems)[3]);

    epr_free_record(record);
    epr_close_product(product_id);
    epr_close_api();

BC_END_TEST()


void set_up_open_product_test(void)
{
    epr_init_api(ll, loghandler, NULL);
}

void tear_down_open_product_test(EPR_SProductId* product_id)
{
    if (product_id != NULL)
    {
        epr_close_product(product_id);
    }
    epr_close_api();
}

EPR_SProductId* test_epr_open_product_with_NULL(void)
{
    EPR_SProductId* product = NULL;
    product = epr_open_product(NULL);
    return product;
}

EPR_SProductId* test_epr_open_product_with_missing_file(void)
{
    EPR_SProductId* product_id = NULL;
    product_id = epr_open_product("D:/sdkjhg.db");
    return product_id;
}

EPR_SProductId* test_epr_open_product_with_bad_api_init_flag(void) {
    EPR_SProductId* product_id = NULL;
    epr_close_api();
    product_id = epr_open_product("testdata/MER_RR__2PNRAL20100429_160201_000003102089_00040_42679_0001.N1");
    return product_id;
}

EPR_SProductId* test_epr_open_product_OK(void) {
    EPR_SProductId* product_id = NULL;
    product_id = epr_open_product("testdata/MER_RR__2PNRAL20100429_160201_000003102089_00040_42679_0001.N1");
    return product_id;
}

BC_BEGIN_TEST(test_epr_open_product)
    EPR_SProductId* product_id = NULL;

    set_up_open_product_test();
    product_id = test_epr_open_product_with_NULL();
    BC_ASSERT_NULL(product_id);
    tear_down_open_product_test(product_id);

    set_up_open_product_test();
    product_id = test_epr_open_product_with_bad_api_init_flag();
    BC_ASSERT_NULL(product_id);
    tear_down_open_product_test(product_id);

    set_up_open_product_test();
    product_id = test_epr_open_product_with_missing_file();
    BC_ASSERT_NULL(product_id);
    tear_down_open_product_test(product_id);

    set_up_open_product_test();
    product_id = test_epr_open_product_OK();
    BC_ASSERT_NOT_NULL(product_id);
    tear_down_open_product_test(product_id);
BC_END_TEST()


EPR_SDatasetId* test_epr_get_ds_id_without_product_id(void)
{
    EPR_SDatasetId* dataset_id = NULL;
    dataset_id = epr_get_dataset_id(NULL, "Tie_points_ADS");
    return dataset_id;
}

EPR_SDatasetId* test_epr_get_ds_id_without_dsn(EPR_SProductId* product_id)
{
    EPR_SDatasetId* dataset_id = NULL;
    dataset_id = epr_get_dataset_id(product_id, NULL);
    return dataset_id;
}

BC_BEGIN_TEST(test_epr_get_dataset_id)
    EPR_SProductId* product_id = NULL;
    EPR_SDatasetId* dataset_id = NULL;

    dataset_id = test_epr_get_ds_id_without_product_id();
    BC_ASSERT_NULL(dataset_id);

    epr_init_api(ll, loghandler, NULL);
    product_id = epr_open_product("testdata/MER_RR__2PNRAL20100429_160201_000003102089_00040_42679_0001.N1");
    if (product_id == NULL)
        BC_ERROR("cannot open product");

    dataset_id = test_epr_get_ds_id_without_dsn(product_id);
    BC_ASSERT_NULL(dataset_id);

    epr_close_product(product_id);
    epr_close_api();
BC_END_TEST()

BC_BEGIN_TEST(test_epr_read_record)
    EPR_SProductId* product_id;
    EPR_SDatasetId* dataset_id = NULL;
    EPR_SRecord* record = NULL;

    epr_init_api(ll, loghandler, NULL);
    product_id = epr_open_product("testdata/MER_RR__2PNRAL20100429_160201_000003102089_00040_42679_0001.N1");
    if (product_id == NULL) {
        BC_FAIL("cannot open product");
    }

    dataset_id = epr_get_dataset_id(product_id, "Tie_points_ADS");

    record = epr_create_record(dataset_id);

    record = epr_read_record(NULL, 1, record);
    BC_ASSERT_NULL(record);

    record = epr_read_record(dataset_id, -1, NULL);
    BC_ASSERT_NULL(record);

    record = epr_read_record(dataset_id, dataset_id->dsd->num_dsr + 2, NULL);
    BC_ASSERT_NULL(record);

    epr_free_record(record);
    epr_close_product(product_id);
    epr_close_api();
BC_END_TEST()

BC_BEGIN_TEST(test_epr_get_data_type_size)
    BC_ASSERT_SAME(1,epr_get_data_type_size(e_tid_uchar));
    BC_ASSERT_SAME(1,epr_get_data_type_size(e_tid_char));
    BC_ASSERT_SAME(2,epr_get_data_type_size(e_tid_ushort));
    BC_ASSERT_SAME(2,epr_get_data_type_size(e_tid_short));
    BC_ASSERT_SAME(4,epr_get_data_type_size(e_tid_uint));
    BC_ASSERT_SAME(4,epr_get_data_type_size(e_tid_int));
    BC_ASSERT_SAME(4,epr_get_data_type_size(e_tid_float));
    BC_ASSERT_SAME(8,epr_get_data_type_size(e_tid_double));
    BC_ASSERT_SAME(12,epr_get_data_type_size(e_tid_time));
    BC_ASSERT_SAME(1,epr_get_data_type_size(e_tid_string));
    BC_ASSERT_SAME(0,epr_get_data_type_size(e_tid_unknown));
BC_END_TEST()

BC_BEGIN_TEST(test_epr_str_to_data_type_id)
    BC_ASSERT_SAME(e_tid_uchar, epr_str_to_data_type_id("UChar"));
    BC_ASSERT_SAME(e_tid_char, epr_str_to_data_type_id("AChar"));
    BC_ASSERT_SAME(e_tid_spare, epr_str_to_data_type_id("Spare"));
    BC_ASSERT_SAME(e_tid_ushort, epr_str_to_data_type_id("UShort"));
    BC_ASSERT_SAME(e_tid_short, epr_str_to_data_type_id("SShort"));
    BC_ASSERT_SAME(e_tid_uint, epr_str_to_data_type_id("ULong"));
    BC_ASSERT_SAME(e_tid_int, epr_str_to_data_type_id("SLong"));
    BC_ASSERT_SAME(e_tid_float, epr_str_to_data_type_id("Float"));
    BC_ASSERT_SAME(e_tid_double, epr_str_to_data_type_id("Double"));
    BC_ASSERT_SAME(e_tid_time, epr_str_to_data_type_id("@/types/UTC.dd"));
    BC_ASSERT_SAME(e_tid_string, epr_str_to_data_type_id("String"));
    BC_ASSERT_SAME(e_tid_unknown, epr_str_to_data_type_id("any other String"));
BC_END_TEST()

BC_BEGIN_TEST(test_epr_parse_header)
    EPR_SRecord* record = NULL;
    EPR_SField* field;

    record = epr_parse_header("mph", "NEGATIVE_VALUE=-999999<a>\n");
    BC_ASSERT_NOT_NULL(record);
    field = record->fields[0];
    BC_ASSERT_SAME(0, strcmp("NEGATIVE_VALUE",field->info->name));
    BC_ASSERT_SAME(-999999, ((int*) field->elems)[0]);
    epr_free_record(record);

    record = epr_parse_header("mph", "CLOCK_STEP=+3906250000<ps>\n");
    BC_ASSERT_NOT_NULL(record);
    field = record->fields[0];
    BC_ASSERT_SAME(0, strcmp("CLOCK_STEP",field->info->name));
    BC_ASSERT_SAME(3906250000ul, ((ulong*) field->elems)[0]);
    epr_free_record(record);

    record = epr_parse_header("mph", "SMALL_VALUES=+0000000000<bytes>\n");
    BC_ASSERT_NOT_NULL(record);
    field = record->fields[0];
    BC_ASSERT_SAME(0, strcmp("SMALL_VALUES",field->info->name));
    BC_ASSERT_SAME(0, ((ulong*) field->elems)[0]);
    epr_free_record(record);

    record = epr_parse_header("mph", "FOR_VALUES=+00000000000189633123<10-3nm>\n");
    BC_ASSERT_NOT_NULL(record);
    BC_ASSERT_SAME(1,record->num_fields);
    field = record->fields[0];
    BC_ASSERT_SAME(1,field->info->num_elems);
    BC_ASSERT_SAME(e_tid_uint,field->info->data_type_id);
    BC_ASSERT_SAME(4,record->info->tot_size);
    BC_ASSERT_SAME(0, strcmp("FOR_VALUES",field->info->name));
    BC_ASSERT_SAME(0, strcmp("10-3nm",field->info->unit));
    BC_ASSERT_SAME(189633123, ((uint*) field->elems)[0]);
    epr_free_record(record);

    record = epr_parse_header("mph", "UINT32_VALUES=+000000000004294967295<bytes>\n");
    BC_ASSERT_NOT_NULL(record);
    field = record->fields[0];
    BC_ASSERT_SAME(0, strcmp("UINT32_VALUES",field->info->name));
    BC_ASSERT_SAME(4294967295ul, ((ulong*) field->elems)[0]);
    epr_free_record(record);

    record = epr_parse_header("mph", "UINT32_VALUES=-000002147483647<bytes>\n");
    BC_ASSERT_NOT_NULL(record);
    field = record->fields[0];
    BC_ASSERT_SAME(e_tid_int,field->info->data_type_id);
    BC_ASSERT_SAME(0, strcmp("UINT32_VALUES",field->info->name));
    BC_ASSERT_SAME(-2147483647L, ((int*) field->elems)[0]);
    epr_free_record(record);

    record = epr_parse_header("mph", "Z_Velocity=-7377.4210000<m/s>\n");
    BC_ASSERT_NOT_NULL(record);
    BC_ASSERT_SAME(1,record->num_fields);
    field = record->fields[0];
    BC_ASSERT_SAME(1,field->info->num_elems);
    BC_ASSERT_SAME(e_tid_double,field->info->data_type_id);
    BC_ASSERT_SAME(8,record->info->tot_size);
    BC_ASSERT_SAME(0, strcmp("Z_Velocity",field->info->name));
    BC_ASSERT_SAME(0, strcmp("m/s",field->info->unit));
    BC_ASSERT_SAME(-7377.421, ((double*) field->elems)[0]);
    epr_free_record(record);

    record = epr_parse_header("mph", "STATE_VECTOR_TIME=\"20-JUN-2000 10:06:52.269120\"\n");
    BC_ASSERT_NOT_NULL(record);
    BC_ASSERT_SAME(1,record->num_fields);
    field = record->fields[0];
    BC_ASSERT_SAME(1,field->info->num_elems);
    BC_ASSERT_SAME(e_tid_string,field->info->data_type_id);
    BC_ASSERT_SAME(27,record->info->tot_size);
    BC_ASSERT_SAME(0, strcmp("STATE_VECTOR_TIME",field->info->name));
    BC_ASSERT_SAME(NULL,field->info->unit);
    BC_ASSERT_SAME(0, strcmp("20-JUN-2000 10:06:52.269120", (char*) field->elems));
    epr_free_record(record);

    record = epr_parse_header("mph", "BAND_WAVELEN=+0000412500+0000442500+0000490000+0000510000+0000560000+0000620000+0000665000+0000681250+0000705000+0000753750+0000760625+0000775000+0000865000+0000885000+0000900000<10-3nm>\n");
    BC_ASSERT_NOT_NULL(record);
    BC_ASSERT_SAME(1,record->num_fields);
    field = record->fields[0];
    BC_ASSERT_SAME(15,field->info->num_elems);
    BC_ASSERT_SAME(e_tid_uint,field->info->data_type_id);
    BC_ASSERT_SAME(60,record->info->tot_size);
    BC_ASSERT_SAME(0, strcmp("BAND_WAVELEN",field->info->name));
    BC_ASSERT_SAME(0, strcmp("10-3nm",field->info->unit));
    BC_ASSERT_SAME(412500, ((uint*) field->elems)[0]);
    BC_ASSERT_SAME(442500, ((uint*) field->elems)[1]);
    BC_ASSERT_SAME(490000, ((uint*) field->elems)[2]);
    BC_ASSERT_SAME(665000, ((uint*) field->elems)[6]);
    BC_ASSERT_SAME(760625, ((uint*) field->elems)[10]);
    BC_ASSERT_SAME(865000, ((uint*) field->elems)[12]);
    BC_ASSERT_SAME(900000, ((uint*) field->elems)[14]);
    epr_free_record(record);

      record = epr_parse_header("mph", "FOR_VALUES=+0049633000-4963300100-0049633002<10-3nm>\n");
    BC_ASSERT_NOT_NULL(record);
    BC_ASSERT_SAME(1,record->num_fields);
    field = record->fields[0];
    BC_ASSERT_SAME(3,field->info->num_elems);
    BC_ASSERT_SAME(0, strcmp("FOR_VALUES",field->info->name));
    BC_ASSERT_SAME(0, strcmp("10-3nm",field->info->unit));
    BC_ASSERT_SAME(e_tid_int,field->info->data_type_id);
    BC_ASSERT_SAME(49633000, ((uint*) field->elems)[0]);
    BC_ASSERT_NOT_SAME(-4963300100, ((uint*) field->elems)[1]);
    BC_ASSERT_SAME(-49633002, ((uint*) field->elems)[2]);
    BC_ASSERT_SAME(12,record->info->tot_size);
    epr_free_record(record);

    record = epr_parse_header("mph", "DS_TYPE=A\nNUM_DSR=+00000036         \n");
    BC_ASSERT_NOT_NULL(record);
    BC_ASSERT_SAME(2,record->num_fields);
    field = record->fields[0];
    BC_ASSERT_SAME(1,field->info->num_elems);
    BC_ASSERT_SAME(e_tid_uchar,field->info->data_type_id);
    BC_ASSERT_SAME(1, field->info->tot_size);
    BC_ASSERT_SAME(0, strcmp("DS_TYPE",field->info->name));
    BC_ASSERT_SAME(NULL,field->info->unit);
    BC_ASSERT_SAME('A', ((uchar*) field->elems)[0]);
    field = record->fields[1];
    BC_ASSERT_SAME(1,field->info->num_elems);
    BC_ASSERT_SAME(e_tid_uint,field->info->data_type_id);
    BC_ASSERT_SAME(4, field->info->tot_size);
    BC_ASSERT_SAME(0, strcmp("NUM_DSR",field->info->name));
    BC_ASSERT_SAME(NULL, field->info->unit);
    BC_ASSERT_SAME(36,((uint*) field->elems)[0]);
    BC_ASSERT_SAME(5,record->info->tot_size);
    epr_free_record(record);

    record = epr_parse_header("mph", "=\"20-JUN-2000\"");
    BC_ASSERT_NULL(record);

    record = epr_parse_header("mph", "20-JUN-2000");
    BC_ASSERT_NULL(record);

    record = epr_parse_header("mph", "X-Achse=");
    BC_ASSERT_NOT_NULL(record);
    BC_ASSERT_SAME(1,record->num_fields);
    field = record->fields[0];
    BC_ASSERT_SAME(1,field->info->num_elems);
    BC_ASSERT_SAME(e_tid_uchar,field->info->data_type_id);
    BC_ASSERT_SAME(0,record->info->tot_size);
    BC_ASSERT_SAME(0, strcmp("X-Achse",field->info->name));
    epr_free_record(record);

    record = epr_parse_header("mph", "X-Achse=XY");
    BC_ASSERT_NOT_NULL(record);
    BC_ASSERT_SAME(1,record->num_fields);
    field = record->fields[0];
    BC_ASSERT_SAME(1,field->info->num_elems);
    BC_ASSERT_SAME(e_tid_int,field->info->data_type_id);
    BC_ASSERT_SAME(4,record->info->tot_size);
    BC_ASSERT_SAME(0, strcmp("X-Achse",field->info->name));
    epr_free_record(record);

    record = epr_parse_header("mph", "Y=+0+");
    BC_ASSERT_NOT_NULL(record);
    BC_ASSERT_SAME(1,record->num_fields);
    field = record->fields[0];
    BC_ASSERT_SAME(1,field->info->num_elems);
    BC_ASSERT_SAME(e_tid_uint,field->info->data_type_id);
    BC_ASSERT_SAME(4,record->info->tot_size);
    BC_ASSERT_SAME(0, strcmp("Y",field->info->name));
    BC_ASSERT_SAME(NULL,field->info->unit);
    BC_ASSERT_SAME(0, ((uint*) field->elems)[0]);
    epr_free_record(record);

    record = epr_parse_header("mph", "X-Achse=+23-4XY");
    BC_ASSERT_NOT_NULL(record);
    BC_ASSERT_SAME(1,record->num_fields);
    field = record->fields[0];
    BC_ASSERT_SAME(2,field->info->num_elems);
    BC_ASSERT_SAME(e_tid_int,field->info->data_type_id);
    BC_ASSERT_SAME(8,record->info->tot_size);
    BC_ASSERT_SAME(23, ((int*) field->elems)[0]);
    epr_free_record(record);

    record = epr_parse_header("mph", "X=+ 23-4");
    BC_ASSERT_NOT_NULL(record);
    BC_ASSERT_SAME(1,record->num_fields);
    field = record->fields[0];
    BC_ASSERT_SAME(2,field->info->num_elems);
    BC_ASSERT_SAME(e_tid_int,field->info->data_type_id);
    BC_ASSERT_SAME(8,record->info->tot_size);
    BC_ASSERT_SAME(0, strcmp("X",field->info->name));
    epr_free_record(record);

    record = epr_parse_header("mph", "PRODUCT=\"MER_FR__2PTACR20000620_104323_00000099X000_00000_00000_0000.N1\"\nPROC_STAGE=T\nREF_DOC=\"PO-RS-MDA-GS-2009_3/B  \"\n                                        \nACQUISITION_STATION=\"ENVISAT SampleData#3\"\nPROC_TIME=\"22-FEB-2000 19:41:46.000000\"\n                                        \nPHASE=X\nREL_ORBIT=+00000\nDELTA_UT1=+.000000<s>\nX_POSITION=-7162215.231<m>\nVECTOR_SOURCE=\"00\"\nBANDWIDTH=+10001+10002+10003+10004+10005+10006<10-3nm>\nINST_FOV=+0000019151<10-6deg>\n");
    BC_ASSERT_NOT_NULL(record);
    BC_ASSERT_SAME(12,record->num_fields);
    field = record->fields[0];
    BC_ASSERT_SAME(1,field->info->num_elems);
    BC_ASSERT_SAME(e_tid_string,field->info->data_type_id);
    BC_ASSERT_SAME(62,field->info->tot_size);
    BC_ASSERT_SAME(0, strcmp("PRODUCT",field->info->name));
    field = record->fields[5];
    BC_ASSERT_SAME(1,field->info->num_elems);
    BC_ASSERT_SAME(e_tid_uchar,field->info->data_type_id);
    BC_ASSERT_SAME(1,field->info->tot_size);
    BC_ASSERT_SAME(0, strcmp("PHASE",field->info->name));
    BC_ASSERT_SAME('X', ((uchar*) field->elems)[0]);
    field = record->fields[8];
    BC_ASSERT_SAME(1,field->info->num_elems);
    BC_ASSERT_SAME(8,field->info->tot_size);
    BC_ASSERT_SAME(0, strcmp("X_POSITION",field->info->name));
    BC_ASSERT_SAME(0, strcmp("m",field->info->unit));
    BC_ASSERT_SAME(e_tid_double,field->info->data_type_id);
    BC_ASSERT_SAME(-7162215.231, ((double*) field->elems)[0]);
    field = record->fields[10];
    BC_ASSERT_SAME(6,field->info->num_elems);
    BC_ASSERT_SAME(24,field->info->tot_size);
    BC_ASSERT_SAME(0, strcmp("BANDWIDTH",field->info->name));
    BC_ASSERT_SAME(0, strcmp("10-3nm",field->info->unit));
    BC_ASSERT_SAME(e_tid_uint,field->info->data_type_id);
    BC_ASSERT_SAME(10001, ((uint*) field->elems)[0]);
    BC_ASSERT_SAME(10002, ((uint*) field->elems)[1]);
    BC_ASSERT_SAME(10003, ((uint*) field->elems)[2]);
    epr_free_record(record);
BC_END_TEST()

BC_BEGIN_TEST(test_epr_parse_band)
    EPR_SProductId* product_id = NULL;
    EPR_SBandId* band_id = NULL;

    epr_init_api(ll, epr_log_message, NULL);
    product_id = epr_open_product("testdata/MER_RR__2PNRAL20100429_160201_000003102089_00040_42679_0001.N1");
    band_id = epr_get_band_id(product_id, "kapusta");
    BC_ASSERT_NULL(band_id);

    BC_ASSERT_SAME(0.0, epr_interpolate2D(0.0, 0.0, 0.0, 1.0, 1.0, 2.0));
    BC_ASSERT_SAME(0.5, epr_interpolate2D(0.5, 0.0, 0.0, 1.0, 1.0, 2.0));
    BC_ASSERT_SAME(1.0, epr_interpolate2D(1.0, 0.0, 0.0, 1.0, 1.0, 2.0));
    BC_ASSERT_SAME(0.5, epr_interpolate2D(0.0, 0.5, 0.0, 1.0, 1.0, 2.0));
    BC_ASSERT_SAME(1.0, epr_interpolate2D(0.5, 0.5, 0.0, 1.0, 1.0, 2.0));
    BC_ASSERT_SAME(1.5, epr_interpolate2D(1.0, 0.5, 0.0, 1.0, 1.0, 2.0));
    BC_ASSERT_SAME(1.0, epr_interpolate2D(0.0, 1.0, 0.0, 1.0, 1.0, 2.0));
    BC_ASSERT_SAME(1.5, epr_interpolate2D(0.5, 1.0, 0.0, 1.0, 1.0, 2.0));
    BC_ASSERT_SAME(2.0, epr_interpolate2D(1.0, 1.0, 0.0, 1.0, 1.0, 2.0));

    epr_close_product(product_id);
    epr_close_api();
BC_END_TEST()

BC_BEGIN_TEST(test_tie_points_ADS_4_4)

    epr_init_api(ll, loghandler, NULL);

    EPR_SProductId* product_id = epr_open_product("testdata/MER_RR__2PNRAL20100429_160201_000003102089_00040_42679_0001.N1");
    EPR_SDatasetId* dataset_id = epr_get_dataset_id(product_id, "Tie_points_ADS");
    EPR_SRecord* record = epr_create_record(dataset_id);
    record = epr_read_record(dataset_id, 2, record);

    EPR_SField* field = record->fields[4];

    BC_ASSERT_SAME(93,((uint*) field->elems)[3]);

    epr_free_record(record);
    epr_close_product(product_id);
    epr_close_api();
BC_END_TEST()

int main(int argc, char** argv) {
    if (argc == 2 && epr_equal_names("nodebug", argv[1])) {
        ll = e_log_error;
    }
    BcTestSuite main_test_suite;
    BcTestSuite test_suite_epr_api;
    BcTestSuite test_suite_epr_core;
    BcTestSuite test_suite_epr_band;
    BcTestSuite test_suite_epr_header;

    BcTestResult result;

    main_test_suite = bc_create_test_suite("main_test_suite");

    test_suite_epr_api = bc_create_test_suite("test_suite_epr_api");
        bc_add_test_case(test_suite_epr_api,"test_get_element_value",test_get_element_value);
        bc_add_test_case(test_suite_epr_api,"test_epr_open_product",test_epr_open_product);
        bc_add_test_case(test_suite_epr_api,"test_epr_get_dataset_id",test_epr_get_dataset_id);
        bc_add_test_case(test_suite_epr_api,"test_epr_read_record",test_epr_read_record);
        bc_add_test_case(test_suite_epr_api,"test_tie_points_ADS_4_4",test_tie_points_ADS_4_4);

    test_suite_epr_core = bc_create_test_suite("test_suite_epr_core");
        bc_add_test_case(test_suite_epr_core,"test_epr_get_data_type_size",test_epr_get_data_type_size);
        bc_add_test_case(test_suite_epr_core,"test_epr_str_to_data_type_id", test_epr_str_to_data_type_id);

    test_suite_epr_header = bc_create_test_suite("test_suite_epr_header");
        bc_add_test_case(test_suite_epr_header,"test_epr_parse_header", test_epr_parse_header);

    test_suite_epr_band = bc_create_test_suite("test_suite_epr_band");
        bc_add_test_case(test_suite_epr_band,"test_epr_parse_band", test_epr_parse_band);

    bc_add_test(main_test_suite,test_suite_epr_api);
    bc_add_test(main_test_suite,test_suite_epr_core);
    bc_add_test(main_test_suite,test_suite_epr_band);
    bc_add_test(main_test_suite,test_suite_epr_header);

    bc_run_test(main_test_suite, &result);

    return 0;
}

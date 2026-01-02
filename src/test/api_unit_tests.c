#include <string.h>
#include <stdio.h>
#include <float.h>
#include <math.h>
#include <ctype.h>
#include "../epr_api.h"
#include "../epr_string.h"
#include "../epr_field.h"
/*#include "api_unit_tests.h"*/

#if defined(WIN32) && defined(_DEBUG)
#include <crtdbg.h>
#endif /* if defined(WIN32) && defined(_DEBUG) */

static int num_errors = 0;
static int num_failures = 0;
static int num_tests = 0;

const char* _prefix_in_p_p = "INPUT_PRODUCT_PATH";
const char* _prefix_assert = "assert";
const char* _prefix_product = "product.";

const char* _type_ascii_str = "ascii";
const char* _type_int8_str = "int8";
const char* _type_int16_str = "int16";
const char* _type_int32_str = "int32";
const char* _type_uint8_str = "uint8";
const char* _type_uint16_str = "uint16";
const char* _type_uint32_str = "uint32";
const char* _type_float32_str = "float32";
const char* _type_float64_str = "float64";
const char* _type_date_str = "date";
const char* _type_char_str = "char";

const char* _pas_band_ids_length = "band_ids.length";
const char* _pas_dataset_ids_length = "dataset_ids.length";
const char* _pas_dsd_array_length = "dsd_array.length";
const char* _pas_file_path = "file_path";
const char* _pas_id_string = "id_string";
const char* _pas_meris_iodd_version = "meris_iodd_version";
const char* _pas_mph_record_num_fields = "mph_record.num_fields";
const char* _pas_sph_record_num_fields = "sph_record.num_fields";
const char* _pas_param_table_length = "param_table.length";
const char* _pas_record_info_cache_length = "record_info_cache.length";
const char* _pas_scene_height = "scene_height";
const char* _pas_scene_width = "scene_width";
const char* _pas_tot_size = "tot_size";


enum LineTypeID {
    line_type_syntax_error = -1,
    line_type_empty = 0,
    line_type_assert_product = 1,
    line_type_assert_field = 2,
    line_type_input_product_path = 3,
    line_type_comment = 4,
    line_type_ignored = 5
};

union Value {
    char c;
    short s;
    int i;
    uchar uc;
    ushort us;
    uint ui;
    float f;
    double d;
    char* str;
};


struct TestDetail {
    int type;
    union Value expected;
    union Value actual;
    double precision;
};

struct FieldAddress {
    char* name_rec;
    char* name_field;
    unsigned int index_rec;
    unsigned int index_field;
};

typedef struct FieldAddress SFieldAddress;
typedef struct TestDetail STestDetail;

struct TestLine {
    int type;
    int line_no;
    char* line;
    char* value_ref;
    SFieldAddress* field_adress;
    STestDetail* test_detail;
    EPR_SProductId* product_id;
    char* msg;
};

typedef struct TestLine STestLine;

SFieldAddress* create_field_address(const char*);
void free_field_adress(SFieldAddress* fa);
STestDetail* create_test_detail(const int type);
void free_test_detail(STestDetail* test_detail);
STestLine* create_test_line(const char* line, const int line_no);
void free_test_line(STestLine* tl);

void print_warning_ignored(const int line_no, const char* line);
void print_failure_type(const STestLine* test_line, const int actual);
void print_syntax_error(const STestLine* test_line);
void print_field_adress_error(const STestLine* test_line);
void print_error_msg(const int line_no, const char* msg);
void print_failure_assert(const STestLine* test_line);
void set_expected_and_actual(const STestDetail* test_detail, char* expected, char* actual);

epr_boolean parse_index(char* str, int* idx);
epr_boolean parse_expected(const char* s, STestDetail* test_detail);
char* parse_input_product_path(const char* config_file);
const char* get_type_str(const int type);
void evaluate_product_assert(STestLine* test_line);
void evaluate_field_assert(const STestLine* test_line);
void test_api(const char*);
int parse_type(const char* s);
epr_boolean parse_field_adress(const char* line, SFieldAddress* fa);
void parse_test_line(STestLine* test_line);
epr_boolean get_field_value(const EPR_SField* field, const uint index, STestDetail* test_detail);
epr_boolean starts_with(const char* s, const char* prefix);
epr_boolean evaluate_test_detail(const STestDetail* test_detail);
epr_boolean evaluate_equal_types(const STestLine* test_line, const int actual);

epr_boolean equal_str(const char* expected, const char* actual);
epr_boolean equal_f(const float expected, const float actual, const double precision);
epr_boolean equal_d(const double expected, const double actual, const double precision);
epr_boolean equal_type(const int expected, const int actual);

int main(int argc, char** argv) {
    int i;

#if defined(WIN32) && defined(_DEBUG)
    /* Determine current attribute */
    int tmpFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);

    /* Enable memory leakage check bit */
    tmpFlag |= _CRTDBG_LEAK_CHECK_DF;

    /* Disable CRT block check bit */
    tmpFlag &= ~_CRTDBG_CHECK_CRT_DF;

    /* Set attribute to new value */
    _CrtSetDbgFlag(tmpFlag);

    /*_CrtSetBreakAlloc(4694);*/

#endif /* if defined(WIN32) && defined(_DEBUG) */

    /* Initialize the API. Set log-level to DEBUG and use default log-output (stdout) */
    epr_init_api(e_log_debug, epr_log_message, NULL);

    for (i = 1; i < argc; i++) {
        printf("\n");
        printf("\n");
        test_api(argv[i]);
    }
    printf("\n");
    printf("\n");

    /* Closes product reader API, release all allocated resources */
    epr_close_api();
    return 0;
}


void test_api(const char*  test_config_file) {
    FILE* stream;
    char line[1000];
    int line_no;
    char* input_product_path = NULL;
    EPR_SProductId* product_id = NULL;
    STestLine* test_line = NULL;

    assert(test_config_file != NULL);

    num_errors = 0;
    num_failures = 0;
    num_tests = 0;

    printf("running tests in %s\n", test_config_file);

    input_product_path = parse_input_product_path(test_config_file);
    if (input_product_path != NULL) {
        product_id = epr_open_product(input_product_path);
        if (product_id != NULL) {
            stream = fopen(test_config_file, "r");
            if (stream != NULL ) {
                for (line_no = 1; fgets(line, 1000, stream) != NULL; line_no ++) {
                    epr_trim_string(line);
                    test_line = create_test_line(line, line_no);
                    parse_test_line(test_line);
                    test_line->product_id = product_id;
                    switch (test_line->type) {
                    case line_type_assert_product:
                        evaluate_product_assert(test_line);
                        break;
                    case line_type_assert_field:
                        evaluate_field_assert(test_line);
                        break;
                    case line_type_syntax_error:
                        print_syntax_error(test_line);
                        num_errors++;
                        break;
                    case line_type_comment:
                    case line_type_input_product_path:
                    case line_type_empty:
                        break;
                    case line_type_ignored:
                    default:
                        print_warning_ignored(line_no, line);
                    }
                    free_test_line(test_line);
                }
                fclose(stream);
            }
            epr_close_product(product_id);
            printf("test summary for %s:\n", test_config_file);
            printf("  #tests:    %6d\n", num_tests);
            printf("  #failures: %6d\n", num_failures);
            printf("  #errors:   %6d\n", num_errors);
        } else {
            printf("error: failed to open input product\n");
        }
    } else {
        printf("error: failed to run %s\n", test_config_file);
    }

    epr_free_string(input_product_path);
}

void evaluate_product_assert(STestLine* test_line) {
    STestDetail* detail = NULL;
    const char* value_ref = NULL;
    const EPR_SProductId* product_id = NULL;

    assert(test_line != NULL);
    assert(test_line->type == line_type_assert_product);
    assert(test_line->test_detail != NULL);
    assert(test_line->value_ref != NULL);
    assert(test_line->product_id != NULL);

    detail = test_line->test_detail;
    value_ref = test_line->value_ref;
    product_id = test_line->product_id;

    if (equal_str(value_ref, _pas_band_ids_length)) {
        if (!evaluate_equal_types(test_line, e_tid_uint))
            goto failure;
        detail->actual.ui = product_id->band_ids == NULL ? 0 : product_id->band_ids->length;
    } else if (equal_str(value_ref, _pas_dataset_ids_length)) {
        if (!evaluate_equal_types(test_line, e_tid_uint))
            goto failure;
        detail->actual.ui = product_id->dataset_ids->length;
    } else if (equal_str(value_ref, _pas_dsd_array_length)) {
        if (!evaluate_equal_types(test_line, e_tid_uint))
            goto failure;
        detail->actual.ui = product_id->dsd_array->length;
    } else if (equal_str(value_ref, _pas_file_path)) {
        if (!evaluate_equal_types(test_line, e_tid_string))
            goto failure;
        detail->actual.str = epr_clone_string(product_id->file_path);
    } else if (equal_str(value_ref, _pas_id_string)) {
        if (!evaluate_equal_types(test_line, e_tid_string))
            goto failure;
        detail->actual.str = epr_clone_string(product_id->id_string);
    } else if (equal_str(value_ref, _pas_meris_iodd_version)) {
        if (!evaluate_equal_types(test_line, e_tid_int))
            goto failure;
        detail->actual.i = product_id->meris_iodd_version;
    } else if (equal_str(value_ref, _pas_mph_record_num_fields)) {
        if (!evaluate_equal_types(test_line, e_tid_uint))
            goto failure;
        detail->actual.ui = product_id->mph_record->num_fields;
    } else if (equal_str(value_ref, _pas_param_table_length)) {
        if (!evaluate_equal_types(test_line, e_tid_uint))
            goto failure;
        detail->actual.ui = product_id->param_table->length;
    } else if (equal_str(value_ref, _pas_record_info_cache_length)) {
        if (!evaluate_equal_types(test_line, e_tid_uint))
            goto failure;
        detail->actual.ui = product_id->record_info_cache->length;
    } else if (equal_str(value_ref, _pas_scene_height)) {
        if (!evaluate_equal_types(test_line, e_tid_uint))
            goto failure;
        detail->actual.ui = product_id->scene_height;
    } else if (equal_str(value_ref, _pas_scene_width)) {
        if (!evaluate_equal_types(test_line, e_tid_uint))
            goto failure;
        detail->actual.ui = product_id->scene_width;
    } else if (equal_str(value_ref, _pas_sph_record_num_fields)) {
        if (!evaluate_equal_types(test_line, e_tid_uint))
            goto failure;
        detail->actual.ui = product_id->sph_record->num_fields;
    } else if (equal_str(value_ref, _pas_tot_size)) {
        if (!evaluate_equal_types(test_line, e_tid_uint))
            goto failure;
        detail->actual.ui = product_id->tot_size;
    } else {
        test_line->msg = epr_clone_string("value reference not supported");
        print_syntax_error(test_line);
        goto error;
    }

    if (!evaluate_test_detail(detail)) {
        print_failure_assert(test_line);
        goto failure;
    }
    return;
failure:
    num_failures++;
    return;
error:
    num_errors++;
    return;
}

void evaluate_field_assert(const STestLine* test_line) {
    SFieldAddress* field_adress = NULL;
    STestDetail* test_detail = NULL;
    const EPR_SField* field = NULL;
    EPR_SDatasetId* dataset_id = NULL;
    EPR_SRecord* record = NULL;
    epr_boolean is_mph_or_sph = FALSE;

    assert(test_line != NULL);
    assert(test_line->type == line_type_assert_field);
    assert(test_line->test_detail != NULL);
    assert(test_line->field_adress != NULL);
    assert(test_line->product_id != NULL);

    field_adress = test_line->field_adress;
    test_detail = test_line->test_detail;

    if (equal_str(field_adress->name_rec, "mph")) {
        record = epr_get_mph(test_line->product_id);
        is_mph_or_sph = TRUE;
    } else if (equal_str(field_adress->name_rec, "sph")) {
        record = epr_get_sph(test_line->product_id);
        is_mph_or_sph = TRUE;
    } else {
        dataset_id = epr_get_dataset_id(test_line->product_id, field_adress->name_rec);
        if (dataset_id == NULL)
            goto fin_inexistent;
        record = epr_read_record(dataset_id, field_adress->index_rec, NULL);
    }
    if (record == NULL)
        goto fin_inexistent;
    field = epr_get_field(record, field_adress->name_field);
    if (field == NULL)
        goto fin_inexistent;
    if (field_adress->index_field >= field->info->num_elems)
        goto fin_inexistent;
    if (!evaluate_equal_types(test_line, field->info->data_type_id))
        goto failure;
    if (!get_field_value(field, field_adress->index_field, test_detail)) {
        print_error_msg(test_line->line_no, "can not get the field value to test");
        goto error;
    }

    if (evaluate_test_detail(test_detail)) {
        goto finally;
    } else {
        print_failure_assert(test_line);
        goto failure;
    }

failure:
    num_failures++;
    goto finally;

fin_inexistent:
    print_field_adress_error(test_line);

error:
    num_errors++;

finally:
    if (!is_mph_or_sph) {
        epr_free_record(record);
    }
}

epr_boolean get_field_value(const EPR_SField* field, const uint index, STestDetail* test_detail) {
    char buffer[1000];
    EPR_STime* time = NULL;

    assert(field != NULL);
    assert(field->info != NULL);
    assert(index < field->info->num_elems);
    assert(test_detail != NULL);
    assert(field->info->data_type_id == test_detail->type);

    buffer[0] = '\0';

    switch (test_detail->type) {
    case e_tid_string:
        test_detail->actual.str = epr_clone_string((const char*) field->elems);
        return TRUE;
    case e_tid_time:
        time = (EPR_STime*) field->elems;
        sprintf(buffer, "d=%d s=%u ms=%u", time->days, time->seconds, time->microseconds);
        test_detail->actual.str = epr_clone_string(buffer);
        return TRUE;
    case e_tid_spare:
        /*todo*/
        return FALSE;
    case e_tid_uchar:
        test_detail->actual.uc = ((uchar*) field->elems)[index];
        return TRUE;
    case e_tid_char:
        test_detail->actual.c = ((char*) field->elems)[index];
        return TRUE;
    case e_tid_ushort:
        test_detail->actual.us = ((ushort*) field->elems)[index];
        return TRUE;
    case e_tid_short:
        test_detail->actual.s = ((short*) field->elems)[index];
        return TRUE;
    case e_tid_uint:
        test_detail->actual.ui = ((uint*) field->elems)[index];
        return TRUE;
    case e_tid_int:
        test_detail->actual.i = ((int*) field->elems)[index];
        return TRUE;
    case e_tid_float:
        test_detail->actual.f = ((float*) field->elems)[index];
        return TRUE;
    case e_tid_double:
        test_detail->actual.d = ((double*) field->elems)[index];
        return TRUE;
    default:
        return FALSE;
    }
}

void parse_test_line(STestLine* test_line) {
    /* unsigned */ int pos_tok = 0;
    int i = 0;
    int len = 0;
    const int num_chars = 4;
    char* chars[] = {NULL, NULL, NULL, NULL};
    SFieldAddress* field_adress = NULL;
    STestDetail* test_detail = NULL;

    assert(test_line != NULL);
    assert(test_line->line != NULL);

    chars[0] = epr_str_tok(test_line->line, " ", &pos_tok);
    chars[1] = epr_str_tok(test_line->line, " ", &pos_tok);
    chars[2] = epr_str_tok(test_line->line, "=", &pos_tok);
    chars[3] = epr_clone_string(test_line->line + pos_tok);
    for (i = 0; i < num_chars; i++) {
        if (chars[i] != NULL) {
                chars[i] = epr_trim_string(chars[i]);
        }
    }

    if (chars[0] == NULL || strlen(chars[0]) == 0) {
        test_line->type = line_type_empty;
    } else if (starts_with(chars[0], "#")) {
        test_line->type = line_type_comment;
    } else if (starts_with(chars[0], _prefix_in_p_p)) {
        test_line->type = line_type_input_product_path;
    } else if (equal_str(chars[0], _prefix_assert)) {
        num_tests++;
        if (chars[1] == NULL || strlen(chars[1]) == 0 || parse_type(chars[1]) == e_tid_unknown) {
            test_line->type = line_type_syntax_error;
            test_line->msg = epr_clone_string("unknown value type");
        } else if (chars[2] == NULL || strlen(chars[2]) == 0) {
            test_line->type = line_type_syntax_error;
            test_line->msg = epr_clone_string("empty value reference");
        } else if (chars[3] == NULL || strlen(chars[3]) == 0) {
            test_line->type = line_type_syntax_error;
            test_line->msg = epr_clone_string("empty expected value");
        } else if(starts_with(chars[2], _prefix_product)) {
            len = strlen(_prefix_product);
            for (i = 0; i < len; i++) {
                chars[2][i] = ' ';
            }
            epr_trim_string(chars[2]);
            if (strlen(chars[2]) == 0) {
                test_line->type = line_type_syntax_error;
                test_line->msg = epr_clone_string("empty value reference");
            } else {
                test_detail = create_test_detail(parse_type(chars[1]));
                if (parse_expected(chars[3], test_detail)) {
                    test_line->test_detail = test_detail;
                    test_line->type = line_type_assert_product;
                    test_line->value_ref = epr_clone_string(chars[2]);
                } else {
                    test_line->type = line_type_syntax_error;
                    test_line->msg = epr_clone_string("unknown expected value");
                    free_test_detail(test_detail);
                }
            }
        } else {
            field_adress = create_field_address(chars[2]);
            if (field_adress != NULL) {
                test_detail = create_test_detail(parse_type(chars[1]));
                if (parse_expected(chars[3], test_detail)) {
                    test_line->test_detail = test_detail;
                    test_line->type = line_type_assert_field;
                    test_line->field_adress = field_adress;
                } else {
                    test_line->type = line_type_syntax_error;
                    test_line->msg = epr_clone_string("unknown expected value");
                    free_test_detail(test_detail);
                }
            } else {
                test_line->type = line_type_syntax_error;
                test_line->msg = epr_clone_string("can not read field identifier");
                free_field_adress(field_adress);
            }
        }
    } else {
        test_line->type = line_type_syntax_error;
        test_line->msg = epr_clone_string("arguments not supported");
    }

    for (i = 0; i < num_chars; i++) {
        epr_free_string(chars[i]);
    }
}

char* parse_input_product_path(const char* config_file) {
    FILE* stream;
    /* int ipp_length; */
    char line[400];
    char buffer[400];
    char* pos_equal_sign;
    stream = fopen(config_file, "r");
    if (stream != NULL ) {
        /* ipp_length = strlen(_prefix_in_p_p); */
        while (fgets(line, 400, stream) != NULL) {
            if (strstr(line, _prefix_in_p_p) == NULL || strcmp(line, strstr(line, _prefix_in_p_p)) != 0) {
                continue;
            }
            pos_equal_sign = strstr(line, "=");
            if (pos_equal_sign == NULL) {
                continue;
            }
            epr_trim_string(pos_equal_sign);
            if (strlen(pos_equal_sign) < 2) {
                continue;
            }
            strcpy(buffer, pos_equal_sign + 1);
            if (strlen(buffer) == 0) {
                continue;
            }
            fclose(stream);
            printf("using input product %s\n", buffer);
            return epr_clone_string(buffer);
        }
        fclose(stream);
        printf("error: no input product defined in %s\n", config_file);
        return NULL;
    } else {
        printf("error: failed to open file %s\n", config_file);
        return NULL;
    }
}


STestDetail* create_test_detail(const int type) {
    STestDetail* test_detail = NULL;
    test_detail = (STestDetail*) calloc(1, sizeof (STestDetail));
    test_detail->type = type;
    switch (type) {
    case e_tid_float:
        test_detail->precision = 10e-6;
        break;
    case e_tid_double:
        test_detail->precision = 10e-10;
        break;
    }
    return test_detail;
}

void free_test_detail(STestDetail* test_detail) {
    if (test_detail != NULL) {
        switch (test_detail->type) {
        case e_tid_time:
        case e_tid_spare:
        case e_tid_string:
            epr_free_string(test_detail->actual.str);
            epr_free_string(test_detail->expected.str);
            break;
        }
        free(test_detail);
    }
}

SFieldAddress* create_field_address(const char* s) {
    SFieldAddress* field_adress = NULL;

    field_adress = (SFieldAddress*) calloc(1, sizeof (SFieldAddress));
    if (!parse_field_adress(s, field_adress)) {
        free_field_adress(field_adress);
        return NULL;
    }
    return field_adress;
}

void free_field_adress(SFieldAddress* fa) {
    if (fa != NULL) {
        epr_free_string(fa->name_field) ;
        epr_free_string(fa->name_rec);
        free(fa);
    }
}

STestLine* create_test_line(const char* line, const int line_no) {
    STestLine* test_line = NULL;

    assert(line != NULL);

    test_line = (STestLine*) calloc(1, sizeof(STestLine));
    test_line->type = line_type_ignored;
    test_line->line = epr_clone_string(line);
    test_line->line_no = line_no;
    return test_line;
}

void free_test_line(STestLine* test_line) {
    if (test_line != NULL) {
        free_field_adress(test_line->field_adress);
        epr_free_string(test_line->line);
        free_test_detail(test_line->test_detail);
        epr_free_string(test_line->value_ref);
        epr_free_string(test_line->msg);
        free(test_line);
    }
}

epr_boolean starts_with(const char* s, const char* prefix) {
    assert(s != NULL);
    assert(prefix != NULL);
    if (strstr(s, prefix) != NULL && strcmp(strstr(s, prefix), s) == 0) {
        return TRUE;
    }
    return FALSE;
}

int parse_type(const char* s) {

    assert(s != NULL);

    if (equal_str(s, _type_ascii_str)) {
        return e_tid_string;
    } else if (equal_str(s, _type_int8_str)) {
        return e_tid_char;
    } else if (equal_str(s, _type_int16_str)) {
        return e_tid_short;
    } else if (equal_str(s, _type_int32_str)) {
        return e_tid_int;
    } else if (equal_str(s, _type_uint8_str)) {
        return e_tid_uchar;
    } else if (equal_str(s, _type_uint16_str)) {
        return e_tid_ushort;
    } else if (equal_str(s, _type_uint32_str)) {
        return e_tid_uint;
    } else if (equal_str(s, _type_float32_str)) {
        return e_tid_float;
    } else if (equal_str(s, _type_float64_str)) {
        return e_tid_double;
    } else if (equal_str(s, _type_date_str)) {
        return e_tid_time;
    } else if (equal_str(s, _type_char_str)) {
        return e_tid_spare;
    } else {
        return e_tid_unknown;
    }
}

const char* get_type_str(const int type) {
    switch (type) {
    case e_tid_string:
        return _type_ascii_str;
    case e_tid_char:
        return _type_int8_str;
    case e_tid_short:
        return _type_int16_str;
    case e_tid_int:
        return _type_int32_str;
    case e_tid_uchar:
        return _type_uint8_str;
    case e_tid_ushort:
        return _type_uint16_str;
    case e_tid_uint:
        return _type_uint32_str;
    case e_tid_float:
        return _type_float32_str;
    case e_tid_double:
        return _type_float64_str;
    case e_tid_time:
        return _type_date_str;
    case e_tid_spare:
        return _type_char_str;
    default:
        return "unknown type";
    }
}

char* parse_string(const char* s) {
    int len, i;
    char buffer[1000];

    assert(s != NULL);

    strcpy(buffer, s);
    epr_trim_string(buffer);
    len = strlen(buffer);
    if (len == 0) {
        return NULL;
    } else if (len == 1) {
        if (buffer[0] == '"')
            buffer[0] = ' ';
    } else {
        for (i = 0; i < len; i += len -1) {
            if (buffer[i] == '"')
                buffer[i] = ' ';
        }
    }
    epr_trim_string(buffer);
    if (strlen(buffer) == 0)
        return NULL;
    return epr_clone_string(buffer);
}

epr_boolean is_a_long(const char* s, int base) {
    char* end_pos = NULL;

    if (s != NULL && strlen(s) > 0) {
        strtol(s, &end_pos, base);
        if (end_pos[0] == '\0') {
            return TRUE;
        }
    }
    return FALSE;
}
epr_boolean is_an_ulong(const char* s, int base) {
    char* end_pos = NULL;

    if (s != NULL && strlen(s) > 0) {
        strtoul(s, &end_pos, base);
        if (end_pos[0] == '\0') {
            return TRUE;
        }
    }
    return FALSE;
}
epr_boolean is_a_double(const char* s) {
    char* end_pos = NULL;

    if (s != NULL && strlen(s) > 0) {
        strtod(s, &end_pos);
        if (end_pos[0] == '\0') {
            return TRUE;
        }
    }
    return FALSE;
}


epr_boolean parse_expected(const char* s, STestDetail* test_detail) {
    assert(s != NULL);
    assert(test_detail != NULL);

    switch (test_detail->type) {
    case e_tid_time:
    case e_tid_spare:
    case e_tid_string:
        test_detail->expected.str = parse_string(s);
        break;
    case e_tid_char:
        if (!is_a_long(s, 10)) return FALSE;
        test_detail->expected.c = (char) strtol(s, NULL, 10);
        break;
    case e_tid_short:
        if (!is_a_long(s, 10)) return FALSE;
        test_detail->expected.s = (short) strtol(s, NULL, 10);
        break;
    case e_tid_int:
        if (!is_a_long(s, 10)) return FALSE;
        test_detail->expected.i = strtol(s, NULL, 10);
        break;
    case e_tid_uchar:
        if (!is_an_ulong(s, 10)) return FALSE;
        test_detail->expected.uc = (unsigned char) strtoul(s, NULL, 10);
        break;
    case e_tid_ushort:
        if (!is_an_ulong(s, 10)) return FALSE;
        test_detail->expected.us = (unsigned short) strtoul(s, NULL, 10);
        break;
    case e_tid_uint:
        if (!is_an_ulong(s, 10)) return FALSE;
        test_detail->expected.ui = strtoul(s, NULL, 10);
        break;
    case e_tid_float:
        if (!is_a_double(s)) return FALSE;
        test_detail->expected.f = (float) strtod(s, NULL);
        break;
    case e_tid_double:
        if (!is_a_double(s)) return FALSE;
        test_detail->expected.d = strtod(s, NULL);
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

epr_boolean parse_field_adress(const char* str, SFieldAddress* field_adress) {
    char* temp = NULL;
    int pos = 0;
    epr_boolean result = FALSE;

    assert(str != NULL);
    assert(field_adress != NULL);

    field_adress->name_rec = epr_str_tok(str, ".", &pos);
    field_adress->name_field = epr_str_tok(str, ".", &pos);
    temp = epr_str_tok(str, ".", &pos);
    if (temp != NULL
            || field_adress->name_rec == NULL
            || field_adress->name_field == NULL
            || strstr(field_adress->name_rec, "[") == field_adress->name_rec
            || strstr(field_adress->name_field, "[") == field_adress->name_field)
        goto finally;
    if (!parse_index(field_adress->name_rec, (int*)&field_adress->index_rec))
        goto finally;
    if (!parse_index(field_adress->name_field, (int*)&field_adress->index_field))
        goto finally;

    result = TRUE;

    finally:
    epr_free_string(temp);

    return result;
}

epr_boolean parse_index(char* str, int* idx) {
    int temp_idx = 0;
    char* idx_pars_start = NULL;
    char* idx_pars_end = NULL;

    if (str == NULL)
        return FALSE;

    idx_pars_start = strstr(str, "[");
    if (idx_pars_start != NULL) {
        temp_idx = strtol(idx_pars_start + 1, &idx_pars_end, 10);
        if (!equal_str("]", idx_pars_end))
            return FALSE;
        idx_pars_start[0] = '\0';
    }
    *idx = temp_idx;
    return TRUE;
}

epr_boolean evaluate_test_detail(const STestDetail* test_detail) {
    assert(test_detail != NULL);
    assert(test_detail->type != e_tid_unknown);

    switch (test_detail->type) {
    case e_tid_char:
        return test_detail->expected.c == test_detail->actual.c ? TRUE: FALSE;
    case e_tid_short:
        return test_detail->expected.s == test_detail->actual.s ? TRUE: FALSE;
    case e_tid_int:
        return test_detail->expected.i == test_detail->actual.i ? TRUE: FALSE;
    case e_tid_uchar:
        return test_detail->expected.uc == test_detail->actual.uc ? TRUE: FALSE;
    case e_tid_ushort:
        return test_detail->expected.us == test_detail->actual.us ? TRUE: FALSE;
    case e_tid_uint:
        return test_detail->expected.ui == test_detail->actual.ui ? TRUE: FALSE;
    case e_tid_float:
        return equal_f(test_detail->expected.f, test_detail->actual.f, test_detail->precision);
    case e_tid_double:
        return equal_d(test_detail->expected.d, test_detail->actual.d, test_detail->precision);
    case e_tid_time:
    case e_tid_spare:
    case e_tid_string:
        return equal_str(test_detail->expected.str, test_detail->actual.str);
    default:
        return FALSE;
    }
}

epr_boolean equal_str(const char* expected, const char* actual) {
    return strcmp(expected, actual)== 0? TRUE: FALSE;
}

epr_boolean equal_l(const long expected, const long actual) {
    return expected == actual? TRUE: FALSE;
}

epr_boolean equal_f(const float expected, const float actual, const double precision) {
    return fabs(expected - actual) < precision ? TRUE: FALSE;;
}

epr_boolean equal_d(const double expected, const double actual, const double precision) {
    return fabs(expected - actual) < precision ? TRUE: FALSE;;
}

epr_boolean equal_type(const int expected, const int actual) {
    return equal_l(expected, actual);
}

epr_boolean evaluate_equal_types(const STestLine* test_line, const int actual) {
    epr_boolean result = FALSE;

    assert(test_line != NULL);
    assert(test_line->test_detail != NULL);

    result = equal_type(test_line->test_detail->type, actual);

    if (!result) {
        print_failure_type(test_line, actual);
    }

    return result;
}

void print_warning_ignored(const int line_no, const char* line) {
    printf("warning: line %d: ignored\n", line_no);
/*    printf("   %s\n", line); */
}

void print_failure_type(const STestLine* test_line, const int actual) {
    assert(test_line != NULL);
    assert(test_line->test_detail != NULL);

    printf("failure: line %d: type assertion failed: expected [%s], but was [%s]\n",
        test_line->line_no,
        get_type_str(test_line->test_detail->type),
        get_type_str(actual));
/*    printf("   %s\n", test_line->line);*/
}


void print_syntax_error(const STestLine* test_line) {
    printf("syntax error: line %d:  %s\n", test_line->line_no, test_line->msg);
/*    printf("   %s\n", test_line->line);*/
}


void print_error_msg(const int line_no, const char* msg) {
    assert(msg != NULL);
    printf("error: line %d:  %s\n", line_no, msg);
/*    printf("   %s\n", test_line->line);*/
}

void print_field_adress_error(const STestLine* test_line) {
    char buffer[500];
    SFieldAddress* fa = NULL;

    assert(test_line != NULL);
    assert(test_line->field_adress != NULL);

    fa = test_line->field_adress;
    buffer[0] = '\0';
    sprintf(buffer, "nonexistent field: %s[%d].%s[%d]",
        fa->name_rec, fa->index_rec,
        fa->name_field, fa->index_field);
    print_error_msg(test_line->line_no, buffer);
}

void print_failure_assert(const STestLine* test_line) {
    STestDetail* test_detail = NULL;
    char expected[1000];
    char actual[1000];

    assert(test_line != NULL);
    assert(test_line->test_detail != NULL);

    test_detail = test_line->test_detail;

    switch (test_detail->type) {
    case e_tid_string:
        set_expected_and_actual(test_detail, expected, actual);
        break;
    case e_tid_char:
        sprintf(expected, "%i", test_detail->expected.c);
        sprintf(actual, "%i", test_detail->actual.c);
        break;
    case e_tid_short:
        sprintf(expected, "%i", test_detail->expected.s);
        sprintf(actual, "%i", test_detail->actual.s);
        break;
    case e_tid_int:
        sprintf(expected, "%d", test_detail->expected.i);
        sprintf(actual, "%d", test_detail->actual.i);
        break;
    case e_tid_uchar:
        sprintf(expected, "%u", test_detail->expected.uc);
        sprintf(actual, "%u", test_detail->actual.uc);
        break;
    case e_tid_ushort:
        sprintf(expected, "%u", test_detail->expected.us);
        sprintf(actual, "%u", test_detail->actual.us);
        break;
    case e_tid_uint:
        sprintf(expected, "%u", test_detail->expected.ui);
        sprintf(actual, "%u", test_detail->actual.ui);
        break;
    case e_tid_float:
        sprintf(expected, "%G", test_detail->expected.f);
        sprintf(actual, "%G", test_detail->actual.f);
        break;
    case e_tid_double:
        sprintf(expected, "%G", test_detail->expected.d);
        sprintf(actual, "%G", test_detail->actual.d);
        break;
    case e_tid_time:
        set_expected_and_actual(test_detail, expected, actual);
        break;
    case e_tid_spare:
        set_expected_and_actual(test_detail, expected, actual);
        break;
    default:;
    }

    printf("failure: line %d: assertion failed:\n   expected: %s\n   but was : %s\n",
        test_line->line_no,
        expected,
        actual);
/*    printf("   %s\n", test_line->line);*/
}

void set_expected_and_actual(const STestDetail* test_detail, char* expected, char* actual) {
    assert(test_detail != NULL);
    if (test_detail->expected.str == NULL) {
        sprintf(expected, "NULL");
    } else {
        sprintf(expected, "%s", test_detail->expected.str);
    }
    if (test_detail->actual.str == NULL) {
        sprintf(actual, "NULL");
    } else {
        sprintf(actual, "%s", test_detail->actual.str);
    }
}

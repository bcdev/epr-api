#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../epr_api.h"


#define EPR_RAW_NAME "epr_subset_test"
#define EPR_RAW_VERSION "1.0"

#if defined(WIN32) && defined(_DEBUG)
#include <crtdbg.h>
#endif /* if defined(WIN32) && defined(_DEBUG) */

/* HELPER STRUCTURES */
typedef struct Extrema
{
    float min;
    float max;
} SExtrema;


#ifdef _MAX_PATH
    #define PATH_MAX  _MAX_PATH
#else
    #define PATH_MAX  1023
#endif

/* HELPER FUNCTIONS */
EPR_SRaster* make_layer(EPR_SProductId* product_id, const char* ds_name, uint source_w, uint source_h, uint subsampl);
SExtrema get_raster_minmax(EPR_SRaster* raster);

void printVersion(void)
{
    printf("%s, version %s (c) by Brockmann Consult\n", EPR_RAW_NAME, EPR_RAW_VERSION);
}

void printUsage(void)
{
    printf("usage: %s <product> <band> [<sub-sampl>]\n", EPR_RAW_NAME);
}

/**
 *
 * Call: epr_asar_test <ENVISAT-Product file path>
 *                     <Dataset name 1>
 *                     [<Sub-sampling in the both directions>]
 *
 * Example:
 *
 *    epr_asar_test
 *              "D:/ENVISAT/data/ASAR/ASA_IMG_1PXPDE20020730_095910_000000162008_00108_02166_0007.N1"
 *              "proc_data"
 *              "10"
 *
 *  relevant parameters for corresponding products:
 *
 *      ASA_IMG_1P  -  "proc_data"
 *      ASA_IMP_1P  -  "proc_data"
 *      ASA_IM__BP  -  "proc_data"
 *      ASA_IMM_1P  -  "img_mod"
 *      ASA_IMS_1P  -  "i" or "q"
 *      ASA_APG_1P  -  "proc_data_1" or "proc_data_2"
 *      ASA_APP_1P  -  "proc_data_1" or "proc_data_2"
 *      ASA_APM_1P  -  "proc_data_1" or "proc_data_2"
 *      ASA_AP__BP  -  "proc_data"
 *      ASA_APS_1P  -  "i" or "q"
 *      ASA_WSM_1P  -  "proc_data"
 *      ASA_WS__BP  -  "proc_data"
 *
 *      ATS_TOA_1P  -  "reflec_nadir_0870"
 *
 *      MER_RR__1P  -  "Radiance_13"
 *
 */


/** Other allowed parameter
 *
 *  "D:/ENVISAT/data/ASAR/ASA_WS__BPXPDE20020714_100425_000001202007_00380_01937_0053.N1"
 *  "proc_data"
 *  1
 *
 *  "D:\ENVISAT\data\MERIS\L2\MER_RR__2PNPDK20021109_095038_000002702011_00065_03626_0219.N1"
 *  "reflec_1"
 *  10
 *
 *  "d:/ENVISAT/data/MERIS/L1b/MER_RR__1PNPDK20021211_115901_000002702012_00023_04085_0133.N1" "latitude" 1
 */
int main(int argc, char** argv)
{
    EPR_SProductId* product_id;
    EPR_SRaster* raster_layer = NULL;

    const char* product_name = NULL;
    const char* band_name = NULL;
    uint subsampl = 1;
    int clip_dir = 0;
    int i;
    char out_name[PATH_MAX+1];
    uint width;
    uint height;
    SExtrema band_layer;
    float band_factor;

    FILE *out_stream;
    uint numwritten;

    uint col;
    uint row;

    unsigned char* buf = NULL;
    float* sample_addr = NULL;

    printVersion();

    if (argc < 3) {
        printUsage();
        exit(-1);
    }

    product_name = argv[1];
    band_name = argv[2];

    if (argc == 4) {
        /* manage the image size */
        subsampl = atoi(argv[3]);
    }

    /* Initialize the API. Set log-level to DEBUG and use default log-output (stdout) */

    epr_init_api(e_log_debug, epr_log_message, NULL);

    /* Open the product; an argument is a path to product data file */
    product_id = epr_open_product(product_name);
    if (product_id == NULL) {
        printf("error: failed to open product '%s'\n", product_name);
        printf("  extended message: %s\n", epr_get_last_err_message());
        exit(1);
    }

    width = (uint) epr_get_scene_width(product_id);
    height = (uint)epr_get_scene_height(product_id);
    printf("info: scene raster height: %u\n", height);
    printf("info: scene raster width:  %u\n", width);

    if (subsampl > width){
        printf("warning: subsampling too large; changed to %u\n", width);
        subsampl = width;
    }

    /* construct output image name */
    clip_dir = 0;
    for (i = strlen(product_name)-1; i>=0; i --){
        if (strrchr("/\\", product_name[i]) != NULL) {
            clip_dir = 1;
            break;
        }
    }

    if (clip_dir) {
        strncpy(out_name, product_name + i + 1, PATH_MAX);
        out_name[strlen(out_name)-2] = '\0';
    } else {
        strcpy(out_name, product_name);
    }
    strcat(out_name, "raw");

    /* build layer for the 'raw' image */
    raster_layer = make_layer(product_id, band_name, width, height, subsampl);
    if (raster_layer == NULL) {
        printf("error: failed to create band from '%s'\n", band_name);
        exit(3);
    }

    band_layer = get_raster_minmax(raster_layer);
    band_factor = 255.0F / (band_layer.max - band_layer.min);


    buf = (unsigned char*)malloc(raster_layer->raster_width);
    if (buf == NULL) {
        printf("error: failed to allocate memory: req. size: %d bytes\n", width);
        exit(4);
    }

    out_stream = fopen (out_name, "wb");

    for (row = 0; row < raster_layer->raster_height; row++) {
        for (col = 0; col < raster_layer->raster_width; col++) {
            sample_addr = (float*)raster_layer->buffer + raster_layer->raster_width * row + col;
            buf[col] = (unsigned char)(*sample_addr * band_factor);
        }

        numwritten = fwrite((unsigned char*)buf, sizeof (uchar), raster_layer->raster_width, out_stream);
        if (numwritten != raster_layer->raster_width) {
            printf("epr_make_image: file write failed\n");
            break;
        }
    }

    free(buf);
    fclose(out_stream);

    epr_close_product(product_id);

    epr_close_api();

    return 0;
}


/**
 * Builds the raster for the given dataset name and given product
 *
 * @return the instance of the scaned data
 */
EPR_SRaster* make_layer(EPR_SProductId* product_id,
                        const char* ds_name,
                        uint source_w,
                        uint source_h,
                        uint subsampl)
{
    EPR_SBandId* band_id = NULL;
    int is_written;
    EPR_SRaster* raster_buffer = NULL;
    uint source_step_x, source_step_y;

    band_id = epr_get_band_id(product_id, ds_name);
    if (band_id == NULL) {
        printf("BAND_ID = NULL\n");
        return NULL;
    }

    /* INCREMENT_X = INCREMENT_Y */
    source_step_x = subsampl;
    source_step_y = subsampl;

    raster_buffer = epr_create_compatible_raster(band_id, source_w, source_h, source_step_x, source_step_y);

    /* OFFSET_X = 0; OFFSET_Y = 0; */
    is_written = epr_read_band_raster(band_id, 0, 0, raster_buffer);

    if (is_written != 0) {
        epr_free_raster(raster_buffer);
        return NULL;
    }

    return raster_buffer;
}


/**
 * Findes maximum-minimum values in the given raster
 *
 * @return SExtrema-structure
 */
SExtrema get_raster_minmax (EPR_SRaster* raster)
{
    uint i;
    SExtrema extrema;

    extrema.max = ((float*)raster->buffer)[0];
    extrema.min = ((float*)raster->buffer)[0];

    for (i = 0; i < raster->raster_height * raster->raster_width; i++) {
        if (((float*)raster->buffer)[i] < extrema.min) {
            extrema.min = ((float*)raster->buffer)[i];
        }
        if (((float*)raster->buffer)[i] > extrema.max) {
            extrema.max = ((float*)raster->buffer)[i];
        }
    }
    return extrema;
}

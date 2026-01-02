#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "../epr_api.h"

int write_raw_image(const char* output_dir, EPR_SProductId* product_id, const char* band_name);

#if defined(WIN32) && defined(_DEBUG)
#include <crtdbg.h>
#endif /* if defined(WIN32) && defined(_DEBUG) */

/**
 * A program which tests the epr-c-api by converting producing ENVI raster
 * information from dataset.
 *
 * It generates a *.raw data file for all rasters included in the
 * given product in the given output directory.
 *
 * Call: api_test <ENVISAT-Product file path> <Output directory for file exports>
 */
int main(int argc, char** argv)
{
    EPR_SProductId* product_id;
    int i, numGrids;
    const char* product_file_path;
    const char* output_dir_path;

    if (argc <= 2) {
        printf("Usage: api_test <envisat-product> <output-dir>");
        printf("  where envisat-product is the input filename\n");
        printf("  and output-dir is the output directory\n");
        printf("Example:\n");
        printf("  api_test \"./MER_RR__2P_TEST.N1\" \".\\output\" \n\n");
        exit(1);
    }

    product_file_path = argv[1];
    output_dir_path = argv[2];

    /* Initialize the API. Set log-level to DEBUG and use default log-output (stdout) */
    epr_init_api(e_log_debug, epr_log_message, NULL);

    /* Open the product; an argument is a path to product data file */
    product_id = epr_open_product(product_file_path);

    numGrids = epr_get_num_bands(product_id);
    for (i = 0; i < numGrids; i ++) {
        write_raw_image(output_dir_path, product_id, epr_get_band_name(epr_get_band_id_at(product_id, i)));
    }

    /* Close product_id and release rest of the allocated memory */
    epr_close_product(product_id);
    /* Closes product reader API, release all allocated resources */
    epr_close_api();

    return 0;
}


/**
 * Generate the ENVI binary pattern image file for an actual DS.
 *
 * The first parameter is the output directory path.
 *
 * The function returns 1, if the file is generated, 0 otherwise.
 */
int write_raw_image(const char* output_dir, EPR_SProductId* product_id, const char* band_name)
{
    FILE *out_stream;
    uint y, numwritten;
    char image_file_path[1024];
    EPR_SBandId* band_id = NULL;
    int err_code;
    EPR_SRaster* raster = NULL;
    uint source_w, source_h;
    uint source_step_x, source_step_y;

    /* Build ENVI file path, DS name specifically */
#ifdef WIN32
    sprintf(image_file_path, "%s\\%s.img", output_dir, band_name);
#else
    sprintf(image_file_path, "%s/%s.img", output_dir, band_name);
#endif

    band_id = epr_get_band_id(product_id, band_name);
    if (band_id == NULL) {
        printf("Error: band '%s' not found\n", band_name);
        return 1;
    }
    source_w = epr_get_scene_width(product_id);
    source_h = epr_get_scene_height(product_id);
    source_step_x = 1;
    source_step_y = 1;

    raster = epr_create_compatible_raster(band_id, source_w, source_h, source_step_x, source_step_y);

    printf("Reading band '%s'...\n", band_name);
    err_code = epr_read_band_raster(band_id, 0, 0, raster);
    if (err_code != 0) {
        printf("Error: can't read raster data from '%s'\n", band_name);
        return 2;
    }

    out_stream = fopen(image_file_path, "wb");
    if (out_stream == NULL) {
        printf("Error: can't open '%s'\n", image_file_path);
        return 3;
    }

    for (y = 0; y < (uint)raster->raster_height; y ++) {
        numwritten = fwrite(epr_get_raster_line_addr(raster, y),
                            raster->elem_size,
                            raster->raster_width,
                            out_stream);

        if (numwritten != raster->raster_width) {
            printf("Error: can't write to %s\n", image_file_path);
            return 4;
        }
    }
    fclose(out_stream);

    printf("Raw image data successfully written to '%s'.\n", image_file_path);
    printf("C data type is '%s', element size %u byte(s), raster size is %u x %u pixels.\n",
        epr_data_type_id_to_str(raster->data_type),
        raster->elem_size,
        raster->raster_width,
        raster->raster_height);

    epr_free_raster(raster);

    return 0;
}

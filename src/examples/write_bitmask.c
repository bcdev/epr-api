#include "../epr_api.h"

/*
 * Generates bit mask from ENVISAT flags information as "raw" image for (e.g.) Photoshop
 *
 * Call: write_bitmask <envisat-product> <bitmask-expression> <output-file>
 *
 * Example to call the main function.
 * "./MER_RR__2P_TEST.N1" "l2_flags.LAND and !l2_flags.BRIGHT" "./my_flags.raw"
 *
 */

#include <string.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
    EPR_SProductId* product_id;
    EPR_SRaster*  bm_raster;
    uint offset_x;
    uint offset_y;
    uint source_width;
    uint source_height;
    uint source_step_x;
    uint source_step_y;
    const char* product_file_path;
    const char* image_file_path;
    const char* bm_expr;
    FILE *out_stream;
    uint i, numwritten;
    int status;

    if (argc != 4) {
        printf("Usage: write_bitmask <envisat-product> <bitmask-expression> <output-file>\n");
        printf("  where envisat-product is the input filename\n");
        printf("  and bitmask-expression is a string containing the bitmask logic\n");
        printf("  and output-file is the output filename.\n");
        printf("Example: \"./MER_RR__2P_TEST.N1\" \"l2_flags.LAND and !l2_flags.BRIGHT\" \"./my_flags.raw\"\n\n");
        exit(1);
    }

    product_file_path = argv[1];
    bm_expr = argv[2];
    image_file_path = argv[3];


    /* Initialize the API. Set log-level to DEBUG and use default log-output (stdout) */
    epr_init_api(e_log_debug, epr_log_message, NULL);

    /* Open the product; an argument is a path to product data file */
    product_id = epr_open_product(product_file_path);
    if (product_id == NULL) {
        printf("Error: failed to open product '%s'\n", product_file_path);
        return 1;
    }

    offset_x = 0;
    offset_y = 0;
    source_width  = epr_get_scene_width(product_id);
    source_height = epr_get_scene_height(product_id);
    source_step_x = 1;
    source_step_y = 1;

    bm_raster = epr_create_raster(e_tid_uchar, source_width, source_height, source_step_x, source_step_y);

    status = epr_read_bitmask_raster(product_id, bm_expr, offset_x, offset_y, bm_raster);
    if (status != 0) {
        printf("Error: %s\n", epr_get_last_err_message());
        return 1;
    }

    out_stream = fopen(image_file_path, "wb");
    if (out_stream == NULL) {
        printf("Error: can't open '%s'\n", image_file_path);
        return 1;
    }
    for (i = 0; i < (uint)bm_raster->raster_height; i ++) {
        numwritten = fwrite(((uchar*) bm_raster->buffer) + bm_raster->raster_width * i,
                             sizeof (uchar),
                             bm_raster->raster_width,
                             out_stream);

        if (numwritten != bm_raster->raster_width) {
            printf("Error: can't write to '%s'\n", image_file_path);
            fclose(out_stream);
            return 2;
        }
    }
    fclose(out_stream);

    printf("Raw image data successfully written to '%s'.\n", image_file_path);
    printf("Data type is 'byte', size is %d x %d pixels.\n", source_width, source_height);

    epr_free_raster(bm_raster);
    /* Close product_id and release rest of the allocated memory */
    epr_close_product(product_id);
    /* Closes product reader API, release all allocated resources */
    epr_close_api();

    return 0;
}

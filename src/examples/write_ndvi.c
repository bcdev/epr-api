#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include "../epr_api.h"

/*
 * Example for using the epr-api
 *
 * Demonstrates how to open a MERIS L1b product and calculate the NDVI
 *
 * This example does not demonstrate how to write good and safe code.
 * It is reduced to the essentials for working with the epr-api.
 *
 * Calling sequence: ndvi <ENVISAT-Product file> <output ndvi raw image file>
 *
 *      for example: ndvi MER_RR__1P_test.N1 my_ndvi.raw
 *
 */

int main(int argc, char* argv[]) {
    FILE *           out_stream;
    EPR_SProductId * product_id;
    EPR_SBandId *    band_id1 = NULL;
    EPR_SBandId *    band_id2 = NULL;
    EPR_SRaster *    raster1;
    EPR_SRaster *    raster2;
    char*            band1_name;
    char*            band2_name;
    char*            log_buffer;
    uint             i, j;
    int              offset_x, offset_y, status;
    float            rad1, rad2, ndvi;
    ulong            width, height;
    uint             subsampling_x, subsampling_y;

    if (argc != 3) {
        printf("Usage: write_ndvi <envisat-product> <output-file>\n");
        printf("  where envisat-product is the input filename\n");
        printf("  and output-file is the output filename.\n");
        printf("Example: \"./MER_RR__1P_TEST.N1\" \"./my_ndvi.raw\"\n\n");
        exit(1);
    }

    /*
       Initialize the API. Set log-level to DEBUG and use default log-output (stdout)
       e_log_debug is an enumeration defined in epr_api.h
       epr_log_message is a function defined in epr_api.h
    */
    epr_init_api(e_log_debug, epr_log_message, NULL);

    /*
       Open the product; the name of the product is in the first argument of the program call
       we do not check here if the product is a valid L1b product to keep the code simple
    */
    product_id = epr_open_product(argv[1]);

    /*
       The NDVI shall be calculated using bands 6 and 8. The names of these bands are
       "radiance_6" and "radiance_10". This can be found in the BEAM documentation or using VISAT.
    */
    band1_name = "radiance_6";
    band2_name = "radiance_10";

    /*
       Now we have to obtain band identifier for these bands. This is a structure which we
       will use in the next step to read the calibrated radiances into the raster (i.e. the
       matrix with the radiance values).
    */
    band_id1 = epr_get_band_id(product_id, band1_name);
    if (band_id1 == NULL) {
        printf("error: band '%s' not found (MERIS L1b product expected)\n", band1_name);
        return 0;
    }
    band_id2 = epr_get_band_id(product_id, band2_name);
    if (band_id2 == NULL) {
        printf("error: band '%s' not found (MERIS L1b product expected)\n", band2_name);
        return 0;
    }
    /*
       Before we can read the data into the raster, we have to allocate memory for the raster,
       i.e. we have to create the raster. We make it simple and define our raster of the same size
       as the whole product, and don't apply subsampling.
    */
    width = epr_get_scene_width(product_id);
    height = epr_get_scene_height(product_id);
    subsampling_x = 1;
    subsampling_y = 1;
    raster1 = epr_create_compatible_raster(band_id1, width, height, subsampling_x, subsampling_y);
    raster2 = epr_create_compatible_raster(band_id2, width, height, subsampling_x, subsampling_y);

    /*
       Now we read the radiance into the raster. Because our raster matches the whole product,
       we start reading at offset (0,0)
    */
    offset_x = 0;
    offset_y = 0;
    log_buffer = calloc(80, 1);
    sprintf(log_buffer, "read '%s' data", band1_name);
    epr_log_message(e_log_info, log_buffer);
    free(log_buffer);
    status = epr_read_band_raster(band_id1, offset_x, offset_y, raster1);

    log_buffer = calloc(80, 1);
    sprintf(log_buffer, "read '%s' data", band2_name);
    epr_log_message(e_log_info, log_buffer);
    free(log_buffer);
    status = epr_read_band_raster(band_id2, offset_x, offset_y, raster2);

    /*
       So, now we hold the two arrays totally in memory. I hope that enough memory is available.
       The rest is easy. We loop over all pixel and calculate the NDVI. We simply write each calculated
       pixel directly into the output image. Not elegant, but simple.
    */

    log_buffer = calloc(80, 1);
    sprintf(log_buffer, "write ndvi to '%s'", argv[2]);
    epr_log_message(e_log_info, log_buffer);
    free(log_buffer);
    out_stream = fopen(argv[2], "wb");
    for (j = 0; j < height; j++) {
        for (i = 0; i < width; i++) {
            rad1 = epr_get_pixel_as_float(raster1, i, j);
            rad2 = epr_get_pixel_as_float(raster2, i, j);
            if ((rad1 + rad2) != 0.0) {
                ndvi = (rad2 - rad1) / (rad2 + rad1);
            } else {
                ndvi = -1.0;
            }
            status = fwrite( & ndvi, sizeof(float), 1, out_stream);
        }
    }
    epr_log_message(e_log_info, "ndvi was written success");

    /*
       This was all. Now we have to close everything, release memory and say goodbye.
       If you want, you can open the written file an image processing program and look at the result.
    */
    fclose(out_stream);
    epr_free_raster(raster1);
    epr_free_raster(raster2);
    epr_close_product(product_id);
    epr_close_api();

    return 0;
}




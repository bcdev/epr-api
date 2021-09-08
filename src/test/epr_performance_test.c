#include <stdio.h>
#include <time.h>

#include "../epr_api.h"

/**
 *
 * Call: epr_performance_test <ENVISAT-Product file path> [<ENVISAT-Product file path>, ...]
 *
 * Example:
 *    epr_performance_test testdata/MER_RR__2PNRAL20100429_160201_000003102089_00040_42679_0001.N1
 *
 * Example output:
 *    checksum: c0619946, duration: 1.818076
 *
 */
int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("usage: %s <product> [<product>, ...]\n", argv[0]);
        return 1;
    }

    /* Starting point for measuring the time */
    clock_t start_time = clock();

    epr_init_api(e_log_warning, epr_log_message, NULL);

    uint checksum = 0;
    int product_index;
    for (product_index = 1; product_index < argc; product_index++) {
        const char *product_name = argv[product_index];
        EPR_SProductId *product_id = epr_open_product(product_name);
        if (product_id == NULL) {
            printf("Error opening product %s: %s\n", product_name, epr_get_last_err_message());
            return 1;
        }

        uint width = (uint) epr_get_scene_width(product_id);
        uint height = (uint)epr_get_scene_height(product_id);
        uint num_bands = epr_get_num_bands(product_id);

        uint band_index;
        for (band_index = 0; band_index < num_bands; band_index++) {
            EPR_SBandId *band_id = epr_get_band_id_at(product_id, band_index);
            if (band_id == NULL) {
                printf("Error creating band ID at index %d: %s\n", band_index, epr_get_last_err_message());
                return 1;
            }

            EPR_SRaster *raster = epr_create_compatible_raster(band_id, width, height, 1, 1);
            if (raster == NULL) {
                printf("Error creating raster for band %s: %s\n", epr_get_band_name(band_id), epr_get_last_err_message());
                return 1;
            }

            int success = epr_read_band_raster(band_id, 0, 0, raster);
            if (success != 0) {
                printf("Error reading band %s: %s\n", epr_get_band_name(band_id), epr_get_last_err_message());
                return 1;
            }

            /**
             * Sum up all values in the raster. We do not care about the actual data type of the band or overflows,
             * there is no further meaning behind this calculation. We just want to avoid that the compiler 
             * removes these calls because their result is seemingly unused.
             */
            uint row, col;
            for (row = 0; row < height; row++) {
                for (col = 0; col < width; col++) {
                    checksum += epr_get_pixel_as_uint(raster, col, row);
                }
            }

            epr_free_raster(raster);
        }

        epr_close_product(product_id);
    }

    epr_close_api();

    /* Calculate and print the duration since the starting point */
    clock_t end_time = clock();
    printf("checksum: %x, duration: %f\n", checksum, (float)(end_time - start_time) / CLOCKS_PER_SEC);

    return 0;
}

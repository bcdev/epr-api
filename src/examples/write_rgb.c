/*
 * This is an example program for creating TIFF-RGB images from three ENVISAT data channels.
 *
 * To compile this file you will need a recent "libtiff" library  (v3.5.7 or higher,
 * http://www.libtiff.org/) for writing TIFF files. Because of the "libtiff" library dependencies
 * you might also need "libjpeg" as well as "zlib" libraries.
 *
 * Note for MS-Windows users: make sure that "libtiff.dll", "libjpeg.dll" and "zlib.dll" can be
 * found via your system's PATH variable or place the DLLs next to this program.
 *
 * This program uses 3 channels of an ENVISAT product for the generation of the RGB image.
 *
 * The created TIF-image has the same name as a ENVISAT product data file
 * however - with the extention ".tif"
 *
 * Usage: create_RGB_image <ENVISAT-file-path> <output-directory>
 *					<R-band-name> <G-band-name> <B-band-name> [<XY-step>]
 *
 * Parameters:
 *      <ENVISAT-file-path> - the file path to the ENVISAT product
 *      <R-band-name>       - the dataset name for the R-channel
 *      <G-band-name>       - the dataset name for the G-channel
 *      <B-band-name>       - the dataset name for the B-channel
 *      <XY-step>           - sub-sampling in the both directions, optinal. Default:1.
 *
 * Example:
 *	  create_RGB_image
 *				"d:/ENVISAT/data/MERIS/L1b/MER_RR__1PNPDK20020415_103725_000002702005_00094_00649_1059.N1"
 *				radiance_13 radiance_5 radiance_1 6
 *
 */

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "../epr_api.h"
#include "../epr_string.h"
#include "tiffio.h"

#if defined(WIN32) && defined(_DEBUG)
#include <crtdbg.h>
#endif /* if defined(WIN32) && defined(_DEBUG) */


#define EPR_TIF_NAME "epr_tif"
#define EPR_TIF_VERSION "1.0"

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
EPR_SRaster* make_layer(EPR_SProductId* product_id, const char* ds_name, uint32 source_w, uint32 source_h, uint32 subsampl);
SExtrema get_raster_minmax(EPR_SRaster* raster);
uint32 prepare_image_properties(TIFF *out, uint32 width, uint32 height);

void printVersion()
{
    printf("%s, version %s (c) by Brockmann Consult\n", EPR_TIF_NAME, EPR_TIF_VERSION);
}

void printUsage()
{
    printf("usage: %s <product> <R-band> <G-band> <B-band> [<sub-sampl>]\n", EPR_TIF_NAME);
}

int main(int argc, char** argv)
{
    EPR_SProductId* product_id;
	EPR_SRaster* r_raster_layer = NULL;
    EPR_SRaster* g_raster_layer = NULL;
    EPR_SRaster* b_raster_layer = NULL;
	float* r_sample_addr = NULL;
    float* g_sample_addr = NULL;
    float* b_sample_addr = NULL;
    uint32 col;
    char out_name[PATH_MAX+1];
    int i;
    TIFF *out = NULL;
	uint32 width;
	uint32 height;
	uint32 scanlineLen = 0;
	uint32 row;
    int clip_dir = 0;
	unsigned char* buf = NULL;
    SExtrema r_layer;
    SExtrema g_layer;
    SExtrema b_layer;
    float r_factor;
    float g_factor;
    float b_factor;
    const char* product_name = NULL;
    const char* r_band_name = NULL;
    const char* g_band_name = NULL;
    const char* b_band_name = NULL;
    uint32 subsampl = 1;

    printVersion();

    if (argc < 5) {
        printUsage();
        exit(-1);
    }

    product_name = argv[1];
    r_band_name = argv[2];
    g_band_name = argv[3];
    b_band_name = argv[4];

    if (argc == 6) {
        /* manage the image size */
        subsampl = atoi(argv[5]);
    }


    /* Initialize the API. Set log-level to DEBUG and use default log-output (stdout) */
	epr_init_api(e_log_debug, epr_log_message, NULL);/*"../../dddb"*/

    /* Open the product; an argument is a path to product data file */
	product_id = epr_open_product(product_name);
    if (product_id == NULL) {
        printf("error: failed to open product '%s'\n", product_name);
        printf("  extended message: %s\n", epr_get_last_err_message());
        exit(1);
    }

    width = (uint32) epr_get_scene_width(product_id);
	height = (uint32)epr_get_scene_height(product_id);
    printf("info: scene raster height: %ld\n", height);
    printf("info: scene raster width:  %ld\n", width);

    if (subsampl > width){
        printf("warning: subsampling too large; changed to %ld\n", width);
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
        out_name[PATH_MAX] = '\0';
    } else {
        strcpy(out_name, product_name);
    }
    strcat(out_name, ".tif");

	out = TIFFOpen(out_name, "w");
    if (out == NULL) {
        printf("error: failed to create TIFF file '%s'\n", out_name);
        exit(2);
    }

    /* build three layer for the RGB image */
    r_raster_layer = make_layer(product_id, r_band_name, width, height, subsampl);
    if (r_raster_layer == NULL) {
        printf("error: failed to create R-band from '%s'\n", r_band_name);
        exit(3);
    }
    g_raster_layer = make_layer(product_id, g_band_name, width, height, subsampl);
    if (g_raster_layer == NULL) {
        printf("error: failed to create G-band from '%s'\n", g_band_name);
        exit(3);
    }
    b_raster_layer = make_layer(product_id, b_band_name, width, height, subsampl);
    if (b_raster_layer == NULL) {
        printf("error: failed to create B-band from '%s'\n", b_band_name);
        exit(3);
    }

    /* Makes settings for the TIFF-image will be created */
    scanlineLen = prepare_image_properties(out, (uint32)r_raster_layer->raster_width, (uint32)r_raster_layer->raster_height);

    /* create RGB image */
	buf = (unsigned char*)_TIFFmalloc(scanlineLen);
	if (buf == NULL) {
        printf("error: failed to allocate memory: req. size: %lu bytes\n", scanlineLen);
        exit(4);
	}

    r_layer = get_raster_minmax(r_raster_layer);
    g_layer = get_raster_minmax(g_raster_layer);
    b_layer = get_raster_minmax(b_raster_layer);
    r_factor = 255.0F / (r_layer.max - r_layer.min);
    g_factor = 255.0F / (g_layer.max - g_layer.min);
    b_factor = 255.0F / (b_layer.max - b_layer.min);

	for (row = 0; row < r_raster_layer->raster_height; row++) {
        for (col = 0; col < r_raster_layer->raster_width; col++) {
            r_sample_addr = (float*)r_raster_layer->buffer + r_raster_layer->raster_width * row + col;
            g_sample_addr = (float*)g_raster_layer->buffer + g_raster_layer->raster_width * row + col;
            b_sample_addr = (float*)b_raster_layer->buffer + b_raster_layer->raster_width * row + col;
		    buf[col*3 + 0] = (unsigned char)(*r_sample_addr * r_factor);
		    buf[col*3 + 1] = (unsigned char)(*g_sample_addr * g_factor);
		    buf[col*3 + 2] = (unsigned char)(*b_sample_addr * b_factor);
        }
        if (TIFFWriteScanline(out, buf, row, 0) < 0) {
			break;
        }
	}

	TIFFClose(out);
    printf("info: successfully created image file '%s'\n", out_name);


    /* releasing & closing */
    /* Release the allocated memory for rasters */
	epr_free_raster(r_raster_layer);
    epr_free_raster(g_raster_layer);
    epr_free_raster(b_raster_layer);
    /* Close product_id and release rest of the allocated memory */
	epr_close_product(product_id);
	/* Closes product reader API, release all allocated resources */
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
                        uint32 source_w,
                        uint32 source_h,
                        uint32 subsampl)
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
    uint32 i;
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


/**
 * Makes settings for the TIFF-image will be created
 *
 * @return image line property
 */
uint32 prepare_image_properties(TIFF *out, uint32 width, uint32 height)
{
    int  config = PLANARCONFIG_CONTIG;
	int photometric = PHOTOMETRIC_RGB;

    TIFFSetField(out, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
	TIFFSetField(out, TIFFTAG_IMAGEWIDTH, (uint32) width);
	TIFFSetField(out, TIFFTAG_IMAGELENGTH, (uint32) height);
	TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 3);
	TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, 8);
    TIFFSetField(out, TIFFTAG_PLANARCONFIG, config);
	TIFFSetField(out, TIFFTAG_PHOTOMETRIC, photometric);

    return TIFFScanlineSize(out);
}


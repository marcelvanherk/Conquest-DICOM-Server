/*
 * wrppm.c
 *
 * Copyright (C) 1991-1996, Thomas G. Lane.
 * Modifided for multibit by Bruce Barton of BITS Limited (shameless plug), 2009, (GPL).
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file contains routines to write output images in PPM/PGM format.
 * The extended 2-byte-per-sample raw PPM/PGM formats are supported.
 * The PBMPLUS library is NOT required to compile this software
 * (but it is highly useful as a set of PPM image manipulation programs).
 *
 * These routines may need modification for non-Unix environments or
 * specialized applications.  As they stand, they assume output to
 * an ordinary stdio stream.
 */

#include "cdjpeg.h"		/* Common decls for cjpeg/djpeg applications */

#ifdef PPM_SUPPORTED


/*
 * For 12-bit JPEG data, we either downscale the values to 8 bits
 * (to write standard byte-per-sample PPM/PGM files), or output
 * nonstandard word-per-sample PPM/PGM files.  Downscaling is done
 * if PPM_NORAWWORD is defined (this can be done in the Makefile
 * or in jconfig.h).
 * (When the core library supports data precision reduction, a cleaner
 * implementation will be to ask for that instead.)
 */

/*#if BITS_IN_JSAMPLE == 8
#define PUTPPMSAMPLE(ptr,v)  *ptr++ = (char) (v)
#define BYTESPERSAMPLE 1
#define PPM_MAXVAL 255
#else
#ifdef PPM_NORAWWORD
#define PUTPPMSAMPLE(ptr,v)  *ptr++ = (char) ((v) >> (cinfo->data_precision-8))
#define BYTESPERSAMPLE 1
#define PPM_MAXVAL 255
#else*/
/* The word-per-sample format always puts the LSB first. */
/*#define PUTPPMSAMPLE(ptr,v)			\
	{ register int val_ = v;		\
	  *ptr++ = (char) (val_ & 0xFF);	\
	  *ptr++ = (char) ((val_ >> 8) & 0xFF);	\
	}
#define BYTESPERSAMPLE 2
#define PPM_MAXVAL ((1<<cinfo->data_precision)-1)
#endif
#endif*/


/*
 * When JSAMPLE is the same size as char, we can just fwrite() the
 * decompressed data to the PPM or PGM file.  On PCs, in order to make this
 * work the output buffer must be allocated in near data space, because we are
 * assuming small-data memory model wherein fwrite() can't reach far memory.
 * If you need to process very wide images on a PC, you might have to compile
 * in large-memory model, or else replace fwrite() with a putc() loop ---
 * which will be much slower.
 */


/* Private version of data destination object */

typedef struct {
  struct djpeg_dest_struct pub;	/* public fields */

  /* Usually these two pointers point to the same place: */
  char *iobuffer;		/* fwrite's I/O buffer */
/*  JSAMPROW16 pixrow;*/		/* decompressor output buffer */
  size_t buffer_width;		/* width of I/O buffer */
  JDIMENSION samples_per_row;	/* JSAMPLEs per output row */
} ppm_dest_struct;

typedef ppm_dest_struct * ppm_dest_ptr;


/*
 * Write some pixel data.
 * In this module rows_supplied will always be 1.
 *
 * put_pixel_rows handles the "normal" 8-bit case where the decompressor
 * output buffer is physically the same as the fwrite buffer.
 */

/*METHODDEF(void)
put_pixel_rows (j_decompress_ptr cinfo, djpeg_dest_ptr dinfo,
		JDIMENSION rows_supplied)
{
  ppm_dest_ptr dest = (ppm_dest_ptr) dinfo;

  (void) JFWRITE(dest->pub.output_file, dest->iobuffer, dest->buffer_width);
}*/


/*
 * This code is used when we have to copy the data and apply a pixel
 * format translation.  Typically this only happens in 12-bit mode.
 */

METHODDEF(void)
copy_pixel_rows8 (j_decompress_ptr cinfo, djpeg_dest_ptr dinfo,
		 JDIMENSION rows_supplied)
{
  ppm_dest_ptr dest = (ppm_dest_ptr) dinfo;
  register char * bufferptr;
  register JSAMPROW16 ptr;
  register JDIMENSION col;

  ptr = (JSAMPROW16)dest->pub.buffer[0];
  bufferptr = dest->iobuffer;
  for (col = dest->samples_per_row; col > 0; col--) {
    *bufferptr++ = (char)GETJSAMPLE(*ptr++);
  }
  (void) JFWRITE(dest->pub.output_file, dest->iobuffer, dest->buffer_width);
}

METHODDEF(void)
copy_pixel_rows16 (j_decompress_ptr cinfo, djpeg_dest_ptr dinfo,
		 JDIMENSION rows_supplied)
{
  ppm_dest_ptr dest = (ppm_dest_ptr) dinfo;
  register char * bufferptr;
  register JSAMPROW16 ptr;
  register JDIMENSION col;
  register int val;

  ptr = (JSAMPROW16)dest->pub.buffer[0];
  bufferptr = dest->iobuffer;
  for (col = dest->samples_per_row; col > 0; col--) {
    val = GETJSAMPLE(*ptr++);
	*bufferptr++ = (char)((val >> 8) & 0xFF);
	*bufferptr++ = (char)(val & 0xFF);
  }
  (void) JFWRITE(dest->pub.output_file, dest->iobuffer, dest->buffer_width);
}

/*
 * Write some pixel data when color quantization is in effect.
 * We have to demap the color index values to straight data.
 */

METHODDEF(void)
put_demapped_rgb8 (j_decompress_ptr cinfo, djpeg_dest_ptr dinfo,
		  JDIMENSION rows_supplied)
{
  ppm_dest_ptr dest = (ppm_dest_ptr) dinfo;
  register char * bufferptr;
  register int pixval;
  register JSAMPROW16 ptr;
  register JSAMPROW16 color_map0 = cinfo->colormap16[0];
  register JSAMPROW16 color_map1 = cinfo->colormap16[1];
  register JSAMPROW16 color_map2 = cinfo->colormap16[2];
  register JDIMENSION col;

  ptr = (JSAMPROW16)dest->pub.buffer[0];
  bufferptr = dest->iobuffer;
  for (col = cinfo->output_width; col > 0; col--) {
    pixval = GETJSAMPLE(*ptr++);
    *bufferptr++ = (char)GETJSAMPLE(color_map0[pixval]);
    *bufferptr++ = (char)GETJSAMPLE(color_map1[pixval]);
    *bufferptr++ = (char)GETJSAMPLE(color_map2[pixval]);
  }
  (void) JFWRITE(dest->pub.output_file, dest->iobuffer, dest->buffer_width);
}

METHODDEF(void)
put_demapped_rgb16 (j_decompress_ptr cinfo, djpeg_dest_ptr dinfo,
		  JDIMENSION rows_supplied)
{
  ppm_dest_ptr dest = (ppm_dest_ptr) dinfo;
  register char * bufferptr;
  register int pixval, val;
  register JSAMPROW16 ptr;
  register JSAMPROW16 color_map0 = cinfo->colormap16[0];
  register JSAMPROW16 color_map1 = cinfo->colormap16[1];
  register JSAMPROW16 color_map2 = cinfo->colormap16[2];
  register JDIMENSION col;

  ptr = (JSAMPROW16)dest->pub.buffer[0];
  bufferptr = dest->iobuffer;
  for (col = cinfo->output_width; col > 0; col--) {
    pixval = GETJSAMPLE(*ptr++);
	val = GETJSAMPLE(color_map0[pixval]);
	*bufferptr++ = (char)((val >> 8) & 0xFF);
	*bufferptr++ = (char)(val & 0xFF);
    val = GETJSAMPLE(color_map1[pixval]);
	*bufferptr++ = (char)((val >> 8) & 0xFF);
	*bufferptr++ = (char)(val & 0xFF);
    val = GETJSAMPLE(color_map2[pixval]);
	*bufferptr++ = (char)((val >> 8) & 0xFF);
	*bufferptr++ = (char)(val & 0xFF);
  }
  (void) JFWRITE(dest->pub.output_file, dest->iobuffer, dest->buffer_width);
}

METHODDEF(void)
put_demapped_gray8 (j_decompress_ptr cinfo, djpeg_dest_ptr dinfo,
		   JDIMENSION rows_supplied)
{
  ppm_dest_ptr dest = (ppm_dest_ptr) dinfo;
  register char * bufferptr;
  register JSAMPROW16 ptr;
  register JSAMPROW16 color_map = cinfo->colormap16[0];
  register JDIMENSION col;

  ptr = (JSAMPROW16)dest->pub.buffer[0];
  bufferptr = dest->iobuffer;
  for (col = cinfo->output_width; col > 0; col--) {
    *bufferptr++ = (char)GETJSAMPLE(color_map[GETJSAMPLE(*ptr++)]);
  }
  (void) JFWRITE(dest->pub.output_file, dest->iobuffer, dest->buffer_width);
}

METHODDEF(void)
put_demapped_gray16 (j_decompress_ptr cinfo, djpeg_dest_ptr dinfo,
		   JDIMENSION rows_supplied)
{
  ppm_dest_ptr dest = (ppm_dest_ptr) dinfo;
  register char * bufferptr;
  register JSAMPROW16 ptr;
  register JSAMPROW16 color_map = cinfo->colormap16[0];
  register JDIMENSION col;
  register int val;

  ptr = (JSAMPROW16)dest->pub.buffer[0];
  bufferptr = dest->iobuffer;
  for (col = cinfo->output_width; col > 0; col--) {
	val = GETJSAMPLE(color_map[GETJSAMPLE(*ptr++)]);
	*bufferptr++ = (char)((val >> 8) & 0xFF);
	*bufferptr++ = (char)(val & 0xFF);
  }  
  (void) JFWRITE(dest->pub.output_file, dest->iobuffer, dest->buffer_width);
}


/*
 * Startup: write the file header.
 */

METHODDEF(void)
start_output_ppm (j_decompress_ptr cinfo, djpeg_dest_ptr dinfo)
{
  ppm_dest_ptr dest = (ppm_dest_ptr) dinfo;
  int ppm_maxval;
  
  /* Create physical I/O buffer.  Note we make this near on a PC. */
  cinfo->buffer_size_char = FALSE; /* JSAMPLE16 buffer sent. */
  if (cinfo->data_precision_other <= 8)
    dest->buffer_width = dest->samples_per_row * SIZEOF(char);
  else dest->buffer_width = dest->samples_per_row * SIZEOF(JSAMPLE16);
  dest->iobuffer = (char *) (*cinfo->mem->alloc_small)
    ((j_common_ptr) cinfo, JPOOL_IMAGE, dest->buffer_width);
    
  ppm_maxval = (1<<cinfo->data_precision_other)-1;
  
  /* Emit file header */
  switch (cinfo->out_color_space) {
  case JCS_GRAYSCALE:
    /* emit header for raw PGM format */
    fprintf(dest->pub.output_file, "P5\n%ld %ld\n%d\n",
	    (long) cinfo->output_width, (long) cinfo->output_height,
	    ppm_maxval);
    break;
  case JCS_RGB:
    /* emit header for raw PPM format */
    fprintf(dest->pub.output_file, "P6\n%ld %ld\n%d\n",
	    (long) cinfo->output_width, (long) cinfo->output_height,
	    ppm_maxval);
    break;
  default:
    ERREXIT(cinfo, JERR_PPM_COLORSPACE);
  }
}


/*
 * Finish up at the end of the file.
 */

METHODDEF(void)
finish_output_ppm (j_decompress_ptr cinfo, djpeg_dest_ptr dinfo)
{
  /* Make sure we wrote the output file OK */
  fflush(dinfo->output_file);
  if (ferror(dinfo->output_file))
    ERREXIT(cinfo, JERR_FILE_WRITE);
}


/*
 * The module selection routine for PPM format output.
 */

GLOBAL(djpeg_dest_ptr)
jinit_write_ppm (j_decompress_ptr cinfo)
{
  ppm_dest_ptr dest;

  /* Create module interface object, fill in method pointers */
  dest = (ppm_dest_ptr)
      (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
				  SIZEOF(ppm_dest_struct));
  dest->pub.start_output = start_output_ppm;
  dest->pub.finish_output = finish_output_ppm;

  /* Calculate output image dimensions so we can allocate space */
  jpeg_calc_output_dimensions(cinfo);

  /* Dimensions for the physical I/O buffer. */
  dest->samples_per_row = cinfo->output_width * cinfo->out_color_components;

  /* A JSAMPLE16 buffer for decompression now that jdapistd take it.
   * Conversion to 8 bit if needed is done later with copy or put 8.
   * Scaling is now done by the jpeg library. */
  dest->pub.buffer = (*cinfo->mem->alloc_sarray)
    ((j_common_ptr) cinfo, JPOOL_IMAGE,
    (JDIMENSION) cinfo->output_width * cinfo->output_components
    * SIZEOF(JSAMPLE16),(JDIMENSION) 1);
  dest->pub.buffer_height = 1;
	
  if (cinfo->data_precision_other <= 8) {
    if (cinfo->quantize_colors) {
      if (cinfo->out_color_space == JCS_GRAYSCALE)
       dest->pub.put_pixel_rows = put_demapped_gray8;
	  else dest->pub.put_pixel_rows = put_demapped_rgb8; /* Maped color */
    } else dest->pub.put_pixel_rows = copy_pixel_rows8; /* Default 8 bit */
  } else { /* > 8 bit output */
	if (cinfo->quantize_colors) {
	  if (cinfo->out_color_space == JCS_GRAYSCALE)
	    dest->pub.put_pixel_rows = put_demapped_gray16;
	  else dest->pub.put_pixel_rows = put_demapped_rgb16;
	} else dest->pub.put_pixel_rows = copy_pixel_rows16; /* No quanttize */
  }
  
  return (djpeg_dest_ptr) dest;
}

#endif /* PPM_SUPPORTED */

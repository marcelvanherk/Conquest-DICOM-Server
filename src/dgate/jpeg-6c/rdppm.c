/*
 * rdppm.c
 *
 * Copyright (C) 1991-1997, Thomas G. Lane.
 * Modifided for multibit by Bruce Barton of BITS Limited (shameless plug), 2009, (GPL).
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file contains routines to read input images in PPM/PGM format.
 * The extended 2-byte-per-sample raw PPM/PGM formats are supported.
 * The PBMPLUS library is NOT required to compile this software
 * (but it is highly useful as a set of PPM image manipulation programs).
 *
 * These routines may need modification for non-Unix environments or
 * specialized applications.  As they stand, they assume input from
 * an ordinary stdio stream.  They further assume that reading begins
 * at the start of the file; start_input may need work if the
 * user interface has already read some data (e.g., to determine that
 * the file is indeed PPM format).
 */

#include "cdjpeg.h"		/* Common decls for cjpeg/djpeg applications */

#ifdef PPM_SUPPORTED


/* Portions of this code are based on the PBMPLUS library, which is:
**
** Copyright (C) 1988 by Jef Poskanzer.
**
** Permission to use, copy, modify, and distribute this software and its
** documentation for any purpose and without fee is hereby granted, provided
** that the above copyright notice appear in all copies and that both that
** copyright notice and this permission notice appear in supporting
** documentation.  This software is provided "as is" without express or
** implied warranty.
*/


/* Macros to deal with unsigned chars as efficiently as compiler allows */

#ifdef HAVE_UNSIGNED_CHAR
typedef unsigned char U_CHAR;
#define UCH(x)	((int) (x))
#else /* !HAVE_UNSIGNED_CHAR */
#ifdef CHAR_IS_UNSIGNED
typedef char U_CHAR;
#define UCH(x)	((int) (x))
#else
typedef char U_CHAR;
#define UCH(x)	((int) (x) & 0xFF)
#endif
#endif /* HAVE_UNSIGNED_CHAR */


#define	ReadOK(file,buffer,len)	(JFREAD(file,buffer,len) == ((size_t) (len)))


/*
 * On most systems, reading individual bytes with getc() is drastically less
 * efficient than buffering a row at a time with fread().  On PCs, we must
 * allocate the buffer in near data space, because we are assuming small-data
 * memory model, wherein fread() can't reach far memory.  If you need to
 * process very wide images on a PC, you might have to compile in large-memory
 * model, or else replace fread() with a getc() loop --- which will be much
 * slower.
 */


/* Private version of data source object */

typedef struct {
  struct cjpeg_source_struct pub; /* public fields */

  U_CHAR *iobuffer;		/* non-FAR pointer to I/O buffer */
  JSAMPROW pixrow;		/* FAR pointer to same */
  size_t buffer_width;		/* width of I/O buffer */
  JSAMPLE *rescale;		/* => maxval-remapping array, or NULL */
} ppm_source_struct;

typedef ppm_source_struct * ppm_source_ptr;


LOCAL(int)
pbm_getc (FILE * infile)
/* Read next char, skipping over any comments */
/* A comment/newline sequence is returned as a newline */
{
  register int ch;

  ch = getc(infile);
  if (ch == '#') {
    do {
      ch = getc(infile);
    } while (ch != '\n' && ch != EOF);
  }
  return ch;
}


LOCAL(unsigned int)
read_pbm_integer (j_compress_ptr cinfo, FILE * infile)
/* Read an unsigned decimal integer from the PPM file */
/* Swallows one trailing character after the integer */
/* Note that on a 16-bit-int machine, only values up to 64k can be read. */
/* This should not be a problem in practice. */
{
  register int ch;
  register unsigned int val;

  /* Skip any leading whitespace */
  do {
    ch = pbm_getc(infile);
    if (ch == EOF)
      ERREXIT(cinfo, JERR_INPUT_EOF);
  } while (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r');

  if (ch < '0' || ch > '9')
    ERREXIT(cinfo, JERR_PPM_NONNUMERIC);

  val = ch - '0';
  while ((ch = pbm_getc(infile)) >= '0' && ch <= '9') {
    val *= 10;
    val += ch - '0';
  }
  return val;
}


/*
 * Read one row of pixels.
 *
 * We provide several different versions depending on input file format.
 * In all cases, input is scaled to the size of JSAMPLE.
 *
 * A really fast path is provided for reading byte/sample raw files with
 * maxval = MAXJSAMPLE, which is the normal case for 8-bit data.
 */


METHODDEF(JDIMENSION)
get_text_gray_row (j_compress_ptr cinfo, cjpeg_source_ptr sinfo)
/* This version is for reading text-format PGM files with any maxval */
{
  ppm_source_ptr source = (ppm_source_ptr) sinfo;
  FILE * infile = source->pub.input_file;
  register JSAMPROW ptr;
  register JSAMPLE *rescale = source->rescale;
  JDIMENSION col;

  ptr = source->pub.buffer[0];
  for (col = cinfo->image_width; col > 0; col--) {
    *ptr++ = rescale[read_pbm_integer(cinfo, infile)];
  }
  return 1;
}

METHODDEF(JDIMENSION)
get_text_gray_row16 (j_compress_ptr cinfo, cjpeg_source_ptr sinfo)
/* This version is for reading text-format PGM files with any maxval */
{
  ppm_source_ptr source = (ppm_source_ptr) sinfo;
  FILE * infile = source->pub.input_file;
  register JSAMPROW16 ptr;
  register JSAMPLE16 *rescale = (JSAMPROW16)source->rescale;
  JDIMENSION col;

  ptr = (JSAMPROW16)source->pub.buffer[0];
  for (col = cinfo->image_width; col > 0; col--) {
    *ptr++ = rescale[read_pbm_integer(cinfo, infile)];
  }
  return 1;
}

METHODDEF(JDIMENSION)
get_text_rgb_row (j_compress_ptr cinfo, cjpeg_source_ptr sinfo)
/* This version is for reading text-format PPM files with any maxval */
{
  ppm_source_ptr source = (ppm_source_ptr) sinfo;
  FILE * infile = source->pub.input_file;
  register JSAMPROW ptr;
  register JSAMPLE *rescale = source->rescale;
  JDIMENSION col;

  ptr = source->pub.buffer[0];
  for (col = cinfo->image_width; col > 0; col--) {
    *ptr++ = rescale[read_pbm_integer(cinfo, infile)];
    *ptr++ = rescale[read_pbm_integer(cinfo, infile)];
    *ptr++ = rescale[read_pbm_integer(cinfo, infile)];
  }
  return 1;
}

METHODDEF(JDIMENSION)
get_text_rgb_row16 (j_compress_ptr cinfo, cjpeg_source_ptr sinfo)
/* This version is for reading text-format PPM files with any maxval */
{
  ppm_source_ptr source = (ppm_source_ptr) sinfo;
  FILE * infile = source->pub.input_file;
  register JSAMPROW16 ptr;
  register JSAMPLE16 *rescale = (JSAMPROW16)source->rescale;
  JDIMENSION col;

  ptr = (JSAMPROW16)source->pub.buffer[0];
  for (col = cinfo->image_width; col > 0; col--) {
    *ptr++ = rescale[read_pbm_integer(cinfo, infile)];
    *ptr++ = rescale[read_pbm_integer(cinfo, infile)];
    *ptr++ = rescale[read_pbm_integer(cinfo, infile)];
  }
  return 1;
}

METHODDEF(JDIMENSION)
get_scaled_gray_row (j_compress_ptr cinfo, cjpeg_source_ptr sinfo)
/* This version is for reading raw-byte-format PGM files with any maxval */
{
  ppm_source_ptr source = (ppm_source_ptr) sinfo;
  register JSAMPROW ptr;
  register U_CHAR * bufferptr;
  register JSAMPLE *rescale = source->rescale;
  JDIMENSION col;

  if (! ReadOK(source->pub.input_file, source->iobuffer, source->buffer_width))
    ERREXIT(cinfo, JERR_INPUT_EOF);
  ptr = source->pub.buffer[0];
  bufferptr = source->iobuffer;
  for (col = cinfo->image_width; col > 0; col--) {
    *ptr++ = rescale[UCH(*bufferptr++)];
  }
  return 1;
}

METHODDEF(JDIMENSION)
get_scaled_gray_row16 (j_compress_ptr cinfo, cjpeg_source_ptr sinfo)
/* This version is for reading raw-byte-format PGM files with any maxval */
{
  ppm_source_ptr source = (ppm_source_ptr) sinfo;
  register JSAMPROW16 ptr;
  register U_CHAR * bufferptr;
  register JSAMPLE16 *rescale = (JSAMPROW16)source->rescale;
  JDIMENSION col;

  if (! ReadOK(source->pub.input_file, source->iobuffer, source->buffer_width))
    ERREXIT(cinfo, JERR_INPUT_EOF);
  ptr = (JSAMPROW16)source->pub.buffer[0];
  bufferptr = source->iobuffer;
  for (col = cinfo->image_width; col > 0; col--) {
    *ptr++ = rescale[UCH(*bufferptr++)];
  }
  return 1;
}

METHODDEF(JDIMENSION)
get_scaled_rgb_row (j_compress_ptr cinfo, cjpeg_source_ptr sinfo)
/* This version is for reading raw-byte-format PPM files with any maxval */
{
  ppm_source_ptr source = (ppm_source_ptr) sinfo;
  register JSAMPROW ptr;
  register U_CHAR * bufferptr;
  register JSAMPLE *rescale = source->rescale;
  JDIMENSION col;

  if (! ReadOK(source->pub.input_file, source->iobuffer, source->buffer_width))
    ERREXIT(cinfo, JERR_INPUT_EOF);
  ptr = source->pub.buffer[0];
  bufferptr = source->iobuffer;
  for (col = cinfo->image_width; col > 0; col--) {
    *ptr++ = rescale[UCH(*bufferptr++)];
    *ptr++ = rescale[UCH(*bufferptr++)];
    *ptr++ = rescale[UCH(*bufferptr++)];
  }
  return 1;
}

METHODDEF(JDIMENSION)
get_scaled_rgb_row16 (j_compress_ptr cinfo, cjpeg_source_ptr sinfo)
/* This version is for reading raw-byte-format PPM files with any maxval */
{
  ppm_source_ptr source = (ppm_source_ptr) sinfo;
  register JSAMPROW16 ptr;
  register U_CHAR * bufferptr;
  register JSAMPLE16 *rescale = (JSAMPROW16)source->rescale;
  JDIMENSION col;

  if (! ReadOK(source->pub.input_file, source->iobuffer, source->buffer_width))
    ERREXIT(cinfo, JERR_INPUT_EOF);
  ptr = (JSAMPROW16)source->pub.buffer[0];
  bufferptr = source->iobuffer;
  for (col = cinfo->image_width; col > 0; col--) {
    *ptr++ = rescale[UCH(*bufferptr++)];
    *ptr++ = rescale[UCH(*bufferptr++)];
    *ptr++ = rescale[UCH(*bufferptr++)];
  }
  return 1;
}

METHODDEF(JDIMENSION)
get_raw_row (j_compress_ptr cinfo, cjpeg_source_ptr sinfo)
/* This version is for reading raw-byte-format files with maxval = MAXJSAMPLE.
 * In this case we just read right into the JSAMPLE buffer!
 * Note that same code works for PPM and PGM files.
 */
{
  ppm_source_ptr source = (ppm_source_ptr) sinfo;

  if (! ReadOK(source->pub.input_file, source->iobuffer, source->buffer_width))
    ERREXIT(cinfo, JERR_INPUT_EOF);
  return 1;
}

METHODDEF(JDIMENSION)
get_corr_row16 (j_compress_ptr cinfo, cjpeg_source_ptr sinfo)
/* This version is for reading raw-word-format maxval = MAXJSAMPLE, any endian */
{
  ppm_source_ptr source = (ppm_source_ptr) sinfo;
  register JSAMPROW16 ptr;
  register U_CHAR * bufferptr;
  JDIMENSION col;

  if (! ReadOK(source->pub.input_file, source->iobuffer, source->buffer_width))
    ERREXIT(cinfo, JERR_INPUT_EOF);
  ptr = (JSAMPROW16)source->pub.buffer[0];
  bufferptr = source->iobuffer;
  for (col = cinfo->image_width; col > 0; col--) {
    *ptr = UCH(*bufferptr++) << 8;
    *ptr++  |= UCH(*bufferptr++);
  }
  return 1;
}

METHODDEF(JDIMENSION)
get_word_gray_row (j_compress_ptr cinfo, cjpeg_source_ptr sinfo)
/* This version is for reading raw-word-format PGM files with any maxval */
{
  ppm_source_ptr source = (ppm_source_ptr) sinfo;
  register JSAMPROW ptr;
  register U_CHAR * bufferptr;
  register JSAMPLE *rescale = source->rescale;
  JDIMENSION col;

  if (! ReadOK(source->pub.input_file, source->iobuffer, source->buffer_width))
    ERREXIT(cinfo, JERR_INPUT_EOF);
  ptr = source->pub.buffer[0];
  bufferptr = source->iobuffer;
  for (col = cinfo->image_width; col > 0; col--) {
    register int temp;
    temp = UCH(*bufferptr++) << 8;
    temp  |= UCH(*bufferptr++);
    *ptr++ = rescale[temp];
  }
  return 1;
}

METHODDEF(JDIMENSION)
get_word_gray_row16 (j_compress_ptr cinfo, cjpeg_source_ptr sinfo)
/* This version is for reading raw-word-format PGM files with any maxval */
{
  ppm_source_ptr source = (ppm_source_ptr) sinfo;
  register JSAMPROW16 ptr;
  register U_CHAR * bufferptr;
  register JSAMPLE16 *rescale = (JSAMPROW16)source->rescale;
  JDIMENSION col;

  if (! ReadOK(source->pub.input_file, source->iobuffer, source->buffer_width))
    ERREXIT(cinfo, JERR_INPUT_EOF);
  ptr = (JSAMPROW16)source->pub.buffer[0];
  bufferptr = source->iobuffer;
  for (col = cinfo->image_width; col > 0; col--) {
    register int temp;
    temp = UCH(*bufferptr++) << 8;
    temp  |= UCH(*bufferptr++);
    *ptr++ = rescale[temp];
  }
  return 1;
}

METHODDEF(JDIMENSION)
get_word_rgb_row (j_compress_ptr cinfo, cjpeg_source_ptr sinfo)
/* This version is for reading raw-word-format PPM files with any maxval */
{
  ppm_source_ptr source = (ppm_source_ptr) sinfo;
  register JSAMPROW ptr;
  register U_CHAR * bufferptr;
  register JSAMPLE *rescale = source->rescale;
  JDIMENSION col;

  if (! ReadOK(source->pub.input_file, source->iobuffer, source->buffer_width))
    ERREXIT(cinfo, JERR_INPUT_EOF);
  ptr = source->pub.buffer[0];
  bufferptr = source->iobuffer;
  for (col = cinfo->image_width; col > 0; col--) {
    register int temp;
    temp = UCH(*bufferptr++) << 8;
    temp  |= UCH(*bufferptr++);
    *ptr++ = rescale[temp];
    temp = UCH(*bufferptr++) << 8;
    temp  |= UCH(*bufferptr++);
    *ptr++ = rescale[temp];
    temp = UCH(*bufferptr++) << 8;
    temp  |= UCH(*bufferptr++);
    *ptr++ = rescale[temp];
  }
  return 1;
}

METHODDEF(JDIMENSION)
get_word_rgb_row16 (j_compress_ptr cinfo, cjpeg_source_ptr sinfo)
/* This version is for reading raw-word-format PPM files with any maxval */
{
  ppm_source_ptr source = (ppm_source_ptr) sinfo;
  register JSAMPROW16 ptr;
  register U_CHAR * bufferptr;
  register JSAMPLE16 *rescale = (JSAMPROW16)source->rescale;
  JDIMENSION col;

  if (! ReadOK(source->pub.input_file, source->iobuffer, source->buffer_width))
    ERREXIT(cinfo, JERR_INPUT_EOF);
  ptr = (JSAMPROW16)source->pub.buffer[0];
  bufferptr = source->iobuffer;
  for (col = cinfo->image_width; col > 0; col--) {
    register int temp;
    temp  = UCH(*bufferptr++);
    temp |= UCH(*bufferptr++) << 8;
    *ptr++ = rescale[temp];
    temp  = UCH(*bufferptr++);
    temp |= UCH(*bufferptr++) << 8;
    *ptr++ = rescale[temp];
    temp  = UCH(*bufferptr++);
    temp |= UCH(*bufferptr++) << 8;
    *ptr++ = rescale[temp];
  }
  return 1;
}

/*
 * Read the file header; return image size and component count.
 */

METHODDEF(void)
start_input_ppm (j_compress_ptr cinfo, cjpeg_source_ptr sinfo)
{
  ppm_source_ptr source = (ppm_source_ptr) sinfo;
  int c, bit_width;
  unsigned int w, h, maxval;
/* On 16-bit-int machines we have to be careful of maxval = 65535 + 1 */
  long maxval_shift;
  boolean need_iobuffer, use_raw_buffer, need_rescale;

  if (getc(source->pub.input_file) != 'P')
    ERREXIT(cinfo, JERR_PPM_NOT);

  c = getc(source->pub.input_file); /* subformat discriminator character */

  /* detect unsupported variants (ie, PBM) before trying to read header */
  switch (c) {
  case '2':			/* it's a text-format PGM file */
  case '3':			/* it's a text-format PPM file */
  case '5':			/* it's a raw-format PGM file */
  case '6':			/* it's a raw-format PPM file */
    break;
  default:
    ERREXIT(cinfo, JERR_PPM_NOT);
    break;
  }

  /* fetch the remaining header info */
  w = read_pbm_integer(cinfo, source->pub.input_file);
  h = read_pbm_integer(cinfo, source->pub.input_file);
  maxval = read_pbm_integer(cinfo, source->pub.input_file);

  if (w <= 0 || h <= 0 || maxval <= 0) /* error check */
    ERREXIT(cinfo, JERR_PPM_NOT);

/*  cinfo->data_precision = BITS_IN_JSAMPLE;*/ /* we always rescale data to this */
/* Find the # of bits for maxval. */
  cinfo->data_precision_other = 0;
  for (maxval_shift = (long)maxval + 1L ; maxval_shift > 1; maxval_shift >>= 1)
    cinfo->data_precision_other++;
/* Set to 0 for: use input value from file */
  if (cinfo->data_precision == 0)
    cinfo->data_precision = cinfo->data_precision_other;

  cinfo->image_width = (JDIMENSION) w;
  cinfo->image_height = (JDIMENSION) h;

  /* initialize flags to most common settings */
  need_iobuffer = TRUE;		/* do we need an I/O buffer? */
  use_raw_buffer = FALSE;	/* do we map input buffer onto I/O buffer? */
  need_rescale = TRUE;		/* do we need a rescale array? */

  switch (c) {
  case '2':			/* it's a text-format PGM file */
    cinfo->input_components = 1;
    cinfo->in_color_space = JCS_GRAYSCALE;
    TRACEMS2(cinfo, 1, JTRC_PGM_TEXT, w, h);
    if (cinfo->data_precision <= 8)
      source->pub.get_pixel_rows = get_text_gray_row;
    else source->pub.get_pixel_rows = get_text_gray_row16;
    need_iobuffer = FALSE;
    break;

  case '3':			/* it's a text-format PPM file */
    cinfo->input_components = 3;
    cinfo->in_color_space = JCS_RGB;
    TRACEMS2(cinfo, 1, JTRC_PPM_TEXT, w, h);
    if (cinfo->data_precision <= 8)
      source->pub.get_pixel_rows = get_text_rgb_row;
    else source->pub.get_pixel_rows = get_text_rgb_row16;
    need_iobuffer = FALSE;
    break;

  case '5':			/* it's a raw-format PGM file */
    cinfo->input_components = 1;
    cinfo->in_color_space = JCS_GRAYSCALE;
    TRACEMS2(cinfo, 1, JTRC_PGM, w, h);
    if (cinfo->data_precision == cinfo->data_precision_other) {
      need_rescale = FALSE;
      if (cinfo->data_precision > 8) {
        source->pub.get_pixel_rows = get_corr_row16;
        use_raw_buffer = FALSE;
      } else { /* 8 bit*/
        source->pub.get_pixel_rows = get_raw_row;
        use_raw_buffer = TRUE;
      }
    } else if (cinfo->data_precision > 8) {
      if (cinfo->data_precision_other > 8)
        source->pub.get_pixel_rows = get_word_gray_row16;
      else source->pub.get_pixel_rows = get_word_gray_row;
    } else {
      if (cinfo->data_precision_other > 8)
       source->pub.get_pixel_rows = get_scaled_gray_row16;
      else source->pub.get_pixel_rows = get_scaled_gray_row;
    }
    break;

  case '6':			/* it's a raw-format PPM file */
    cinfo->input_components = 3;
    cinfo->in_color_space = JCS_RGB;
    TRACEMS2(cinfo, 1, JTRC_PPM, w, h);
    if (cinfo->data_precision == cinfo->data_precision_other) {
      need_rescale = FALSE;
      if (cinfo->data_precision > 8) {
        source->pub.get_pixel_rows = get_corr_row16;
        use_raw_buffer = FALSE;
      } else { /* 8 bit*/
        source->pub.get_pixel_rows = get_raw_row;
        use_raw_buffer = TRUE;
      }
    } else if (cinfo->data_precision > 8) {
      if (cinfo->data_precision_other > 8)
        source->pub.get_pixel_rows = get_word_rgb_row16;
      else source->pub.get_pixel_rows = get_word_rgb_row;
    } else {
      if (cinfo->data_precision_other > 8)
       source->pub.get_pixel_rows = get_scaled_rgb_row16;
      else source->pub.get_pixel_rows = get_scaled_rgb_row;
    }
    break;
  }

  /* Allocate space for I/O buffer: 1 or 3 bytes or words/pixel. */
  if (need_iobuffer) {
    source->buffer_width = (size_t) w * cinfo->input_components *
      ((maxval<=255) ? SIZEOF(U_CHAR) : (2*SIZEOF(U_CHAR)));
    source->iobuffer = (U_CHAR *)
      (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
				  source->buffer_width);
  }

  /* Create compressor input buffer. */
  bit_width = ((cinfo->data_precision<=8) ? SIZEOF(JSAMPLE) : SIZEOF(JSAMPLE16));
  if (use_raw_buffer) {
    /* For unscaled raw-input case, we can just map it onto the I/O buffer. */
    /* Synthesize a JSAMPARRAY pointer structure */
    /* Cast here implies near->far pointer conversion on PCs */
    source->pixrow = (JSAMPROW) source->iobuffer;
    source->pub.buffer = & source->pixrow;
    source->pub.buffer_height = 1;
  } else {
    /* Need to translate anyway, so make a separate sample buffer. */
    source->pub.buffer = (*cinfo->mem->alloc_sarray)
      ((j_common_ptr) cinfo, JPOOL_IMAGE,
      (JDIMENSION) w * cinfo->input_components *
      bit_width, (JDIMENSION) 1);
    source->pub.buffer_height = 1;
  }

  /* Compute the rescaling array if required. */
  cinfo->lossless_scaling = FALSE;
  if (need_rescale) {
    INT32 val, half_maxval, maxjsample;

    /* On 16-bit-int machines we have to be careful of maxval = 65535 */
    source->rescale = (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
				  (size_t) (((long) maxval + 1L) * (long)bit_width));
    half_maxval = maxval / 2;
    maxjsample = (1 << cinfo->data_precision) - 1;
    if (bit_width == 1) {
      if (maxjsample == maxval) {
        for (val = 0; val <= (INT32) maxval; val++) source->rescale[val] = val;
      } else {
        for (val = 0; val <= (INT32) maxval; val++) {
        /* The multiplication here must be done in 32 bits to avoid overflow */
          source->rescale[val] = (JSAMPLE) ((val*maxjsample + half_maxval)/maxval);
        }
      }
    } else {
      JSAMPLE16 *rescale16;
      rescale16 = (JSAMPLE16 *)source->rescale;
      if (maxjsample == maxval) {
        for (val = 0; val <= (INT32) maxval; val++) rescale16[val] = val;
      } else {
        for (val = 0; val <= (INT32) maxval; val++) {
        /* The multiplication here must be done in 32 bits to avoid overflow */
          rescale16[val] = (JSAMPLE16) ((val*maxjsample + half_maxval)/maxval);
        }
      }
    }
  }
}


/*
 * Finish up at the end of the file.
 */

METHODDEF(void)
finish_input_ppm (j_compress_ptr cinfo, cjpeg_source_ptr sinfo)
{
  /* no work */
}


/*
 * The module selection routine for PPM format input.
 */

GLOBAL(cjpeg_source_ptr)
jinit_read_ppm (j_compress_ptr cinfo)
{
  ppm_source_ptr source;

  /* Create module interface object */
  source = (ppm_source_ptr)
      (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
				  SIZEOF(ppm_source_struct));
  /* Fill in method ptrs, except get_pixel_rows which start_input sets */
  source->pub.start_input = start_input_ppm;
  source->pub.finish_input = finish_input_ppm;

  return (cjpeg_source_ptr) source;
}

#endif /* PPM_SUPPORTED */
